#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CModel;
class CAnim;
class CNeoShader;
class CNavigation;
class CComputeShader;
END

BEGIN(Client)
class CLandObject abstract :
    public CGameObject
{
public:
    enum ANIMATION_COMPONENT_TYPE {
        DEFAULT,
        CUTSCENE,
        ADVENTURE,
        ANIM_TYPE_END
    };

    // ���ҵ� �ִϸ��̼� ������Ʈ ���� (�ʿ� �� �þ ����)
    enum SEPARATION_ANIMATION_COM_TYPE
    {
        FACE_ANIM, HAND_ANIM, DEFAULT_ANIM, PART_ANIMATION_COM_TYPE_END
    };

    /* 
    * ���� �̸�: Prototype_Component_Anim_Hand 
    * ������Ʈ �̸�: Com_Anim_Hand
    */
    // HandAnim �������� �ε����� enum���� �����ص�
    enum HAND_ANIMATION_TYPE {
        HAND_NEUTRAL,               //[0] [hand_parts_0_neutral] ��� ���ϰ� ����ִ� ����
        HAND_MIDDLE,                //[1] [hand_parts_10_hand_middle] 
        HAND_MIDDLE2,               //[2] [hand_parts_11_hand_middle2]
        HAND_MIN,                   //[3] [hand_parts_12_hand_min]
        HAND_CHOP = 8,              //[8] [hand_parts_17_chop]  ��!!! ��ų �� �� �� ���� ġ�� ������ ���� �� ������ ����̴�
        HAND_GU,                    //[9] [hand_parts_1_gu] �ָ� ��°�
        HAND_HAKO,                  //[10] [hand_parts_3_hako] ū ��ü �� �� ������ ��
        HAND_TAIKEN,                //[11] [hand_parts_4_taiken] �β��� �Ѽ� ��ü �� ��
        HAND_BOU,                   //[12] [hand_parts_5_bou] �Ϲ� �������� �� ��
        HAND_GUN = 14,              //[14] [hand_parts_7_gun] ���� �� ��
        HAND_TONFA,                 //[15] [hand_parts_8_tonfa] �������� �� �� (��������)
        HAND_MAX                    //[16] [hand_parts_9_hand_max] ���� �� ��
    };

    //�̰� ui���� �˷��� public���� ��ȯ��Ŵ
public: //������Ʈ���� ������ ���� 
    typedef struct tLandObjectInfo {
        _float fMaxHP;
        _float fHp;
    } LAND_OBJ_INFO;

    typedef struct tMapLandObj : public CGameObject::GAMEOBJECT_DESC
    {
        XMMATRIX		vStartPos;
        wstring			wstrModelName;
        int             iObjectType;
        int				iShaderPass;
        int             iNaviNum;
        int             iNaviRouteNum;
        int             iNPCDirection;

        int             iGroupMonster;
        int             iGroupNum;
    }LANDOBJ_MAPDESC;

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

    /* Default Virtual Funtion */
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual HRESULT Render_LightDepth() override;
    virtual HRESULT Render_BoneCompute() override;

    /* �浹���� �Լ��� */
    virtual void ImpulseResolution(CGameObject* pTargetObject, _float fDistance = 0.5f) override;
    virtual void Attack_Event(CGameObject* pHitObject, _bool isItem = false) override {};
    virtual void Take_Damage(_uint iHitColliderType, const _float3& vDir, _float fDamage, CLandObject* pAttackedObject, _bool isBlowAttack = false) {};

    /* Virtual Funtion */
protected:
    virtual HRESULT Add_CharacterData();
    virtual void Apply_ChracterData();

    virtual void Animation_Event();
    virtual void RimLight_Event();
    virtual void Trail_Event();
    virtual void Sound_Event();

    virtual _bool Checked_Animation_Ratio(_float fRatio);

public:
    // iHandType: 0���, 1 �޼�, 2 ������
    virtual void On_Separation_Hand(_uint iHandType = 0);           // �� �̸��� �ٸ��� �������ؼ� ����ؾ��Ѵ�
    virtual void Off_Separation_Hand(_uint iHandType = 0);
    virtual void On_Separation_Face();
    virtual void Off_Separation_Face();

    // iAnimType�� -1�̶�� �и�����, �� ���� ���̶�� �ִϸ��̼� ������Ʈ �ε����� ��������Ѵ�.
    // isExceptParent �� true��� �θ���� �и� �����Ѵ�.
    void Separation_Bone(string strBoneName, _int iAnimType = -1, _bool isExceptParent = true);
    void Separation_SingleBone(string strBoneName, _int iAnimType = -1);

    /* Fublic Virtual Funtion */
public:
    virtual string Get_CurrentAnimationName() = 0;

    virtual void Off_Attack_Colliders();                               // ���� �ݶ��̴��� ���� ���� ���
    virtual void Off_Trails();                               // ���� �ݶ��̴��� ���� ���� ���

    /* Getter */
public:
    const wstring& Get_ModelName() {
        return m_wstrModelName;
    }
    
    const wstring& Get_ScndModelName() {
        return m_wstrScndModelName;
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

    void    Set_FaceAnimIndex(_uint iFaceAnimIndex) { 
        m_iFaceAnimIndex = iFaceAnimIndex; 
    }

    void    Set_HandAnimIndex(_uint iHandAnimIndex) { 
        m_iHandAnimIndex = iHandAnimIndex;
    }

    // �� �� set
    void    Set_StartPos(XMMATRIX    vStartPos) { m_pTransformCom->Set_WorldMatrix(vStartPos); }
    void    Set_NavigationIndex(int iIndex);
    void    Set_NaviRouteIndex(int iIndex) { m_iNaviRouteNum = iIndex; }

#ifdef _DEBUG
public:
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
    CModel* m_pModelCom = { nullptr };
    CShader* m_pShaderCom = { nullptr };
    CComputeShader* m_pComputeShaderCom = { nullptr };
    CNeoShader* m_pMaterialCom = { nullptr };
    CNavigation* m_pNavigationCom = { nullptr };

    // �ִϸ��̼� ������ ���� ������ �Ľ��ؼ� ����ִ� ��ü
    class CCharacterData* m_pData = { nullptr };

protected:
    //��ü�� ���� ����ü 
    LAND_OBJ_INFO m_Info = {};

    // ����� ���� �̸� (��ü ���� ���� �ٲ��� �ʴ´�)
    wstring m_wstrModelName = TEXT("");
    wstring m_wstrScndModelName = TEXT("");

    // ������Ʈ�� �������� �޽��� �̸� (�ʿ��� ������ �ٲ�� ��)
    string m_strRimMeshName = "";

    // ������ ���� �� (�⺻�� 1��, ��ų ��� �� ���� �������ش�.)
    _float m_fDamageAmplify = { 1.f };

protected:
    // first: �� �ε���, secone: ���� �ݶ��̴�
    // �Ľ̵� �ݶ��̴� ��ü�� ������ �迭
    unordered_map<_uint, class CSocketCollider*>      m_pColliders;

    //�� �̸�, ���� ����Ʈ
    multimap<string, class CSocketEffect*>      m_pEffects;
    map<string, class CSocketEffect*>           m_pTrailEffects;
    map<string, class CSocketEffect*>           m_pBloodEffects;

    // ������ �ִϸ��̼��� ������ �ִ� ������Ʈ�� ������� �迭
    vector<CAnim*>        m_SeparationAnimComs;
    _uint           m_iHandAnimIndex = HAND_MIDDLE;
    _uint           m_iFaceAnimIndex = { 0 };

    _bool           m_isMonster = { true };        // ������ �Ľ� �� �ʿ��� ����


#ifdef _DEBUG
    _bool m_isObjectRender = { true };
#endif // _DEBUG

    // ������Ʈ ���� ����
protected:
    _float m_isRimLight ;//
    _float2 m_fRimArmUV = { 0.5f , 0.7f };//0~1���� [����v,��v]���Ǿ
    _float2 m_fRimTopUV = { 0.0f , 0.3f };//0~1���� [����v,��v]����

    _float2 m_fRimBotUV = { 0.9f, 1.0f };//0~1���� [����v,��v]����
    _float2 m_fRimPartsUV = { 0.0f, 1.0f };//0~1���� [����v,��v]��,��


protected:
        int             m_iNaviRouteNum = { 0 }; //��Ʈ
        int             m_iNPCDirection = { 0 };

protected:
    virtual HRESULT Add_Components();
    virtual HRESULT Bind_ResourceData();
    virtual void Compute_Height();
    string ExtractString(string strName);

public:
    virtual void Free() override;
};
END
