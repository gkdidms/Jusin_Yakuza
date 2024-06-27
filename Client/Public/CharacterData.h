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
    HRESULT Initialize(wstring wstrModelName);

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
    const vector<ANIMATION_EVENT>& Get_CurrentEvents() const {
        return m_CurrentEvents;
    }

public:
    void Set_CurrentAnimation(string strAnimName);
    
private:
    CGameInstance* m_pGameInstance = { nullptr };

    // �� ��ü������ ���� �ε���
    vector<_uint> m_AlphaMeshes;
    vector<_uint> m_LoopAnimations;

    // first: �ִϸ��̼� �̸�
    multimap<string, ANIMATION_EVENT> m_AnimationEvents;            
    // first: ���ε���
    unordered_map<_uint, COLLIDER_STATE> m_Colliders;

    vector<ANIMATION_EVENT> m_CurrentEvents;

public:
    static CCharacterData* Create(wstring wstrModelName);
    virtual void Free() override;
};
END
