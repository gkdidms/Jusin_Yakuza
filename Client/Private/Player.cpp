#include "Player.h"

#include "GameInstance.h"
#include "SystemManager.h"

#include "CharacterData.h"
#include "SoketCollider.h"

#include "BehaviorAnimation.h"
#include "Mesh.h"

#include "BTNode.h"

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

	Ready_AnimationTree();
	ZeroMemory(&m_MoveDirection, sizeof(_bool) * MOVE_DIRECTION_END);

	return S_OK;
}

void CPlayer::Priority_Tick(const _float& fTimeDelta)
{
	m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Tick(fTimeDelta);
	m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();

	if (m_pGameInstance->GetKeyState(DIK_0) == TAP)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0, 0, 0, 1));
	}
	if (m_pGameInstance->GetKeyState(DIK_UP) == HOLD)
	{
		m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Change_Animation();
	}

	Synchronize_Root(fTimeDelta);

	if (m_isAnimStart)
		m_pModelCom->Play_Animation(fTimeDelta);

	for (auto& pCollider : m_pColliders)
		pCollider.second->Tick(fTimeDelta);


	Move_KeyInput(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	Animation_Event();
}

void CPlayer::Tick(const _float& fTimeDelta)
{

}

void CPlayer::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_SHADOWOBJ, this); // Shadow�� ���� �߰�

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
			m_pShaderCom->Begin(1);     //����
		else
			m_pShaderCom->Begin(0);		//����Ʈ

		m_pModelCom->Render(i);

		i++;
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CPlayer::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	/* ���� ������ �� ��ȯ���. */
	_vector vViewPos = m_pSystemManager->Get_ShadowViewPos();
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(vViewPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 1000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CPlayer::Ready_AnimationTree()
{
	for (size_t i = 0; i < (_uint)ADVENTURE_BEHAVIOR_STATE::ADVENTURE_BEHAVIOR_END; i++)
	{
		m_AnimationTree[ADVENTURE].emplace(i, CBehaviorAnimation::Create(ADVENTURE, i, this));
	}

}

// ���� �ִϸ��̼��� y���� �����ϰ� ����ϴ� �����̴� (Ȥ�� �ִϸ��̼��� y�� �̵��� ������ �ʿ��ϴٸ� ���� ������ �ʿ���
void CPlayer::Synchronize_Root(const _float& fTimeDelta)
{
	_vector vFF = XMVector3TransformNormal(XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove()), m_pTransformCom->Get_WorldMatrix());
	vFF = XMVectorSet(XMVectorGetX(vFF), XMVectorGetZ(vFF), XMVectorGetY(vFF), 1.f);

	// ���� ���
	_matrix worldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float4 vQuaternion = *m_pModelCom->Get_AnimationCenterRotation();

	_vector scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, worldMatrix);

	_vector resultQuaternionVector = XMQuaternionMultiply(XMLoadFloat4(&vQuaternion), rotation);

	// m_pModelCom->Get_AnimChanged()  ���������� ��������
	// m_pModelCom->Get_AnimLerp() ���������� �ʿ��� �ִϸ��̼�����
	if (m_pModelCom->Get_AnimChanged() || !m_pModelCom->Get_AnimLerp())
	{
		if (m_pModelCom->Get_AnimRestart())
		{
			XMStoreFloat4(&m_vPrevMove, XMVectorZero());
			m_fPrevSpeed = 0.f;
		}	
		else
		{
			// ���ʹϾ� ȸ���� ������ �ߴ� (���� ���� ���࿹��)
			//_float4 v;
			//_vector diffQuaternionVector = XMQuaternionMultiply(resultQuaternionVector, XMQuaternionConjugate(XMLoadFloat4(&m_vPrevRotation)));
			//XMStoreFloat4(&v, diffQuaternionVector);
			//m_pTransformCom->Change_Rotation_Quaternion(v);

			//_float4 vb;
			//XMStoreFloat4(&vb, vFF - XMLoadFloat4(&m_vPrevMove));
			//m_pTransformCom->Go_Straight_CustumDir(vb, fTimeDelta);
			_float fMoveSpeed = XMVectorGetX(XMVector3Length(vFF - XMLoadFloat4(&m_vPrevMove)));
			m_pTransformCom->Go_Straight_CustumSpeed(m_fPrevSpeed, 1);
			m_fPrevSpeed = fMoveSpeed;
		}
	}
	else
	{
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat4(&m_vPrevMove));
		//m_pTransformCom->Go_Straight_CustumSpeed(m_fPrevSpeed);
	}
	
	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4(&m_vPrevMove, vFF);
	//m_vPrevRotation = vQuaternion;
	XMStoreFloat4(&m_vPrevRotation, resultQuaternionVector);
}

void CPlayer::Animation_Event()
{
	auto& pCurEvents = m_pData->Get_CurrentEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition());
		_double Duration = *(m_pModelCom->Get_AnimationDuration());

		if (CurPos >= pEvent.fPlayPosition && CurPos < Duration)
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
				cout << "���� ���" << endl;
				break;
			case 3:
				cout << "����Ʈ ���" << endl;
				break;
			}
		}
		
	}
}

void CPlayer::Move_KeyInput(const _float& fTimeDelta)
{
	switch (m_eCurrentStyle)
	{
	case CPlayer::ADVENTURE:
	{
		if(m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Get_AnimationEnd())
			m_iCurrentBehavior = (_uint)ADVENTURE_BEHAVIOR_STATE::IDLE;

		_bool isShift = { false };
		_bool isMove = { false };

		if (m_pGameInstance->GetKeyState(DIK_LSHIFT) == HOLD)
		{
			isShift = true;
		}

		if (m_pGameInstance->GetKeyState(DIK_W) == HOLD)
		{
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamLook());
			_vector vLookDir = XMVectorSet(XMVectorGetX(vLookPos), XMVectorGetY(vLookPos), XMVectorGetZ(vLookPos), 0.f);
			_vector vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;

			isMove = true;

			//m_MoveDirection[B] = true;

			////����� ��� ���� ����, ������ ��� �ݴ����, 0�ΰ�� ����
			if (XMVectorGetX(XMVector3Dot(vLookDir, vPlayerLook)) < 0)
				m_MoveDirection[B] = true;
			else if (XMVectorGetX(XMVector3Dot(vLookDir, vPlayerLook)) > 0)
				m_MoveDirection[F] = true;

			m_pTransformCom->LookAt_For_LandObject(vLookPos);
		}
		else if (m_pGameInstance->GetKeyState(DIK_W) == AWAY)
		{
			m_MoveDirection[B] = false;
			m_MoveDirection[F] = false;
		}

		if (m_pGameInstance->GetKeyState(DIK_S) == HOLD)
		{
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamLook());
			m_pTransformCom->LookAt_For_LandObject(vLookPos);

			m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
			m_MoveDirection[B] = true;

			isMove = true;
		}
		else if (m_pGameInstance->GetKeyState(DIK_S) == AWAY)
		{
			m_MoveDirection[B] = false;
		}

		if (m_pGameInstance->GetKeyState(DIK_A) == HOLD)
		{
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) - m_pGameInstance->Get_CamRight());
			m_pTransformCom->LookAt_For_LandObject(vLookPos);

			m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
			m_MoveDirection[L] = true;

			isMove = true;
		}
		else if (m_pGameInstance->GetKeyState(DIK_A) == AWAY)
		{
			m_MoveDirection[L] = false;
		}

		if (m_pGameInstance->GetKeyState(DIK_D) == HOLD)
		{
			_vector vLookPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pGameInstance->Get_CamRight());
			m_pTransformCom->LookAt_For_LandObject(vLookPos);

			m_iCurrentBehavior = isShift ? (_uint)ADVENTURE_BEHAVIOR_STATE::WALK : (_uint)ADVENTURE_BEHAVIOR_STATE::RUN;
			m_MoveDirection[R] = true;

			isMove = true;
		}
		else if (m_pGameInstance->GetKeyState(DIK_D) == AWAY)
		{
			m_MoveDirection[R] = false;
		}

		if(!isMove && m_iCurrentBehavior == (_uint)ADVENTURE_BEHAVIOR_STATE::RUN)
			m_AnimationTree[m_eCurrentStyle].at(m_iCurrentBehavior)->Stop();

		break;
	}
	case CPlayer::KRS:
		break;
	case CPlayer::KRH:
		break;
	case CPlayer::KRC:
		break;
	}



}

HRESULT CPlayer::Add_Componenets()
{
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Model_Kiryu"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_OBB;
	ColliderDesc.vExtents = _float3(0.8, 0.8, 0.8);
	ColliderDesc.vCenter = _float3(0, 0.f, 0);
	ColliderDesc.vRotation = _float3(0, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ModelWorldMatrix)))
	//	return E_FAIL;
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
		CSoketCollider::SOKET_COLLIDER_DESC Desc{};
		Desc.pParentMatrix = &m_ModelWorldMatrix;
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix_AtIndex(Collider.first);
		Desc.iBoneIndex = Collider.first;
		Desc.ColliderState = Collider.second;

		CGameObject* pSoketCollider = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SoketCollider"), &Desc);
		if (nullptr == pSoketCollider)
			return;
	
		auto [it, success] = m_pColliders.emplace(Collider.first, static_cast<CSoketCollider*>(pSoketCollider));

		//������ ��� �ݶ��̴��� �ϴ� ���д�.
		// ��ü�� ���� (�÷��̾ �ǰݴ���) �ݶ��̴��� �׽� �����־���ϹǷ� ��������������
		it->second->Off();
	}

}

void CPlayer::Change_Animation(_uint iIndex)
{
	//if(314 <= iIndex && 323 > iIndex)
	//	m_pModelCom->Set_AnimationIndex(iIndex, 0);
	//else
	if (m_pModelCom->Set_AnimationIndex(iIndex, ANIM_INTERVAL))
	{
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
		m_fPrevSpeed = 0.f;
	}
			
	
	string strAnimName = string(m_pModelCom->Get_AnimationName(iIndex));
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
	Safe_Release(m_pData);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}