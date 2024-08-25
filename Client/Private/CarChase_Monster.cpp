#include "CarChase_Monster.h"

#include "GameInstance.h"
#include "UIManager.h"

#include "AI_CarChase.h"

#include "Highway_Taxi.h"
#include "EffectManager.h"



CCarChase_Monster::CCarChase_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster { pDevice, pContext },
	m_pUIManager{ CUIManager::GetInstance()}
{
	Safe_AddRef(m_pUIManager);
}

CCarChase_Monster::CCarChase_Monster(const CCarChase_Monster& rhs)
	: CMonster{ rhs },
	m_pUIManager{ CUIManager::GetInstance()}
{
	Safe_AddRef(m_pUIManager);
}

void CCarChase_Monster::Set_ReactorDead(_bool isReactorDead)
{
	m_isReactorDead = isReactorDead;
	m_pUIManager->Remove_Target(m_iObjectIndex);
}

HRESULT CCarChase_Monster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_Monster::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CARCHASE_MONSTER_DESC* pDesc = static_cast<CARCHASE_MONSTER_DESC*>(pArg);
	m_pParentMatrix = pDesc->pParentMatrix;
	m_pParentBoneMatrix = pDesc->pBoneMatrix;
	m_iWeaponType = pDesc->iWeaponType;
	m_iLineDir = pDesc->iLineDir;
	m_iStageDir = pDesc->iStageDir;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iCurrentAnimType = CLandObject::DEFAULT;

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

	m_pTargetBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("kubi_c_n");
	m_pUIManager->Add_Target(m_iObjectIndex, this);

	m_Info.fMaxHP = 100.f;
	m_Info.fHp = m_Info.fMaxHP;

	//���Ͱ� �ٶ���� �� ���� ���
	Get_LookDir();

	return S_OK;
}

void CCarChase_Monster::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChase_Monster::Tick(const _float& fTimeDelta)
{
	m_pTree->Tick(fTimeDelta);

	Change_Animation(); //�ִϸ��̼� ����

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_ModelWorldMatrix));
}

void CCarChase_Monster::Late_Tick(const _float& fTimeDelta)
{
	//�ø�
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	Update_TargetingUI();

	//�浹ó�� �ʱ�ȭ
	m_isColl = false;
}

//���ȿ� Ÿ�ִ� ���Ͱ� ������
void CCarChase_Monster::Set_Coll()
{
	m_isColl = true;

	//�浹ó��
	m_Info.fHp -= 30.f;
	//������Ʈ ���
	CEffect::EFFECT_DESC EffectDesc;

	EffectDesc.pWorldMatrix = &m_pWorldMatrix;
	CEffectManager::GetInstance()->Blood_Effect(EffectDesc);

	if (m_Info.fHp <= 0.f)
	{
		m_Info.fHp = 0.f;
		m_isObjectDead = true;

		m_pUIManager->Remove_Target(m_iObjectIndex);
	}
}

void CCarChase_Monster::Change_Animation()
{
}

void CCarChase_Monster::Update_TargetingUI()
{
	//���� ���̱� (kubi_c_n)
	_matrix BoneMatrix = XMMatrixIdentity();

	//3D -> 2D ��ȯ
	_matrix VPMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) * m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

	_matrix vWorldMaitrx = XMLoadFloat4x4(m_pTargetBoneMatrix) * XMLoadFloat4x4(&m_ModelWorldMatrix);

	//ƽ���� ������Ʈ
	XMStoreFloat4x4(&m_pWorldMatrix, vWorldMaitrx);	

	_matrix ResultMatrix = vWorldMaitrx * VPMatrix;
	_vector vPos = ResultMatrix.r[3];
	_float fX = XMVectorGetX(vPos) / XMVectorGetW(vPos);
	_float fY = XMVectorGetY(vPos) / XMVectorGetW(vPos);

	_float windowX = fX * (g_iWinSizeX / 2);
	_float windowY = fY * (g_iWinSizeY / 2);

	BoneMatrix.r[3] = XMVectorSet(windowX, windowY, 0.f, 1.f);
	m_pUIManager->Update_TargetMatrix(m_iObjectIndex, BoneMatrix, m_Info.fHp);
}

HRESULT CCarChase_Monster::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_BoneCompute"),
		TEXT("Com_ComputeShader"), reinterpret_cast<CComponent**>(&m_pComputeShaderCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.5, 0.8, 0.5);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_CarChaseAnim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom[CLandObject::DEFAULT]))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_SyncAnim"),
		TEXT("Com_SyncAnim"), reinterpret_cast<CComponent**>(&m_pAnimCom[CLandObject::CUTSCENE]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarChase_Monster::Bind_ResourceData()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ModelWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isRimLight", &m_isRimLight, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CCarChase_Monster::Get_LookDir()
{
	//���Ͱ� �ٶ�����ϴ� ����(��, ��, ��)
	//van, heli �� ��� ������ ������ ���� (������ ���ʿ� ������)

	if (m_iStageDir == DIR_B)
		m_iDir = DIR_F;
	else if (m_iStageDir == DIR_R)
		m_iDir = DIR_L;
	else if (m_iStageDir == DIR_L)
		m_iDir = DIR_R;
	else if (m_iStageDir == DIR_F)
	{
		//�÷��̾ ���� �����ʿ� �ִ���, ���ʿ� �ִ��� Ȯ���ؾ� ��.
		m_iDir == DirFromPlayerPos() == DIR_R ? DIR_L : DIR_R;
	}
}

//�÷��̾� ���⿡ ���� �޶����� �÷��̾��� ����
_uint CCarChase_Monster::DirFromPlayerPos()
{
	CHighway_Taxi* pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Taxi"), 0));
	_vector vPlayerPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerLook = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);

	_vector vLeft = XMVector3Cross(vPlayerLook, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vPlayerLook);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vPlayerPos;

	_float vLeftDot = XMVectorGetX(XMVector3Dot(vPos, vLeft));
	_float vRightDot = XMVectorGetX(XMVector3Dot(vPos, vRight));
	
	if (vLeftDot > 0)
	{
		//���ʿ� ����
		return DIR_L;
	}
	else if (vRightDot > 0)
	{
		//�����ʿ� ����
		return DIR_R;
	}

	return DIR_END;
}

void CCarChase_Monster::Set_ParentMatrix(const _float& fTimeDelta)
{
	m_pModelCom->Play_Animation_Monster(fTimeDelta, m_pAnimCom[m_iCurrentAnimType], m_isAnimLoop, false);

	_matrix ParentMatrix = XMLoadFloat4x4(m_pParentBoneMatrix) * XMLoadFloat4x4(m_pParentMatrix);
	//ParentMatrix.r[3] = (XMLoadFloat4x4(m_pParentBoneMatrix) * XMLoadFloat4x4(m_pParentMatrix)).r[3];
	//ParentMatrix.r[2] = XMLoadFloat4x4(m_pParentMatrix).r[2];
	_matrix		RotationMatrix = XMMatrixRotationAxis(ParentMatrix.r[0], XMConvertToRadians(-90.f));

	ParentMatrix.r[0] = XMVector3TransformNormal(ParentMatrix.r[0], RotationMatrix);
	ParentMatrix.r[1] = XMVector3TransformNormal(ParentMatrix.r[1], RotationMatrix);
	ParentMatrix.r[2] = XMVector3TransformNormal(ParentMatrix.r[2], RotationMatrix);

	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix() * ParentMatrix);
}

void CCarChase_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pTree);
	Safe_Release(m_pUIManager);
}
