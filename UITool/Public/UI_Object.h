#pragma once
#include "GameObject.h"

#include "UITool_Defines.h"

BEGIN(UITool)
class CUI_Object abstract :
    public CGameObject
{
public:
    typedef struct tUIObjectDesc : public CGameObject::GAMEOBJECT_DESC {
        string strName;
        _uint iTypeIndex;
    } UI_OBJECT_DESC;

protected:
    CUI_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_Object(const CUI_Object& rhs);
    virtual ~CUI_Object() = default;

public:
    void Set_Name(string strName) { m_strName = strName; }

public:
    string Get_Name() { return m_strName; }
    _int Get_TypeIndex() { return m_iTypeIndex; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Tick(const _float& fTimeDelta) override;
    virtual void Tick(const _float& fTimeDelta) override;
    virtual void Late_Tick(const _float& fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    virtual HRESULT Save_binary(const string strDirectory);
    virtual HRESULT Save_Groupbinary(ofstream& out);
    virtual HRESULT Load_binary();
protected:
    string m_strName = { "" }; // 저장될 이름
    _uint m_iTypeIndex = { 0 }; // 오브젝트 타입

public:
    virtual void Free();
};
END
