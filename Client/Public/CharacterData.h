/*
* 24.06.26 혜원 추가
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
        _float3 vValue;        // Sphere라면 x에만 값을 저장하자.
    };

private:
    CCharacterData();
    virtual ~CCharacterData() = default;

public:
    HRESULT Initialize(wstring wstrModelName);
    
private:
    CGameInstance* m_pGameInstance = { nullptr };

    // 모델 전체에서의 각기 인덱스
    vector<_uint> m_AlphaMeshes;
    vector<_uint> m_LoopAnimations;

    // first: 애니메이션 이름
    multimap<string, ANIMATION_EVENT> m_AnimationEvents;            
    // first: 뼈인덱스
    unordered_map<_uint, COLLIDER_STATE> m_Colliders;

public:
    static CCharacterData* Create(wstring wstrModelName);
    virtual void Free() override;
};
END
