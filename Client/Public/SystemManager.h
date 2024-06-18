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
    LEVEL Get_Level() { return m_iCurrentLevel; }

public:
    void Set_Level(LEVEL iLevel) { m_iCurrentLevel = iLevel; }

private:
    LEVEL m_iCurrentLevel = { LEVEL_END };

public:
    virtual void Free();
};
END
