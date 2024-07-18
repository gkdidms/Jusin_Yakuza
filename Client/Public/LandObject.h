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
private: //������Ʈ���� ������ ���� 
    typedef struct tLandObjectInfo {
        _float iMaxHP;
        _float iHp;
    } LAND_OBJ_INFO;

protected:
    CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLandObject(const CLandObject& rhs);
    virtual ~CLandObject() = default;

public:
    LAND_OBJ_INFO Get_Info() { return m_Info; } //��ü�� ������ �����ϴ� ����ü ��ȯ

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

    /* �浹���� �Լ��� */
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

    virtual _bool Is_BlowAttack() { return false; }             // ������ �ʿ�

    virtual string Get_CurrentAnimationName() = 0;

    virtual void Off_Attack_Colliders();                               // ���� �ݶ��̴��� ���� ���� ���
 
#ifdef _DEBUG
    //uv üũ ����׿�
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

    //��ü�� ���� ����ü 
    LAND_OBJ_INFO m_Info = {};

    // �ִϸ��̼� ������ ���� ������ �Ľ��ؼ� ����ִ� ��ü
    class CCharacterData* m_pData = { nullptr };

    // first: �� �ε���, secone: ���� �ݶ��̴�
    // �Ľ̵� �ݶ��̴� ��ü�� ������ �迭
    unordered_map<_uint, class CSocketCollider*>      m_pColliders;

    //�� �̸�, ���� ����Ʈ
    multimap<string, class CSocketEffect*>      m_pEffects;

    wstring m_wstrModelName = TEXT("");

#ifdef _DEBUG
    _bool m_isObjectRender = { true };
#endif // _DEBUG

    _float m_isRimLight ;//
    _float2 m_fRimTopUV = { 0.0f , 0.3f };//0~1���� [����v,��v]����
    _float2 m_fRimBotUV = { 0.9f, 1.0f };//0~1���� [����v,��v]����
    _float2 m_fRimPartsUV = { 0.0f, 1.0f };//0~1���� [����v,��v]��,��
protected:
    virtual HRESULT Add_Components();
    virtual HRESULT Bind_ResourceData();
    virtual void Compute_Height();

public:
    virtual void Free() override;
};
END
