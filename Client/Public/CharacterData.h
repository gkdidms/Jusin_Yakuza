/*
* 24.06.26 ���� �߰�
*/

#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CCharacterData : public CBase
{
public:
    struct ANIMATION_EVENT
    {
        _uint iType;            //0: �ݶ��̴� Ȱ��ȭ, 1: �ݶ��̴� ��Ȱ��ȭ, 2:����, 3:����Ʈ
        _float fPlayPosition;
        _uint iBoneIndex;
        string strChannelName;
    };

    struct COLLIDER_STATE
    {   
        _int iActionType;       // ATTACK, HIT
        _int iPartType;         //HAND_A, FOOT_A, JOINT_A, HEAD_A, HEAD_H = 10, BODY_H, LEG_H
        _bool isAlways;
        _uint iType;             //(AABB, OBB, Sphere)
        _float3 vCenter;
        _float3 vValue;        // Sphere��� x���� ���� ��������.
    };


    struct ANIMATION_RIMLIGHTSTATE
    {
        _uint iType;					//0���� on 1���� off
        _float fAinmPosition;
        string strMeshName;
    };

    struct ANIMATION_TRAILSTATE
    {
        _uint iType;					//0���� on 1���� off
        _float fAinmPosition;
        string strBonelName;
        string strTrailProtoName;
        _uint iBoneIndex;
    };

    struct ANIMATION_FACEEVENTSTATE
    {
        _uint iType;					//0���� on 1���� off 2���� change
        _float fAinmPosition;
        _uint iFaceAnimIndex;
    };


    struct ANIMATION_BLOODEVENTSTATE
    {
        _float fAinmPosition;
        _uint iBoneIndex;
        string strBonelName;
        _uint iBloodEffectType;		// ��½�ų ����Ʈ�� Ÿ�� (0: ��, 1: ��, 2: ����) / 2 ���ķ� �˾Ƽ� �����ؼ� ���缭 �������
        _bool isLoop;				    // ����Ʈ ��������
        _bool isOn;				    // ����Ʈ On/Off
        _bool isPlayed = { false };				    // ����Ʈ �����߾�����

    };

    struct ANIMATION_RADIALEVENTSTATE
    {
        _uint iType;				//0���� on 1���� off
        _float fAinmPosition;
        _float fForce;
    };

    struct ANIMATION_SOUNDEVENTSTATE
    {
        _uint iChannel;
        _float fAinmPosition;
        _float fSoundVolume;
        string strSoundFileName;
        _bool isPlayed = { false };
    };


private:
    CCharacterData();
    virtual ~CCharacterData() = default;

public:
    HRESULT Initialize(class CLandObject* pCharacter);

public:
    const vector<_uint>& Get_AlphaMeshes() const {
        return m_AlphaMeshes;
    }
    const vector<_uint>& Get_LoopAnimations() const {
        return m_LoopAnimations;
    }
    const multimap<string, ANIMATION_EVENT>& Get_AnimationEvents() const {
        return m_AnimationEvents;
    }
    const unordered_map<_uint, COLLIDER_STATE>& Get_Colliders() const {
        return m_Colliders;
    }

    const multimap<string, wstring>& Get_Effets() const {
        return m_Effects;
    }

    const multimap<string, ANIMATION_TRAILSTATE>& Get_TrailEvents() const {
        return m_TrailEvents;
    }

    const multimap<string, ANIMATION_BLOODEVENTSTATE>& Get_BloodEffectEvents() const {
        return m_BloodEvents;
    }

    const multimap<string, ANIMATION_RADIALEVENTSTATE>& Get_RadialEvents() const {
        return m_RadialEvents;
    }

    const multimap<string, ANIMATION_SOUNDEVENTSTATE>& Get_SoundEvents() const {
        return m_SoundEvents;
    }

    const vector<ANIMATION_EVENT>& Get_CurrentEvents() const {
        return m_CurrentEvents;
    }

    const vector<ANIMATION_RIMLIGHTSTATE>& Get_Current_Rim_Events() const {
        return m_CurrentRimEvents;
    }

    const vector<ANIMATION_TRAILSTATE>& Get_Current_Trail_Events() const {
        return m_CurrentTrailEvents;
    }

    const vector<ANIMATION_FACEEVENTSTATE>& Get_Current_FaceEvents() const {
        return m_CurrentFaceEvents;
    }

    vector<ANIMATION_BLOODEVENTSTATE>& Get_Current_BloodEffectEvents() {
        return m_CurrentBloodEffectEvents;
    }

    const vector<ANIMATION_RADIALEVENTSTATE>& Get_Current_RadialEvents() const {
        return m_CurrentRadialEvents;
    }

    vector<ANIMATION_SOUNDEVENTSTATE*>& Get_Current_SoundEvents() {
        return m_CurrentSoundEvents;
    }

public:
    void Set_CurrentAnimation(string strAnimName);          //���� �ִϸ��̼ǿ� �´� �̺�Ʈ�� �����صд�.
    void Set_CurrentCutSceneAnimation(string strAnimName);          //���� �ִϸ��̼ǿ� �´� �̺�Ʈ�� �����صд�.

private:
    HRESULT Load_AlphaMeshes(string strFilePath);
    HRESULT Load_LoopAnimations(string strFilePath);
    HRESULT Load_AnimationEvents(string strFilePath);
    HRESULT Load_Colliders(string strFilePath);
    HRESULT Load_EffectState(string strFilePath);
    HRESULT Load_RimLightEvent(string strFilePath);
    HRESULT Load_TrailEvent(string strFilePath);
    HRESULT Load_FaceEvent(string strFilePath);
    HRESULT Load_BloodEffectEvent(string strFilePath);
    HRESULT Load_RadialEvent(string strFilePath);
    HRESULT Load_SoundEvent(string strFilePath);
    HRESULT Load_CutSceneSoundEvent(string strFilePath);

private:
    CGameInstance* m_pGameInstance = { nullptr };
    class CLandObject* m_pCharacter = { nullptr };

    // �� ��ü������ ���� �ε���
    vector<_uint> m_AlphaMeshes;
    vector<_uint> m_LoopAnimations;

    string m_iCurrentAnimName = "";

    // first: �ִϸ��̼� �̸�
    multimap<string, ANIMATION_EVENT>               m_AnimationEvents;            
    // first: ���ε���
    unordered_map<_uint, COLLIDER_STATE>            m_Colliders;
    // first �� �̸�, second ����Ʈ ���� �̸� (.dat ������ �̸�)
    multimap<string, wstring>                       m_Effects;
       
    // first: �ִϸ��̼� �̸�, second: ������Ʈ �̺�Ʈ����
    multimap<string, ANIMATION_RIMLIGHTSTATE>	    m_RimLightEvents;

    // first: �ִϸ��̼� �̸�, second: Ʈ���� �̺�Ʈ����
    multimap<string, ANIMATION_TRAILSTATE>		    m_TrailEvents;

    // first: �ִϸ��̼� �̸�, second: ����� �̺�Ʈ ����
    multimap<string, ANIMATION_FACEEVENTSTATE>		    m_FaceEvents;

    // first: �ִϸ��̼� �̸�, second: ����� �̺�Ʈ ����
    multimap<string, ANIMATION_BLOODEVENTSTATE>		    m_BloodEvents;

    // first: �ִϸ��̼� �̸�, second: ����� �̺�Ʈ ����
    multimap<string, ANIMATION_RADIALEVENTSTATE>		m_RadialEvents;

    // first: �ִϸ��̼� �̸�, second: ����� �̺�Ʈ ����
    multimap<string, ANIMATION_SOUNDEVENTSTATE>		    m_SoundEvents;

    // first: �ִϸ��̼� �̸�, second: ����� �̺�Ʈ ����
    multimap<string, ANIMATION_SOUNDEVENTSTATE>		    m_CutSceneSoundEvents;


    vector<ANIMATION_EVENT> m_CurrentEvents; 
    vector<ANIMATION_RIMLIGHTSTATE> m_CurrentRimEvents;
    vector<ANIMATION_TRAILSTATE> m_CurrentTrailEvents;
    vector<ANIMATION_FACEEVENTSTATE> m_CurrentFaceEvents;
    vector<ANIMATION_BLOODEVENTSTATE> m_CurrentBloodEffectEvents;
    vector<ANIMATION_RADIALEVENTSTATE> m_CurrentRadialEvents;
    vector<ANIMATION_SOUNDEVENTSTATE*> m_CurrentSoundEvents;

public:
    static CCharacterData* Create(class CLandObject* pCharacter);
    virtual void Free() override;
};
END
