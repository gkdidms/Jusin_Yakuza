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

public:
    //자신이 만들 ui씬은 미리 할당(수동)
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) ;

    HRESULT Add_Data(const wstring strSceneName, const wstring strProtoName);

    void Open_Scene(const wstring strSceneName);
    void Close_Scene();

    void Click();
    HRESULT Tick(const _float& fTimeDelta);
    HRESULT Late_Tick(const _float& fTimeDelta);

    /* Setter */
public:
    void Set_Player(class CPlayer* pPlayer)
    {
        m_pPlayer = pPlayer;
    }

    //임시 토크매니저 확인용 제작
public:
    void Set_TalkData(wstring Name, wstring TalkData);


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

private:
    class CPlayer* m_pPlayer = { nullptr };

private:
   CUIScene* Find_Scene(wstring strSceneName);      

public:
    virtual void Free();
};
END
