#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CNoteBase abstract :
    public CBase
{
public:
    /*
    *   0 UP
        1 DOWN
        2 LEFT
        3 RIGHT

        0 �Ϲݳ��
        1 ���� ���
        2 ��Ÿ ��� 
    */
    enum NOTE_SCORE
    {
        GREAT, GOOD, BAD, MISS, NONE
    };

protected:
    CNoteBase(_uint iNoteType, _uint iButtonType, _float fStartTime);
    virtual ~CNoteBase() = default;

public:
    // 0 �Ϲݳ��    1 ���� ���      2 ��Ÿ ���
    _uint Get_Type() {
        return m_iNoteType;
    }

    // 0 UP    1 DOWN     2 LEFT     3 RIGHT
    _uint Get_ButtonType() {
        return m_iButtonType;
    }

    _float Get_StartTime() {
        return m_fStartTime;
    }

    NOTE_SCORE Get_Score() {
        return m_eScore;
    }

public:
    void Set_Score(NOTE_SCORE eScore)
    {
        m_eScore = eScore;
    }

protected:
    CGameInstance* m_pGameInstance = { nullptr };

    _uint   m_iButtonType = { 0 };
    _uint   m_iNoteType = { 0 };                
    _float m_fStartTime = { 0.f };

    NOTE_SCORE   m_eScore = { NONE };           //NONE�� ���� ����ġ������

public:
    virtual void Free();
};
END
