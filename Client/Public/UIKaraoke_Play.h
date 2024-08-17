#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIKaraoke_Play :
    public CUIScene
{
private:
    const _float FADE_START_POSITION = 64.479f;
    const _float CUTSCENE_START_POSITION = 64.9f;

public:
    enum UILIST { BACK, BLUE, MIC, CURRENTBAR, GOODEFFECT, GRADE, GREATEFFECT, HOLD, ROLL, DOWN, LEFT, RIGHT, UP, UILIST_END };

    struct LYRICS_DESC
    {
        _float fTime;
        _float fDuration;
        _uint iSocketIndex;
    };

protected:
    CUIKaraoke_Play();
    CUIKaraoke_Play(const CUIKaraoke_Play& rhs);
    virtual ~CUIKaraoke_Play() = default;

public:
    virtual HRESULT Show_Scene()override;//ui 애님 준비(초기화/열때 정방향 진행)
    virtual HRESULT Close_Scene()override;//ui 애님 준비(초기화/닫을떄 반대로진행)

public:
    virtual HRESULT Add_UIData(class CUI_Object* pUIObject, wstring wstrPrototypeTag = TEXT("")) override;
    virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg) override;
    virtual HRESULT Tick(const _float& fTimeDelta) override;
    virtual HRESULT Late_Tick(const _float& fTimeDelta) override;

    virtual _bool Click_InterSect()override;//클릭시충돌
    virtual _bool Over_InterSect()override;//오버시 충동
    virtual void Action() override;
    virtual void OverAction() override;

private:
    void Ready_LyricsTime();
    void Ready_LyricsSocket();

private:
    void Change_Lyrics();
    void Setting_BackUI(LYRICS_DESC Desc, _fvector vPos, _uint iLyricsIndex);

    _uint Compute_Num(_uint iCount);

private:
    _int m_iCurrentLyrics = { -1 };
    _int m_iCloneCount = { 0 };

    vector<class CGroup*> m_pPlayUI[UILIST_END];
    class CGroup* m_Lyrics;

    vector<LYRICS_DESC> m_LyricsTime;
    vector<_float3> m_LyricsSocket;

public:
    static CUIKaraoke_Play* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
