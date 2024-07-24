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
    void Start_Root(class CNavigation* pNavi, _fvector vGoalPos);

private:
    list<_uint> m_OpenList;
    list<_uint> m_CloseList;
    list<CCell*> m_BestList;

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
