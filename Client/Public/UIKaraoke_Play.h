#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIKaraoke_Play :
    public CUIScene
{
public:
    enum UILIST { BACK, BLUE, MIC, CURRENTBAR, GOODEFFECT, GRADE, GREATEFFECT, HOLD, ROLL, DOWN, LEFT, RIGHT, UP, UILIST_END };

protected:
    CUIKaraoke_Play();
    CUIKaraoke_Play(const CUIKaraoke_Play& rhs);
    virtual ~CUIKaraoke_Play() = default;

public:
    virtual HRESULT Show_Scene()override;//ui 애님 준비(초기화/열때 정방향 진행)
    virtual HRESULT Close_Scene()override;//ui 애님 준비(초기화/닫을떄 반대로진행)

public:
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject) override;
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

    virtual _bool Click_InterSect()override;//클릭시충돌
    virtual _bool Over_InterSect()override;//오버시 충동
    virtual void Action() override;
    virtual void OverAction() override;

private:
    vector<class CUI_Object*> m_pPlayUI;
    class CGroup* m_Lyrics;

public:
    static CUIKaraoke_Play* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
