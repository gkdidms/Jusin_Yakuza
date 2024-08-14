#pragma once
#include "LandObject.h"
#include "Navigation.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CNavigation;
class CAnim;
END

BEGIN(Client)
class CKaraoke_Kiryu :
    public CLandObject
{
public:
    enum MOUTH_ANIM
    {
        CLOSE, OPEN, MOUTH_ANIM_END
    };
private:
    const _float ANIM_INTERVAL = 4.f;
    
private:
    CKaraoke_Kiryu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CKaraoke_Kiryu(const CKaraoke_Kiryu& rhs);
    virtual ~CKaraoke_Kiryu() = default;
    
    /* Virtual Funtion */
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

    // CLandObject��(��) ���� ��ӵ�
    string Get_CurrentAnimationName() override;

public:
    void Set_CutSceneAnim();
    void Play_CutScene(const _float& fTimeDelta);
    void Reset_CutSceneEvent();

private:
    void Ready_SingingInterval();

    void SingOff() {
        m_isSinging = false;
        m_iFaceAnimIndex = CLOSE;
    }
    void Play_SingingAnim(const _float& fTimeDelta);

    void Change_MouthAnim();
    
    /* ���, �ൿ ���� ������ ������ */
private:
    CAnim* m_pAnimCom = { nullptr };                // �ƽ� ���� �� ����ϴ� �ִϸ��̼� ������Ʈ
    CModel* m_pCameraModel = { nullptr };           // ī�޶� ���� �� ���� �� (������������)

    //ui
    class CUIManager* m_pUIManager = { nullptr };
    
#ifdef _DEBUG
    class CDebugManager* m_pDebugManager = { nullptr };
#endif // _DEBUG


    /* �ִϸ��̼� ���� */
private:
    ANIMATION_COMPONENT_TYPE    m_eAnimComType = { DEFAULT };

    _uint                       m_iCutSceneAnimIndex = { 0 };
    _uint                       m_iCutSceneCamAnimIndex = { 0 };

    _uint                       m_iDefaultAnimIndex = { 0 };

    _float                      m_fPrevSpeed = { 0.f };
    _float4                     m_vPrevMove;
    _float4                     m_vPrevRotation;
    _float4x4                   m_ModelWorldMatrix;

    list<_float>                m_fMouthChangeInterval;

    _bool                       m_isSinging = { false };
    _float                      m_fMouthTimer = { 0.f };

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Bind_ResourceData() override;
    virtual void Compute_Height() override;

public:
    static CKaraoke_Kiryu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;


};
END
