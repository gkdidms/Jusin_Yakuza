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

public:
    //자신이 만들 ui씬은 미리 할당(수동)
    HRESULT Initialize() ;

    HRESULT Add_Data(const wstring strSceneName, const wstring strProtoName);

    void Open_Scene(const wstring strSceneName);
    void Close_Scene();

    void Click();
    HRESULT Tick(const _float& fTimeDelta);
    HRESULT Late_Tick(const _float& fTimeDelta);

private:
    class CGameInstance* m_pGameInstance; 
    map <wstring ,class CUIScene*> m_AllScene;//모든 씬데이터 보관(폴더이름 ,파일)
    vector< class CUIScene*> m_PlayScene ;//사용하는 씬만 담아 상호 작용(팝업용)창
    vector<class CUIScene*> m_AlwaysUI;//항상 떠있는 ui (피통 스킬상태 소지품)
    class CInventoryManager* m_pInventory;
private:
   CUIScene* Find_Scene(wstring strSceneName);      

public:
    virtual void Free();
};
END
