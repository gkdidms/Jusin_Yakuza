#include "UIKaraoke_Play.h"
#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"

#include "Karaoke_Kiryu.h"

#include "NoteBurstHold.h"
#include "NoteLong.h"
#include "NoteSingle.h"

CUIKaraoke_Play::CUIKaraoke_Play()
    :CUIScene{}
{
}

CUIKaraoke_Play::CUIKaraoke_Play(const CUIKaraoke_Play& rhs)
    :CUIScene{ rhs }
{
}

HRESULT CUIKaraoke_Play::Show_Scene()
{
    if (FAILED(__super::Show_Scene()))
        return E_FAIL;

    m_Lyrics->Show_Off_All();

    for (size_t i = 0; i < UILIST_END; i++)
    {
        for (auto& pUI : m_pPlayUI[i])
        {
            pUI->Show_Off_All();
        }
    }

    return S_OK;
}

HRESULT CUIKaraoke_Play::Close_Scene()
{
    if (FAILED(__super::Close_Scene()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIKaraoke_Play::Add_UIData(CUI_Object* pUIObject, wstring wstrPrototypeTag)
{
    if (pUIObject->Get_Event())
    {
        if (m_iCloneCount < 13)
        {
            m_pPlayUI[m_iCloneCount].push_back(dynamic_cast<CGroup*>(pUIObject));

            _uint iNum = Compute_Num(m_iCloneCount);

            for (size_t i = 0; i < iNum; i++)
            {
                m_pPlayUI[m_iCloneCount].push_back(dynamic_cast<CGroup*>(m_pGameInstance->Clone_Object(wstrPrototypeTag, nullptr)));
            }

            m_iCloneCount++;
        }
        else
        {
            m_Lyrics=dynamic_cast<CGroup*>(pUIObject);
        }
        return S_OK;
    }


    return S_OK;
}

HRESULT CUIKaraoke_Play::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    if (FAILED(__super::Initialize(pDevice, pContext, pArg)))
        return E_FAIL;

    Ready_LyricsTime();
    Ready_LyricsSocket();

    return S_OK;
}

HRESULT CUIKaraoke_Play::Tick(const _float& fTimeDelta)
{
    m_fCurSoundTime = m_pGameInstance->GetSoundPosition(L"Bakamita.mp3", SOUND_BGM);

    if (m_pGameInstance->Get_SoundEnd(TEXT("Bakamita.mp3"), SOUND_BGM))
        m_isSongEnd = true;

    Update_CurrentLyricsIndex();

    Change_Lyrics();
    CurrentBar_Control();
    Show_Notes();

    // 컷신 실행시키기
    if (CUTSCENE_START_POSITION < m_fCurSoundTime)
    {
        CKaraoke_Kiryu* pPlayer = dynamic_cast<CKaraoke_Kiryu*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));
        pPlayer->Set_CutSceneAnim();
    }

    m_Lyrics->Tick(fTimeDelta);

    for (size_t i = 0; i < UILIST_END; i++)
    {
        for (auto& pUI : m_pPlayUI[i])
        {
            pUI->Tick(fTimeDelta);
        }
    }
    return S_OK;
}

HRESULT CUIKaraoke_Play::Late_Tick(const _float& fTimeDelta)
{
    //m_pPlayUI[BACK][0]->Late_Tick(fTimeDelta);
    //for (auto& iter : m_pPlayUI)
    //    iter->Late_Tick(fTimeDelta);

    for (size_t i = 0; i < UILIST_END; i++)
    {
        for (auto& pUI : m_pPlayUI[i])
        {
            pUI->Late_Tick(fTimeDelta);
        }
    }
    m_Lyrics->Late_Tick(fTimeDelta);

    if (!m_isAnimFin)
        Check_AimFin();

    return S_OK;
}

_bool CUIKaraoke_Play::Click_InterSect()
{
    return _bool();
}

_bool CUIKaraoke_Play::Over_InterSect()
{
    return _bool();
}

void CUIKaraoke_Play::Action()
{
}

void CUIKaraoke_Play::OverAction()
{
}

void CUIKaraoke_Play::Ready_LyricsTime()
{
    m_LyricsTime.reserve(17);
    LYRICS_DESC Desc{ 9.461, 0 };
    m_LyricsTime.push_back(Desc);
    
    //바보같이 어린애인가봐
    Desc.fTime = 14.327;
    Desc.iSocketIndex = 1;
    m_LyricsTime.push_back(Desc);

    //꿈을 쫓아서 상처를 입고
    Desc.fTime = 20.643;
    Desc.iSocketIndex = 2;
    m_LyricsTime.push_back(Desc);

    // 거짓말도 잘 못하면서
    Desc.fTime = 27.609;
    Desc.iSocketIndex = 1;
    m_LyricsTime.push_back(Desc);

    //웃음기 없는 미소를 보였지
    Desc.fTime = 33.275;
    Desc.iSocketIndex = 2;
    m_LyricsTime.push_back(Desc);

    // 아이러브유
    Desc.fTime = 39.776;                     // 동시에
    Desc.iSocketIndex = 0;                   // 동시에
    m_LyricsTime.push_back(Desc);            // 동시에
                                             // 동시에
    //한번 말한 적 없는                       // 동시에
    Desc.fTime = 42.866;                     // 동시에
    Desc.iSocketIndex = 1;                   // 동시에
    m_LyricsTime.push_back(Desc);            // 동시에

    //말주변없고 어쩌구
    Desc.fTime = 47.231;                      
    Desc.iSocketIndex = 2;
    m_LyricsTime.push_back(Desc);

    //그런데 그런데 어째서
    Desc.fTime = 53.733;
    Desc.iSocketIndex = 1;
    m_LyricsTime.push_back(Desc);

    // 안녕이란말을 어쩌구
    Desc.fTime = 59.387;
    Desc.iSocketIndex = 2;
    m_LyricsTime.push_back(Desc);           // 여기서 컷신전환됨

    //다메다네
    Desc.fTime = 65.517;
    Desc.iSocketIndex = 1;
    m_LyricsTime.push_back(Desc);

    //다메요 ~~
    Desc.fTime = 67.746;
    Desc.iSocketIndex = 2;
    m_LyricsTime.push_back(Desc);

    // 네가 좋아서
    Desc.fTime = 72.053;
    Desc.iSocketIndex = 1;
    m_LyricsTime.push_back(Desc);

    // 너무 좋아서
    Desc.fTime = 75.826;
    Desc.iSocketIndex = 2;
    m_LyricsTime.push_back(Desc);

    // 아무리
    Desc.fTime = 78.984;
    Desc.iSocketIndex = 1;
    m_LyricsTime.push_back(Desc);

    // 센 술을 어쩌구
    Desc.fTime = 81.028;
    Desc.iSocketIndex = 2;
    m_LyricsTime.push_back(Desc);

    // 떠나지않아 우리 추억이
    Desc.fTime = 85.191;
    Desc.iSocketIndex = 1;
    m_LyricsTime.push_back(Desc);

    // 바보같이
    Desc.fTime = 91.136;
    Desc.iSocketIndex = 2;
    m_LyricsTime.push_back(Desc);

    //Desc.fTime = 95.130;
    //Desc.iSocketIndex = 1;
    //m_LyricsTime.push_back(Desc);

    for (size_t i = 0; i < m_LyricsTime.size(); i++)
    {
        if(i == m_LyricsTime.size() - 1)
            m_LyricsTime[i].fDuration = 95.719f - m_LyricsTime[i].fTime;                    // 마지막소절은 그냥 끝나는 시간 정해져잇음
        else
            m_LyricsTime[i].fDuration = m_LyricsTime[i + 1].fTime - m_LyricsTime[i].fTime;
    }
}

void CUIKaraoke_Play::Ready_LyricsSocket()
{
    m_LyricsSocket.push_back(_float3(-288.371887, 11.66042638, 0.f));
    m_LyricsSocket.push_back(_float3(-288.371887, -101.741051, 0.f));
    m_LyricsSocket.push_back(_float3(-288.371887, -220.680878, 0.f));
}

void CUIKaraoke_Play::Update_CurrentLyricsIndex()
{
    for (size_t i = 0; i < m_LyricsTime.size(); i++)
    {
        if (m_LyricsTime[i].fTime <= m_fCurSoundTime)
        {
            m_iCurLyricsIndex = i;
        }
    }

}

void CUIKaraoke_Play::Change_Lyrics()
{
    for (size_t i = 0; i < m_LyricsTime.size(); i++)
    {
        if (m_fCurSoundTime > m_LyricsTime[i].fTime - 1.f)        //2초 전에 가사를 미리 띄운다.
        {
            if (i != 0)
            {
                if (i == 5)
                {
                    m_Lyrics->Show_On(i - 1);
                    _vector vPos = XMVectorSetW(XMLoadFloat3(&m_LyricsSocket[m_LyricsTime[i].iSocketIndex]), 1.f);
                    m_Lyrics->Get_PartObject(i - 1)->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
                    Setting_BackUI(m_LyricsTime[i], vPos, i - 1);

                    m_Lyrics->Show_On(i);
                    vPos = XMVectorSetW(XMLoadFloat3(&m_LyricsSocket[m_LyricsTime[i + 1].iSocketIndex]), 1.f);
                    m_Lyrics->Get_PartObject(i)->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
                    Setting_BackUI(m_LyricsTime[i + 1], vPos, i);
                }
                else if(i != 6)
                {
                    m_Lyrics->Show_On(i - 1);

                    _vector vPos = XMVectorSetW(XMLoadFloat3(&m_LyricsSocket[m_LyricsTime[i].iSocketIndex]), 1.f);
                    m_Lyrics->Get_PartObject(i - 1)->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);

                    Setting_BackUI(m_LyricsTime[i], vPos, i - 1);
                }
            }
            else
            {
                _vector vPos = XMVectorSetW(XMLoadFloat3(&m_LyricsSocket[m_LyricsTime[i].iSocketIndex]), 1.f);
                Setting_BackUI(m_LyricsTime[i], vPos, i - 1);
            }
        }

        if (m_LyricsTime.size() - 1 > i)
        {
            if (m_fCurSoundTime > m_LyricsTime[i + 1].fTime + 1.f)        //다음가사 on 1초 뒤에 가사끄기
            {
                if (i != 0)
                {
                    if (i == 6)                         // 아이러브유는 다음 가사 꺼질 때 같이 꺼진다
                    {
                        m_Lyrics->Show_Off(i - 2);
                        m_Lyrics->Show_Off(i - 1);

                        m_pPlayUI[BACK][m_LyricsTime[i - 1].iSocketIndex]->Show_Off_All();
                        m_pPlayUI[BACK][m_LyricsTime[i].iSocketIndex]->Show_Off_All();
                        m_pPlayUI[BLUE][m_LyricsTime[i-1].iSocketIndex]->Show_Off_All();
                        m_pPlayUI[BLUE][m_LyricsTime[i].iSocketIndex]->Show_Off_All();
                    }
                    if (i != 5)
                    {
                        m_Lyrics->Show_Off(i - 1);

                        m_pPlayUI[BACK][m_LyricsTime[i].iSocketIndex]->Show_Off_All();
                        m_pPlayUI[BLUE][m_LyricsTime[i].iSocketIndex]->Show_Off_All();
                    }
                }
                else
                {
                    m_pPlayUI[BACK][m_LyricsTime[i].iSocketIndex]->Show_Off_All();
                    m_pPlayUI[BLUE][m_LyricsTime[i].iSocketIndex]->Show_Off_All();
                }
            }
        }
        else
        {
            // 마지막 소절 직전이라면 마지막 가사 꺼질 때 같이 꺼진다.
            if (i == m_LyricsTime.size() - 1)
            {
                if (m_fCurSoundTime > (m_LyricsTime[m_LyricsTime.size() - 1].fTime + m_LyricsTime[m_LyricsTime.size() - 1].fDuration))
                {
                    m_Lyrics->Show_Off(i - 1);
                    m_pPlayUI[BACK][m_LyricsTime[i].iSocketIndex]->Show_Off_All();
                    m_pPlayUI[BLUE][m_LyricsTime[i].iSocketIndex]->Show_Off_All();
                }
            }
        }
    }
}

void CUIKaraoke_Play::Setting_BackUI(LYRICS_DESC Desc, _fvector vPos, _uint iLyricsIndex)
{
    // 1. 흰색 바의 스케일 조정
    _float3 vScaled = m_pPlayUI[BACK][Desc.iSocketIndex]->Get_TransformCom()->Get_Scaled();
    m_pPlayUI[BACK][Desc.iSocketIndex]->Get_TransformCom()->Set_Scale(Desc.fDuration * 0.2f, vScaled.y, vScaled.z);

    // 2. 텍스트의 위치와 스케일 가져오기
    _float3 vLyricsScaled = m_Lyrics->Get_PartObject(0)->Get_TransformCom()->Get_Scaled();
    _vector vLyricsPos = vPos;
    //_vector vLyricsPos = m_Lyrics->Get_PartObject(iLyricsIndex)->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

    // 3. 흰색 바의 위치를 텍스트의 왼쪽 시작 부분에 맞추기
    // 텍스트의 시작 위치와 스케일을 고려하여 흰색 바의 X 위치를 조정
    _float3 vScaled2 = m_pPlayUI[BACK][Desc.iSocketIndex]->Get_PartObject(1)->Get_TransformCom()->Get_Scaled();

    _vector vBarPos = vLyricsPos;
    vBarPos = XMVectorSetY(vBarPos, vBarPos.m128_f32[1] - 60.f);
    vBarPos.m128_f32[0] = vLyricsPos.m128_f32[0] - (vLyricsScaled.x * 0.5f) + (vScaled2.x * 0.5f * Desc.fDuration * 0.2f);
    vBarPos.m128_f32[0] += 20.f;

    // 4. 흰색 바의 위치 설정
    m_pPlayUI[BACK][Desc.iSocketIndex]->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vBarPos);
    m_pPlayUI[BACK][Desc.iSocketIndex]->Show_On_All();

    Setting_BlueUI(Desc, vBarPos, iLyricsIndex);
}

void CUIKaraoke_Play::Setting_BlueUI(LYRICS_DESC Desc, _fvector vPos, _uint iLyricsIndex)
{
    if (-1 != m_iCurLyricsIndex)
    {
        _float fCurrentPos = m_fCurSoundTime - m_LyricsTime[m_iCurLyricsIndex].fTime;
        _float fRatio = (fCurrentPos / m_LyricsTime[m_iCurLyricsIndex].fDuration);

        _float fSize = LerpFloat(0.f, (m_LyricsTime[m_iCurLyricsIndex].fDuration * 0.2f), fRatio);

        // 1. 파란색바의 스케일 조정
        _float3 vScaled = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Get_Scaled();
        m_pPlayUI[BLUE][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Set_Scale(fSize, vScaled.y, vScaled.z);

        _float a = fSize * 0.5f * (m_pPlayUI[BLUE][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(1)->Get_TransformCom()->Get_Scaled().x);
        _float b = fSize * 0.5f * (m_pPlayUI[BLUE][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(0)->Get_TransformCom()->Get_Scaled().x
            + m_pPlayUI[BLUE][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(2)->Get_TransformCom()->Get_Scaled().x);

        _vector vCurBarPos = m_pPlayUI[CURRENTBAR].front()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
        vCurBarPos.m128_f32[0] -= (a + b);

        m_pPlayUI[BLUE][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vCurBarPos);
        m_pPlayUI[BLUE][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Show_On_All();
    }
 
}

void CUIKaraoke_Play::CurrentBar_Control()
{
    if ((m_LyricsTime[m_LyricsTime.size() - 1].fTime + m_LyricsTime[m_LyricsTime.size() - 1].fDuration) <= m_fCurSoundTime)
    {
        m_pPlayUI[CURRENTBAR].front()->Show_Off_All();
        return;
    }

    if (-1 < m_iCurLyricsIndex)
    {
        _vector vPos = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
        _vector vEndPos = vPos;
        
        // 시작 위치 잡기
        _float3 vScaled_Center = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(1)->Get_TransformCom()->Get_Scaled();
        _float3 vScaled_Front = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(0)->Get_TransformCom()->Get_Scaled();
        _float3 vScaled_Back = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(2)->Get_TransformCom()->Get_Scaled();

        // 해당 그룹 전체에 적용된 크기를 가져와서 적용해줘야한다.
        _float3 vScaled_All = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Get_Scaled();

        vPos.m128_f32[0] -= (vScaled_Center.x * 0.5f * vScaled_All.x + vScaled_Front.x * vScaled_All.x);
        vEndPos.m128_f32[0] += (vScaled_Center.x * 0.5f * vScaled_All.x + vScaled_Back.x * vScaled_All.x);

        _float fCurrentPos = m_fCurSoundTime - m_LyricsTime[m_iCurLyricsIndex].fTime;

        vPos.m128_f32[0] = LerpFloat(vPos.m128_f32[0], vEndPos.m128_f32[0], (fCurrentPos / m_LyricsTime[m_iCurLyricsIndex].fDuration));
        
        m_pPlayUI[CURRENTBAR].front()->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
        m_pPlayUI[CURRENTBAR].front()->Show_On_All();
    }
}

void CUIKaraoke_Play::Show_Notes()
{
    if (nullptr != m_pNotes)
    {
        for (auto& pNote : *m_pNotes)
        {
            // 0 일반노드    1 유지 노드     2 연타 노드
            switch (pNote->Get_Type())
            {
            case 0:
            {
                Verse_On_SingleNote(pNote);

                break;
            }
            case 1:
            {

                break;
            }
            case 2:
            {

                break;
            }
            default:
                break;
            }
            
        }
    }
}

void CUIKaraoke_Play::Verse_On_SingleNote(CNoteBase* pNote)
{
    if (0 > m_iCurLyricsIndex) return;

    _float fVerseStartTime = m_LyricsTime[m_iCurLyricsIndex].fTime;
    _float fVerseEndTime = m_LyricsTime[m_iCurLyricsIndex].fTime + m_LyricsTime[m_iCurLyricsIndex].fDuration;

    if (fVerseStartTime <= pNote->Get_StartTime() && fVerseEndTime > pNote->Get_StartTime())
    {
        switch (pNote->Get_ButtonType())
        {
        case 0:     //Up
        {
            m_pPlayUI[UP].front()->Show_On_All();
            _float fRatio = ((pNote->Get_StartTime() - fVerseStartTime) / m_LyricsTime[m_iCurLyricsIndex].fDuration);
            
            _vector vPos = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
            _vector vEndPos = vPos;

            // 시작 위치 잡기
            _float3 vScaled_Center = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(1)->Get_TransformCom()->Get_Scaled();
            _float3 vScaled_Front = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(0)->Get_TransformCom()->Get_Scaled();
            _float3 vScaled_Back = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(2)->Get_TransformCom()->Get_Scaled();

            // 해당 그룹 전체에 적용된 크기를 가져와서 적용해줘야한다.
            _float3 vScaled_All = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Get_Scaled();

            vPos.m128_f32[0] -= (vScaled_Center.x * 0.5f * vScaled_All.x + vScaled_Front.x * vScaled_All.x);
            vEndPos.m128_f32[0] += (vScaled_Center.x * 0.5f * vScaled_All.x + vScaled_Back.x * vScaled_All.x);

            vPos.m128_f32[0] = LerpFloat(vPos.m128_f32[0], vEndPos.m128_f32[0], fRatio);

            m_pPlayUI[UP].front()->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);

            break;
        }
        case 1:     //Down
        {
            m_pPlayUI[DOWN].front()->Show_On_All();

            _float fRatio = ((pNote->Get_StartTime() - fVerseStartTime) / m_LyricsTime[m_iCurLyricsIndex].fDuration);

            _vector vPos = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
            _vector vEndPos = vPos;

            // 시작 위치 잡기
            _float3 vScaled_Center = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(1)->Get_TransformCom()->Get_Scaled();
            _float3 vScaled_Front = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(0)->Get_TransformCom()->Get_Scaled();
            _float3 vScaled_Back = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(2)->Get_TransformCom()->Get_Scaled();

            // 해당 그룹 전체에 적용된 크기를 가져와서 적용해줘야한다.
            _float3 vScaled_All = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Get_Scaled();

            vPos.m128_f32[0] -= (vScaled_Center.x * 0.5f * vScaled_All.x + vScaled_Front.x * vScaled_All.x);
            vEndPos.m128_f32[0] += (vScaled_Center.x * 0.5f * vScaled_All.x + vScaled_Back.x * vScaled_All.x);

            vPos.m128_f32[0] = LerpFloat(vPos.m128_f32[0], vEndPos.m128_f32[0], fRatio);

            m_pPlayUI[DOWN].front()->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
            break;
        }
        case 2:     //Left
        {
            m_pPlayUI[LEFT].front()->Show_On_All();
            _float fRatio = ((pNote->Get_StartTime() - fVerseStartTime) / m_LyricsTime[m_iCurLyricsIndex].fDuration);

            _vector vPos = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
            _vector vEndPos = vPos;

            // 시작 위치 잡기
            _float3 vScaled_Center = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(1)->Get_TransformCom()->Get_Scaled();
            _float3 vScaled_Front = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(0)->Get_TransformCom()->Get_Scaled();
            _float3 vScaled_Back = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(2)->Get_TransformCom()->Get_Scaled();

            // 해당 그룹 전체에 적용된 크기를 가져와서 적용해줘야한다.
            _float3 vScaled_All = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Get_Scaled();

            vPos.m128_f32[0] -= (vScaled_Center.x * 0.5f * vScaled_All.x + vScaled_Front.x * vScaled_All.x);
            vEndPos.m128_f32[0] += (vScaled_Center.x * 0.5f * vScaled_All.x + vScaled_Back.x * vScaled_All.x);

            vPos.m128_f32[0] = LerpFloat(vPos.m128_f32[0], vEndPos.m128_f32[0], fRatio);

            m_pPlayUI[LEFT].front()->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);

            break;
        }
        case 3:     //Right
        {
            m_pPlayUI[RIGHT].front()->Show_On_All();

            _float fRatio = ((pNote->Get_StartTime() - fVerseStartTime) / m_LyricsTime[m_iCurLyricsIndex].fDuration);

            _vector vPos = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
            _vector vEndPos = vPos;

            // 시작 위치 잡기
            _float3 vScaled_Center = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(1)->Get_TransformCom()->Get_Scaled();
            _float3 vScaled_Front = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(0)->Get_TransformCom()->Get_Scaled();
            _float3 vScaled_Back = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_PartObject(2)->Get_TransformCom()->Get_Scaled();

            // 해당 그룹 전체에 적용된 크기를 가져와서 적용해줘야한다.
            _float3 vScaled_All = m_pPlayUI[BACK][m_LyricsTime[m_iCurLyricsIndex].iSocketIndex]->Get_TransformCom()->Get_Scaled();

            vPos.m128_f32[0] -= (vScaled_Center.x * 0.5f * vScaled_All.x + vScaled_Front.x * vScaled_All.x);
            vEndPos.m128_f32[0] += (vScaled_Center.x * 0.5f * vScaled_All.x + vScaled_Back.x * vScaled_All.x);

            vPos.m128_f32[0] = LerpFloat(vPos.m128_f32[0], vEndPos.m128_f32[0], fRatio);

            m_pPlayUI[RIGHT].front()->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
            break;
        }
        default:
            break;
        }

    }
}

_uint CUIKaraoke_Play::Compute_Num(_uint iCount)
{
    switch (iCount)
    {
    case BACK:
        return 2;
    case BLUE:
        return 2;
    case MIC:
        return 20;
    case CURRENTBAR:
        return 0;
    case GOODEFFECT:
        return 20;
    case GRADE:
        return 20;
    case GREATEFFECT:
        return 20;
    case HOLD:
        return 5;
    case ROLL:
        return 5;
    case DOWN:
        return 30;
    case LEFT:
        return 30;
    case RIGHT:
        return 30;
    case UP:
        return 30;
    default:
        return 0;
    }
}

CUIKaraoke_Play* CUIKaraoke_Play::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CUIKaraoke_Play* pInstance = new CUIKaraoke_Play();
    if (FAILED(pInstance->Initialize(pDevice, pContext, pArg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CUIKaraoke_Play::Free()
{
    __super::Free();
    for (size_t i = 0; i < UILIST_END; i++)
    {
        for (auto& iter : m_pPlayUI[i])
            Safe_Release(iter);

        m_pPlayUI[i].clear();
    }

    Safe_Release(m_Lyrics);
    //Safe_Release(m_Title);
}
