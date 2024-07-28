#pragma once
#include "Quest.h"

/*
* [��ȭ ����Ʈ]
* Ư�� Ÿ���� ã�ư��� �� �� �����ϴ� ����Ʈ.
* ObjectIndex���� �־���� �Ѵ�.
*/

BEGIN(Client)
class CTalkQuest :
    public CQuest
{
public:
    typedef struct tTalkQuestDesc : public CQuest::QUEST_DESC {
        _int iObjectIndex;
    } TALK_QUEST_DESC;

public:
    void Set_Coll(_uint iObjectIndex, _bool isColl) { 
        if (iObjectIndex == m_iObjectIndex)
            m_isColl = isColl; 
    }

private:
    CTalkQuest();
    virtual ~CTalkQuest() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual _bool Execute();

private:
    _int m_iObjectIndex = { -1 };

    _bool m_isColl = { false };

public:
    static CTalkQuest* Create(void* pArg);
    virtual void Free();
};
END
