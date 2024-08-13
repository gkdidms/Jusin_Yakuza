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

    // CLandObject을(를) 통해 상속됨
    string Get_CurrentAnimationName() override;
    
    /* 출력, 행동 관련 포인터 변수들 */
private:
    CAnim* m_pAnimCom = { nullptr };                // 컷신 돌릴 때 사용하는 애니메이션 컴포넌트
    CModel* m_pCameraModel = { nullptr };           // 카메라 돌릴 때 쓰는 모델 (랜더하지않음)

    //ui
    class CUIManager* m_pUIManager = { nullptr };
    
#ifdef _DEBUG
    class CDebugManager* m_pDebugManager = { nullptr };
#endif // _DEBUG


    /* 애니메이션 관련 */
private:
    ANIMATION_COMPONENT_TYPE    m_eAnimComType = { DEFAULT };

    _uint                       m_iCutSceneAnimIndex = { 0 };
    _uint                       m_iCutSceneCamAnimIndex = { 0 };

    _uint                       m_iDefaultAnimIndex = { 0 };

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
