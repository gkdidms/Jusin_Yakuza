#include "Monster.h"

#include "GameInstance.h"

#include "CharacterData.h"
#include "SocketCollider.h"
#include "SocketEffect.h"
#include "Collision_Manager.h"

#include "Mesh.h"


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& rhs)
	: CLandObject { rhs }
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonster::Priority_Tick(const _float& fTimeDelta)
{
}

void CMonster::Tick(const _float& fTimeDelta)
{
	for (auto& pCollider : m_pColliders)
		pCollider.second->Tick(fTimeDelta);

	for (auto& pEffect : m_pEffects)
		pEffect.second->Tick(fTimeDelta);

	Animation_Event();

	// 현재 켜져있는 Attack용 콜라이더 삽입
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::ATTACK && pPair.second->IsOn())
			m_pCollisionManager->Add_AttackCollider(pPair.second, CCollision_Manager::ENEMY);
	}

	// 현재 켜져있는 Hit용 콜라이더 삽입 (아직까지는 Hit용 콜라이더는 항상 켜져있음)
	for (auto& pPair : m_pColliders)
	{
		if (pPair.second->Get_CollierType() == CSocketCollider::HIT && pPair.second->IsOn())
			m_pCollisionManager->Add_HitCollider(pPair.second, CCollision_Manager::ENEMY);
	}
}

void CMonster::Late_Tick(const _float& fTimeDelta)
{
	for (auto& pCollider : m_pColliders)
		pCollider.second->Late_Tick(fTimeDelta);

	for (auto& pEffect : m_pEffects)
		pEffect.second->Late_Tick(fTimeDelta);
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack)
{
}

void CMonster::Animation_Event()
{
	auto& pCurEvents = m_pData->Get_CurrentEvents();
	for (auto& pEvent : pCurEvents)
	{
		_double CurPos = *(m_pAnimCom->Get_AnimPosition());
		_double Duration = *(m_pAnimCom->Get_AnimDuration());

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
				cout << "사운드 재생" << endl;
				break;
			case 3:
				cout << "이펙트 재생" << endl;
				break;
			}
		}

	}
}

string CMonster::Get_CurrentAnimationName()
{
	return m_pAnimCom->Get_AnimationName(m_pAnimCom->Get_CurrentAnimIndex());
}

void CMonster::Synchronize_Root(const _float& fTimeDelta)
{
	_vector vFF = XMVector3TransformNormal(XMVectorSetZ(XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove(m_pAnimCom)), 0), m_pTransformCom->Get_WorldMatrix());

	// m_pModelCom->Get_AnimChanged()  선형보간이 끝났는지
	// m_pModelCom->Get_AnimLerp() 선형보간이 필요한 애니메이션인지
	if (m_pAnimCom->Get_AnimChanged() || !m_pAnimCom->Get_AnimLerp(m_fChangeInterval))
	{
		if (m_pAnimCom->Get_AnimRestart(m_isAnimLoop))
		{
			XMStoreFloat4(&m_vPrevMove, XMVectorZero());
			m_fPrevSpeed = 0.f;
		}
		else
		{
			_float4 fMoveDir;
			_float fMoveSpeed = XMVectorGetX(XMVector3Length(vFF - XMLoadFloat4(&m_vPrevMove)));

			//Y값 이동을 죽인 방향으로 적용해야한다.
			XMStoreFloat4(&fMoveDir, XMVectorSetY(XMVector3Normalize(vFF - XMLoadFloat4(&m_vPrevMove)), 0.f));
			
			if (0.01 > m_fPrevSpeed)
				m_fPrevSpeed = 0.f;

			m_pTransformCom->Go_Straight_CustumSpeed(m_fPrevSpeed, 1);
			m_pTransformCom->Go_Move_Custum(fMoveDir, m_fPrevSpeed, 1);
			m_fPrevSpeed = fMoveSpeed;

			XMStoreFloat4(&m_vPrevMove, vFF);
		}
	}
	else
	{
		// 선형보간중일때는 무조건 초기화
		XMStoreFloat4(&m_vPrevMove, XMVectorZero());
	}

	XMStoreFloat4x4(&m_ModelWorldMatrix, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CMonster::Add_Components()
{
	
	return S_OK;
}

HRESULT CMonster::Bind_ResourceData()
{
	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pAnimCom);
	Safe_Release(m_pShaderCom);
}
