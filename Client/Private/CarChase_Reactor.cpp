#include "CarChase_Reactor.h"

#include "GameInstance.h"
#include "CharacterData.h"

#include "Player.h"
#include "CarChase_Monster.h"

CCarChase_Reactor::CCarChase_Reactor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CCarChase_Reactor::CCarChase_Reactor(const CCarChase_Reactor& rhs)
	: CLandObject{ rhs }
{
}

string CCarChase_Reactor::Get_CurrentAnimationName()
{
	return m_strAnimName;
}

HRESULT CCarChase_Reactor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_Reactor::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	HIGHWAY_IODESC* gameobjDesc = (HIGHWAY_IODESC*)pArg;
	m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
	m_wstrModelName = gameobjDesc->wstrModelName;
	m_iNaviRouteNum = gameobjDesc->iNaviRouteNum;
	m_iStageDir = gameobjDesc->iStageDir;
	m_iLineDir = gameobjDesc->iLineDir;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pNavigationCom->Set_Index(gameobjDesc->iNaviNum);

	if (FAILED(Ready_Monster(gameobjDesc->iMonsterWeaponType)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

	return S_OK;
}

void CCarChase_Reactor::Priority_Tick(const _float& fTimeDelta)
{
	for (auto& pMonster : m_Monsters)
		pMonster->Priority_Tick(fTimeDelta);
}

void CCarChase_Reactor::Tick(const _float& fTimeDelta)
{
	Change_Animation(); //애니메이션 변경

	if (m_iAnim != -1)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::ANIMATION_DESC{m_iAnim, m_isAnimLoop});

#ifdef _DEBUG
	if (m_pGameInstance->GetKeyState(DIK_LCONTROL) == TAP)
		m_isStop = !m_isStop;
#endif // _DEBUG
	if (!m_isStop)
		Move_Waypoint(fTimeDelta);

	//m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	for (auto& pMonster : m_Monsters)
		pMonster->Tick(fTimeDelta);
}

void CCarChase_Reactor::Late_Tick(const _float& fTimeDelta)
{
	//컬링
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	//높이값 태우기
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (nullptr != m_pNavigationCom)
	{
		float fHeight = m_pNavigationCom->Compute_Height(vCurrentPos);
		vCurrentPos = XMVectorSetY(vCurrentPos, fHeight);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPos);
	}

	for (auto& pMonster : m_Monsters)
		pMonster->Late_Tick(fTimeDelta);
}

HRESULT CCarChase_Reactor::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		_bool isRS = true;
		_bool isRD = true;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
			isRS = false;
		m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;

		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		m_pShaderCom->Begin(0);		//디폴트
		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}

void CCarChase_Reactor::Change_Animation()
{
}

HRESULT CCarChase_Reactor::Setup_Animation()
{
	m_iAnim = m_pAnimCom->Get_AnimationIndex(m_strAnimName.c_str());

	if (m_iAnim == -1)
		return E_FAIL;

	// 실제로 애니메이션 체인지가 일어났을 때 켜져있던 어택 콜라이더를 전부 끈다
	if (m_pModelCom->Set_AnimationIndex(m_iAnim, m_pAnimCom->Get_Animations(), m_fChangeInterval))
	{
		m_pModelCom->Set_PreAnimations(m_pAnimCom->Get_Animations());

		Off_Attack_Colliders();
	}

	return S_OK;
}

_bool CCarChase_Reactor::Check_Dead()
{
	for (auto& pMonster : m_Monsters)
	{
		if (!pMonster->isObjectDead())
			return false;
	}
		
	return true;
}

void CCarChase_Reactor::Move_Waypoint(const _float& fTimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//웨이포인트 
	_float4 vMovePos;
	_vector vDir = m_pNavigationCom->Compute_WayPointDir(vPosition, fTimeDelta);
	m_pTransformCom->LookAt_For_LandObject(vDir, true);

	//플레이어와 멀다면
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Taxi"), 0);
	_float fDistance = XMVectorGetX(XMVector3Length(pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - vPosition));

	if (m_iAnim != 0)
	{
		m_fSpeed = fDistance < 300.f ? 41.f : 43.f;
	}
	else
		m_fSpeed = m_fSpeed <= 0.f ? 0.f : m_fSpeed - fTimeDelta * 10.f;

	m_pTransformCom->Go_Straight_CustumSpeed(m_fSpeed, fTimeDelta, m_pNavigationCom);
}

HRESULT CCarChase_Reactor::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_ReactorHighwayAnim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC Desc{};
	Desc.iCurrentLine = m_iNaviRouteNum;
	Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarChase_Reactor::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CCarChase_Reactor::Free()
{
	__super::Free();

	Safe_Release(m_pAnimCom);
	Safe_Release(m_pNavigationCom);

	for (auto& pMonster : m_Monsters)
		Safe_Release(pMonster);
	m_Monsters.clear();
}
