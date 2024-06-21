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
		m_iChanged = true;
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

// 현재 애니메이션의 y축을 제거하고 사용하는 상태이다 (혹시 애니메이션의 y축 이동도 적용이 필요하다면 로직 수정이 필요함
void CPlayer::Synchronize_Root()
{
	// center_c_n의 컴바인드 행렬을 가져온다 (모델 본들의 스페이스 상태임)
	string strBoneName = "center_c_n";
	const _float4x4* pCenterBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(strBoneName.c_str());

	// RootNode의 컴바인드 행렬을 가져온다. (모델 본들의 스페이스 상태임)
	strBoneName = "RootNode";
	const _float4x4* pRootBoneMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(strBoneName.c_str());
	
	// 구해온 각각의 매트릭스들을 본들의 스페이스에서 해당 객체의 월드 스페이스로 변환한다.
	_matrix CenterBoneMatrix = XMLoadFloat4x4(pCenterBoneMatrix) * m_pTransformCom->Get_WorldMatrix();
	_matrix RootBoneMatrix = XMLoadFloat4x4(pRootBoneMatrix) * m_pTransformCom->Get_WorldMatrix();

	// 월드 변환이 완료된 행렬들에서 포지션값을 꺼내 저장한다.
	_vector vCenterBonePosistion, vRootBonePosistion;
	memcpy(&vCenterBonePosistion, &CenterBoneMatrix.r[CTransform::STATE_POSITION], sizeof(_float4));
	memcpy(&vRootBonePosistion, &RootBoneMatrix.r[CTransform::STATE_POSITION], sizeof(_float4));

	// 애니메이션이 새로 시작하면 m_vPrevMove에는 이전 애님의 마지막 move의 큰 값이 남아있고, vMovePos는 새로운정보가 되어서 초기화가 필요하다.
	_vector vMovePos = (XMVectorSet(XMVectorGetX(vCenterBonePosistion), 0, XMVectorGetZ(vCenterBonePosistion), 1.f) - XMVectorSet(XMVectorGetX(vRootBonePosistion), 0, XMVectorGetZ(vRootBonePosistion), 1.f));

	//모델이 출력될 월드 위치는, 트랜스폼 컴포넌트의 월드위치에서 센터본과 루트본의 차이만큼 빼준 위치
	// 모델 랜더 위치잡기
	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&m_vPrevMove);
	memcpy(&m_ModelWorldMatrix.m[CTransform::STATE_POSITION], &vPos, sizeof(_float4));
	
	// 트랜스폼 위치잡기
	if (m_pModelCom->Get_AnimChanged())
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_iChanged ? vMovePos : (vMovePos - XMLoadFloat4(&m_vPrevMove))));
	}
	else
	{
		cout << "Playings" << endl;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	

	XMStoreFloat4(&m_vPrevMove, vMovePos);

	//애니메이션 실행되는 동안에 false를 반환한다.


	m_iChanged = false;
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
