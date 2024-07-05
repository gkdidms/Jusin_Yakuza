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
        _uint iType;            //0: 콜라이더 활성화, 1: 콜라이더 비활성화, 2:사운드, 3:이펙트
        _float fPlayPosition;
        _uint iBoneIndex;
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
    void Set_CurrentAnimation(string strAnimName);          //현재 애니메이션에 맞는 이벤트를 설정해둔다.

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

    // 모델 전체에서의 각기 인덱스
    vector<_uint> m_AlphaMeshes;
    vector<_uint> m_LoopAnimations;

    // first: 애니메이션 이름
    multimap<string, ANIMATION_EVENT> m_AnimationEvents;            
    // first: 뼈인덱스
    unordered_map<_uint, COLLIDER_STATE> m_Colliders;
    // first 뼈 이름, second 이펙트 파일 이름 (.dat 제거한 이름)
    multimap<string, wstring> m_Effects;

    vector<ANIMATION_EVENT> m_CurrentEvents; 

public:
    static CCharacterData* Create(class CLandObject* pCharacter);
    virtual void Free() override;
};
END
