#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CImguiManager final:
    public CBase
{
private:
	CImguiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImguiManager() = default;

public:
	virtual void Free() override;
};
END
