#pragma once
#include"UIScene.h"

//EventUI 0번 =첫줄
//EventUI 1번 =두번쨰줄
//EventUI 2번 =세번째 줄
 //EventUI 3번 =이름
BEGIN(Client)
class CUITalk :
    public CUIScene
{
public:
    enum TEXTCASE {TEXT0 , TEXT1 , TEXT2 ,NAME , TEXTCASE_END};
protected:
    CUITalk();
    CUITalk(const CUITalk& rhs);
    virtual ~CUITalk() = default;

public:
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;
    virtual void Action() override;
    virtual void OverAction() override;

public:
    //구조체로 받아오는거 이거바꿔서 써도됨
    void Set_TalkData(wstring Name, wstring TalkData);
    //위에꺼 먼저 부르고 StartTalk 불러주기.
    void StartTalk();
private:
    void Talk_Effect(const _float& fTimeDelta);
private:
    wstring m_TalkData;//여기다가 말할 데이터를 다 받아와서 시작하자.
    wstring m_EffectText[3];//여긴 텍스트애니메이션용 통 건들지 마셈
    _uint m_iIndex = { 0 };
    _uint m_LineIndex = { 0 };
    _float m_PlayTime = { 0.f };
    _bool m_TalkFin = { false };
public:
    static CUITalk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
