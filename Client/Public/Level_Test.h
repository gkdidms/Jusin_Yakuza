#pragma once
#include "Level.h"

#include "Client_Defines.h"

BEGIN(Client)
class CLevel_Test :
    public CLevel
{
private:
    CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel_Test() = default;

public:
    virtual HRESULT Initialize();
    virtual void Tick(const _float& fTimeDelta) override;

private:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CClient_MapDataMgr* m_pClientMapDataMgr = { nullptr };

private:
    HRESULT Ready_Light();
    HRESULT Ready_Camera(const wstring& strLayerTag);
    HRESULT Ready_Player(const wstring& strLayerTag);
    HRESULT Ready_Map(const wstring& strLayerTag);

public:
    static CLevel_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};
END
