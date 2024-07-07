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


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CText::Priority_Tick(const _float& fTimeDelta)
{
}

void CText::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CText::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CText::Render()
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float2 vFontPos = _float2(vPos.x + g_iWinSizeX * 0.5f, -vPos.y + g_iWinSizeY * 0.5f);
	m_pGameInstance->Render_Font(TEXT("Font_Default"), m_strText, vFontPos, XMLoadFloat4(&m_vColor));	

	return S_OK;
}

HRESULT CText::Save_binary(const string strDirectory)
{
	string Directory = strDirectory;

	Directory = Directory + m_strName + ".dat";
	ofstream out(Directory, ios::binary);
	//공용저장
	out.write((char*)&m_iTypeIndex, sizeof(_uint));

	_int strTexturelength = m_strName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strName.c_str(), strTexturelength);


	out.write((char*)&m_isParent, sizeof(_bool));

	strTexturelength = m_strParentName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strParentName.c_str(), strTexturelength);

	out.write((char*)&m_vColor, sizeof(_float4));

	out.write((char*)&m_iShaderPass, sizeof(_uint));

	_float4x4 WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();

	out.write((char*)&WorldMatrix, sizeof(_float4x4));


	out.write((char*)&m_isAnim, sizeof(_bool));

	m_fAnimTime.x = 0.f;
	out.write((char*)&m_fAnimTime, sizeof(_float2));

	out.write((char*)&m_vStartPos, sizeof(_float3));


	//개별적인 저장

	string Text = m_pGameInstance->WstringToString(m_strText);
	strTexturelength = Text.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(Text.c_str(), strTexturelength);

	out.close();

	return S_OK;
}

HRESULT CText::Save_Groupbinary(ofstream& out)
{
	out.write((char*)&m_iTypeIndex, sizeof(_uint));

	_int strTexturelength = m_strName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strName.c_str(), strTexturelength);


	out.write((char*)&m_isParent, sizeof(_bool));

	strTexturelength = m_strParentName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strParentName.c_str(), strTexturelength);

	out.write((char*)&m_vColor, sizeof(_float4));

	out.write((char*)&m_iShaderPass, sizeof(_uint));

	_float4x4 WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();

	out.write((char*)&WorldMatrix, sizeof(_float4x4));

	out.write((char*)&m_isAnim, sizeof(_bool));

	m_fAnimTime.x = 0.f;
	out.write((char*)&m_fAnimTime, sizeof(_float2));

	out.write((char*)&m_vStartPos, sizeof(_float3));


	//개별적인 저장

	string Text = m_pGameInstance->WstringToString(m_strText);
	strTexturelength = Text.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(Text.c_str(), strTexturelength);

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
