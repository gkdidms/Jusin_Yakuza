#include "QuestManager.h"

#include "Quest.h"
#include "KillQuest.h"
#include "MoveQuest.h"
#include "TalkQuest.h"

/*
[스토리라인]

1. 사체업자 애니메이션
2. 골목으로 시작함
3. 튜토리얼
4. 니시키 삐삐
    1. 삐삐 캡쳐
5. 니시키한테 가기
    1. 니시키한테 직접 골목으로 걸어다니면서 넣는다.
        1. 큰길안됨.
    2. 니시키한테 쿠제가 화가 낫다는 말
6. 가라오케로 감
7. 사체업자
8. 길거리
9. 총격전
10. 도지마조
*/

IMPLEMENT_SINGLETON(CQuestManager)

CQuestManager::CQuestManager()
{
}

/*
* 현재 퀘스트에 맞는지 확인 후 스크립트 인덱스 값을 넣어준다.
*/
const CScriptManager::SCRIPT_INFO CQuestManager::Get_ScriptInfo(_int iScriptCount)
{
    return m_pScriptManager->Get_Script(m_QuestInfo[m_iCurrentQuestIndex].iScriptIndex, iScriptCount);
}

HRESULT CQuestManager::Initialize()
{
    m_pScriptManager = CScriptManager::Create();
    if (nullptr == m_pScriptManager)
        return E_FAIL;

    m_iCurrentQuestIndex = 0;
    
    return S_OK;
}

HRESULT CQuestManager::Ready_Quest()
{
    _int iScriptIndex = 1;

    
    return S_OK;
}

_bool CQuestManager::Execute()
{
    if (true == m_pCurrentQuest->Execute())
    {
        // 퀘스트 완료했다면
        Safe_Release(m_pCurrentQuest);

        ++m_iCurrentQuestIndex;
        QUEST_INFO Info = m_QuestInfo[m_iCurrentQuestIndex];

        //새로운 퀘스트 부여하기
        if (Info.iType == QUEST_KILL)
            Add_KillQuest(Info.iQuestIndex, Info.iNextQuestIndex, Info.iTargetIndex, Info.iScriptIndex);
        else if (Info.iType == QUEST_MOVE)
            Add_MoveQuest(Info.iQuestIndex, Info.iNextQuestIndex, Info.iTriggerIndex, Info.iScriptIndex);
        else if (Info.iType == QUEST_TALK)
            Add_MoveQuest(Info.iQuestIndex, Info.iNextQuestIndex, Info.iObjectIndex, Info.iScriptIndex);
        else
            return false;

        return true;
    }

    return false;
}

/* Kill Quest 생성 함수*/
HRESULT CQuestManager::Add_KillQuest(_int iQuestIndex, _int iNextQuestIndex, _int iTargetIndex, _int iScriptIndex)
{
    CKillQuest::KILL_QUEST_DESC Desc{};
    Desc.iType = QUEST_KILL;
    Desc.iQuestIndex = iQuestIndex;
    Desc.iNextQuestIndex = iNextQuestIndex;
    Desc.iTargetIndex = iTargetIndex;

    CKillQuest* pKillQuest = CKillQuest::Create(&Desc);
    if (nullptr == pKillQuest)
        return E_FAIL;

    m_pCurrentQuest = pKillQuest;

    return S_OK;
}

/* Move Quest 생성 함수*/
HRESULT CQuestManager::Add_MoveQuest(_int iQuestIndex, _int iNextQuestIndex, _int iTriggerIndex, _int iScriptIndex)
{
    CMoveQuest::MOVE_QUEST_DESC Desc{};
    Desc.iType = QUEST_MOVE;
    Desc.iQuestIndex = iQuestIndex;
    Desc.iNextQuestIndex = iNextQuestIndex;
    Desc.iTriggerIndex = iTriggerIndex;

    CMoveQuest* pMoveQuest = CMoveQuest::Create(&Desc);
    if (nullptr == pMoveQuest)
        return E_FAIL;

    m_pCurrentQuest = pMoveQuest;

    return S_OK;
}

/* Talk Quest 생성 함수*/
HRESULT CQuestManager::Add_TalkQuest(_int iQuestIndex, _int iNextQuestIndex, _int iObjectIndex, _int iScriptIndex)
{
    CTalkQuest::TALK_QUEST_DESC Desc{};
    Desc.iType = QUEST_TALK;
    Desc.iQuestIndex = iQuestIndex;
    Desc.iNextQuestIndex = iNextQuestIndex;
    Desc.iObjectIndex = iObjectIndex;

    CTalkQuest* pTalkQuest = CTalkQuest::Create(&Desc);
    if (nullptr == pTalkQuest)
        return E_FAIL;

    m_pCurrentQuest = pTalkQuest;

    return S_OK;
}

void CQuestManager::Free()
{
    Safe_Release(m_pScriptManager);

    Safe_Release(m_pCurrentQuest);
}
