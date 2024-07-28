#include "QuestManager.h"

#include "Quest.h"
#include "KillQuest.h"
#include "MoveQuest.h"
#include "TalkQuest.h"

/*
[���丮����]

1. ��ü���� �ִϸ��̼�
2. ������� ������
3. Ʃ�丮��
4. �Ͻ�Ű �߻�
    1. �߻� ĸ��
5. �Ͻ�Ű���� ����
    1. �Ͻ�Ű���� ���� ������� �ɾ�ٴϸ鼭 �ִ´�.
        1. ū��ȵ�.
    2. �Ͻ�Ű���� ������ ȭ�� ���ٴ� ��
6. ������ɷ� ��
7. ��ü����
8. ��Ÿ�
9. �Ѱ���
10. ��������
*/

IMPLEMENT_SINGLETON(CQuestManager)

CQuestManager::CQuestManager()
{
}

/*
* ���� ����Ʈ�� �´��� Ȯ�� �� ��ũ��Ʈ �ε��� ���� �־��ش�.
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
        // ����Ʈ �Ϸ��ߴٸ�
        Safe_Release(m_pCurrentQuest);

        ++m_iCurrentQuestIndex;
        QUEST_INFO Info = m_QuestInfo[m_iCurrentQuestIndex];

        //���ο� ����Ʈ �ο��ϱ�
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

/* Kill Quest ���� �Լ�*/
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

/* Move Quest ���� �Լ�*/
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

/* Talk Quest ���� �Լ�*/
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
