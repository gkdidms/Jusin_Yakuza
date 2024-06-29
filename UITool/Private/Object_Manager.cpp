#include "Object_Manager.h"

#include "GameInstance.h"

#include "Image_Texture.h"
#include "Text.h"
#include "Group.h"
#include "Btn.h"

#include "UI_Object.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager(): m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

vector<class CUI_Object*> CObject_Manager::Get_GroupObject(const wstring& strTag)
{
	vector<class CUI_Object*>* pFineObjects = Find_RenderTextureObject(strTag);

	if (nullptr == pFineObjects)
		return vector<class CUI_Object*>();

	return *pFineObjects;
}

vector<class CUI_Object*> CObject_Manager::Get_BinaryGroupObject(const wstring& strTag, _int iIndex)
{
	vector<class CUI_Object*>* pFineObjects = Find_BinaryObject(strTag);

	if (nullptr == pFineObjects)
		return vector<class CUI_Object*>();

	if ((*pFineObjects)[iIndex]->Get_TypeIndex() != GROUP)
		return vector<class CUI_Object*>();

	return dynamic_cast<CGroup*>((*pFineObjects)[iIndex])->Get_PartObjects();
}

vector<class CUI_Object*> CObject_Manager::Get_GroupBinaryObject(const wstring& strTag)
{
	vector<class CUI_Object*>* pFineObjects = Find_BinaryObject(strTag);

	if (nullptr == pFineObjects)
		return vector<class CUI_Object*>();

	return *pFineObjects;
}

HRESULT CObject_Manager::Copy_Group(const wstring& strTag)
{
	static _uint iCopyIndex = 0;

	vector<class CUI_Object*>* pFineObjects = Find_RenderTextureObject(strTag);

	if (nullptr == pFineObjects)
		return E_FAIL;

	_tchar strCopyTag[MAX_PATH];
	wstring strOrigTag = strTag + L"%d";
	wsprintfW(strCopyTag, strOrigTag.c_str(), iCopyIndex);

	vector<class CUI_Object*> CopyFineObjects;

	for (auto pTexture : *pFineObjects)
	{
		if (pTexture->Get_TypeIndex() == IMG)
		{
			CUI_Texture* pUI = dynamic_cast<CUI_Texture*>(pTexture);

			CUI_Texture::UI_TEXTURE_DESC Desc{};
			Desc.strTextureFilePath = pUI->Get_FilePath();
			Desc.strTextureFileName = pUI->Get_FileName();
			Desc.iTypeIndex = pUI->Get_TypeIndex();

			CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &Desc));
			if (nullptr == pImage)
				return E_FAIL;

			CopyFineObjects.emplace_back(pImage);
		}
		else if (pTexture->Get_TypeIndex() == TEXT)
		{
			CText* pText = dynamic_cast<CText*>(pTexture);

			CText::TEXT_DESC Desc{};
			Desc.strText = pText->Get_Text();
			Desc.vColor = pText->Get_Color();
			Desc.iTypeIndex = pText->Get_TypeIndex();


			CText* pObjText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &Desc));
			if (nullptr == pObjText)
				return E_FAIL;

			CopyFineObjects.emplace_back(pObjText);
		}

	}

	m_Objects.emplace(strCopyTag, CopyFineObjects);
	if (FAILED(Create_Texture2D()))
		return E_FAIL;
	
	//Binary용 
	vector<class CUI_Object*> BinaryObjects;
	m_BinaryObjects.emplace(strCopyTag, BinaryObjects);
	if (FAILED(Create_BinaryTexture2D()))
		return E_FAIL;

	iCopyIndex++; //인덱스 늘리기

	return S_OK;
}

HRESULT CObject_Manager::Remove_Group(const wstring& strTag)
{
	vector<class CUI_Object*>* pFineObjects = Find_RenderTextureObject(strTag);

	if (nullptr == pFineObjects)
		return E_FAIL;

	for (auto& pObject : *pFineObjects)
		Safe_Release(pObject);

	m_Objects.erase(strTag);

	return S_OK;
}

HRESULT CObject_Manager::Remove_Object(const wstring& strTag, _uint iIndex)
{
	vector<class CUI_Object*>* pFineObjects = Find_RenderTextureObject(strTag);

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

HRESULT CObject_Manager::Move_ObjectIndex(const wstring& strTag, _uint iIndex, _uint iMoveType)
{
	vector<class CUI_Object*>* pFineObjects = Find_RenderTextureObject(strTag);

	if (nullptr == pFineObjects)
		return E_FAIL;

	//예외처리
	if ((iMoveType == DOWN && iIndex <= 0) || (iMoveType == UP && iIndex >= (*pFineObjects).size() - 1) || (iMoveType == MOVE_END))
		return E_FAIL;

	//스왑
	auto iter1 = (*pFineObjects).begin() + iIndex;
	auto iter2 = iMoveType == DOWN ? (*pFineObjects).begin() + iIndex - 1 : (*pFineObjects).begin() + iIndex + 1;

	iter_swap(iter1, iter2);

	return S_OK;
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

	//백버퍼 카피
	CRenderTarget* pCopyBackBuffer = CRenderTarget::Create(m_pDevice, m_pContext, 1280.f, 720.f, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 0.f, 0.f, 1.f));
	if (nullptr == pCopyBackBuffer)
		return E_FAIL;

	pCopyBackBuffer->Ready_Debug(g_iWinSizeX - 100.f, 100.f, 200.f, 200.f);
	m_CopyBackTexture2Ds.emplace_back(pCopyBackBuffer);

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
	if (FAILED(Render_Object()))
		return E_FAIL;

	if (FAILED(Render_Copy()))
		return E_FAIL;

	if (FAILED(Render_BackBuffer()))
		return E_FAIL;

	//디버깅용 렌더 타겟 뷰
	for (auto& pTexture : m_Texture2Ds)
		pTexture->Render_Debug(m_pShaderCom, m_pVIBufferCom);
		
	for (auto& pTexture : m_CopyBackTexture2Ds)
		pTexture->Render_Debug(m_pShaderCom, m_pVIBufferCom);

	return S_OK;
}

HRESULT CObject_Manager::Render_Object()
{
	_uint iIndex = 0;
	auto BinaryIter = m_BinaryObjects.begin();

	for (auto& Pair : m_Objects)
	{
		Texture2D_Begin(m_Texture2Ds, iIndex);

		//렌더타겟 먼저 그려줌
		for (auto& pObj : Pair.second)
			pObj->Render();

		Texture2D_End();

		Texture2D_Begin(m_BinaryTexture2Ds, iIndex);

		//바이너리화 할 오브젝트들 그려줌
		for (auto& pObj : BinaryIter->second)
			pObj->Render();

		Texture2D_End();

		++BinaryIter;
		++iIndex;
	}

	return S_OK;
}

HRESULT CObject_Manager::Render_Copy()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	//CopyTexture2D에 여태까지 그린 모든 렌더타겟 합산 
	for (int i = 0; i < m_Texture2Ds.size(); ++i)
	{
		Texture2D_Begin(m_CopyBackTexture2Ds, 0, i == 0);
		if (FAILED(m_Texture2Ds[i]->Bind_SVR(m_pShaderCom, "g_Texture")))
			return E_FAIL;

		m_pShaderCom->Begin(3);
		m_pVIBufferCom->Render();

		Texture2D_End();

		Texture2D_Begin(m_CopyBackTexture2Ds, 0, false);
		if (FAILED(m_BinaryTexture2Ds[i]->Bind_SVR(m_pShaderCom, "g_Texture")))
			return E_FAIL;

		m_pShaderCom->Begin(3);
		m_pVIBufferCom->Render();

		Texture2D_End();
	}

	return S_OK;
}

HRESULT CObject_Manager::Render_BackBuffer()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_CopyBackTexture2Ds[0]->Bind_SVR(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CObject_Manager::Add_Group(const wstring& strObjectTag) 
{
	//그룹 생성
	//그룹 생성 후 RenderTexture에 들어갈 오브젝트용 map과 바이너리화를 진행할 오브젝트용 map에 똑같이 저장한다.

	vector<CUI_Object*>* pObjects = Find_RenderTextureObject(strObjectTag);

	if (nullptr != pObjects)
		return E_FAIL;

	vector<CUI_Object*> Object;
	vector<CUI_Object*> TypeObjects;
	
	m_Objects.emplace(strObjectTag, Object);
	m_BinaryObjects.emplace(strObjectTag, TypeObjects);

	if (FAILED(Create_Texture2D()))
		return E_FAIL;

	if (FAILED(Create_BinaryTexture2D()))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject_Manager::Add_RenderTextureObject(const wstring& strObjectTag, void* pArg)
{
	//RenderTargetGroup에 오브젝트 추가

	vector<CUI_Object*>* pObjects = Find_RenderTextureObject(strObjectTag);

	OBJ_MNG_DESC* pDesc = static_cast<OBJ_MNG_DESC*>(pArg);
	_uint iType = pDesc->iTextureType;
	// 타입별로 클래스 생성.

	if (nullptr == pObjects)
	{
		//생성
		vector<CUI_Object*> Objects;
		pObjects = &Objects;
	}

	if (iType == IMG)
	{
		CUI_Texture::UI_TEXTURE_DESC TextureDesc = {};
		TextureDesc.strTextureFileName = pDesc->strFileName;
		TextureDesc.strTextureFilePath = pDesc->strFilePath;
		TextureDesc.iTypeIndex = pDesc->iTextureType;

		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &TextureDesc));
		if (nullptr == pImage)
			return E_FAIL;

		pObjects->emplace_back(pImage);
	}
	else if (iType == TEXT)
	{
		CText::TEXT_DESC TextDesc = {};
		TextDesc.strText = pDesc->strText;
		TextDesc.iTextType = TEXT;
		TextDesc.vColor = pDesc->vColor;

		CText* pText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &TextDesc));
		if (nullptr == pText)
			return E_FAIL;

		pObjects->emplace_back(pText);
	}

	if (nullptr == pObjects)
	{
		m_Objects.emplace(strObjectTag, *pObjects);
	}

	return S_OK;
}

HRESULT CObject_Manager::Add_BinaryObject(const wstring& strObjectTag, void* pArg)
{
	//BinaryObjects에 오브젝트 추가

	vector<CUI_Object*>* pObjects = Find_BinaryObject(strObjectTag);

	OBJ_MNG_DESC* pDesc = static_cast<OBJ_MNG_DESC*>(pArg);
	_uint iType = pDesc->iTextureType;

	if (nullptr == pObjects)
	{
		vector<CUI_Object*> Objects;
		pObjects = &Objects;
	}

	// 타입별로 클래스 생성.
	if (iType == IMG)
	{
		CUI_Texture::UI_TEXTURE_DESC TextureDesc = {};
		TextureDesc.strTextureFileName = pDesc->strFileName;
		TextureDesc.strTextureFilePath = pDesc->strFilePath;
		TextureDesc.strName = pDesc->strName;
		TextureDesc.iTypeIndex = pDesc->iTextureType;

		CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &TextureDesc));
		if (nullptr == pImage)
			return E_FAIL;

		pObjects->emplace_back(pImage);
	}
	else if (iType == BTN)
	{
		CBtn::BTN_DESC BtnDesc{};
		BtnDesc.strTextureFilePath = pDesc->strFilePath;
		BtnDesc.strTextureFileName = pDesc->strFileName;
		BtnDesc.strClickFileName = pDesc->strBtnClickFileName;
		BtnDesc.strClickFilePath = pDesc->strBtnClickFilePath;
		BtnDesc.strName = pDesc->strName;
		BtnDesc.iTypeIndex = pDesc->iTextureType;

		CBtn* mBtn = dynamic_cast<CBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Btn"), &BtnDesc));
		
	}
	else if (iType == TEXT || iType == TEXT_FORMAT)
	{
		CText::TEXT_DESC TextDesc = {};
		TextDesc.strText = pDesc->strText;
		TextDesc.iTextType = TEXT;
		TextDesc.vColor = pDesc->vColor;
		TextDesc.strName = pDesc->strName;
		TextDesc.iTypeIndex = pDesc->iTextureType;

		CText* pText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &TextDesc));
		if (nullptr == pText)
			return E_FAIL;

		pObjects->emplace_back(pText);
	}
	else if (iType == GROUP)
	{
		CGroup::UI_OBJECT_DESC GroupDesc{};
		GroupDesc.strName = pDesc->strName;
		GroupDesc.iTypeIndex = pDesc->iTextureType;

		CGroup* pGroup = dynamic_cast<CGroup*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Group"), &GroupDesc));
		if (nullptr == pGroup)
			return E_FAIL;

		pObjects->emplace_back(pGroup);
	}

	if (nullptr == pObjects)
	{
		m_Objects.emplace(strObjectTag, *pObjects);
	}

	return S_OK;
}

vector<CUI_Object*>* CObject_Manager::Find_RenderTextureObject(const wstring& strObjectTag)
{
	auto pObject = m_Objects.find(strObjectTag);

	if (pObject == m_Objects.end())
		return nullptr;

	return &(pObject->second);
}

vector<class CUI_Object*>* CObject_Manager::Find_BinaryObject(const wstring& strObjectTag)
{
	auto pObject = m_BinaryObjects.find(strObjectTag);

	if (pObject == m_BinaryObjects.end())
		return nullptr;

	return &(pObject->second);
}

HRESULT CObject_Manager::Create_Texture2D()
{
	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, 1280.f, 720.f, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f));

	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_Texture2Ds.emplace_back(pRenderTarget);
	
	pRenderTarget->Ready_Debug(m_fDebugStart + ( 100 * m_iTextureWidthCount), m_fDebugStart + (100 * m_iTextureHeightCount), 100.f, 100.f);
	
	m_iTextureHeightCount++;
	if (m_fDebugStart * m_iTextureHeightCount >= 450.f)
	{
		m_iTextureHeightCount = 0.f;
		m_iTextureWidthCount++;
	}

	return S_OK;
}

HRESULT CObject_Manager::Create_BinaryTexture2D()
{
	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, 1280.f, 720.f, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f));

	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_BinaryTexture2Ds.emplace_back(pRenderTarget);

	return S_OK;
}

HRESULT CObject_Manager::Texture2D_Begin(vector<CRenderTarget*> Textures, _uint iIndex, _bool isClear)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
	nullptr
	};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV); //초기화 해주는 작업

	_uint		iNumRenderTargets = {};

	ID3D11RenderTargetView* pRenderTargets[8] = {};

	if (isClear) Textures[iIndex]->Clear();
	pRenderTargets[iNumRenderTargets++] = Textures[iIndex]->Get_RTV();

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

	for (auto& Pair : m_BinaryObjects)
	{
		for (auto& pObj : Pair.second)
			Safe_Release(pObj);
	}

	for (auto& pTexture2D : m_Texture2Ds)
		Safe_Release(pTexture2D);

	for (auto& pTexture2D : m_BinaryTexture2Ds)
		Safe_Release(pTexture2D);

	for (auto& pTexture2D : m_CopyBackTexture2Ds)
		Safe_Release(pTexture2D);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferView);


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
}
