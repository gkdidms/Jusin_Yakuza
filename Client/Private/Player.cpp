#include "Player.h"

#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CLandObject{ rhs }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Componenets()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC{ 1, false });
	return S_OK;
}

void CPlayer::Priority_Tick(const _float& fTimeDelta)
{
	Synchronize_Root();
}

void CPlayer::Tick(const _float& fTimeDelta)
{
	if (m_pGameInstance->GetKeyState(DIK_UP) == TAP)
	{
		m_iAnimIndex++;

		m_ModelMatrix = *(m_pTransformCom->Get_WorldFloat4x4());
		m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC{ m_iAnimIndex, false });
	}

	if (m_pGameInstance->GetKeyState(DIK_DOWN) == TAP)
	{
		m_iAnimIndex--;

		m_ModelMatrix = *(m_pTransformCom->Get_WorldFloat4x4());
		m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC{ m_iAnimIndex, false });
	}

	if (m_pGameInstance->GetKeyState(DIK_LEFT) == HOLD)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (m_pGameInstance->GetKeyState(DIK_RIGHT) == HOLD)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);
		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

// TODO: 코드 정리 필요
void CPlayer::Synchronize_Root()
{
	string strRootBoneName = "center_c_n";
	const _float4x4* pRootBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(strRootBoneName.c_str());

	strRootBoneName = "RootNode";
	const _float4x4* Test = m_pModelCom->Get_BoneCombinedTransformationMatrix(strRootBoneName.c_str());

	_vector vPos, vTest;
	memcpy(&vPos, pRootBoneMatrix->m[CTransform::STATE_POSITION], sizeof(_float4));
	memcpy(&vTest, Test->m[CTransform::STATE_POSITION], sizeof(_float4));

	_vector vPosition = vTest;
	m_ModelMatrix = *(m_pTransformCom->Get_WorldFloat4x4());

	_vector vTemp = (vPos - vTest);
	vTemp = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vTemp;

	memcpy(&m_ModelMatrix.m[CTransform::STATE_POSITION], &vTemp, sizeof(_float4));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

HRESULT CPlayer::Add_Componenets()
{
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_OBB;
	ColliderDesc.vExtents = _float3(0.1, 0.1, 0.1);
	ColliderDesc.vCenter = _float3(0, 0.f, 0);
	ColliderDesc.vRotation = _float3(0, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Bind_ResourceData()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ModelMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
