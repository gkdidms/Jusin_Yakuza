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
        FROM_PLAYER, FROM_ENEMY, TYPE_END
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
    
    /*
    * ���� �浹ó���� ���� �Լ�
    * ���ڷ� �ִ� ��ü�� m_BattleColliders ���� ���ҵ��� �浹�� ���Ѵ�.
    */
    _bool Collision_FromPlayer(class CLandObject* pObejct);
    _bool Collision_FromEnemy(class CLandObject* pObejct);

private:
    vector<class CLandObject*> m_ImpulseResolutionObjects;

    /*
    * FROM Ÿ���� �÷��̾��� �÷��̾��� ATTACK �ݶ��̴��� ��ȣ�ۿ��� ������ �����Ѵ�
    * ex) ������ HIT �ݶ��̴�(����, �ٸ� ��), �������� �ݶ��̴�
    * �÷��̾ �������� ����� ������̶�� Collision_FromPlayer() �Լ��� ���ڿ� �������� �־��ش�.
    * 
    * FROM Ÿ���� ���Ͷ�� ������ ATTACK �ݶ��̴��� ��ȣ�ۿ��� ������ �����Ѵ�.
    * ex) �÷��̾��� HIT �ݶ��̴� (����, �ٸ� ��)
    */
    vector<class CLandObject*> m_BattleObjects[TYPE_END];

private:
    void Impulse_Clear();
    void Battle_Clear();

public:
    virtual void Free();
};
END
