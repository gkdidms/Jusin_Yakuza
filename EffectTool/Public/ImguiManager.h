#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Effect.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CVIBuffer_Instance;
END

BEGIN(Client)
class CImguiManager final:
    public CBase
{
private:
	CImguiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImguiManager() = default;

public:
	HRESULT Initialize(void* pArg=nullptr) ;
	void Tick(const _float& fTimeDelta) ;
	HRESULT Render() ;

private:
	void EditTransform(_float* cameraView, _float* cameraProjection, _float* matrix);
	void Guizmo(_float fTimeDelta);
	HRESULT Create_Particle(_uint iType);
	HRESULT Edit_Particle();

	void Create_Tick(_float fTimeDelta);
	void Editor_Tick(_float fTimeDelta);
	void Guizmo_Tick(_float fTimeDelta);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	//파티클 액션에 대한 bool 값.
	_bool m_bSpread = { false };
	_bool m_bDrop = { false };
	_bool m_bGuizmo = { false };
	//생성 파티클 담는 곳
	CGameObject* m_EditParticle = { nullptr };
	//CVIBuffer_Instance::INSTANCE_DESC m_InstaceDesc = {};//초기값
	CEffect::EFFECT_DESC m_EffectDesc = {};

public:
	static CImguiManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END
