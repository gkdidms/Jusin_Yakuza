#include "Collision_Manager.h"
#include "EffectManager.h"

#include "LandObject.h"
#include "SocketCollider.h"
#include "Component_Manager.h"
#include "Player.h"
#include "Effect.h"
#include "GameInstance.h"
#include "Item.h"
#include "Monster.h"


IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() },
    m_pEffectManager{ CEffectManager::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pEffectManager);
}

HRESULT CCollision_Manager::Initialize()
{
    return S_OK;
}

HRESULT CCollision_Manager::Add_ImpulseResolution(CGameObject* pObejct)
{
    //Safe_AddRef(pObejct);
    m_ImpulseResolutionObjects.push_back(pObejct);

    return S_OK;
}

HRESULT CCollision_Manager::Add_AttackCollider(CSocketCollider* pCollider, COLLIDER_TYPE eType)
{
    //Safe_AddRef(pCollider);
    m_AttackColliders[eType].push_back(pCollider);

    return S_OK;
}

HRESULT CCollision_Manager::Add_HitCollider(CSocketCollider* pCollider, COLLIDER_TYPE eType)
{
    //Safe_AddRef(pCollider);
    m_HitColliders[eType].push_back(pCollider);

    return S_OK;
}

HRESULT CCollision_Manager::Add_ItemCollider(CCollider* pCollider)
{
    //Safe_AddRef(pCollider);
    m_ItemColliders.clear();
    m_ItemColliders.push_back(pCollider);

    return S_OK;
}

HRESULT CCollision_Manager::Add_MapCollider(CCollider* pCollider)
{
    m_fIntersectDistance = 0;

    Safe_AddRef(pCollider);
    m_MapColliders.push_back(pCollider);

    return S_OK;
}

void CCollision_Manager::Clear_MapCollider()
{
    for (auto& pCollider : m_MapColliders)
    {
        Safe_Release(pCollider);
    }
    m_MapColliders.clear();
}


void CCollision_Manager::Tick()
{
    ImpulseResolution();

    Enemy_Hit_Collision();
    Player_Hit_Collision();
    Item_Attack_Collision();    

    Battle_Clear();
}

void CCollision_Manager::All_Clear()
{
    Impulse_Clear();
    Battle_Clear();
    Item_Clear();
}

void CCollision_Manager::ImpulseResolution()
{
    // ���⿡ �浹ó�� �ڵ� ����
    for (size_t i = 0; i < m_ImpulseResolutionObjects.size(); i++)
    {
        for (size_t j = 0; j < m_ImpulseResolutionObjects.size(); j++)
        {
            if (i == j) continue;

            _vector vPosition_Object_I = m_ImpulseResolutionObjects[i]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
            _vector vPosition_Object_J = m_ImpulseResolutionObjects[j]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

            // �� ��ü���� �Ÿ��� 1 �̸��� ��쿡�� �о�� �Լ� ����
            _vector vDistance = vPosition_Object_I - vPosition_Object_J;

            _float fDistance = XMVectorGetX(XMVector3Length(vDistance));
            if (1.f > fDistance)
                m_ImpulseResolutionObjects[i]->ImpulseResolution(m_ImpulseResolutionObjects[j], 2.f);
        }
    }

    Impulse_Clear();
}

void CCollision_Manager::ResolveCollision(BoundingSphere* sphere, BoundingBox* box, CTransform* pTransform)
{
    // Box�� �� �ະ ȸ�� ���͸� ���մϴ�.
    XMVECTOR boxCenter = XMLoadFloat3(&box->Center);
    XMVECTOR boxExtents = XMLoadFloat3(&box->Extents);
    //XMVECTOR boxOrientation = XMLoadFloat4(&box->Orientation);

    //XMVECTOR boxAxisX = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), boxOrientation);
    //XMVECTOR boxAxisY = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), boxOrientation);
    //XMVECTOR boxAxisZ = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), boxOrientation);

    // AABB�� �� ���� ���մϴ�.
    XMVECTOR boxAxisX = XMVectorSet(1, 0, 0, 0);
    XMVECTOR boxAxisY = XMVectorSet(0, 1, 0, 0);
    XMVECTOR boxAxisZ = XMVectorSet(0, 0, 1, 0);

    // Sphere�� Box�� �߽� �� ���͸� ���մϴ�.
    XMVECTOR sphereCenter = XMLoadFloat3(&sphere->Center);
    XMVECTOR centerToCenter = XMVectorSubtract(sphereCenter, boxCenter);

    // �� �ະ�� �浹 ���̸� ����մϴ�.
    float dotX = XMVectorGetX(XMVector3Dot(centerToCenter, boxAxisX));
    float dotY = XMVectorGetY(XMVector3Dot(centerToCenter, boxAxisY));
    float dotZ = XMVectorGetZ(XMVector3Dot(centerToCenter, boxAxisZ));

    float overlapX = sphere->Radius + XMVectorGetX(boxExtents) - fabs(dotX);
    float overlapY = sphere->Radius + XMVectorGetY(boxExtents) - fabs(dotY);
    float overlapZ = sphere->Radius + XMVectorGetZ(boxExtents) - fabs(dotZ);

    // �ּ� �浹 ���̸� ���մϴ�.
    float overlap = min(overlapX, min(overlapY, overlapZ));

    // �浹 ������ ����մϴ�.
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

    // Sphere�� �о�� ���͸� ����մϴ�.
    XMVECTOR pushVector = XMVectorScale(collisionAxis, overlap);

    // �浹 �ؼ�: Sphere�� ��ġ�� �о���ϴ�.
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

XMVECTOR CCollision_Manager::Find_Collision_Position(BoundingSphere* sphere, BoundingBox* box, CTransform* pTransform)
{

    // Box�� �� �ະ ȸ�� ���͸� ���մϴ�.
    XMVECTOR boxCenter = XMLoadFloat3(&box->Center);
    XMVECTOR boxExtents = XMLoadFloat3(&box->Extents);
    //XMVECTOR boxOrientation = XMLoadFloat4(&box->Orientation);

    //XMVECTOR boxAxisX = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), boxOrientation);
    //XMVECTOR boxAxisY = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), boxOrientation);
    //XMVECTOR boxAxisZ = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), boxOrientation);

    // AABB�� �� ���� ���մϴ�.
    XMVECTOR boxAxisX = XMVectorSet(1, 0, 0, 0);
    XMVECTOR boxAxisY = XMVectorSet(0, 1, 0, 0);
    XMVECTOR boxAxisZ = XMVectorSet(0, 0, 1, 0);

    // Sphere�� Box�� �߽� �� ���͸� ���մϴ�.
    XMVECTOR sphereCenter = XMLoadFloat3(&sphere->Center);
    XMVECTOR centerToCenter = XMVectorSubtract(sphereCenter, boxCenter);

    // �� �ະ�� �浹 ���̸� ����մϴ�.
    float dotX = XMVectorGetX(XMVector3Dot(centerToCenter, boxAxisX));
    float dotY = XMVectorGetY(XMVector3Dot(centerToCenter, boxAxisY));
    float dotZ = XMVectorGetZ(XMVector3Dot(centerToCenter, boxAxisZ));

    float overlapX = sphere->Radius + XMVectorGetX(boxExtents) - fabs(dotX);
    float overlapY = sphere->Radius + XMVectorGetY(boxExtents) - fabs(dotY);
    float overlapZ = sphere->Radius + XMVectorGetZ(boxExtents) - fabs(dotZ);

    // �ּ� �浹 ���̸� ���մϴ�.
    float overlap = min(overlapX, min(overlapY, overlapZ));

    // �浹 ������ ����մϴ�.
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

    // Sphere�� �о�� ���͸� ����մϴ�.
    XMVECTOR pushVector = XMVectorScale(collisionAxis, overlap);

    // �浹 �ؼ�: Sphere�� ��ġ�� �о���ϴ�.
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

    // �о ��ġ�� ��ȯ
    return      vSpherePos;
}

bool CCollision_Manager::Check_PositionAABB_Collision(BoundingSphere* sphere, BoundingBox* box, XMVECTOR vPosition, XMVECTOR& pCollisionPos)
{
    XMVECTOR boxCenter = XMLoadFloat3(&box->Center);
    XMVECTOR boxExtents = XMLoadFloat3(&box->Extents);

    // AABB�� �� ���� ���մϴ�.
    XMVECTOR boxAxisX = XMVectorSet(1, 0, 0, 0);
    XMVECTOR boxAxisY = XMVectorSet(0, 1, 0, 0);
    XMVECTOR boxAxisZ = XMVectorSet(0, 0, 1, 0);

    // Sphere�� Box�� �߽� �� ���͸� ���մϴ�.
    XMVECTOR sphereCenter = vPosition;
    XMVECTOR centerToCenter = XMVectorSubtract(sphereCenter, boxCenter);

    // �� �ະ�� �浹 ���̸� ����մϴ�.
    float dotX = XMVectorGetX(XMVector3Dot(centerToCenter, boxAxisX));
    float dotY = XMVectorGetY(XMVector3Dot(centerToCenter, boxAxisY));
    float dotZ = XMVectorGetZ(XMVector3Dot(centerToCenter, boxAxisZ));


    // Radius �������� ���ؼ�
    float overlapX = sphere->Radius + XMVectorGetX(boxExtents) - fabs(dotX);
    float overlapY = sphere->Radius + XMVectorGetY(boxExtents) - fabs(dotY);
    float overlapZ = sphere->Radius + XMVectorGetZ(boxExtents) - fabs(dotZ);

    // �ּ� �浹 ���̸� ���մϴ�.
    float overlap = min(overlapX, min(overlapY, overlapZ));


    // �浹�� �߻����� �ʴ� ���
    if (overlap <= 0) {
        return false;
    }


    // �浹 ������ ����մϴ�.
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

    // Sphere�� �о�� ���͸� ����մϴ�.
    XMVECTOR pushVector = XMVectorScale(collisionAxis, overlap);

    // �浹 �ؼ�: Sphere�� ��ġ�� �о���ϴ�.
    XMFLOAT3 push;
    XMStoreFloat3(&push, XMVectorZero());
    XMStoreFloat3(&push, pushVector);

    //sphere->Center.x += push.x;
    //sphere->Center.y += push.y;
    //sphere->Center.z += push.z;

    _vector vSpherePos = vPosition;
    vSpherePos.m128_f32[0] += push.x;
    vSpherePos.m128_f32[1] += push.y;
    vSpherePos.m128_f32[2] += push.z;
    vSpherePos.m128_f32[3] = 1;

    pCollisionPos = vSpherePos;


    return true;
}

// �÷��̾����� ���� ���� ��
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

                _matrix WorldMatrix = XMLoadFloat4x4(pPlayerAttackCollider->Get_PartWorldMatrix());
                
                _float4x4 matrix;
                XMStoreFloat4x4(&matrix, WorldMatrix);
                EffectDesc.pWorldMatrix = &matrix;
                m_pEffectManager->Player_Attack_Effect(EffectDesc);

                
                _matrix MoneyMatrix = XMMatrixIdentity();
                MoneyMatrix.r[3] = WorldMatrix.r[3];
                XMStoreFloat4x4(&matrix, MoneyMatrix);
                EffectDesc.pWorldMatrix = &matrix;

                //��üũ
                //m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Mesh_Money"), TEXT("Layer_Particle"), &EffectDesc);
                //m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Particle_Point_Coin"), TEXT("Layer_Particle"), &EffectDesc);

                pEnemyHitCollider->ParentObject_Hit(pPlayerAttackCollider);
                pPlayerAttackCollider->ParentObject_Attack(pEnemyHitCollider);
            }

        }
    }
}

// ������ �÷��̾ ���� ��
void CCollision_Manager::Player_Hit_Collision()
{
    for (auto pEnemyAttackCollider : m_AttackColliders[ENEMY])
    {
        for (auto& pPlayerHitCollider : m_HitColliders[PLAYER])
        {
            if (pPlayerHitCollider->Intersect(pEnemyAttackCollider->Get_Collider()))
            {
                CEffect::EFFECT_DESC EffectDesc;

                _matrix WorldMatrix = XMLoadFloat4x4(pEnemyAttackCollider->Get_PartWorldMatrix()); //�´� ���ݶ��̴�

                _float4x4 matrix;
                XMStoreFloat4x4(&matrix, WorldMatrix);

                EffectDesc.pWorldMatrix = &matrix;

                m_pEffectManager->Enemy_Attack_Effect(EffectDesc);


                pPlayerHitCollider->ParentObject_Hit(pEnemyAttackCollider);
                pEnemyAttackCollider->ParentObject_Attack(pPlayerHitCollider);
            }
        }
    }
}

void CCollision_Manager::Item_Attack_Collision()
{
    for (auto& pItemCollider : m_ItemColliders)
    {
        for (auto& pEnemyHitCollider : m_HitColliders[ENEMY])
        {
            if (pItemCollider->Intersect(pEnemyHitCollider->Get_Collider(), 1.f))
            {
                // �������� �����ϰ� �������� �����ǰ��� ���� (�����ʿ��� �Ⱦ��°���)
                pEnemyHitCollider->Get_Parent()->Take_Damage(0, _float3(), 10.f, nullptr, false);
                dynamic_cast<CMonster*>(pEnemyHitCollider->Get_Parent())->Set_isColl(true);
            }
        }
    }

}

void CCollision_Manager::ItemCollision()
{
    int iCurLevel = m_pGameInstance->Get_CurrentLevel();
    vector<CGameObject*>  vItems = m_pGameInstance->Get_GameObjects(iCurLevel, TEXT("Layer_Item"));

    for (auto& item : vItems)
    {
        for (auto pPlayerAttackCollider : m_AttackColliders[PLAYER])
        {
            CItem::ITEM_MODE itemMode = dynamic_cast<CItem*>(item)->Get_ItemMode();

            if (itemMode == CItem::ITEM_BRIGHT)
            {
                CCollider* pItemCollider = dynamic_cast<CItem*>(item)->Get_AABBCollider();

                if (pItemCollider->Intersect(pPlayerAttackCollider->Get_Collider()))
                {
                    // ������� ó�� �Լ� 
                    CPlayer* pPlayer = dynamic_cast<CPlayer*>(pPlayerAttackCollider->Get_Parent());
                    CModel* pModel = dynamic_cast<CModel*>(pPlayer->Get_Component(TEXT("Com_Model")));

                    dynamic_cast<CItem*>(item)->Set_ParentMatrix(pModel->Get_BoneCombinedTransformationMatrix("buki_r_n"));
                    dynamic_cast<CItem*>(item)->Set_Grab(true);

                    pPlayerAttackCollider->ParentObject_Attack(item, true);
                }
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

_bool CCollision_Manager::Check_Map_Collision(CCollider* pCollider, XMVECTOR& pCollisionPos, CTransform* pTransform)
{
    _float3 vCenter;
    XMStoreFloat3(&vCenter, XMVectorZero());

    for (auto& pMapCollider : m_MapColliders)
    {
        BoundingBox* pDesc = static_cast<BoundingBox*>(pMapCollider->Get_Desc());

        _float3 vExtents = pDesc->Extents;

        _float  fMax = vExtents.x;

        if (fMax < vExtents.y)
            fMax = vExtents.y;

        if (fMax < vExtents.z)
            fMax = vExtents.z;


        if (pMapCollider->Intersect(pCollider, fMax * 2))
        {
            switch (pMapCollider->Get_Type())
            {
            case CCollider::COLLIDER_AABB:
            {
                BoundingBox* pDesc = static_cast<BoundingBox*>(pMapCollider->Get_Desc());
                vCenter = pDesc->Center;

                // �浹�ϰ� �о ��ġ�� ��ȯ
                pCollisionPos = Find_Collision_Position(static_cast<BoundingSphere*>(pCollider->Get_Desc()), pDesc, pTransform);

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

_bool CCollision_Manager::Check_Map_Collision_Using_Position(CCollider* pCollider, XMVECTOR vPosition, XMVECTOR& pCollisionPos)
{
    _float3 vCenter;
    XMStoreFloat3(&vCenter, XMVectorZero());

    for (auto& pMapCollider : m_MapColliders)
    {
        if (Check_PositionAABB_Collision(static_cast<BoundingSphere*>(pCollider->Get_OriginDesc()), static_cast<BoundingBox*>(pMapCollider->Get_Desc()), vPosition, pCollisionPos))
        {
            return true;
        }
    }

    return false;
}

CGameObject* CCollision_Manager::Get_Near_Object(CGameObject* pObject, vector<CGameObject*>& pObjects, _float fDistance)
{
    // 5�Ÿ� �̳��� �ִ� �ֵ鸸 �˻��Ѵ�.
    CLandObject* pValue = { nullptr };

    _float fMinDistance = 99999.f;

    for (auto& pTarget : pObjects)
    {
        if (pTarget == pObject) continue;
        if (pTarget->isObjectDead()) continue;
        if (pTarget->isDead()) continue;

        _vector vBasePosition = pObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
        _vector vTargetPosition = pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

        if (XMVectorGetX(XMVector3Length(vBasePosition - vTargetPosition)) < fDistance && XMVectorGetX(XMVector3Length(vBasePosition - vTargetPosition)) < fMinDistance)
        {
            fMinDistance = XMVectorGetX(XMVector3Length(vBasePosition - vTargetPosition));
            pValue = static_cast<CLandObject*>(pTarget);
        }
    }

    return pValue;
}

CCollider* CCollision_Manager::Get_Near_Collider(CGameObject* pObject, vector<CCollider*>& pColliders, _float fDistance)
{
    // 5�Ÿ� �̳��� �ִ� �ֵ鸸 �˻��Ѵ�.
    CCollider* pValue = { nullptr };

    _float fMinDistance = 99999.f;

    for (auto& pTarget : pColliders)
    {
        _vector vBasePosition = pObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
        _float3 vTargetCenter = pTarget->Get_Center();

        if (XMVectorGetX(XMVector3Length(vBasePosition - XMLoadFloat3(&vTargetCenter))) < fDistance && XMVectorGetX(XMVector3Length(vBasePosition - XMLoadFloat3(&vTargetCenter))) < fMinDistance)
        {
            fMinDistance = XMVectorGetX(XMVector3Length(vBasePosition - XMLoadFloat3(&vTargetCenter)));
            pValue = pTarget;
        }
    }

    return pValue;
}

void CCollision_Manager::Impulse_Clear()
{
    //for (auto& pObject : m_ImpulseResolutionObjects)
    //    Safe_Release(pObject);

    m_ImpulseResolutionObjects.clear();
}

void CCollision_Manager::Battle_Clear()
{
    for (size_t i = 0; i < TYPE_END; i++)
    {
        //for (auto& pObject : m_AttackColliders[i])
        //    Safe_Release(pObject);

        //for (auto& pObject : m_HitColliders[i])
        //    Safe_Release(pObject);

        m_AttackColliders[i].clear();
        m_HitColliders[i].clear();
    }
}

void CCollision_Manager::Item_Clear()
{
    for (auto& pCollider : m_ItemColliders)
    {
        Safe_Release(pCollider);
    }
    m_ItemColliders.clear();
}

void CCollision_Manager::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pEffectManager);

    Impulse_Clear();
    Battle_Clear();

    for (auto& pCollider : m_MapColliders)
    {
        Safe_Release(pCollider);
    }
    m_MapColliders.clear();

    Item_Clear();
}