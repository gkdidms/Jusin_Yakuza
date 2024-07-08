#include "Collision_Manager.h"
#include "LandObject.h"
#include "SocketCollider.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Initialize()
{
    return S_OK;
}

HRESULT CCollision_Manager::Add_ImpulseResolution(CLandObject* pObejct)
{
    Safe_AddRef(pObejct);
    m_ImpulseResolutionObjects.push_back(pObejct);

    return S_OK;
}

HRESULT CCollision_Manager::Add_AttackCollider(CSocketCollider* pCollider, COLLIDER_TYPE eType)
{
    Safe_AddRef(pCollider);
    m_AttackColliders[eType].push_back(pCollider);

    return S_OK;
}

HRESULT CCollision_Manager::Add_HitCollider(CSocketCollider* pCollider, COLLIDER_TYPE eType)
{
    Safe_AddRef(pCollider);
    m_HitColliders[eType].push_back(pCollider);

    return S_OK;
}

void CCollision_Manager::Tick()
{
    ImpulseResolution();
    Collision_FromPlayer();
    Collision_FromEnemy();
    Battle_Clear();
}

void CCollision_Manager::ImpulseResolution()
{
    // 여기에 충돌처리 코드 구현
    for (size_t i = 0; i < m_ImpulseResolutionObjects.size(); i++)
    {
        for (size_t j = 0; j < m_ImpulseResolutionObjects.size(); j++)
        {
            if (i >= j) continue;

            _vector vPosition_Object_I = m_ImpulseResolutionObjects[i]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
            _vector vPosition_Object_J = m_ImpulseResolutionObjects[j]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

            // 각 객체간의 거리가 1 미만인 경우에만 밀어내기 함수 실행
            _vector vDistance = vPosition_Object_I - vPosition_Object_J;
            if (0.5f > XMVectorGetX(XMVector3Length(vDistance)))
                m_ImpulseResolutionObjects[i]->ImpulseResolution(m_ImpulseResolutionObjects[j]);
        }
    }

        Impulse_Clear();
}
 
void CCollision_Manager::Collision_FromPlayer()
{
    for (auto pPlayerAttackCollider : m_AttackColliders[PLAYER])
    {
        for (auto& pEnemyHitCollider : m_HitColliders[ENEMY])
        {
            if (pEnemyHitCollider->Intersect(pPlayerAttackCollider->Get_Collider()))
            {
                cout << "Enemy Hit!!!" << endl;
            }

        }
    }
}

void CCollision_Manager::Collision_FromEnemy()
{
    for (auto pEnemyAttackCollider : m_AttackColliders[ENEMY])
    {
        for (auto& pPlayerHitCollider : m_HitColliders[PLAYER])
        {
            if (pPlayerHitCollider->Intersect(pEnemyAttackCollider->Get_Collider()))
            {
                cout << "Player Hit!!!!" << endl;
            }
        }
    }
}

void CCollision_Manager::Impulse_Clear()
{
    for (auto& pObject : m_ImpulseResolutionObjects)
        Safe_Release(pObject);

    m_ImpulseResolutionObjects.clear();
}

void CCollision_Manager::Battle_Clear()
{
    for (size_t i = 0; i < TYPE_END; i++)
    {
        for (auto& pObject : m_AttackColliders[i])
            Safe_Release(pObject);

        for (auto& pObject : m_HitColliders[i])
            Safe_Release(pObject);

        m_AttackColliders[i].clear();
        m_HitColliders[i].clear();
    }
}

void CCollision_Manager::Free()
{
    Impulse_Clear();
    Battle_Clear();
}
