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
    
private:
    CTalkQuest();
    virtual ~CTalkQuest() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual _bool Execute();

private:
    _int m_iObjectIndex = { -1 };

public:
    static CTalkQuest* Create(void* pArg);
    virtual void Free();
};
END
