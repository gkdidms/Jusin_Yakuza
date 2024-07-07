#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)
class CCollision_Manager :
    public CBase
{
    DECLARE_SINGLETON(CCollision_Manager)

private:
    CCollision_Manager();
    virtual ~CCollision_Manager() = default;

public:
    HRESULT Initialize();
    HRESULT Add_ImpulseResolution(class CLandObject* pObejct);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�
    HRESULT Add_HitCollider(class CLandObject* pObejct);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�
    HRESULT Add_AttackCollider(class CLandObject* pObejct);          // ���� �о�� �۾��� �� ��ü�� �߰����ִ� �Լ�

    void Tick();

private:
    // ĳ���Ͱ� ��ġ���ʰ� �о�� �Լ�
    void ImpulseResolution();           // ���� ��Ƶ� ���Ϳ��� ���� �о�� �۾��� �Ѵ� (Tick���� ����)
    void AttackCollision();


private:
    vector<class CLandObject*> m_ImpulseResolutionObjects;
    vector<class CLandObject*> m_AttackColliders;
    vector<class CLandObject*> m_HitColliders;

private:
    void Clear();

public:
    virtual void Free();
};
END
