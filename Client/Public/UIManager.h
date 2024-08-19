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

public: // 튜토리얼 용
    _bool isShowTutorialUI(_uint iUIType);
    _bool isCloseTutorialUIAnim();
    void Set_TutorialState(_uint iType);
    void Set_TutorialText(wstring strText);

public: // 카체이스 용
    HRESULT Add_Target(_uint iIndex, class CCarChase_Monster* pMonster);
    HRESULT Update_TargetMatrix(_uint iIndex, _matrix TargetMatrix, _float iHP);
    HRESULT Remove_Target(_uint iIndex);
    class CCarChase_Monster* Get_Target();

public: //타이틀 용
    void Set_TitleStart(_bool isStart);
    _bool isTitleStart();
    void Start_Title(_int iIndex);
    _bool isTitleAnimFinished();

public:
    //자신이 만들 ui씬은 미리 할당(수동)
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

    //임시 토크매니저 확인용 제작
public:
    void Start_Talk(_uint iScriptIndex);
    _int Get_CurrentPage();
    _bool isTalkFinished();
    void Change_TutorialUI(_uint iUIType);
    _bool Check_Scene(wstring SceneName);

public: // 스토리 UI
    void Close_Image();

    //카라오케 점수 전달용
public:
    void Set_Score(_uint iScore);
    //파이트 스코어 (돈)
public:
    void Set_Money(_uint Money) { m_Money = Money; }
    _uint Get_Money() { return m_Money; }
private:

    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

    map <wstring ,class CUIScene*> m_AllScene;//모든 씬데이터 보관(폴더이름 ,파일)
    vector< class CUIScene*> m_PlayScene ;//사용하는 씬만 담아 상호 작용(팝업용)창
    vector<class CUIScene*> m_AlwaysUI;//항상 떠있는 ui (피통 스킬상태 소지품)
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
