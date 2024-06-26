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

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC{ 3, false }, ANIM_INTERVAL);
	return S_OK;
}

void CPlayer::Priority_Tick(const _float& fTimeDelta)
{
	Synchronize_Root();
}

void CPlayer::Tick(const _float& fTimeDelta)
{
	//if (m_pGameInstance->GetKeyState(DIK_UP) == TAP)
	//{
	//	m_iChanged = true;
	//	m_iAnimIndex++;

	//	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC{ m_iAnimIndex, false }, ANIM_INTERVAL);
	//}

	//if (m_pGameInstance->GetKeyState(DIK_DOWN) == TAP)
	//{
	//	m_iChanged = true;
	//	m_iAnimIndex--;

	//	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC{ m_iAnimIndex, true }, ANIM_INTERVAL);
	//}

	if (m_pModelCom->Get_AnimFinished())
	{
		m_iAnimIndex += m_iTemp;

		m_iTemp *= -1;

		m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC{ m_iAnimIndex, false }, ANIM_INTERVAL);
	}

	if (m_pGameInstance->GetKeyState(DIK_UP) == HOLD)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (m_pGameInstance->GetKeyState(DIK_DOWN) == HOLD)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	if (m_pGameInstance->GetKeyState(DIK_LEFT) == HOLD)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta);
	}
	if (m_pGameInstance->GetKeyState(DIK_RIGHT) == HOLD)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), -fTimeDelta);
	}

	if (m_pGameInstance->GetKeyState(DIK_0) == TAP)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0, 0, 0, 1));
	}
	if (m_pGameInstance->GetKeyState(DIK_9) == TAP)
	{
		m_iAnimIndex++;
	}
	if (m_pGameInstance->GetKeyState(DIK_8) == TAP)
	{
		m_iAnimIndex--;
	}

	if (m_isAnimStart)
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

// ���� �ִϸ��̼��� y���� �����ϰ� ����ϴ� �����̴� (Ȥ�� �ִϸ��̼��� y�� �̵��� ������ �ʿ��ϴٸ� ���� ������ �ʿ���
void CPlayer::Synchronize_Root()
{
	// center_c_n�� �Ĺ��ε� ����� �����´� (�� ������ �����̽� ������)
	string strBoneName = "center_c_n";
	const _float4x4* pCenterBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(strBoneName.c_str());

	// RootNode�� �Ĺ��ε� ����� �����´�. (�� ������ �����̽� ������)
	strBoneName = "RootNode";
	const _float4x4* pRootBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(strBoneName.c_str());
	
	// ���ؿ� ������ ��Ʈ�������� ������ �����̽����� �ش� ��ü�� ���� �����̽��� ��ȯ�Ѵ�.
	_matrix CenterBoneMatrix = XMLoadFloat4x4(pCenterBoneMatrix) * m_pTransformCom->Get_WorldMatrix();
	_matrix RootBoneMatrix = XMLoadFloat4x4(pRootBoneMatrix) * m_pTransformCom->Get_WorldMatrix();

	// ���� ��ȯ�� �Ϸ�� ��ĵ鿡�� �����ǰ��� ���� �����Ѵ�.
	_vector vCenterBonePosistion, vRootBonePosistion;
	memcpy(&vCenterBonePosistion, &CenterBoneMatrix.r[CTransform::STATE_POSITION], sizeof(_float4));
	memcpy(&vRootBonePosistion, &RootBoneMatrix.r[CTransform::STATE_POSITION], sizeof(_float4));

	// �ִϸ��̼��� ���� �����ϸ� m_vPrevMove���� ���� �ִ��� ������ move�� ū ���� �����ְ�, vMovePos�� ���ο������� �Ǿ �ʱ�ȭ�� �ʿ��ϴ�.
	_vector vMovePos = (XMVectorSet(XMVectorGetX(vCenterBonePosistion), 0, XMVectorGetZ(vCenterBonePosistion), 1.f) - XMVectorSet(XMVectorGetX(vRootBonePosistion), 0, XMVectorGetZ(vRootBonePosistion), 1.f));

	// Ʈ������ ��ġ���
	// �ִϸ��̼��� ���� ����Ǵ� ���, center�� �ٽ� 0,0 ���� �������µ� m_vPrevMove���� ������ center�� move�� �״�� ����־ �������� ���������� ����
	// �׷��� �ִϸ��̼��� �ٲ�������� �����ؼ�, �ٲ����(����������)�̶�� ���� �������� ���������ִ� �ڵ�
	if (m_pModelCom->Get_AnimChanged())
	{
		// �ִϸ��̼��� �������� ���� �������� �״�� ����, �ִϸ��̼��� �������� ������ ���� ���� ƽ������ �����Ӹ�ŭ�� ���־ ���̸�ŭ�� �̵��ǰ��Ѵ�
		//if(m_isChanged)
		//	XMStoreFloat4(&m_vPrevMove, XMVectorZero());

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (vMovePos - XMLoadFloat4(&m_vPrevMove)));
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_iChanged ? vMovePos : (vMovePos - XMLoadFloat4(&m_vPrevMove))));
	}
	else
	{
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}

	//���� ��µ� ���� ��ġ��, Ʈ������ ������Ʈ�� ������ġ���� ���ͺ��� ��Ʈ���� ���̸�ŭ ���� ��ġ
	// �� ���� ��ġ���
	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vMovePos;
	memcpy(&m_ModelWorldMatrix.m[CTransform::STATE_POSITION], &vPos, sizeof(_float4));

	XMStoreFloat4(&m_vPrevMove, vMovePos);
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

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ModelWorldMatrix)))
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
