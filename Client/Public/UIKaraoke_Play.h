#pragma once
#include "UIScene.h"
BEGIN(Client)
class CUIKaraoke_Play :
    public CUIScene
{
private:
    const _float FADE_START_POSITION = 64.479f;
    const _float CUTSCENE_START_POSITION = 64.9f;
    const _float DURATION_SCALE = 0.3f;

public:
    enum UILIST { BACK, BLUE, MIC, CURRENTBAR, GOODEFFECT, GRADE, GREATEFFECT, HOLD, ROLL, DOWN, LEFT, RIGHT, UP, UILIST_END };

    struct LYRICS_DESC
    {
        _float fTime;
        _float fDuration;
        _uint iSocketIndex;
    };

    struct LYRICS_NOTE_DESC
    {
        class CNoteBase* pNote;
        _bool   isVisible = { false };
        _uint iIndex;
        _uint iIndex_End;
        _uint iBarIndex;
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

public:
    void Set_Notes(vector<class CNoteBase*>* pNotes);

public:
    _bool IsSongEnd() {
        return m_isSongEnd;
    }

private:
    void Ready_LyricsTime();
    void Ready_LyricsSocket();

private:
    void Update_CurrentLyricsIndex();

    void Change_Lyrics();
    void Setting_BackUI(LYRICS_DESC Desc, _fvector vPos, _uint iLyricsIndex);
    void Setting_BlueUI(LYRICS_DESC Desc, _fvector vPos, _uint iLyricsIndex);
    void CurrentBar_Control();
    void Visible_Notes(_uint iLyricsIndex);
    void Invisible_Notes(_uint iLyricsIndex);

    void Verse_On_SingleNote(LYRICS_NOTE_DESC& Desc, _uint iLyricsIndex);
    void Verse_Off_SingleNote(LYRICS_NOTE_DESC& Desc, _uint iLyricsIndex);

    void Verse_On_LongNote(LYRICS_NOTE_DESC& Desc, _uint iLyricsIndex);
    void Verse_Off_LongNote(LYRICS_NOTE_DESC& Desc, _uint iLyricsIndex);

    void Verse_On_BurstNote(LYRICS_NOTE_DESC& Desc, _uint iLyricsIndex);
    void Verse_Off_BurstNote(LYRICS_NOTE_DESC& Desc, _uint iLyricsIndex);

    _uint Compute_Num(_uint iCount);
    _fvector Compute_UIPosition(LYRICS_NOTE_DESC& Desc, _uint iLyricsIndex, _float fRatio);

    _uint Trans_ButtonType_To_UI(_uint iNum);

    // 0 일반노드    1 유지 노드     2 연타 노드
    // 0 UP        1 DOWN        2 LEFT        3 RIGHT

private:
    _int m_iCloneCount = { 0 };
    _int m_iCurLyricsIndex = { -1 };

    _float m_fCurSoundTime = { 0.f };

    vector<class CGroup*> m_pPlayUI[UILIST_END];
    class CGroup* m_Lyrics;

    vector<LYRICS_DESC> m_LyricsTime;
    vector<_float3> m_LyricsSocket;

    // 래퍼런스 카운트 증가하지않음
    multimap<_uint, LYRICS_NOTE_DESC> m_LyricsNotes;

    vector<_uint> m_Pivots;

    _bool               m_isSongEnd = { false };

public:
    static CUIKaraoke_Play* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
};
END
