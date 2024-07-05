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
        _uint iType;             //(AABB, OBB, Sphere)
        _float3 vCenter;
        _float3 vValue;        // Sphere��� x���� ���� ��������.
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

    const vector<ANIMATION_EVENT>& Get_CurrentEvents() const {
        return m_CurrentEvents;
    }

public:
    void Set_CurrentAnimation(string strAnimName);          //���� �ִϸ��̼ǿ� �´� �̺�Ʈ�� �����صд�.

private:
    HRESULT Load_AlphaMeshes(string strFilePath);
    HRESULT Load_LoopAnimations(string strFilePath);
    HRESULT Load_AnimationEvents(string strFilePath);
    HRESULT Load_Colliders(string strFilePath);
    HRESULT Load_EffectState(string strFilePath);

private:
    void Create_Effect(string& strBoneName, string& strEffectName);
    
private:
    CGameInstance* m_pGameInstance = { nullptr };
    class CLandObject* m_pCharacter = { nullptr };

    // �� ��ü������ ���� �ε���
    vector<_uint> m_AlphaMeshes;
    vector<_uint> m_LoopAnimations;

    // first: �ִϸ��̼� �̸�
    multimap<string, ANIMATION_EVENT> m_AnimationEvents;            
    // first: ���ε���
    unordered_map<_uint, COLLIDER_STATE> m_Colliders;
    // first �� �̸�, second ����Ʈ ���� �̸� (.dat ������ �̸�)
    multimap<string, wstring> m_Effects;

    vector<ANIMATION_EVENT> m_CurrentEvents; 

public:
    static CCharacterData* Create(class CLandObject* pCharacter);
    virtual void Free() override;
};
END
