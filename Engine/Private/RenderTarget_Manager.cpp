#include "RenderTarget_Manager.h"

#include "RenderTarget.h"

#include "GameInstance.h"

CRenderTarget_Manager::CRenderTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

ID3D11Texture2D* CRenderTarget_Manager::Get_TextureBuffer(const wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_TextureBuffer();
}

HRESULT CRenderTarget_Manager::Initialize()
{
	m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isCompute, _uint iArrayCount)
{
	if (nullptr != Find_RenderTarget(strRenderTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iSizeX, iSizeY, ePixelFormat, vClearColor, isCompute, iArrayCount);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>* pMRT = Find_MRT(strMRTTag);
	if (nullptr == pMRT)
	{
		// ������ ���� �����
		list<CRenderTarget*> RenderTargets;
		RenderTargets.emplace_back(pRenderTarget);
		m_MRTs.emplace(strMRTTag, RenderTargets);
	}
	else
	{
		//�ִٸ� ���� �ִ� �ּҿ� �־���
		pMRT->emplace_back(pRenderTarget);
	}

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSView, _bool isClear)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
		nullptr
	};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV); //�ʱ�ȭ ���ִ� �۾�

	list<CRenderTarget*>* pMRT = Find_MRT(strMRTTag);
	if (nullptr == pMRT)
		return E_FAIL;

	_uint		iNumRenderTargets = {};

	ID3D11RenderTargetView* pRenderTargets[8] = {};

	for (auto& pRenderTarget : *pMRT)
	{
		if (isClear) pRenderTarget->Clear();
		pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}

	if (nullptr != pDSView)
		m_pContext->ClearDepthStencilView(pDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, nullptr == pDSView ? m_pDepthStencilView : pDSView);

	return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);
	return S_OK;
}

HRESULT CRenderTarget_Manager::Bind_RenderTargetSRV(const wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_SVR(pShader, pConstantName);
}

void CRenderTarget_Manager::Bind_ComputeRenderTargetSRV(const wstring& strTargetTag, _uint iSlot)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return;

	pRenderTarget->Bind_ComputeSVR(iSlot);
}

void CRenderTarget_Manager::Bind_ComputeRenderTargetUAV(const wstring& strTargetTag, _uint iSlot)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return;

	pRenderTarget->Bind_ComputeUAV(iSlot);
}

HRESULT CRenderTarget_Manager::Copy_Resource(const wstring& strTargetTag, ID3D11Texture2D* pDesc)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;


	return pRenderTarget->Copy_Resource(pDesc);
}

HRESULT CRenderTarget_Manager::Create_Texture(const wstring& strTargetTag, const wstring& strSaveFilePath)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Create_Texture(strSaveFilePath);
}

HRESULT CRenderTarget_Manager::Clear_RenderTarget(const wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Clear();
}

#ifdef _DEBUG
HRESULT CRenderTarget_Manager::Ready_Debug(const wstring strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CRenderTarget_Manager::Render_Debug(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer, _bool isArray)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->Render_Debug(pShader, pVIBuffer, isArray);

	return S_OK;
}
#endif // _DEBUG

CRenderTarget* CRenderTarget_Manager::Find_RenderTarget(const wstring& strRenderTarget)
{
	auto pRenderTarget = m_RenderTargets.find(strRenderTarget);

	if (pRenderTarget == m_RenderTargets.end())
		return nullptr;

	return pRenderTarget->second;
}

list<class CRenderTarget*>* CRenderTarget_Manager::Find_MRT(const wstring& strMRTTag)
{
	auto pMRT = m_MRTs.find(strMRTTag);

	if (pMRT == m_MRTs.end())
		return nullptr;

	return &pMRT->second;
}

CRenderTarget_Manager* CRenderTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderTarget_Manager* pInstance = new CRenderTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CRenderTarget_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget_Manager::Free()
{
	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);
	m_RenderTargets.clear();

	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);
		Pair.second.clear();
	}
	m_MRTs.clear();

	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferView);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}