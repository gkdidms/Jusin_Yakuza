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
public:
    enum MUSIC { STUPID , JUDGEMENT,MERMAID , MUSIC_END};

    typedef struct tMusicInfo{
        _uint Playnum;//부른횟수
        _uint HighScore;//최고점수
    }MUSICINFO;
    
private:
    CKaraokeManager();
    virtual ~CKaraokeManager() = default;

public:
    HRESULT Initialize();
    void Tick(const _float& fTimeDelta);

private:
    void KeyInput();

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };
    _uint m_iCurrentMusic = { 0 };//지금 진행하는 노래인덱스
    vector<class CNoteManager*> m_Music = {};//노래 노트 담는 벡터
    MUSICINFO* m_MusicBoard[MUSIC_END];//최고점수 , 연주횟수 저장

    _bool m_isStart = { false };
    _uint m_iCurrentScore = { 0 };//이번판 점수 기입

private:
    HRESULT Ready_Karaoke();


public:
    static CKaraokeManager* Create();
    virtual void Free();
};
END
