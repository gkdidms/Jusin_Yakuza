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
    const _float GREAT_TIME = 0.08f;
    const _float GOOD_TIME = 0.15f;
    const _float MISS_TIME = 0.2f;
    const _float MISS_TIME2 = 0.22f;

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

    void Update_CurrentNoteIndex();

    void SingleNote_KeyInput(class CNoteBase* pNote);
    void LongNote_KeyInput(class CNoteBase* pNote);
    void BurstNote_KeyInput(class CNoteBase* pNote);

    void Setting_NoteUIs();
    void Check_NextNote();

    void Store_SingleNoteScore(class CNoteBase* pNote);
    void Acc_LongNoteScore(class CNoteBase* pNote);
    void Store_LongNoteScore(class CNoteBase* pNote);
    void Acc_BurstNoteScore(class CNoteBase* pNote);
    void Store_BurstNoteScore(class CNoteBase* pNote, _float fCurSoundPos);

    void Store_Miss(class CNoteBase* pNote);

    void Show_Grade_UI(class CNoteBase* pNote);
    
public:
    _bool IsScoreEnd() {
        return m_isScoreEnd;
    }

    _uint Get_MusicSelected() {
        return m_iMusicSelected;
    }

    void Set_MusicSelected(_bool isValue) {
        m_iMusicSelected = isValue;
    }

    void Open_SelectWindow();

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };
    _uint m_iCurrentMusic = { 0 };//���� �����ϴ� �뷡�ε���
    vector<class CNoteManager*> m_Music = {};//�뷡 ��Ʈ ��� ����
    //MUSICINFO* m_MusicBoard[MUSIC_END];//�ְ����� , ����Ƚ�� ����

    _bool m_isStart = { false };
    _uint m_iCurrentScore = { 0 };//�̹��� ���� ����

    _float  m_fScoreTimer = { 0.f };
    _bool m_isScoreTimerOn = { false };
    _bool m_isScoreEnd = { false };

    _uint m_iCheckNoteIndex = { 0 };        // �˻��� ��Ʈ �ε���

    _bool m_iMusicSelected = { false };

private:
    HRESULT Ready_Karaoke();


public:
    static CKaraokeManager* Create();
    virtual void Free();
};
END
