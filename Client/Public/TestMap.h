#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)
class CTestMap :
    public CGameObject
{
private:
    CTestMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTestMap(const CTestMap& rhs);
    virtual ~CTestMap() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };

private:
    virtual HRESULT Add_Componenets();
    virtual HRESULT Bind_ResourceData();

public:
    static CTestMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;
};
END
