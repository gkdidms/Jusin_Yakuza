#pragma once
#include "Quest.h"

/*
* [kill ����Ʈ]
* Ư�� Ÿ�ٸ� ����ؾ� �� �� �����ϴ� ����Ʈ.
* TargetIndex���� �־���� �Ѵ�.
*/

BEGIN(Client)
class CKillQuest :
    public CQuest
{
public:
    typedef struct tKillQuestDesc : public CQuest::QUEST_DESC {
        _int iTargetIndex;
    } KILL_QUEST_DESC ; 

private:
    CKillQuest();
    virtual ~CKillQuest() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual _bool Execute();

private:
    _int m_iTargetIndex = { -1 };

public:
    static CKillQuest* Create(void* pArg);
    virtual void Free();
};
END
