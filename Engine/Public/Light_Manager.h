#pragma once
#include "Base.h"

BEGIN(Engine)
class CLight_Manager :
    public CBase
{
private:
    CLight_Manager();
    virtual ~CLight_Manager() = default;

public:
    const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;

public:
    HRESULT Initialize();
    HRESULT Add_Light(const LIGHT_DESC& LightDesc);
    HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public:
    void    Edit_Light(int  iLightIndex, LIGHT_DESC lightDesc);
    void    Delete_Light(int iLightIndex);
    void    Delete_AllLights();
    
private:
    vector<class CLight*> m_Lights;

public:
    static CLight_Manager* Create();
    virtual void Free();
};
END
