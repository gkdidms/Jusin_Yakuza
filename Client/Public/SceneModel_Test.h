#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Decal.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CSceneModel_Test final : public CGameObject
{
private:
	CSceneModel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSceneModel_Test(const CSceneModel_Test& rhs);
	virtual ~CSceneModel_Test() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CSystemManager* m_pSystemManager = { nullptr };

private:
	_bool m_isFirst = { true };
	wstring					m_wstrModelName;

	_float fTestX = { 90.f };		//90
	_float fTestY = { -180 };		//-180
	_float fTestZ = { 90 };			//90

	_bool test = { false };

public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CSceneModel_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END