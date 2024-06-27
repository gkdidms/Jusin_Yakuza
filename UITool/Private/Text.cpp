#include "Text.h"

#include "GameInstance.h"

CText::CText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext}
{
}

CText::CText(const CText& rhs)
	: CUI_Texture{ rhs }
{
}

HRESULT CText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CText::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return E_FAIL;

	TEXT_DESC* pDesc = static_cast<TEXT_DESC*>(pArg);
	m_strText = pDesc->strText;
	m_vColor = pDesc->vColor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CText::Priority_Tick(const _float& fTimeDelta)
{
}

void CText::Tick(const _float& fTimeDelta)
{
}

void CText::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CText::Render()
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Render_Font(TEXT("Font_Default"), m_strText, _float2(vPos.x, vPos.y), m_vColor);

	return S_OK;
}

CText* CText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CText* pInstance = new CText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CText::Clone(void* pArg)
{
	CText* pInstance = new CText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CText::Free()
{
	__super::Free();
}
