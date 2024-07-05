#include "Image_Texture.h"

#include "GameInstance.h"

CImage_Texture::CImage_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CImage_Texture::CImage_Texture(const CImage_Texture& rhs)
	: CUI_Texture{ rhs }
{
}

HRESULT CImage_Texture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CImage_Texture::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CImage_Texture::Priority_Tick(const _float& fTimeDelta)
{
}

void CImage_Texture::Tick(const _float& fTimeDelta)
{
}

void CImage_Texture::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CImage_Texture::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iShaderPass);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CImage_Texture::Save_binary(const string strDirectory)
{
	string Directory = strDirectory;

	Directory = Directory + m_strName+".dat";
	ofstream out(Directory, ios::binary);

	out.write((char*)&m_iTypeIndex, sizeof(_uint));

	_int strTexturelength = m_strName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strName.c_str(), strTexturelength);

	
	out.write((char*)&m_isParent, sizeof(_bool));

	strTexturelength = m_strParentName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strParentName.c_str(), strTexturelength);

	string FilePath = m_pGameInstance->WstringToString(m_strTextureFilePath);
	strTexturelength = FilePath.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FilePath.c_str(), strTexturelength);
	
	string FileName = m_pGameInstance->WstringToString(m_strTextureName);
	strTexturelength = FileName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FileName.c_str(), strTexturelength);

	out.write((char*)&m_fStartUV, sizeof(_float2));

	out.write((char*)&m_fEndUV, sizeof(_float2));

	out.write((char*)&m_vColor, sizeof(_float4));

	out.write((char*)&m_iShaderPass, sizeof(_uint));

	out.close();

	return S_OK;
}

HRESULT CImage_Texture::Save_Groupbinary( ofstream& out)
{

	out.write((char*)&m_iTypeIndex, sizeof(_uint));

	_int strTexturelength = m_strName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strName.c_str(), strTexturelength);


	out.write((char*)&m_isParent, sizeof(_bool));

	strTexturelength = m_strParentName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strParentName.c_str(), strTexturelength);

	string FilePath = m_pGameInstance->WstringToString(m_strTextureFilePath);
	strTexturelength = FilePath.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FilePath.c_str(), strTexturelength);

	string FileName = m_pGameInstance->WstringToString(m_strTextureName);
	strTexturelength = FileName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FileName.c_str(), strTexturelength);

	out.write((char*)&m_fStartUV, sizeof(_float2));

	out.write((char*)&m_fEndUV, sizeof(_float2));

	out.write((char*)&m_vColor, sizeof(_float4));

	out.write((char*)&m_iShaderPass, sizeof(_uint));

	return S_OK;
}

HRESULT CImage_Texture::Load_binary()
{
	return S_OK;
}

CImage_Texture* CImage_Texture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImage_Texture* pInstance = new CImage_Texture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CImage_Texture::Clone(void* pArg)
{
	CImage_Texture* pInstance = new CImage_Texture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CImage_Texture::Free()
{
	__super::Free();
}
