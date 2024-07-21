#include "LandObject.h"  

#include "SystemManager.h"
#include "GameInstance.h"
#include "Collision_Manager.h"

#include "SocketCollider.h"
#include "SocketEffect.h"


#include "Mesh.h"

CLandObject::CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext},
	m_pSystemManager{ CSystemManager::GetInstance() },
	m_pCollisionManager{ CCollision_Manager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
}
CLandObject::CLandObject(const CLandObject& rhs)
	: CGameObject{ rhs },
	m_pSystemManager{ rhs.m_pSystemManager },
	m_pCollisionManager{ rhs.m_pCollisionManager }
{
	Safe_AddRef(m_pSystemManager);
	Safe_AddRef(m_pCollisionManager);
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//�׽�Ʈ ������
	m_Info.iMaxHP = 100.f;
	m_Info.iHp = m_Info.iMaxHP;

	return S_OK;
}

void CLandObject::Priority_Tick(const _float& fTimeDelta)
{
}

void CLandObject::Tick(const _float& fTimeDelta)
{
}

void CLandObject::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

void CLandObject::ImpulseResolution(CLandObject* pTargetObject, _float fDistance)
{
	if (nullptr == m_pColliderCom) return;

	_float3 vDir = m_pColliderCom->ImpulseResolution(pTargetObject->Get_Collider(), fDistance);

	if (!XMVector3Equal(XMLoadFloat3(&vDir), XMVectorZero()))
	{
		_vector vMovePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&vDir);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMovePos);
	}
}

HRESULT CLandObject::Add_CharacterData()
{
	m_pData = CCharacterData::Create(this);

	if (nullptr == m_pData)
		return E_FAIL;

	Apply_ChracterData();

	return S_OK;
}

void CLandObject::Apply_ChracterData()
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
		CSocketCollider::SOCKET_COLLIDER_DESC Desc{};
		Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix_AtIndex(Collider.first);
		Desc.iBoneIndex = Collider.first;
		Desc.ColliderState = Collider.second;
		//Desc.iType = Collider.second.isAlways ? 1 : 0;
		Desc.pParentObject = this;

		CGameObject* pSoketCollider = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SoketCollider"), &Desc);
		if (nullptr == pSoketCollider)
			return;

		auto [it, success] = m_pColliders.emplace(Collider.first, static_cast<CSocketCollider*>(pSoketCollider));

		// ������ ��� �ݶ��̴��� �ϴ� ���д�.
		// ��ü�� ���� (�÷��̾ �ǰݴ���) �ݶ��̴��� �׽� �����־���ϹǷ� On���� �ѵд�
		if (Collider.second.isAlways)
			it->second->On();
		else
			it->second->Off();
	}

	auto& pEffects = m_pData->Get_Effets();

	for (auto& pEffect : pEffects)
	{
		CSocketEffect::SOKET_EFFECT_DESC Desc{};
		Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(pEffect.first.c_str());
		Desc.wstrEffectName = pEffect.second;

		CGameObject* pSoketEffect = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SoketEffect"), &Desc);
		if (nullptr == pSoketEffect)
			return;

		auto it = m_pEffects.emplace(pEffect.first, static_cast<CSocketEffect*>(pSoketEffect));

		//it->second->On();
		it->second->Off();
	}

	auto& pTrailEvents = m_pData->Get_TrailEvents();

	for (auto& pTrailEvent : pTrailEvents)
	{
		CSocketEffect::SOKET_EFFECT_DESC Desc{};
		Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Desc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix(pTrailEvent.second.strBonelName.c_str());
		Desc.wstrEffectName = m_pGameInstance->StringToWstring(pTrailEvent.second.strTrailProtoName);

		CGameObject* pSoketEffect = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SoketEffect"), &Desc);
		if (nullptr == pSoketEffect)
			return;

		auto it = m_pTrailEffects.emplace(pTrailEvent.second.strBonelName, static_cast<CSocketEffect*>(pSoketEffect));

		if (it.second)
			it.first->second->Off();
		else
			Safe_Release(pSoketEffect);
	}

}

void CLandObject::Animation_Event()
{
	auto& pCurEvents = m_pData->Get_CurrentEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition());
		_double Duration = *(m_pModelCom->Get_AnimationDuration());

		if (CurPos >= pEvent.fPlayPosition && CurPos < Duration)
		{
			CSocketCollider* pCollider = m_pColliders.at(pEvent.iBoneIndex);

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

void CLandObject::RimLight_Event()
{
	auto& pCurEvents = m_pData->Get_Current_Rim_Events();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition());
		_double Duration = *(m_pModelCom->Get_AnimationDuration());

		if (CurPos >= pEvent.fAinmPosition && CurPos < Duration)
		{
			if (pEvent.iType == 0)
				m_strRimMeshName = pEvent.strMeshName;
			else
				m_strRimMeshName = "";
		}
	}
}

void CLandObject::Trail_Event()
{
	auto& pCurEvents = m_pData->Get_Current_Trail_Events();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pModelCom->Get_AnimationCurrentPosition());
		_double Duration = *(m_pModelCom->Get_AnimationDuration());

		if (CurPos >= pEvent.fAinmPosition && CurPos < Duration)
		{
			if (pEvent.iType == 0)		// Ʈ���� ���ֱ�
			{
				auto iter = m_pTrailEffects.find(pEvent.strBonelName);
				iter->second->On();
			}
			else
			{
				auto iter = m_pTrailEffects.find(pEvent.strBonelName);
				iter->second->Off();
			}
				
		}
	}
}

void CLandObject::Off_Attack_Colliders()
{
	for (auto& pSocketCollider : m_pColliders)
	{
		if(pSocketCollider.second->Get_CollierType() == CSocketCollider::ATTACK)
			pSocketCollider.second->Off();
	}
}

HRESULT CLandObject::Add_Components()
{
	return S_OK;
}

HRESULT CLandObject::Bind_ResourceData()
{
	return S_OK;
}

void CLandObject::Free()
{
	__super::Free();

	for (auto& pCollider : m_pColliders)
		Safe_Release(pCollider.second);
	m_pColliders.clear();

	for (auto& pEffect : m_pEffects)
		Safe_Release(pEffect.second);
	m_pEffects.clear();

	for (auto& pEffect : m_pTrailEffects)
		Safe_Release(pEffect.second);
	m_pTrailEffects.clear();

	Safe_Release(m_pData);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pSystemManager);
	Safe_Release(m_pCollisionManager);
}

void CLandObject::Compute_Height()
{
}
