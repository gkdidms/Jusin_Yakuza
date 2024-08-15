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

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };
    _uint m_iCurrentMusic = { 0 };//���� �����ϴ� �뷡�ε���
    vector<class CNoteManager*> m_Music = {};//�뷡 ��Ʈ ��� ����
    MUSICINFO* m_MusicBoard[MUSIC_END];//�ְ����� , ����Ƚ�� ����

    _bool m_isStart = { false };
    _uint m_iCurrentScore = { 0 };//�̹��� ���� ����

private:
    HRESULT Ready_Karaoke();


public:
    static CKaraokeManager* Create();
    virtual void Free();
};
END
