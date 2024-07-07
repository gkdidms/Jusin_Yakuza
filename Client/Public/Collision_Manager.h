#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CCollision_Manager :
    public CBase
{
    DECLARE_SINGLETON(CCollision_Manager)

public:
    enum COLLIDER_TYPE
    {
        ATTACK, HIT, TYPE_END
    };

private:
    CCollision_Manager();
    virtual ~CCollision_Manager() = default;

public:
    HRESULT Initialize();
    HRESULT Add_ImpulseResolution(class CLandObject* pObejct);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�
    HRESULT Add_BattleCollider(class CLandObject* pObejct);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�

    void Tick();

private:
    // ĳ���Ͱ� ��ġ���ʰ� �о�� �Լ�
    // AABB������ �����Ǿ�����
    void ImpulseResolution();           // ���� ��Ƶ� ���Ϳ��� ���� �о�� �۾��� �Ѵ� (Tick���� ����)
    
    void BattleCollision();             // ���������� �浹ó��


private:
    vector<class CLandObject*> m_ImpulseResolutionObjects;
    vector<class CCollider*> m_BattleColliders[TYPE_END];

private:
    void Impulse_Clear();
    void Battle_Clear();

public:
    virtual void Free();
};
END
