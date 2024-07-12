#pragma once
#include "Base.h"

#include "Client_Defines.h"

/*
Client �ý��ۿ� ���������� �ʿ��� �������� �����Ѵ� (ex. ���� ����)
*/

BEGIN(Client)
class CSystemManager :
    public CBase
{
    DECLARE_SINGLETON(CSystemManager)

private:
    CSystemManager();
    virtual ~CSystemManager() = default;

public:
    void Set_Camera(CAMERA iCamNum) { m_iCamera = iCamNum; }
    void Set_ShadowViewPos(_vector vShadowPos) { m_vShadowViewPos = vShadowPos; }
    
public:
    CAMERA Get_Camera() { return m_iCamera; }
    _vector Get_ShadowViewPos() { return m_vShadowViewPos; }

private:
    LEVEL                       m_iCurrentLevel = { LEVEL_END };
    CAMERA                      m_iCamera = { CAMERA_END };

    _vector m_vShadowViewPos = { XMVectorSet(0.f, -0.7f, 0.1f, 1.f) };

public:
    virtual void Free();
};
END
