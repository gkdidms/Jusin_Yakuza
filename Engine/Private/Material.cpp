#include "Material.h"

#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBase(),
	m_pDevice {pDevice},
	m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMaterial::Initialize(METERIAL_DESC* pDesc)
{
	memcpy(&m_Info, pDesc, sizeof(METERIAL_DESC));

	if (FAILED(Ready_Buffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMaterial::Bind_Shader(CShader* pShader)
{
	if (FAILED(pShader->Bind_RawValue("Opacity", &m_Info.fOpacity, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("fSpecularPower", &m_Info.fSpecularPower, sizeof(_float))))
		return E_FAIL;
	_vector vSpecularColor = XMLoadFloat4(&m_Info.vSpecularColor);
	if (FAILED(pShader->Bind_RawValue("vSpecularColor", &vSpecularColor, sizeof(_vector))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("AssetShader", &m_Info.fAssetShader, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("DisableRDRT", &m_Info.fDisableRDRS, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("Engine", &m_Info.fEngine, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("IsOEClothShader", &m_Info.fIsClothShader, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("SkinShader", &m_Info.fSkinShader, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("Rough", &m_Info.fRough, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("IsY3Shader", &m_Info.fIsY3Shader, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("Imperfection", &m_Info.Imperfection, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("SPShader", &m_Info.SPShader, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_isUVShader", &m_Info.isUVShader, sizeof(_bool))))
		return E_FAIL;

	if (m_Info.isUVShader)
	{
		if (FAILED(pShader->Bind_RawValue("g_fRTX", &m_Info.fRTX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fRTY", &m_Info.fRTY, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fRDRMRS_X", &m_Info.fRDRMRS_X, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fRDRMRS_Y", &m_Info.fRDRMRS_Y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("f_fImperfection_UV", &m_Info.fImperfection_UV, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("f_fEngine_UV", &m_Info.fEngine_UV, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}

void CMaterial::Bind_ComputeShader()
{
	m_pContext->CSSetConstantBuffers(2, 1, &m_pMaterialBuffer);
}

HRESULT CMaterial::Ready_Buffer()
{
	D3D11_BUFFER_DESC Desc{};
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.ByteWidth = sizeof(MATERIAL_BUFFER);
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = sizeof(MATERIAL_BUFFER);

	MATERIAL_BUFFER MaterialDesc{};
	MaterialDesc.AssetShader = m_Info.fAssetShader;
	MaterialDesc.DisableRDRT = m_Info.fDisableRDRS;
	MaterialDesc.Engine = m_Info.fEngine;
	MaterialDesc.fSpecularPower = m_Info.fSpecularPower;
	MaterialDesc.f_fEngine_UV = m_Info.fEngine_UV;
	MaterialDesc.f_fImperfection_UV = m_Info.fImperfection_UV;
	MaterialDesc.g_fRDRMRS_X = m_Info.fRDRMRS_X;
	MaterialDesc.g_fRDRMRS_Y = m_Info.fRDRMRS_Y;
	MaterialDesc.g_fRTX = m_Info.fRTX;
	MaterialDesc.g_fRTY = m_Info.fRTY;
	MaterialDesc.g_isUVShader = _int(m_Info.isUVShader);
	MaterialDesc.Imperfection = m_Info.Imperfection;
	MaterialDesc.IsOEClothShader = m_Info.fIsClothShader;
	MaterialDesc.IsY3Shader = m_Info.fIsY3Shader;
	MaterialDesc.Opacity = m_Info.fOpacity;
	MaterialDesc.Rough = m_Info.fRough;
	MaterialDesc.SkinShader = m_Info.fSkinShader;
	MaterialDesc.SPShader = m_Info.SPShader;
	MaterialDesc.vSpecularColor = XMLoadFloat4(&m_Info.vSpecularColor);

	D3D11_SUBRESOURCE_DATA ResourceDesc{};
	ResourceDesc.pSysMem = &MaterialDesc;

	if (FAILED(m_pDevice->CreateBuffer(&Desc, &ResourceDesc, &m_pMaterialBuffer)))
		return E_FAIL;

	return S_OK;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, METERIAL_DESC* pDesc)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);
	
	if (FAILED(pInstance->Initialize(pDesc)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMaterial::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pMaterialBuffer);
}
