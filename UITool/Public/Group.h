#pragma once
#include "UI_Object.h"

#include "UITool_Defines.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(UITool)
class CGroup :
    public CUI_Object
{
private:
    CGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGroup(const CGroup& rhs);
    virtual ~CGroup() = default;

public:
    void Set_PartObject(CUI_Object* pObject) { m_PartObjects.emplace_back(pObject); }

public:
    vector<CUI_Object*> Get_PartObjects() { return m_PartObjects; }
    CUI_Object* Get_PartObject(_uint iIndex) { return m_PartObjects[iIndex]; }

    void Remove_PartObject(_uint iIndex);
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    vector<CUI_Object*> m_PartObjects;
public:
    virtual HRESULT Save_binary(const string strDirectory)override;
    virtual HRESULT Save_Groupbinary( ofstream& out)override;
public:
    static CGroup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free();
};
END
