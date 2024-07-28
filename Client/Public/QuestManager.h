#pragma once
#include "Base.h"
#include "Client_Defines.h"

#include "ScriptManager.h"

BEGIN(Client)
class CQuestManager :
    public CBase
{
    DECLARE_SINGLETON(CQuestManager)

public:
    enum QUEST_TYPE { QUEST_KILL, QUEST_MOVE, QUEST_TALK, QUEST_END };
    typedef struct tQuestInfo {
        _uint iType;
        _uint iScriptIndex;

        _int iQuestIndex;
        _int iNextQuestIndex;
        _int iTargetIndex; //Kill
        _int iTriggerIndex; //Move
        _int iObjectIndex; // Talk
    } QUEST_INFO;

private:
    CQuestManager();
    virtual ~CQuestManager() = default;

public:
    const CScriptManager::SCRIPT_INFO Get_ScriptInfo(_int iScriptCount);

public:
    HRESULT Initialize();
    _bool Execute();

public:
    HRESULT Add_KillQuest(_int iQuestIndex, _int iNextQuestIndex, _int iTargetIndex, _int iScriptIndex);
    HRESULT Add_MoveQuest(_int iQuestIndex, _int iNextQuestIndex, _int iTriggerIndex, _int iScriptIndex);
    HRESULT Add_TalkQuest(_int iQuestIndex, _int iNextQuestIndex, _int iObjectIndex, _int iScriptIndex);

private:
    CScriptManager* m_pScriptManager = { nullptr };

    class CQuest* m_pCurrentQuest = { nullptr };

    vector<QUEST_INFO> m_QuestInfo;
    _int m_iCurrentQuestIndex = { -1 };

private:
    HRESULT Ready_Quest();

public:
    virtual void Free();
};

END

