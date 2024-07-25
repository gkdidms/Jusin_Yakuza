#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CAStart :
    public CComponent
{
private:
    CAStart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CAStart(const CAStart& rhs);
    virtual ~CAStart() = default;

public:
    list<CCell*>& Get_BestList() { return m_BestList; }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    void Start_Root(class CNavigation* pNavi, _fvector vGoalPos);
    
#ifdef _DEBUG
public:
    virtual HRESULT Render() override;
#endif // _DEBUG

private:
    list<_uint> m_OpenList;
    list<_uint> m_CloseList;
    list<CCell*> m_BestList;

#ifdef _DEBUG
private:
    class CShader* m_pShaderCom = { nullptr };
    class CVIBuffer_Line* m_pVIBufferCom = { nullptr };

private:
    _float4x4 m_WorldMatrix = {};
#endif // _DEBUG

private:
    _bool Make_Route(_uint iStartIndex, _uint iGoalIndex, vector<class CCell*> Cells);
    void Make_BastList(_uint iStartIndex, _uint iGoalIndex, vector<class CCell*> Cells);

    _bool Check_Close(_uint iIndex);
    _bool Check_Open(_uint iIndex);

    void Data_Release();

public:
    static CAStart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free();
};
END
