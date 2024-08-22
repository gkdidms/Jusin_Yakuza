#pragma once
#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)
class CLevel_Roadway :
    public CLevel
{
private:
    const _float MIN_TIME = 10.f;
    const _float MAX_TIME = 30.f;

    const _float RADIAL_TIME = 3.f;

    const _uint RADIAL_SAMPLE_MIN = 3;
    const _uint RADIAL_SAMPLE_MAX = 10;

private:
    CLevel_Roadway(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel_Roadway() = default;

public:
    virtual HRESULT Initialize();
    virtual void Tick(const _float& fTimeDelta) override;

private:
    class CSystemManager* m_pSystemManager = { nullptr };
    class CFileTotalMgr* m_pFileTotalManager = { nullptr };
    class CCarChaseManager* m_pCarChaseManager = { nullptr };
    class CUIManager* m_pUIManager = { nullptr };
    class CFightManager* m_pFightManager = { nullptr };

private:
    _bool m_isTitleEnd = { false };

    _bool m_isRadialOnEventPlay = { true };                     //����� On �̺�Ʈ Ÿ�̸Ӹ� ��������, off Ÿ�̸Ӹ� �������� ���Ѵ�.
    _float m_fRadialTimer = { 0.f };
    _float m_fRadialTime = { 0.f };

    _uint m_iRadialValue = RADIAL_SAMPLE_MIN;

private:
    void RadialOnTimer(const _float& fTimeDelta);               // ������� �������� �� Ÿ�̸�
    void RadialOffTimer(const _float& fTimeDelta);              // ����� �������� �� Ÿ�̸�

    void RadialValue_Control();

private:
    HRESULT Ready_Camera(const wstring& strLayerTag);
    HRESULT Ready_Player(const wstring& strLayerTag);

public:
    static CLevel_Roadway* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
END
