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

    _bool m_isLoading = { false };
    

#ifdef _DEBUG
private:
    _bool m_isRender = { true };
#endif // _DEBUG

private:
    _bool m_isClose = { true };

private:
   CUIScene* Find_Scene(wstring strSceneName);      

public:
    virtual void Free();
};
END
