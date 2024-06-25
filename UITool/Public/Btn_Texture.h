#pragma once
#include "UI_Texture.h"


BEGIN(UITool)
class CBtn_Texture :
    public CUI_Texture
{
private:
    CBtn_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBtn_Texture(const CBtn_Texture& rhs);
    virtual ~CBtn_Texture() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    static CBtn_Texture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
