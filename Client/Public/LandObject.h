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
public:
    enum ANIMATION_COMPONENT_TYPE {
        DEFAULT,
        CUTSCENE,
        ANIM_TYPE_END
    };

private: //오브젝트들의 정보를 저장 
    typedef struct tLandObjectInfo {
        _float iMaxHP;
        _float iHp;
    } LAND_OBJ_INFO;

protected:
    CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLandObject(const CLandObject& rhs);
    virtual ~CLandObject() = default;

public:
    LAND_OBJ_INFO Get_Info() { return m_Info; } //객체의 정보를 저장하는 구조체 반환

#ifdef _DEBUG
public:
    void Set_ObjectRender(_bool isRender) { m_isObjectRender = isRender; }
    
public:
    _bool Get_ObjectRender() { return m_isObjectRender; }
#endif // _DEBUG

    /* Default Virtual Funtion */
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual HRESULT Render_LightDepth() override;

    /* 충돌관련 함수들 */
    virtual void ImpulseResolution(CLandObject* pTargetObject, _float fDistance = 0.5f);
    virtual void Attack_Event(CLandObject* pHitObject) {};
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) {};

    /* Virtual Funtion */
protected:
    virtual HRESULT Add_CharacterData();
    virtual void Apply_ChracterData();

    virtual void Animation_Event();
    virtual void RimLight_Event();
    virtual void Trail_Event();

    /* Fublic Virtual Funtion */
public:
    virtual string Get_CurrentAnimationName() = 0;

    virtual void Off_Attack_Colliders();                               // 어택 콜라이더만 전부 끄는 기능

    /* Getter */
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

    _float Get_DamageAmplify() {
        return m_fDamageAmplify;
    }

    /* Setter */
public:
    void Set_RimMeshName(string strMeshName) {
        m_strRimMeshName = strMeshName;
    }

    void Set_DamageAmplify(_float fAmplify) {
        m_fDamageAmplify = fAmplify;
    }

#ifdef _DEBUG
public:
    //uv 체크 디버그용
    _float2 Get_TopUV() { return m_fRimTopUV; }
    _float2 Get_BotUV() { return m_fRimBotUV; }
    _float2 Get_PartUV() { return m_fRimPartsUV; }

    void Set_TopUV(_float2 UV) { m_fRimTopUV = UV; }
    void Set_BotUV(_float2 UV) { m_fRimBotUV = UV; }
    void Set_PartUV(_float2 UV) { m_fRimPartsUV = UV; }
#endif // DEBUG

protected:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CCollision_Manager* m_pCollisionManager = { nullptr };
    CCollider* m_pColliderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };
    CShader* m_pShaderCom = { nullptr };

    // 애니메이션 툴에서 찍은 데이터 파싱해서 들고있는 객체
    class CCharacterData* m_pData = { nullptr };

protected:
    //객체의 정보 구조체 
    LAND_OBJ_INFO m_Info = {};

    // 사용할 모델의 이름 (객체 생성 이후 바뀌지 않는다)
    wstring m_wstrModelName = TEXT("");

    // 림라이트를 적용해줄 메시의 이름 (필요할 때마다 바뀌는 값)
    string m_strRimMeshName = "";

    // 데미지 증폭 값 (기본은 1배, 스킬 사용 시 마다 변경해준다.)
    _float m_fDamageAmplify = { 1.f };

protected:
    // first: 뼈 인덱스, secone: 소켓 콜라이더
    // 파싱된 콜라이더 객체를 저장할 배열
    unordered_map<_uint, class CSocketCollider*>      m_pColliders;

    //뼈 이름, 소켓 이펙트
    multimap<string, class CSocketEffect*>      m_pEffects;
    map<string, class CSocketEffect*>           m_pTrailEffects;


#ifdef _DEBUG
    _bool m_isObjectRender = { true };
#endif // _DEBUG

    // 림라이트 관련 정보
protected:
    _float m_isRimLight ;//
    _float2 m_fRimTopUV = { 0.0f , 0.3f };//0~1사이 [시작v,끝v]상의
    _float2 m_fRimBotUV = { 0.9f, 1.0f };//0~1사이 [시작v,끝v]하의
    _float2 m_fRimPartsUV = { 0.0f, 1.0f };//0~1사이 [시작v,끝v]손,발

protected:
    virtual HRESULT Add_Components();
    virtual HRESULT Bind_ResourceData();
    virtual void Compute_Height();

public:
    virtual void Free() override;
};
END
