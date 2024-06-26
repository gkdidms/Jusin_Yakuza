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
        _uint iType;
        _float fPlayPosition;
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
    
private:
    CGameInstance* m_pGameInstance = { nullptr };

    // �� ��ü������ ���� �ε���
    vector<_uint> m_AlphaMeshes;
    vector<_uint> m_LoopAnimations;

    // first: �ִϸ��̼� �̸�
    multimap<string, ANIMATION_EVENT> m_AnimationEvents;            
    // first: ���ε���
    unordered_map<_uint, COLLIDER_STATE> m_Colliders;

public:
    static CCharacterData* Create(wstring wstrModelName);
    virtual void Free() override;
};
END
