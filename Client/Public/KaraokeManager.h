#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CKaraokeManager :
    public CBase
{
private:
    const _float SCORE_TIMER = 3.f;

public:
    enum MUSIC { STUPID , JUDGEMENT,MERMAID , MUSIC_END};

    typedef struct tMusicInfo{
        _uint Playnum;//�θ�Ƚ��
        _uint HighScore;//�ְ�����
    }MUSICINFO;
    
private:
    CKaraokeManager();
    virtual ~CKaraokeManager() = default;

public:
    HRESULT Initialize();
    void Tick(const _float& fTimeDelta);

private:
    void KeyInput();

    void Setting_NoteUIs();
    
public:
    _bool IsSongEnd() {
        return m_isScoreEnd;
    }

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };
    _uint m_iCurrentMusic = { 0 };//���� �����ϴ� �뷡�ε���
    vector<class CNoteManager*> m_Music = {};//�뷡 ��Ʈ ��� ����
    MUSICINFO* m_MusicBoard[MUSIC_END];//�ְ����� , ����Ƚ�� ����

    _bool m_isStart = { false };
    _uint m_iCurrentScore = { 0 };//�̹��� ���� ����

    _float  m_fScoreTimer = { 0.f };
    _bool m_isScoreTimerOn = { false };
    _bool m_isScoreEnd = { false };

private:
    HRESULT Ready_Karaoke();


public:
    static CKaraokeManager* Create();
    virtual void Free();
};
END
