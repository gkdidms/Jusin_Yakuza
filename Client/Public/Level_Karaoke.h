#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)
class CLevel_Karaoke :
    public CLevel
{
private:
    CLevel_Karaoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel_Karaoke() = default;

public:
    virtual HRESULT Initialize();
    virtual void Tick(const _float& fTimeDelta) override;

private:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CFileTotalMgr* m_pFileTotalManager = { nullptr };
    class CQuestManager* m_pQuestManager = { nullptr };
    class CKaraokeManager* m_pKaraokeManager = { nullptr };

private:
    HRESULT Ready_Camera(const wstring& strLayerTag);
    HRESULT Ready_Player(const wstring& strLayerTag);

public:
    static CLevel_Karaoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
END
