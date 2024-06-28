#include "Btn.h"

#include "GameInstance.h"

CBtn::CBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext}
{
}

CBtn::CBtn(const CBtn& rhs)
	: CUI_Texture{ rhs }
{
}

HRESULT CBtn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBtn::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return E_FAIL;

	BTN_DESC* pDesc = static_cast<BTN_DESC*>(pArg);
	m_strClickFilePath = pDesc->strClickFilePath;
	m_StrClickFileName = pDesc->strClickFileName;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBtn::Priority_Tick(const _float& fTimeDelta)
{
}

void CBtn::Tick(const _float& fTimeDelta)
{
}

void CBtn::Late_Tick(const _float& fTimeDelta)
{
	//피킹 확인
	m_isClick = m_pGameInstance->Picking_UI(m_pTransformCom);
}

HRESULT CBtn::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBtn::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	m_pClickTextureCom = CTexture::Create(m_pDevice, m_pContext, m_strClickFilePath, 1);
	if (nullptr == m_pClickTextureCom)
		return E_FAIL;


	return S_OK;
}

HRESULT CBtn::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (m_isClick)
	{
		if (FAILED(m_pClickTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}

	return S_OK;
}

CBtn* CBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBtn* pInstance = new CBtn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CBtn::Clone(void* pArg)
{
	CBtn* pInstance = new CBtn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBtn::Free()
{
	__super::Free();
	
	Safe_Release(m_pClickTextureCom);
}
