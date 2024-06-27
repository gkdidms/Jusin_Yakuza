#include "Player.h"

#include "GameInstance.h"
#include "CharacterData.h"
#include "SoketCollider.h"

#include "Mesh.h"

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

	if (FAILED(Add_CharacterData()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Priority_Tick(const _float& fTimeDelta)
{
}

void CPlayer::Tick(const _float& fTimeDelta)
{
	//if (m_pModelCom->Get_AnimFinished())
	//{
	//	m_iAnimIndex += m_iTemp;

	//	m_iTemp *= -1;

	//	Change_Animation(m_iAnimIndex);
	//}

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
		Change_Animation(m_iAnimIndex);
	}
	if (m_pGameInstance->GetKeyState(DIK_8) == TAP)
	{
		m_iAnimIndex--;
		Change_Animation(m_iAnimIndex);
	}
	if (m_pGameInstance->GetKeyState(DIK_7) == TAP)
	{
		m_iAnimIndex = 37;
		Change_Animation(m_iAnimIndex);
	}

	if (m_isAnimStart)
		m_pModelCom->Play_Animation(fTimeDelta);

	for (auto& pCollider : m_pColliders)
		pCollider.second->Tick(fTimeDelta);

	Synchronize_Root();
	Animation_Event();
}

void CPlayer::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);

	for (auto& pCollider : m_pColliders)
		pCollider.second->Late_Tick(fTimeDelta);
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		if (pMesh->Get_AlphaApply())
			m_pShaderCom->Begin(1);     //블랜드
		else
			m_pShaderCom->Begin(0);		//디폴트

		m_pModelCom->Render(i);

		i++;
	}

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
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

	// 트랜스폼 위치잡기
	// 애니메이션이 새로 실행되는 경우, center가 다시 0,0 으로 맞춰지는데 m_vPrevMove에는 이전에 center의 move를 그대로 들고있어서 움직임이 정상적이지 않음
	// 그래서 애니메이션이 바뀌었는지를 구분해서, 바뀌는중(선형보간중)이라면 기존 포지션을 유지시켜주는 코드
	if (m_pModelCom->Get_AnimChanged())
	{
		// 애니메이션이 끝났으면 본의 움직임을 그대로 적용, 애니메이션이 실행중일 때에는 본의 이전 틱에서의 움직임만큼을 빼주어서 차이만큼만 이동되게한다
		//if(m_isChanged)
		//	XMStoreFloat4(&m_vPrevMove, XMVectorZero());

		// 여기서 루프애니메이션일 경우 vMovePos가 0이 되면서 다시 뒤로 가는 문제가 생긴다.
		// 새로운 루프가 시작하는지를 검사해서 그 때에만 else로 빠지게 해줘야한다.
		if (m_pModelCom->Get_AnimRestart())
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}
		else
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (vMovePos - XMLoadFloat4(&m_vPrevMove)));
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_iChanged ? vMovePos : (vMovePos - XMLoadFloat4(&m_vPrevMove))));
	}
	else
	{
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}

	//모델이 출력될 월드 위치는, 트랜스폼 컴포넌트의 월드위치에서 센터본과 루트본의 차이만큼 빼준 위치
	// 모델 랜더 위치잡기
	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vMovePos;
	memcpy(&m_ModelWorldMatrix.m[CTransform::STATE_POSITION], &vPos, sizeof(_float4));

	XMStoreFloat4(&m_vPrevMove, vMovePos);
}

void CPlayer::Animation_Event()
{
	auto& pCurEvents = m_pData->Get_CurrentEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition());

		if (CurPos >= pEvent.fPlayPosition)
		{
			CSoketCollider* pCollider = m_pColliders.at(pEvent.iBoneIndex);

			switch (pEvent.iType)
			{
			case 0:
				pCollider->On();
				break;
			case 1:
				pCollider->Off();
				break;
			case 2:
				cout << "사운드 재생" << endl;
				break;
			case 3:
				cout << "이펙트 재생" << endl;
				break;
			}
		}
		
	}
}

HRESULT CPlayer::Add_Componenets()
{
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
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

HRESULT CPlayer::Add_CharacterData()
{
	m_pData = CCharacterData::Create(TEXT("Kiryu"));

	if (nullptr == m_pData)
		return E_FAIL;

	Apply_ChracterData();

	return S_OK;
}

void CPlayer::Apply_ChracterData()
{
	auto& pAlphaMeshes = m_pData->Get_AlphaMeshes();
	auto pMeshes = m_pModelCom->Get_Meshes();

	for (size_t i = 0; i < pMeshes.size(); i++)
	{
		for (auto& iMeshIndex : pAlphaMeshes)
		{
			if (i == iMeshIndex)
				pMeshes[i]->Set_AlphaApply(true);
		}
	}

	auto& pLoopAnimations = m_pData->Get_LoopAnimations();
	for (auto& iAnimIndex : pLoopAnimations)
	{
		m_pModelCom->Set_AnimLoop(iAnimIndex, true);
	}

	auto& pColliders = m_pData->Get_Colliders();

	for (auto& Collider : pColliders)
	{
		//Collider.first
		/* 플래시 이펙트 수정 필요. */
		CSoketCollider::SOKET_COLLIDER_DESC Desc{};
		Desc.pParentMatrix = &m_ModelWorldMatrix;
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix_AtIndex(Collider.first);
		Desc.iBoneIndex = Collider.first;
		Desc.ColliderState = Collider.second;

		CGameObject* pSoketCollider = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SoketCollider"), &Desc);
		if (nullptr == pSoketCollider)
			return;
	
		m_pColliders.emplace(Collider.first, static_cast<CSoketCollider*>(pSoketCollider));
	}

}

void CPlayer::Change_Animation(_uint iIndex)
{
	m_pModelCom->Set_AnimationIndex(m_iAnimIndex, ANIM_INTERVAL);
	string strAnimName = string(m_pModelCom->Get_AnimationName(m_iAnimIndex));
	strAnimName = m_pGameInstance->Extract_String(strAnimName, '[', ']');
	m_pData->Set_CurrentAnimation(strAnimName);
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

	for (auto& pCollider : m_pColliders)
		Safe_Release(pCollider.second);
	m_pColliders.clear();
	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pData);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
