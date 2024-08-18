#pragma once
#include "Quest.h"

/*
* [��ȭ ����Ʈ]
* Ư�� Ÿ���� ã�ư��� �� �� �����ϴ� ����Ʈ.
* ObjectIndex���� �־���� �Ѵ�.
*/

BEGIN(Client)
class CKaraokeQuest :
    public CQuest
{
public:
    typedef struct tKaraokeQuestDesc : public CQuest::QUEST_DESC {
    } KARAOKE_QUEST_DESC;
    
private:
    CKaraokeQuest();
    virtual ~CKaraokeQuest() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual _bool Execute();

public:
    static CKaraokeQuest* Create(void* pArg);
    virtual void Free();
};
END
