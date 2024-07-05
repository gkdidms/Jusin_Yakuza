#pragma once
#include "UI_Texture.h"

#include "UITool_Defines.h"

BEGIN(UITool)
class CImage_Texture :
    public CUI_Texture
{
private:
    CImage_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CImage_Texture(const CImage_Texture& rhs);
    virtual ~CImage_Texture() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    virtual HRESULT Save_binary(const string strDirectory)override;
    virtual HRESULT Save_Groupbinary( ofstream& out)override;
    virtual HRESULT Load_binary()override;

public:
    static CImage_Texture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
