#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CModel;
END

BEGIN(Client)
class CLandObject abstract :
    public CGameObject
{
protected:
    CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLandObject(const CLandObject& rhs);
    virtual ~CLandObject() = default;
    

#ifdef _DEBUG
public:
    void Set_ObjectRender(_bool isRender) { m_isObjectRender = isRender; }
    
public:
    _bool Get_ObjectRender() { return m_isObjectRender; }
#endif // _DEBUG

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

    /* 충돌관련 함수들 */
    virtual void ImpulseResolution(CLandObject* pTargetObject);
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) {};

protected:
    virtual HRESULT Add_CharacterData();
    virtual void Apply_ChracterData();

    virtual void Animation_Event();

public:
    const wstring& Get_ModelName() {
        return m_wstrModelName;
    }

    CCollider* Get_Collider() {
        return m_pColliderCom;
    }

    const unordered_map<_uint, class CSocketCollider*>& Get_Colliders() {
        return m_pColliders;
    }

    virtual _bool Is_BlowAttack() { return false; }             // 재정의 필요

    virtual string Get_CurrentAnimationName() = 0;
 
protected:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CCollision_Manager* m_pCollisionManager = { nullptr };
    CCollider* m_pColliderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };

    // 애니메이션 툴에서 찍은 데이터 파싱해서 들고있는 객체
    class CCharacterData* m_pData = { nullptr };

    // first: 뼈 인덱스, secone: 소켓 콜라이더
    // 파싱된 콜라이더 객체를 저장할 배열
    unordered_map<_uint, class CSocketCollider*>      m_pColliders;

    //뼈 이름, 소켓 이펙트
    unordered_map<string, class CSocketEffect*>      m_pEffects;

    wstring m_wstrModelName = TEXT("");

#ifdef _DEBUG
    _bool m_isObjectRender = { true };
#endif // _DEBUG


protected:
    virtual HRESULT Add_Components();
    virtual HRESULT Bind_ResourceData();

public:
    virtual void Free() override;
};
END
