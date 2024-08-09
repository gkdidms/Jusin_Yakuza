#pragma once
#include "GameObject.h"

// 엔진 뭐 필요한게 없는듯?
BEGIN(Engine)
class CAnim;
END

class CQteManager :
    public CGameObject
{
public:
    enum QTE_TYPE {
        YONEDA, CARCHASE, QTE_TYPE_END
    };

    struct QTE_DESC {
        _uint iAnimIndex;
        _uint iStartKeyFrameIndex;
        _uint iEndKeyFrameIndex;
        _uint iKeyFrames;
    };

    const _float QTE_TIME = 2.f;

private:
    CQteManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CQteManager(const CQteManager& rhs);
    virtual ~CQteManager() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    HRESULT Ready_SlowKeyFrame();

public:
    void Slowing(CAnim* pAnimCom);

private:
    _float m_fSlowSpeed = { 0.f };
    _float  m_fOutTimer = { 0.f };
    _bool   m_bButton = { false };                  // false: Q, true: E
    _bool   m_isSuccess = { false };                

    string m_strAnimationName;

    unordered_map<string, QTE_DESC> m_QTEs;

public:
    CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

