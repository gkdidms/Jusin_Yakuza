#pragma once
#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Client)
class CUI :
    public CGameObject
{
private:
    CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI(const CUI& rhs);
    virtual ~CUI() = default;

public:
    virtual void Free();
};
END
