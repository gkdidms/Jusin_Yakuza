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
    void Set_TutorialState(_uint iType);
    void Set_TutorialText(wstring strText);

public: // īü�̽� ��
    HRESULT Add_Target(_uint iIndex, class CCarChase_Monster* pMonster);
    HRESULT Update_TargetMatrix(_uint iIndex, _matrix TargetMatrix, _float iHP);
    HRESULT Remove_Target(_uint iIndex);
    class CCarChase_Monster* Get_Target();

public: //Ÿ��Ʋ ��
    void Set_TitleStart(_bool isStart);
    _bool isTitleStart();
    void Start_Title(_int iIndex);
    _bool isTitleAnimFinished();

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


public:
    void Fade_In();
    void Fade_Out();
public:
   CUIScene* Find_Scene(wstring strSceneName);      

public:
    virtual void Free();
};
END
