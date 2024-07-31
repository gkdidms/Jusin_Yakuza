#include "Highway_Taxi.h"

#include "GameInstance.h"
#include "Highway_Kiryu.h"

CHighway_Taxi::CHighway_Taxi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CHighway_Taxi::CHighway_Taxi(const CHighway_Taxi& rhs)
	: CLandObject{ rhs }
{
}

void CHighway_Taxi::Set_NavigationRouteIndex(_uint iLine)
{
	m_pNavigationCom->Set_NavigationRouteIndex(iLine);
}

HRESULT CHighway_Taxi::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHighway_Taxi::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	HIGHWAY_TEXI_DESC* gameobjDesc = (HIGHWAY_TEXI_DESC*)pArg;
	m_pTransformCom->Set_WorldMatrix(gameobjDesc->vStartPos);
	m_iNaviRouteNum = gameobjDesc->iNaviRouteNum;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Objects()))
		return E_FAIL;

	m_pNavigationCom->Set_Index(gameobjDesc->iNaviNum);

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

	return S_OK;
}

void CHighway_Taxi::Priority_Tick(const _float& fTimeDelta)
{
	m_pKiryu->Priority_Tick(fTimeDelta);

	if (m_pGameInstance->GetKeyState(DIK_N) == HOLD)
		m_pTransformCom->Go_Straight(fTimeDelta);
}

void CHighway_Taxi::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_isObjectDead)
		m_pModelCom->Play_Animation(fTimeDelta);

	Move_Waypoint(fTimeDelta);

	m_pKiryu->Tick(fTimeDelta);
}

void CHighway_Taxi::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	m_pKiryu->Late_Tick(fTimeDelta);
}

HRESULT CHighway_Taxi::Render()
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

		m_pShaderCom->Begin(0);		//디폴트
		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}

string CHighway_Taxi::Get_CurrentAnimationName()
{
	return m_pModelCom->Get_AnimationName(m_pModelCom->Get_CurrentAnimationIndex());
}

void CHighway_Taxi::Change_Animation()
{
}

void CHighway_Taxi::Move_Waypoint(const _float& fTimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//웨이포인트 
	//_vector vDir = m_pNavigationCom->Compute_WayPointDir(vPosition);
	//m_pTransformCom->LookAt_For_LandObject(vDir, true);

	//_float fSpeed = 20.f;
	//m_pTransformCom->Go_Straight_CustumSpeed(fSpeed, fTimeDelta, m_pNavigationCom);
}

HRESULT CHighway_Taxi::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Taxi"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC Desc{};
	Desc.iCurrentLine = m_iNaviRouteNum;
	Desc.vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHighway_Taxi::Add_Objects()
{
	CHighway_Kiryu::CARCHASE_KIRYU_DESC Desc{ const_cast<_float4x4*>(m_pTransformCom->Get_WorldFloat4x4()) };
	m_pKiryu = dynamic_cast<CHighway_Kiryu*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CarChase_Kiryu"), &Desc));

	return S_OK;
}

HRESULT CHighway_Taxi::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CHighway_Taxi* CHighway_Taxi::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHighway_Taxi* pInstance = new CHighway_Taxi(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CHighway_Taxi::Clone(void* pArg)
{
	CHighway_Taxi* pInstance = new CHighway_Taxi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHighway_Taxi::Free()
{
	__super::Free();

	Safe_Release(m_pKiryu);
	Safe_Release(m_pNavigationCom);
}
