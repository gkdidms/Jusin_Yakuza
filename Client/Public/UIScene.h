#pragma once
#include "Base.h"

#include "Client_Defines.h"
BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CUIScene :
    public CBase
{
public:
    typedef struct tSceneDesc {
        _bool isLoading;
        wstring strSceneName;
    }SCENE_DESC;

protected:
    CUIScene();
    CUIScene(const CUIScene& rhs);
    virtual ~CUIScene() = default;

public:
    //loader ���� ���� ui��ü�� �޾ƿͼ� �����ص�. ������ ���� ������� �޾ƿͼ� ������(�����߿�)
    //������ ���� �ٸ��� �������̵� ��
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject, wstring wstrPrototypeTag = TEXT(""));
    
    virtual HRESULT Show_Scene();//ui �ִ� �غ�(�ʱ�ȭ/���� ������ ����)
    virtual HRESULT Close_Scene();//ui �ִ� �غ�(�ʱ�ȭ/������ �ݴ������)

    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,void* pArg);
    virtual HRESULT Tick(const _float& fTimeDelta);
    virtual HRESULT Late_Tick(const _float& fTimeDelta);

    virtual void Check_AimFin();//��� ��ȣ�ۿ��� �ִϸ��̼��� �� ������ ������(m_UI �� �� ��� ���ִ� ui)

    virtual _bool Click_InterSect();//Ŭ�����浹
    virtual _bool Over_InterSect();//������ �浿
    virtual void Action();//Ŭ���� �ൿ
    virtual void OverAction();//������ �ൿ

public:
    _bool Get_isAnimFinish() { return m_isAnimFin; }
    _bool Get_isClose() { return m_isClose; }
    _bool Get_isOpen() { return m_isOpen; }
    _bool Get_isLoading() { return m_isLoading; }
    virtual void Reset() {
        m_isClose = false;  
    }
    wstring Get_SceneName() { return m_strSceneName; }

public:
    void Set_Open(_bool isOpen = true) { m_isOpen = isOpen; }

protected:
    class CGameInstance* m_pGameInstance = { nullptr };
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    vector<class CUI_Object*> m_UI;//�̺�Ʈ ������ ui�� �޴°�(��� ������ ui)
    vector<class CUI_Object*> m_EventUI;//�̺�Ʈ ui�� �޴°�(Ư�� ��Ȳ�� �� ui)ex.Ŭ��,����
    vector<class CBtn*> m_Button;//��ư�� �����ؼ� ��� ��(������ ��ư�� ��Ƽ� ������� ����)

    _int m_iCurButton = { -1 };
    _int m_iPrevButton = { -1 };


    _bool m_isAnimFin = { false };
    _bool m_isClose = { true };
    wstring m_strSceneName = { TEXT("") };

    _bool m_isLoading = { false };
    _bool m_isOpen = { false };

public:
    virtual void Free();
};

END