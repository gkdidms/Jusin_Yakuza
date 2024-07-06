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
    HRESULT Add_ImpulseResolution(class CLandObject* pObejct);          // 서로 밀어내는 작업을 할 객체를 추가해주는 함수

    void Tick();

private:
    void ImpulseResolution();           // 따로 모아둔 벡터에서 서로 밀어내는 작업을 한다 (Tick에서 돌림)

private:
    vector<class CLandObject*> m_ImpulseResolutionObjects;

private:
    void Clear();

public:
    virtual void Free();
};
END
