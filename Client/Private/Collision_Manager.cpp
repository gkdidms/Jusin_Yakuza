#include "Collision_Manager.h"
#include "LandObject.h"
#include "SocketCollider.h"
#include "Component_Manager.h"
#include "Player.h"
#include "Effect.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
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

HRESULT CCollision_Manager::Add_MapCollider(CCollider* pCollider)
{
    m_fIntersectDistance = 0;

    Safe_AddRef(pCollider);
    m_MapColliders.push_back(pCollider);

    return S_OK;
}

void CCollision_Manager::Tick()
{
    ImpulseResolution();

    Enemy_Hit_Collision();
    Player_Hit_Collision();

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

            _float fDistance = XMVectorGetX(XMVector3Length(vDistance));
            if (2.f > fDistance)
                m_ImpulseResolutionObjects[i]->ImpulseResolution(m_ImpulseResolutionObjects[j], 2.f);
        }
    }

    Impulse_Clear();
}

void CCollision_Manager::ResolveCollision(BoundingSphere* sphere, BoundingBox* box, CTransform* pTransform)
{
    // Box의 각 축별 회전 벡터를 구합니다.
    XMVECTOR boxCenter = XMLoadFloat3(&box->Center);
    XMVECTOR boxExtents = XMLoadFloat3(&box->Extents);
    //XMVECTOR boxOrientation = XMLoadFloat4(&box->Orientation);

    //XMVECTOR boxAxisX = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), boxOrientation);
    //XMVECTOR boxAxisY = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), boxOrientation);
    //XMVECTOR boxAxisZ = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), boxOrientation);

    // AABB의 각 축을 구합니다.
    XMVECTOR boxAxisX = XMVectorSet(1, 0, 0, 0);
    XMVECTOR boxAxisY = XMVectorSet(0, 1, 0, 0);
    XMVECTOR boxAxisZ = XMVectorSet(0, 0, 1, 0);

    // Sphere와 Box의 중심 간 벡터를 구합니다.
    XMVECTOR sphereCenter = XMLoadFloat3(&sphere->Center);
    XMVECTOR centerToCenter = XMVectorSubtract(sphereCenter, boxCenter);

    // 각 축별로 충돌 깊이를 계산합니다.
    float dotX = XMVectorGetX(XMVector3Dot(centerToCenter, boxAxisX));
    float dotY = XMVectorGetY(XMVector3Dot(centerToCenter, boxAxisY));
    float dotZ = XMVectorGetZ(XMVector3Dot(centerToCenter, boxAxisZ));

    float overlapX = sphere->Radius + XMVectorGetX(boxExtents) - fabs(dotX);
    float overlapY = sphere->Radius + XMVectorGetY(boxExtents) - fabs(dotY);
    float overlapZ = sphere->Radius + XMVectorGetZ(boxExtents) - fabs(dotZ);

    // 최소 충돌 깊이를 구합니다.
    float overlap = min(overlapX, min(overlapY, overlapZ));

    // 충돌 방향을 계산합니다.
    XMVECTOR collisionAxis;
    if (overlap == overlapX)
    {
        collisionAxis = boxAxisX;
        if (dotX < 0) collisionAxis = XMVectorNegate(collisionAxis);
    }
    else if (overlap == overlapY)
    {
        collisionAxis = boxAxisY;
        if (dotY < 0) collisionAxis = XMVectorNegate(collisionAxis);
    }
    else
    {
        collisionAxis = boxAxisZ;
        if (dotZ < 0) collisionAxis = XMVectorNegate(collisionAxis);
    }

    // Sphere를 밀어내는 벡터를 계산합니다.
    XMVECTOR pushVector = XMVectorScale(collisionAxis, overlap);

    // 충돌 해소: Sphere의 위치를 밀어냅니다.
    XMFLOAT3 push;
    XMStoreFloat3(&push, XMVectorZero());
    XMStoreFloat3(&push, pushVector);

    //sphere->Center.x += push.x;
    //sphere->Center.y += push.y;
    //sphere->Center.z += push.z;

    _vector vSpherePos = pTransform->Get_State(CTransform::STATE_POSITION);
    vSpherePos.m128_f32[0] += push.x;
    vSpherePos.m128_f32[1] += push.y;
    vSpherePos.m128_f32[2] += push.z;
    vSpherePos.m128_f32[3] = 1;

    pTransform->Set_State(CTransform::STATE_POSITION, vSpherePos);
}

// 플레이어한테 적이 맞을 때
void CCollision_Manager::Enemy_Hit_Collision()
{
    for (auto pPlayerAttackCollider : m_AttackColliders[PLAYER])
    {
        for (auto& pEnemyHitCollider : m_HitColliders[ENEMY])
        {
            if (pEnemyHitCollider->Intersect(pPlayerAttackCollider->Get_Collider()))
            {
                static_cast<CPlayer*>(pPlayerAttackCollider->Get_Parent())->AccHitGauge();

                CEffect::EFFECT_DESC EffectDesc;

                _matrix WorldMatrix = pPlayerAttackCollider->Get_TransformCom()->Get_WorldMatrix();
                
                _float4x4 matrix;
                XMStoreFloat4x4(&matrix, WorldMatrix);

                EffectDesc.pWorldMatrix = &matrix;

                // 플레이어 타격 이펙트 (== 몬스터 피격 이펙트)
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Glow0"), TEXT("Layer_Particle"), &EffectDesc);
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part0"), TEXT("Layer_Particle"), &EffectDesc);
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part2"), TEXT("Layer_Particle"), &EffectDesc);
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part3"), TEXT("Layer_Particle"), &EffectDesc);
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Hit1_Part4"), TEXT("Layer_Particle"), &EffectDesc);
               
 
                WorldMatrix = XMMatrixIdentity();
                WorldMatrix.r[3] = pPlayerAttackCollider->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

                XMStoreFloat4x4(&matrix, WorldMatrix);

                EffectDesc.pWorldMatrix = &matrix;
                //돈체크
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Money"), TEXT("Layer_Particle"), &EffectDesc);
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Coin"), TEXT("Layer_Particle"), &EffectDesc);

                pPlayerAttackCollider->ParentObject_Attack(pEnemyHitCollider);
                pEnemyHitCollider->ParentObject_Hit(pPlayerAttackCollider);
            }

        }
    }
}

// 적한테 플레이어가 맞을 때
void CCollision_Manager::Player_Hit_Collision()
{
    for (auto pEnemyAttackCollider : m_AttackColliders[ENEMY])
    {
        for (auto& pPlayerHitCollider : m_HitColliders[PLAYER])
        {
            if (pPlayerHitCollider->Intersect(pEnemyAttackCollider->Get_Collider()))
            {
                CEffect::EFFECT_DESC EffectDesc;

                _matrix WorldMatrix = pEnemyAttackCollider->Get_TransformCom()->Get_WorldMatrix(); //맞는 얘콜라이더

                _float4x4 matrix;
                XMStoreFloat4x4(&matrix, WorldMatrix);

                EffectDesc.pWorldMatrix = &matrix;

                // 몬스터 타격 이펙트 (== 플레이어 피격 이펙트)
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part0"), TEXT("Layer_Particle"), &EffectDesc);
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part1"), TEXT("Layer_Particle"), &EffectDesc);
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part2"), TEXT("Layer_Particle"), &EffectDesc);
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Part3"), TEXT("Layer_Particle"), &EffectDesc);
                m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Damage1_Glow0"), TEXT("Layer_Particle"), &EffectDesc);

                pEnemyAttackCollider->ParentObject_Attack(pPlayerHitCollider);
                pPlayerHitCollider->ParentObject_Hit(pEnemyAttackCollider);
            }
        }
    }
}

_bool CCollision_Manager::Map_Collision_Move(CCollider* pCollider, CTransform* pTransform)
{
    _float3 vCenter;
    XMStoreFloat3(&vCenter, XMVectorZero());

    for (auto& pMapCollider : m_MapColliders)
    {
        if (pMapCollider->Intersect(pCollider, 500.f))
        {
            switch (pMapCollider->Get_Type())
            {
            case CCollider::COLLIDER_AABB:
            {
                BoundingBox* pDesc = static_cast<BoundingBox*>(pMapCollider->Get_Desc());
                vCenter = pDesc->Center;

                ResolveCollision(static_cast<BoundingSphere*>(pCollider->Get_Desc()), pDesc, pTransform);

                return true;
            }


            case CCollider::COLLIDER_OBB:
            {
                BoundingOrientedBox* pDesc = static_cast<BoundingOrientedBox*>(pMapCollider->Get_Desc());
                vCenter = pDesc->Center;

                //ResolveCollision(static_cast<BoundingSphere*>(pCollider->Get_Desc()), pDesc, pTransform);

                return true;
            }


            case CCollider::COLLIDER_SPHERE:
            {
                BoundingSphere* pDesc = static_cast<BoundingSphere*>(pMapCollider->Get_Desc());
                vCenter = pDesc->Center;

                return true;
            }

            }
        }
    }

    return false;
}

_bool CCollision_Manager::Check_Map_Collision(CCollider* pCollider)
{
    _float3 vCenter;
    XMStoreFloat3(&vCenter, XMVectorZero());

    for (auto& pMapCollider : m_MapColliders)
    {
        if (pMapCollider->Intersect(pCollider, 500.f))
        {
            switch (pMapCollider->Get_Type())
            {
            case CCollider::COLLIDER_AABB:
            {
                BoundingBox* pDesc = static_cast<BoundingBox*>(pMapCollider->Get_Desc());
                vCenter = pDesc->Center;

                return true;
            }


            case CCollider::COLLIDER_OBB:
            {
                BoundingOrientedBox* pDesc = static_cast<BoundingOrientedBox*>(pMapCollider->Get_Desc());
                vCenter = pDesc->Center;

                return true;
            }


            case CCollider::COLLIDER_SPHERE:
            {
                BoundingSphere* pDesc = static_cast<BoundingSphere*>(pMapCollider->Get_Desc());
                vCenter = pDesc->Center;

                return true;
            }

            }
        }
    }

    return false;
}

CLandObject* CCollision_Manager::Get_Near_LandObject(CLandObject* pObject, vector<CGameObject*>& pObjects)
{
    // 5거리 이내에 있는 애들만 검색한다.
    _float fDinstance = { 5.f };
    CLandObject* pValue = { nullptr };

    for (auto& pTarget : pObjects)
    {
        if (pTarget == pObject) continue;
        if (pTarget->isObjectDead()) continue;

        _vector vBasePosition = pObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
        _vector vTargetPosition = pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

        if (XMVectorGetX(XMVector3Length(vBasePosition - vTargetPosition)) < fDinstance)
        {
            fDinstance = XMVectorGetX(XMVector3Length(vBasePosition - vTargetPosition));
            pValue = static_cast<CLandObject*>(pTarget);
        }
    }

    return pValue;
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
    Safe_Release(m_pGameInstance);

    Impulse_Clear();
    Battle_Clear();

    for (auto& pCollider : m_MapColliders)
    {
        Safe_Release(pCollider);
    }
    m_MapColliders.clear();
}