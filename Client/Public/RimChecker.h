#pragma once
#include "SocketObject.h"
#include "CharacterData.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Instance_Rect;
class CTexture;
END

BEGIN(Client)
class CRimChecker :
    public CSocketObject
{
public:
	typedef struct tRim_Checker_DESC : public SOCKETOBJECT_DESC
	{
		const _float4x4* pNeckMatrix;
		const _float4x4* pLHandMatrix;
		const _float4x4* pRHandMatrix;
		const _float4x4* pLFootMatrix;
		const _float4x4* pRFootMatrix;
	}RIM_CHECKER_DESC;

private:
	CRimChecker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRimChecker(const CRimChecker& rhs);
	virtual ~CRimChecker() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Instance_Rect* m_pVIBufferCom = { nullptr };


public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CRimChecker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
