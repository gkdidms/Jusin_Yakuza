#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Effect.h"
BEGIN(Engine)
class CCollider;
class CGameObject;
class CGameInstance;
END

BEGIN(Client)
class CCollision_Manager :
    public CBase
{
    DECLARE_SINGLETON(CCollision_Manager)

public:
    // �����ݶ��̴��� ���� (�������� �����ݶ��̴��� ������� ����)
    enum COLLIDER_TYPE
    {
        PLAYER, ENEMY, TYPE_END
    };

private:
    CCollision_Manager();
    virtual ~CCollision_Manager() = default;

public:
    HRESULT Initialize();
    HRESULT Add_ImpulseResolution(CGameObject* pObejct);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�
    HRESULT Add_AttackCollider(class CSocketCollider* pCollider, COLLIDER_TYPE eType);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�
    HRESULT Add_HitCollider(class CSocketCollider* pCollider, COLLIDER_TYPE eType);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�
    HRESULT Add_ItemCollider(CCollider* pCollider);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�

    HRESULT Add_MapCollider(CCollider* pCollider);
    void    Clear_MapCollider();

    void Tick();

public:
    void All_Clear();

public:
    /*
    * ���� �浹ó���� ���� �Լ�

    * ���ڷ� �ִ� ��ü�� m_HitColliders ���� ���ҵ��� �浹�� ���Ѵ�.
    */
    void Enemy_Hit_Collision();
    void Player_Hit_Collision();
    void Item_Attack_Collision();           // �����ۿ� ���Ͱ� �¾Ҵ���?
    void ItemCollision();

    // ī�޶� - �� �浹 ����
    _bool Map_Collision_Move(CCollider* pCollider, CTransform* pTransform); // �浹 �� position �ٽ� ���α�
    _bool Check_Map_Collision(CCollider* pCollider, XMVECTOR& pCollisionPos, CTransform* pTransform); // �浹 Ȯ�ο� - transform���� Ȯ��
    _bool Check_Map_Collision_Using_Position(CCollider* pCollider, XMVECTOR vPosition, XMVECTOR& pCollisionPos); //�浹 Ȯ�ο� - position(vector)�� Ȯ��

    CGameObject* Get_Near_Object(CGameObject* pObject, vector<CGameObject*>& pObjects, _float fDistance = 5.f);
    CCollider* Get_Near_Collider(CGameObject* pObject, vector<CCollider*>& pObjects, _float fDistance = 5.f);

private:
    // ĳ���Ͱ� ��ġ���ʰ� �о�� �Լ�
    // AABB������ �����Ǿ�����
    void ImpulseResolution();           // ���� ��Ƶ� ���Ϳ��� ���� �о�� �۾��� �Ѵ� (Tick���� ����)

    void ResolveCollision(BoundingSphere* sphere, BoundingBox* box, CTransform* pTransform);

    XMVECTOR Find_Collision_Position(BoundingSphere* sphere, BoundingBox* box, CTransform* pTransform);


    // Radius �������� ���ؼ� sphere ������
    // transform�� position���� �浹 Ȯ��
    bool Check_PositionAABB_Collision(BoundingSphere* sphere, BoundingBox* box, XMVECTOR vPosition, XMVECTOR& pCollisionPos);

private:
    vector<CGameObject*> m_ImpulseResolutionObjects;

    /*
    * AttackColliders�� AttackŸ���� ���� �ݶ��̴����� ��Ƶ� ����
    * m_AttackColliders[PLAYER] ��� �÷��̾��� AttackŸ���� �ݶ��̴� ��, ���� �����ִ� �ֵ鸸 ��Ƶ�
    * m_AttackColliders[ENEMY] ��� ������ Attack Ÿ���� �ݶ��̴� �� ���� ƽ���� �����ִ� �ֵ鸸 ��Ƶ�
    * 
    * HitColliders�� HitŸ���� ���� �ݶ��̴����� ��Ƶ� ����
    * m_HitColliders[PLAYER] ��� �÷��̾��� Hit�ݶ��̴��� ��Ƶ� ���̸�, m_AttackColliders[ENEMY]�� �浹ó�� �Ѵ�.
    * m_HitColliders[ENEMY] ��� �÷��̾��� Hit�ݶ��̴��� ��Ƶ� ���̸�, m_AttackColliders[PLAYER]�� �浹ó�� �Ѵ�.
    */
    vector<class CSocketCollider*> m_AttackColliders[TYPE_END];
    vector<class CSocketCollider*> m_HitColliders[TYPE_END];


    vector<CCollider*> m_MapColliders;
    vector<CCollider*> m_ItemColliders;

    CGameInstance* m_pGameInstance = { nullptr };
    class CEffectManager* m_pEffectManager = { nullptr };


    float           m_fIntersectDistance = { 0 };

private:
    void Impulse_Clear();
    void Battle_Clear();
    void Item_Clear();

public:
    virtual void Free();
};
END
