#pragma once

#include "Camera.h"

#include "Client_Defines.h"

BEGIN(Client)

class CPlayerCamera final : public CCamera
{
public:
	typedef struct FREE_CAMERA_DESC : public CCamera::CAMERA_DESC
	{
		_float		fSensor = { 0.f };
	};

private:
	CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerCamera(const CPlayerCamera& rhs);
	virtual ~CPlayerCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void			Mouse_Fix();



private:
	_float		m_fSensor = { 0.0f };
	_bool		m_bCameraFix = false;

public:
	static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END