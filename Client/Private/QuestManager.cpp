#include "QuestManager.h"

#include "Quest.h"
#include "KillQuest.h"
#include "MoveQuest.h"
#include "TalkQuest.h"
#include "KaraokeQuest.h"

#include "Chapter1_0.h"
#include "Chapter1_1.h"
#include "Chapter2_0.h"
#include "Chapter3_0.h"
#include "Chapter3_1.h"
#include "Chapter4_0.h"

#include "UIManager.h"

/*
[���丮����]
1. Ʃ�丮��
2. �Ͻ�Ű �߻�
    1. �߻� ĸ��
3. �Ͻ�Ű���� ����
    1. �Ͻ�Ű���� ���� ������� �ɾ�ٴϸ鼭 �ִ´�.
        1. ū��ȵ�.
    2. �Ͻ�Ű���� ������ ȭ�� ���ٴ� ��
4. ������ɷ� ��
5. ��ü����
6. ��Ÿ�
7. �Ѱ���
8. ��������
*/

IMPLEMENT_SINGLETON(CQuestManager)

CQuestManager::CQuestManager()
{
}

/*
* ���� ����Ʈ�� �´��� Ȯ�� �� ��ũ��Ʈ �ε��� ���� �־��ش�.
*/
_uint CQuestManager::Get_ScriptIndex()
{
    return m_QuestInfo[m_iCurrentChapter][m_iCurrentQuestIndex].iScriptIndex;
}

_uint CQuestManager::Get_CurrentQuestType()
{
    return m_pCurrentQuest->Get_Type();
}

void CQuestManager::Set_CurrentColl(_bool isColl)
{
    m_pCurrentQuest->Set_Coll(isColl);
}

HRESULT CQuestManager::Initialize()
{
    if (FAILED(Ready_Quest()))
        return E_FAIL;

    m_iCurrentQuestIndex = 0;
    
    return S_OK;
}

void CQuestManager::Start_Quest(_uint iChapter)
{
    m_iCurrentChapter = iChapter;
    Add_MainQuest(m_QuestInfo[m_iCurrentChapter][0].iQuestIndex, m_QuestInfo[m_iCurrentChapter][0].iNextQuestIndex, m_QuestInfo[m_iCurrentChapter][0].iObjectIndex, m_QuestInfo[m_iCurrentChapter][0].iScriptIndex);
}

HRESULT CQuestManager::Ready_Quest()
{
    //_int iScriptIndex = 1;

    //_uint iType;
    //_uint iScriptIndex;

    //_int iQuestIndex;
    //_int iNextQuestIndex;
    //_int iTargetIndex; //Kill
    //_int iTriggerIndex; //Move
    //_int iObjectIndex; // Talk
    _uint iQuestIndex = 0;
    _uint iNextQuestIndex = 1;

    //Ʃ�丮�� 
    vector<QUEST_INFO> Chapter1;
    Chapter1 = {
        QUEST_INFO(
            QUEST_MAIN,
            0,

            iQuestIndex++,
            iNextQuestIndex++
        ),
        QUEST_INFO(
            QUEST_TALK,
            0,

            iQuestIndex++,
            iNextQuestIndex++,
            101 // �Ͻ�Ű ������Ʈ index
        ),
        QUEST_INFO(
            QUEST_TALK,
            1,

            iQuestIndex++,
            iNextQuestIndex++
        ),
    };
    m_QuestInfo.emplace(CHAPTER_1, Chapter1);

    //�Ͻ�Ű�߸��� �ȱ�
    vector<QUEST_INFO> Chapter2;
    Chapter2 = {
        QUEST_INFO(
            QUEST_MAIN,
            2,

            iQuestIndex++,
            iNextQuestIndex++
        ),
        QUEST_INFO(
            QUEST_MOVE,
            0,

            iQuestIndex++,
            iNextQuestIndex++,
            -1,
            80001
        )
    };
    m_QuestInfo.emplace(CHAPTER_2, Chapter2);

    //�������
    vector<QUEST_INFO> Chapter3;
    Chapter3 = {
        QUEST_INFO(
            QUEST_MAIN,
            3,

            iQuestIndex++,
            iNextQuestIndex++
        ),
        QUEST_INFO(
            QUEST_KARAOKE,
            0,

            iQuestIndex++,
            iNextQuestIndex++
        ),
        QUEST_INFO(
            QUEST_MAIN,
            4,

            iQuestIndex++,
            iNextQuestIndex++
        )
    };
    m_QuestInfo.emplace(CHAPTER_3, Chapter3);

    //��Ÿ�
    vector<QUEST_INFO> Chapter4;
    Chapter4 = {
        QUEST_INFO(
            QUEST_MAIN,
            5,

            iQuestIndex++,
            iNextQuestIndex++
        ),
        QUEST_INFO(
            QUEST_TALK,
            0,

            iQuestIndex++,
            iNextQuestIndex++
        ),
        QUEST_INFO(
            QUEST_MAIN,
            6,

            iQuestIndex++,
            iNextQuestIndex++
        ),
    };
    m_QuestInfo.emplace(CHAPTER_4, Chapter4);

    //��ü����
    vector<QUEST_INFO> Chapter5;
    Chapter4 = {
        QUEST_INFO(
            QUEST_KILL,
            0,

            iQuestIndex++,
            iNextQuestIndex++,
            4001 //iTargetIndex
        )
    };
    m_QuestInfo.emplace(CHAPTER_5, Chapter5);
    
    return S_OK;
}

_bool CQuestManager::Execute()
{
    if (true == m_pCurrentQuest->Execute())
    {
        // ����Ʈ �Ϸ��ߴٸ�
        Safe_Release(m_pCurrentQuest);

        ++m_iCurrentQuestIndex;
        QUEST_INFO Info = m_QuestInfo[m_iCurrentChapter][m_iCurrentQuestIndex];

        //���ο� ����Ʈ �ο��ϱ�
        if (Info.iType == QUEST_MAIN)
            Add_MainQuest(Info.iQuestIndex, Info.iNextQuestIndex, Info.iTargetIndex, Info.iScriptIndex);
        else if (Info.iType == QUEST_KILL)
            Add_KillQuest(Info.iQuestIndex, Info.iNextQuestIndex, Info.iTargetIndex, Info.iScriptIndex);
        else if (Info.iType == QUEST_MOVE)
            Add_MoveQuest(Info.iQuestIndex, Info.iNextQuestIndex, Info.iTriggerIndex, Info.iScriptIndex);
        else if (Info.iType == QUEST_TALK)
            Add_TalkQuest(Info.iQuestIndex, Info.iNextQuestIndex, Info.iObjectIndex, Info.iScriptIndex);
        else if (Info.iType == QUEST_KARAOKE)
            Add_KaraokeQuest(Info.iQuestIndex, Info.iNextQuestIndex, Info.iObjectIndex, Info.iScriptIndex);
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

/*���� ���丮 ���� �Լ�*/
HRESULT CQuestManager::Add_MainQuest(_int iQuestIndex, _int iNextQuestIndex, _int iObjectIndex, _int iScriptIndex)
{
    if (iQuestIndex == 0)
    {
        CChapter1_0::MAIN_QUEST_DESC Desc{};
        Desc.iQuestIndex = iQuestIndex;
        Desc.iNextQuestIndex = iNextQuestIndex;
        Desc.iScriptIndex = iScriptIndex;

        CChapter1_0* pMainQuest = CChapter1_0::Create(&Desc);
        if (nullptr == pMainQuest)
            return E_FAIL;

        m_pCurrentQuest = pMainQuest;
    }
    else if (iQuestIndex == 2)
    {
        CChapter1_1::MAIN_QUEST_DESC Desc{};
        Desc.iQuestIndex = iQuestIndex;
        Desc.iNextQuestIndex = iNextQuestIndex;
        Desc.iScriptIndex = iScriptIndex;

        CChapter1_1* pMainQuest = CChapter1_1::Create(&Desc);
        if (nullptr == pMainQuest)
            return E_FAIL;

        m_pCurrentQuest = pMainQuest;
    }
    else if (iQuestIndex == 3)
    {
        CChapter2_0::MAIN_QUEST_DESC Desc{};
        Desc.iQuestIndex = iQuestIndex;
        Desc.iNextQuestIndex = iNextQuestIndex;
        Desc.iScriptIndex = iScriptIndex;

        CChapter2_0* pMainQuest = CChapter2_0::Create(&Desc);
        if (nullptr == pMainQuest)
            return E_FAIL;

        m_pCurrentQuest = pMainQuest;
    }
    else if (iQuestIndex == 5)
    {
        CChapter3_0::MAIN_QUEST_DESC Desc{};
        Desc.iQuestIndex = iQuestIndex;
        Desc.iNextQuestIndex = iNextQuestIndex;
        Desc.iScriptIndex = iScriptIndex;

        CChapter3_0* pMainQuest = CChapter3_0::Create(&Desc);
        if (nullptr == pMainQuest)
            return E_FAIL;

        m_pCurrentQuest = pMainQuest;
    }
    else if (iQuestIndex == 7)
    {
        CChapter3_1::MAIN_QUEST_DESC Desc{};
        Desc.iQuestIndex = iQuestIndex;
        Desc.iNextQuestIndex = iNextQuestIndex;
        Desc.iScriptIndex = iScriptIndex;

        CChapter3_1* pMainQuest = CChapter3_1::Create(&Desc);
        if (nullptr == pMainQuest)
            return E_FAIL;

        m_pCurrentQuest = pMainQuest;
    }

    return S_OK;
}

HRESULT CQuestManager::Add_KaraokeQuest(_int iQuestIndex, _int iNextQuestIndex, _int iObjectIndex, _int iScriptIndex)
{
    CKaraokeQuest::KARAOKE_QUEST_DESC Desc{};
    Desc.iType = QUEST_TALK;
    Desc.iQuestIndex = iQuestIndex;
    Desc.iNextQuestIndex = iNextQuestIndex;

    CKaraokeQuest* pKaraoKeQuest = CKaraokeQuest::Create(&Desc);
    if (nullptr == pKaraoKeQuest)
        return E_FAIL;

    m_pCurrentQuest = pKaraoKeQuest;

    return S_OK;
}

void CQuestManager::Free()
{
    Safe_Release(m_pCurrentQuest);
}
