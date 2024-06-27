#include "Object_Manager.h"

#include "GameInstance.h"

#include "Image_Texture.h"
#include "Text.h"

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

HRESULT CObject_Manager::Create_Texture(_uint iIndex, const wstring& strFilePath)
{
	return m_Texture2Ds[iIndex]->Create_Texture(strFilePath);
}

HRESULT CObject_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	m_pVIBufferCom = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	D3D11_VIEWPORT ViewPort{};
	_uint iNumViewPort = 1;

	m_pContext->RSGetViewports(&iNumViewPort, &ViewPort);

	/* 화면을 꽉 채워주기 위한 월드변환행렬. */
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewPort.Width, ViewPort.Height, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPort.Width, ViewPort.Height, 0.f, 1.f));

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
	_uint iIndex = 0; 
	for (auto& Pair : m_Objects)
	{
		Texture2D_Begin(iIndex);

		m_Texture2Ds.begin();
		for (auto& pObj : Pair.second)
			pObj->Render();

		Texture2D_End();

		++iIndex;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	for (auto& pTexture : m_Texture2Ds)
	{
		if (FAILED(pTexture->Bind_SVR(m_pShaderCom, "g_Texture")))
			return E_FAIL;

		m_pShaderCom->Begin(3);
		m_pVIBufferCom->Render();
	}

	for (auto& pTexture : m_Texture2Ds)
		pTexture->Render_Debug(m_pShaderCom, m_pVIBufferCom);
		


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

	if (FAILED(Create_Texture2D()))
		return E_FAIL;

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

HRESULT CObject_Manager::Create_Texture2D()
{
	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, 1280.f, 720.f, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f));

	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_Texture2Ds.emplace_back(pRenderTarget);
	m_iTextureHeightCount++;

	pRenderTarget->Ready_Debug(m_fDebugStart * m_iTextureWidthCount, m_fDebugStart * m_iTextureHeightCount, 100.f, 100.f);
	
	if (m_fDebugStart * m_iTextureHeightCount >= 450.f)
	{
		m_iTextureHeightCount = 0.f;
		m_iTextureWidthCount++;
	}

	return S_OK;
}

HRESULT CObject_Manager::Texture2D_Begin(_uint iIndex)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
	nullptr
	};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV); //초기화 해주는 작업

	_uint		iNumRenderTargets = {};

	ID3D11RenderTargetView* pRenderTargets[8] = {};

	m_Texture2Ds[iIndex]->Clear();
	pRenderTargets[iNumRenderTargets++] = m_Texture2Ds[iIndex]->Get_RTV();

	m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CObject_Manager::Texture2D_End()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);
	return S_OK;
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

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferView);


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
}
