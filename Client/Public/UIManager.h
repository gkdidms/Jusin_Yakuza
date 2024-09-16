#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CUIManager :
    public CBase
{
    DECLARE_SINGLETON(CUIManager)
public:
    enum INFO{HP, SKILLGAUAGE , EAGLEEYE , CARHP , INFO_END};

    // ��ųâ �� �̳�
    enum KRS_SKILL_TYPE { FLY_KICK, KICK_COMBO, PUNCH, DOUBLE_FINISH, FINISHBLOW, POWER, WALL_ATTACK, ATTACK_DEEPDOWN};
    enum KRH_SKILL_TYPE { RISE_TORNADO, SWAY_BLOW, CANCLE_WEABBING, WEABBING_TO_SWAY, RUSH_COMBO_7, RUSH_COMBO_8 };
    enum KRC_SKILL_TYPE { GUARD_RUSH, LARRIOTT, RUSH_HUG, WEAPON_RUSH, CRASH_HAMMER };

private:
    CUIManager();
    virtual ~CUIManager() = default;

#ifdef _DEBUG
public:
    void Set_Render(_bool isRender) { m_isRender = isRender; }

public:
    _bool Get_Render() { return m_isRender; }
#endif // _DEBUG

public:
    _bool isInvenClose() { return m_isClose; }
    _bool isOpen(wstring strName);

public: // Ʃ�丮�� ��
    _bool isShowTutorialUI(_uint iUIType);
    _bool isCloseTutorialUIAnim();
    _bool isTutorialStartEnd();
    void Set_TutorialState(_uint iType);
    void Set_TutorialText(wstring strText);

public: // īü�̽� ��
    HRESULT Add_Target(_uint iIndex, class CCarChase_Monster* pMonster, _bool isBullet = false);
    HRESULT Update_TargetMatrix(_uint iIndex, _matrix TargetMatrix, _float iHP);
    HRESULT Remove_Target(_uint iIndex);
    class CCarChase_Monster* Get_Target();

public: //Ÿ��Ʋ ��
    /*
    * Ÿ��Ʋ ���
    0 �ⱸ�� ����
    1 ��� �谨���� ����
    2 �����並 ã�ƶ�
    3 �����Ķ�
    4 �����ö� ���ξ�
    5 ���� ���̻���
    6 ������ ����
    7 ������ ���ڵ�
    8 ������
    9 è�Ǿ� �Ÿ� �ֹ�
    10 ȣ���϶�
    11 �Ÿ��� ���ġ
    12 ������
    13 ����
    14 �Ͻ�Ű�߸��� ã�ƶ�
    15 ���ڸ� ������ �����Ķ�
    16 �������� ������
    17 ���� ũ����Ʈ�� ���ڵ�
    18 ���󸮵�
    */
    void Set_TitleStart(_bool isStart);
    _bool isTitleStart();
    void Start_Title(_int iIndex);
    _bool isTitleEnd();
    _bool isBattleStart();

public: //���̵������̵�ƿ�
    _bool isFindFinished();

public:
    //�ڽ��� ���� ui���� �̸� �Ҵ�(����)
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) ;

    HRESULT Add_Data(const wstring strSceneName, const wstring strProtoName);

    void Open_Scene(const wstring strSceneName);
    void Close_Scene(const wstring strSceneName = TEXT(""));

    void Click();
    HRESULT Tick(const _float& fTimeDelta);
    HRESULT Late_Tick(const _float& fTimeDelta);

    _bool IsOpend() {
        return (0 < m_PlayScene.size());
    }

    //�ӽ� ��ũ�Ŵ��� Ȯ�ο� ����
public:
    void Start_Talk(_uint iScriptIndex);
    _int Get_CurrentPage();
    _bool isTalkFinished();
    void Change_TutorialUI(_uint iUIType);
    _bool Check_Scene(wstring SceneName);

public: // ���丮 UI
    void Close_Image();

    //ī����� ���� ���޿�
public:
    void Set_Score(_uint iScore);

    //����Ʈ ���ھ� (��)
public:
    void Set_Money(_uint Money) { m_Money = Money; }
    _uint Get_Money() { return m_Money; }
    void Set_AlwayUI(_bool isTrue) { m_isAlways = isTrue; }

    //��Ÿ�� ��ü��(lifeUI)
public:
    void Change_Style(_uint iStyle);

/* ��ųâ �� */
public:
    _bool* Get_Skill_Holligan();
    _bool* Get_Skill_Rush();
    _bool* Get_Skill_Destoryer();

private:

    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

    map <wstring ,class CUIScene*> m_AllScene;//��� �������� ����(�����̸� ,����)
    vector< class CUIScene*> m_PlayScene ;//����ϴ� ���� ��� ��ȣ �ۿ�(�˾���)â
    vector<class CUIScene*> m_AlwaysUI;//�׻� ���ִ� ui (���� ��ų���� ����ǰ)
    class CInventoryManager* m_pInventory = {nullptr};

#ifdef _DEBUG
private:
    _bool m_isRender = { true };
#endif // _DEBUG

private:
    _bool m_isClose = { true };
    wstring m_iCurScene = { TEXT("") };

private:
    _uint m_Money = { 0 };

private:
    _bool m_isAlways = { true };

public:
    void Fade_In();
    void Fade_Out();
    _bool Fade_Finished();

 public://QTE
    void PressKey();
    void Set_QTEKey(wstring strKey);
    _bool QTE_Finished();

public://DamgeEffecdt
    void Add_MoneyEffect(_fmatrix world, _uint iMoney, _uint iTime);

public:
   CUIScene* Find_Scene(wstring strSceneName);      

public:
    virtual void Free();
};
END
