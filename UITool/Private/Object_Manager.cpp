#include "Object_Manager.h"

#include "GameInstance.h"

#include "Image_Texture.h"
#include "Btn_Texture.h"

#include "UI_Texture.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager(): m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

vector<class CUI_Texture*> CObject_Manager::Get_GroupObject(const wstring& strTag)
{
	vector<class CUI_Texture*>* pFineObjects = Find_Object(strTag);

	if (nullptr == pFineObjects)
		return vector<class CUI_Texture*>();

	return *pFineObjects;
}

HRESULT CObject_Manager::Copy_Group(const wstring& strTag)
{
	static _uint iCopyIndex = 0;

	vector<class CUI_Texture*>* pFineObjects = Find_Object(strTag);

	if (nullptr == pFineObjects)
		return E_FAIL;

	_tchar strCopyTag[MAX_PATH];
	wstring strOrigTag = strTag + L"%d";
	wsprintfW(strCopyTag, strOrigTag.c_str(), iCopyIndex);

	vector<class CUI_Texture*> CopyFineObjects;

	for (auto pTexture : *pFineObjects)
	{
		CUI_Texture::UI_TEXTURE_DESC Desc{};
		Desc.strTextureFilePath = pTexture->Get_FilePath();
		Desc.strTextureFileName = pTexture->Get_FileName();

		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &Desc));
		if (nullptr == pImage)
			return E_FAIL;

		CopyFineObjects.emplace_back(pImage);
	}

	m_Objects.emplace(strCopyTag, CopyFineObjects);

	return S_OK;
}

HRESULT CObject_Manager::Remove_Group(const wstring& strTag)
{
	vector<class CUI_Texture*>* pFineObjects = Find_Object(strTag);

	if (nullptr == pFineObjects)
		return E_FAIL;

	for (auto& pObject : *pFineObjects)
		Safe_Release(pObject);

	m_Objects.erase(strTag);

	return S_OK;
}

HRESULT CObject_Manager::Remove_Object(const wstring& strTag, _uint iIndex)
{
	vector<class CUI_Texture*>* pFineObjects = Find_Object(strTag);

	if (nullptr == pFineObjects)
		return E_FAIL;

	Safe_Release((*pFineObjects)[iIndex]);
	pFineObjects->erase((*pFineObjects).begin() + iIndex);

	return S_OK;
}

HRESULT CObject_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

void CObject_Manager::Tick(const _float& fTimeDelta)
{
	for (auto& Pair : m_Objects)
	{
		for (auto& pObj : Pair.second)
			pObj->Tick(fTimeDelta);
	}
}

void CObject_Manager::Late_Tick(const _float& fTimeDelta)
{
	for (auto& Pair : m_Objects)
	{
		for (auto& pObj : Pair.second)
			pObj->Late_Tick(fTimeDelta);
	}
}

HRESULT CObject_Manager::Render()
{
	for (auto& Pair : m_Objects)
	{
		for (auto& pObj : Pair.second)
			pObj->Render();
	}

	return S_OK;
}

HRESULT CObject_Manager::Add_Group(const wstring& strObjectTag)
{
	vector<CUI_Texture*>* pObjects = Find_Object(strObjectTag);

	if (nullptr != pObjects)
		return E_FAIL;

	vector<CUI_Texture*> Object;
	
	//CUI_Texture::UI_TEXTURE_DESC Desc{};
	//Desc.strTextureFileName = TEXT("Background");
	//Desc.strTextureFilePath = TEXT("../../Client/Bin/Resources/Textures/T_Sand_06_A.png");
	//CUI_Texture* pBack = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &Desc));
	//if (nullptr == pBack)
	//	return E_FAIL;

	//Object.emplace_back(pBack);

	m_Objects.emplace(strObjectTag, Object);

	return S_OK;
}

HRESULT CObject_Manager::Add_Object(const wstring& strObjectTag, void* pArg)
{
	vector<CUI_Texture*>* pObjects = Find_Object(strObjectTag);

	OBJ_MNG_DESC* pDesc = static_cast<OBJ_MNG_DESC*>(pArg);
	_uint iType = pDesc->iTextureType;
	// 타입별로 클래스 생성.

	if (nullptr == pObjects)
	{
		//생성
		vector<CUI_Texture*> Objects;
		pObjects = &Objects;
	}

	//구조체 보내기
	CUI_Texture::UI_TEXTURE_DESC TextureDesc = {};
	TextureDesc.strTextureFileName = pDesc->strFileName;
	TextureDesc.strTextureFilePath = pDesc->strFilePath;

	if (iType == IMG)
	{
		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &TextureDesc));
		if (nullptr == pImage)
			return E_FAIL;

		pObjects->emplace_back(pImage);
	}
	else if (iType == BTN)
	{
		CUI_Texture* pBtn = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Btn_Texture"), &TextureDesc));
		if (nullptr == pBtn)
			return E_FAIL;

		pObjects->emplace_back(pBtn);
	}

	if (nullptr == pObjects)
	{
		m_Objects.emplace(strObjectTag, *pObjects);
	}

	return S_OK;
}

vector<CUI_Texture*>* CObject_Manager::Find_Object(const wstring& strObjectTag)
{
	auto pObject = m_Objects.find(strObjectTag);

	if (pObject == m_Objects.end())
		return nullptr;

	return &(pObject->second);
}

void CObject_Manager::Release_ObjectManager()
{
	//CObject_Manager::GetInstance()->Free();

	CObject_Manager::GetInstance()->DestroyInstance();
}

void CObject_Manager::Free()
{
	for (auto& Pair : m_Objects)
	{
		for (auto& pObj : Pair.second)
			Safe_Release(pObj);
	}

	for (auto& pTexture2D : m_Texture2Ds)
		Safe_Release(pTexture2D);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
}
