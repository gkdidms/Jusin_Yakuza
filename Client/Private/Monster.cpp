#include "Monster.h"

#include "GameInstance.h"

#include "CharacterData.h"
#include "SocketCollider.h"
#include "SocketEffect.h"

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

void CMonster::Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, _bool isBlowAttack)
{
	// ��Ʈ�� ����, ������ �Ѱܹ޾Ƽ� �ʿ��� �ڵ� �ۼ�
	// iHitColliderType �� �浹�� HITŸ�� �ݶ��̴��� ���, �ٵ�, ���������� �����ִ�.
	// isBlowAttack �� �ǴϽú�ο�(�Ѿ�߸���)���� ���θ� �Ѱܹ���
}

void CMonster::Synchronize_Root(const _float& fTimeDelta)
{
	_vector vFF = XMVector3TransformNormal(XMVectorSetZ(XMLoadFloat3(m_pModelCom->Get_AnimationCenterMove(m_pAnimCom)), 0), m_pTransformCom->Get_WorldMatrix());

	// m_pModelCom->Get_AnimChanged()  ���������� ��������
	// m_pModelCom->Get_AnimLerp() ���������� �ʿ��� �ִϸ��̼�����
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

			//Y�� �̵��� ���� �������� �����ؾ��Ѵ�.
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
		// �����������϶��� ������ �ʱ�ȭ
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
