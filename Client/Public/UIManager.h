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
    //�ڽ��� ���� ui���� �̸� �Ҵ�(����)
    HRESULT Initialize() ;

    HRESULT Add_Data(const wstring strSceneName, const wstring strProtoName);

    void Open_Scene(const wstring strSceneName);
    void Close_Scene();

    void Click();
    HRESULT Tick(const _float& fTimeDelta);
    HRESULT Late_Tick(const _float& fTimeDelta);

private:
    class CGameInstance* m_pGameInstance; 
    map <wstring ,class CUIScene*> m_AllScene;//��� �������� ����(�����̸� ,����)
    vector< class CUIScene*> m_PlayScene ;//����ϴ� ���� ��� ��ȣ �ۿ�(�˾���)â
    vector<class CUIScene*> m_AlwaysUI;//�׻� ���ִ� ui (���� ��ų���� ����ǰ)
    class CInventoryManager* m_pInventory;
private:
   CUIScene* Find_Scene(wstring strSceneName);      

public:
    virtual void Free();
};
END
