#include "Btn_Texture.h"

#include "GameInstance.h"

CBtn_Texture::CBtn_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext}
{
}

CBtn_Texture::CBtn_Texture(const CBtn_Texture& rhs)
	: CUI_Texture{ rhs }
{
}

HRESULT CBtn_Texture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBtn_Texture::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBtn_Texture::Priority_Tick(const _float& fTimeDelta)
{
}

void CBtn_Texture::Tick(const _float& fTimeDelta)
{
}

void CBtn_Texture::Late_Tick(const _float& fTimeDelta)
{
	//m_pGameInstance->Add_Renderer(CRenderer::RENDER_BLENDER, this);
}

HRESULT CBtn_Texture::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

CBtn_Texture* CBtn_Texture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBtn_Texture* pInstance = new CBtn_Texture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CBtn_Texture::Clone(void* pArg)
{
	CBtn_Texture* pInstance = new CBtn_Texture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBtn_Texture::Free()
{
	__super::Free();
}
