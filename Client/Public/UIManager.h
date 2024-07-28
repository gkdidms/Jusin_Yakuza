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

    //�ӽ� ��ũ�Ŵ��� Ȯ�ο� ����
public:
    void Set_TalkData(wstring Name, wstring TalkData);


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

private:
    class CPlayer* m_pPlayer = { nullptr };

private:
   CUIScene* Find_Scene(wstring strSceneName);      

public:
    virtual void Free();
};
END
