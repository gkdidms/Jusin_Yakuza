#pragma once
#include "Component.h"

#include "Cell.h"

BEGIN(Engine)
class ENGINE_DLL CNavigation :
    public CComponent
{
public:
    typedef struct tNavigationDesc{
        _int iIndex;
    }NAVIGATION_DESC;

private:
    CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNavigation(const CNavigation& rhs);
    virtual ~CNavigation() = default;

public:
    void Set_Points(const _float3* vPoints, _int OptionType);
    void Set_Index(_uint iIndex) { m_iCurrentIndex = iIndex; }

public:
    CCell::OPTION Get_OptionType() { return m_Cells[m_iCurrentIndex]->Get_Option(); }
    _uint Get_Index() {
        return m_iCurrentIndex;
    }

public:
    virtual HRESULT Initialize_Prototype(); // Tool��
    virtual HRESULT Initialize_Prototype(const wstring strFilePath);
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick() override;

public:
    _int Find_Index(POINT ptMouse, _fvector vTargetPos, _float fZ, _fmatrix WorldMatirx, _int* pIndex = nullptr);
    _bool isMove(_fvector vMovePos);
    _float Compute_Height(_fvector vPosition);
    _bool isLook(_fvector vLook);
    _bool isFlyCell(_fvector vLook);
#ifdef _DEBUG
public:
    HRESULT Render();
    _bool isRemove(_int iIndex);
#endif // _DEBUG

private:
    class CShader* m_pShaderCom = { nullptr };
    vector<CCell*> m_Cells; // �ﰢ������ ���� ����

    _uint m_iIndexCount = { 0 };

    _float4x4 m_WorldMatrix = {};

private:
    _int m_iCurrentIndex = { -1 };

private:
    HRESULT Load_File(const wstring strFilePath);
    HRESULT SetUp_Neighbors();

public:
    static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring strFilePath);
    static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); // Tool��
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END