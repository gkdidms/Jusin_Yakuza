#pragma once
#include "LandObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CPlayer :
    public CLandObject
{
public:
    //KRS: ���Ѵ�, KRH: ����, KRC: �ı���
    enum BATTLE_STYLE
    {
        ADVENTURE, KRS, KRH, KRC, BATTLE_STYLE_END
    };

    /* 06.28 ���� �߰�
    *  ��Ÿ�ϸ��� ��ġ�� �ൿ�� �־ enum class ��� 
    *  �ൿ�� �ӽ÷� �Ϻθ� ��Ƶ� ���·� ���������� �߰��� ������
    */
    enum class ADVENTURE_BEHAVIOR_STATE
    {
        IDLE, Shift, RUN, ADVENTURE_BEHAVIOR_END
    };    
    enum class KRS_BEHAVIOR_STATE
    {
        IDLE, Shift, RUN, GUARD, ATTACK, HIT, KRS_BEHAVIOR_END
    };

    enum MOVE_DIRECTION
    {
        F, B, L, R, MOVE_DIRECTION_END
    };


private:
    const _float ANIM_INTERVAL = 4.f;
    
private:
    CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() = default;
    
#ifdef _DEBUG
public:
    _bool isAnimStart() { return m_isAnimStart; }

public:
    void Set_AnimStart(_bool isAnim) { m_isAnimStart = isAnim; }
#endif // DEBUG

public:
    _uint Get_BattleStyle() { return m_eCurrentStyle; }
    _bool isAttack() { return m_iCurrentBehavior == static_cast<_uint>(KRS_BEHAVIOR_STATE::ATTACK); }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual HRESULT Render_LightDepth() override;

public:
    const _bool* Get_MoveDirection() {
        return m_MoveDirection;
    }

private:
    void Ready_AnimationTree();
    void Synchronize_Root(const _float& fTimeDelta);
    void Animation_Event();
    void Move_KeyInput(const _float& fTimeDelta);

public:
    void Change_Animation(_uint iAnimIndex);

private:
    CShader*                m_pShaderCom = { nullptr };
    CModel*                 m_pModelCom = { nullptr };
    CCollider*              m_pColliderCom = { nullptr };

    //�� �ε���, ���� �ݶ��̴�
    unordered_map<_uint, class CSoketCollider*>      m_pColliders;     
    class CCharacterData*   m_pData = { nullptr };

    // �� ��, ����ϴ� Ű ���� �ൿ�� ���� Ű���� ������. (��Ÿ���� �迭 �ε���)
    map<_uint, class CBehaviorAnimation*> m_AnimationTree[BATTLE_STYLE_END];

private:
    BATTLE_STYLE    m_eCurrentStyle = { ADVENTURE };
    // ��Ÿ�ϸ��� ��ġ�� �ൿ�� ���� �� �־ int������ �����ϰ� ����ȯ�Ͽ� �����Ѵ�.
    _uint           m_iCurrentBehavior = static_cast<_uint>(ADVENTURE_BEHAVIOR_STATE::IDLE);

    _bool           m_MoveDirection[MOVE_DIRECTION_END];

    _float          m_fPrevSpeed = { 0.f };
    _float4         m_vPrevMove;
    _float4         m_vPrevRotation;
    _float4x4       m_ModelWorldMatrix;

#ifdef _DEBUG
    _bool m_isAnimStart = { true };
#endif // _DEBUG

private:
    virtual HRESULT Add_Componenets() override;
    virtual HRESULT Bind_ResourceData() override;
    HRESULT Add_CharacterData();
    void Apply_ChracterData();

public:
    static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};
END
