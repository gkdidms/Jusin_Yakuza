#include "Renderer.h"

#include "GameInstance.h"
#include "GameObject.h"

#include "BlendObject.h"

_uint		g_iSizeX = 8192;
_uint		g_iSizeY = 4608;

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{pDevice}, m_pContext{pContext},
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderer::Initialize()
{
	D3D11_VIEWPORT ViewPort{};
	_uint iNumViewPort = 1;

	m_pContext->RSGetViewports(&iNumViewPort, &ViewPort);

	/*Target_Diffuse*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 0.f, 1.f, 0.f))))
		return E_FAIL;

	/*Target_Normal*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/*Target_Depth*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_LightDepth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/*Target_Shader*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/*Target_BackBuffer*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BackBuffer"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/*Target_ToneMapping*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ToneMapping"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	//Luminance
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_128x128"), 128, 128, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_64x64"), 64, 64, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_32x32"), 32, 32, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_16x16"), 16, 16, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_8x8"), 8, 8, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_4x4"), 4, 4, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_2x2"), 2, 2, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_1x1"), 1, 1, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_CopyLuminance"), 1, 1, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Luminance"), ViewPort.Width, ViewPort.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/*MRT_GameObjects*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;

	/* MRT_ShadowObject */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowObjects"), TEXT("Target_LightDepth"))))
		return E_FAIL;

	/*MRT_LightAcc*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;

	/*MRT_CopyBackBuffer*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_CopyBackBuffer"), TEXT("Target_BackBuffer"))))
		return E_FAIL;

	/*MRT_HDR*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_ToneMapping"))))
		return E_FAIL;

	/*MRT_128*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_128"), TEXT("Target_128x128"))))
		return E_FAIL;

	/*MRT_64*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_64"), TEXT("Target_64x64"))))
		return E_FAIL;

	/*MRT_32*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_32"), TEXT("Target_32x32"))))
		return E_FAIL;

	/*MRT_16*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_16"), TEXT("Target_16x16"))))
		return E_FAIL;

	/*MRT_8*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_8"), TEXT("Target_8x8"))))
		return E_FAIL;

	/*MRT_4*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_4"), TEXT("Target_4x4"))))
		return E_FAIL;

	/*MRT_2*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_2"), TEXT("Target_2x2"))))
		return E_FAIL;

	/*MRT_1*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_1"), TEXT("Target_1x1"))))
		return E_FAIL;

	/*MRT_Luminance*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Luminance"), TEXT("Target_Luminance"))))
		return E_FAIL;

	/*MRT_CopyLuminance*/ 
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_CopyLuminance"), TEXT("Target_CopyLuminance"))))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	/* 화면을 꽉 채워주기 위한 월드변환행렬. */
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewPort.Width, ViewPort.Height, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPort.Width, ViewPort.Height, 0.f, 1.f));

	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iSizeX;
	TextureDesc.Height = g_iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);


#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Diffuse"), 50.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Normal"), 50.f, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Depth"), 50.f, 250.f, 100.f, 100.f)))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_LightDepth"), ViewPort.Width - 150.0f, 150.0f, 300.f, 300.f)))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Shade"), 150.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_BackBuffer"), 150.f, 150.f, 100.f, 100.f)))
		return E_FAIL;

#if !defined(_CLIEND)
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_128x128"), 250.f, 050.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Luminance"), 250.f, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_ToneMapping"), 250.f, 250.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_64x64"), 350.f, 050.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_32x32"), 350.f, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_16x16"), 350.f, 250.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_8x8"), 350.f, 350.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_4x4"), 450.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_2x2"), 450.f, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_1x1"), 450.f, 250.f, 100.f, 100.f)))
		return E_FAIL;
#endif // defined(_CLIEND)

#endif // _DEBUG


	return S_OK;
}

void CRenderer::Add_Renderer(RENDERER_STATE eRenderState,CGameObject* pGameObject)
{
	m_RenderObject[eRenderState].push_back(pGameObject);

	Safe_AddRef(pGameObject);
}

void CRenderer::Draw()
{
	Render_Priority();
	//Render_ShadowObjects();
	Render_NonBlender();
	Render_LightAcc();
	Render_CopyBackBuffer();
	Render_DeferredResult();

//#if !defined(_CLIEND)
//	if (m_isHDR)
//	{
//		Render_Luminance();
//		Render_AvgLuminance();
//		Render_CopyLuminance();
//		Render_HDR();
//		Redner_LuminanceResult();
//	}
//#endif // defined(_CLIEND)
//	if (m_isHDR)
//{
//		Render_Luminance();
//		Render_AvgLuminance();
//		Render_CopyLuminance();
//		Render_HDR();
//		Redner_LuminanceResult();
//	}

	Render_NonLight();
	Render_Blender();
	Render_UI();

#ifdef _DEBUG
//	Render_Debug();
#endif // _DEBUG
}

void CRenderer::Clear()
{
	for (size_t i = 0; i < RENDER_END; i++)
	{
		for (auto& pGameObject : m_RenderObject[i])
			Safe_Release(pGameObject);

		m_RenderObject[i].clear();
	}

#ifdef _DEBUG
	for (auto& iter : m_DebugComponents)
		Safe_Release(iter);
	m_DebugComponents.clear();
#endif // _DEBUG
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
	m_DebugComponents.push_back(pComponent);

	Safe_AddRef(pComponent);

	return S_OK;
}
#endif // _DEBUG

void CRenderer::Render_Priority()
{
	for (auto& iter : m_RenderObject[RENDER_PRIORITY])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_PRIORITY].clear();
		
}

void CRenderer::Render_ShadowObjects()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowObjects"), m_pLightDepthStencilView)))
		return;

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iSizeX;
	ViewPortDesc.Height = (_float)g_iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);


	for (auto& pGameObject : m_RenderObject[RENDER_SHADOWOBJ])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_LightDepth();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_SHADOWOBJ].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
	

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.0f;
	ViewPortDesc.Height = (_float)720.0f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
}

void CRenderer::Render_NonBlender()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return;

	for (auto& iter : m_RenderObject[RENDER_NONBLENDER])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_NONBLENDER].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_LightAcc()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamDir", m_pGameInstance->Get_ComLook_Float4(), sizeof(_float4))))
		return;
	if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CamFar(), sizeof(_float))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;

	m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_CopyBackBuffer()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_CopyBackBuffer"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	//if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
	//	return;
	//if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
	//	return;

	//_float4x4		ViewMatrix, ProjMatrix;

	/* 광원 기준의 뷰 변환행렬. */
	//XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 10.f, -10.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	//XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iSizeX / g_iSizeY, 0.1f, 1000.f));

	//if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &ViewMatrix)))
	//	return;
	//if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &ProjMatrix)))
	//	return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return;
	//if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_LightDepth"), m_pShader, "g_LightDepthTexture")))
	//	return;
	//if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
	//	return;

	m_pShader->Begin(3);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_DeferredResult() // 백버퍼에 Diffuse와 Shade를 더해서 그려줌
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BackBuffer"), m_pShader, "g_BackBufferTexture")))
		return;

	//if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
	//	return;
	//if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
	//	return;

	m_pShader->Begin(4);

	m_pVIBuffer->Render();
}

void CRenderer::Render_Luminance()
{
	//128x128

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)128.f;
	ViewPortDesc.Height = (_float)128.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_128"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BackBuffer"), m_pShader, "g_BackBufferTexture")))
		return;

	m_pShader->Begin(5);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	//ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	//ViewPortDesc.TopLeftX = 0;
	//ViewPortDesc.TopLeftY = 0;
	//ViewPortDesc.Width = (_float)1280.f;
	//ViewPortDesc.Height = (_float)720.f;
	//ViewPortDesc.MinDepth = 0.f;
	//ViewPortDesc.MaxDepth = 1.f;

	//m_pContext->RSSetViewports(1, &ViewPortDesc);

	//64x64

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_64"))))
		return;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)64.f;
	ViewPortDesc.Height = (_float)64.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_128x128"), m_pShader, "g_LuminanceTexture")))
		return;

	m_pShader->Begin(6);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;





	//32x32

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)32.f;
	ViewPortDesc.Height = (_float)32.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_32"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_64x64"), m_pShader, "g_LuminanceTexture")))
		return;

	m_pShader->Begin(6);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	//16x16

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)16.f;
	ViewPortDesc.Height = (_float)16.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_16"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_32x32"), m_pShader, "g_LuminanceTexture")))
		return;

	m_pShader->Begin(6);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	//8x8

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)8.f;
	ViewPortDesc.Height = (_float)8.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_8"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_16x16"), m_pShader, "g_LuminanceTexture")))
		return;

	m_pShader->Begin(6);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	//4x4

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)4.f;
	ViewPortDesc.Height = (_float)4.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_4"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_8x8"), m_pShader, "g_LuminanceTexture")))
		return;

	m_pShader->Begin(6);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	//2x2

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)2.f;
	ViewPortDesc.Height = (_float)2.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_2"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_4x4"), m_pShader, "g_LuminanceTexture")))
		return;

	m_pShader->Begin(6);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	//1x1

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1.f;
	ViewPortDesc.Height = (_float)1.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	//_bool isFninshed = { true };


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_1"))))
		return;

	//if (FAILED(m_pShader->Bind_RawValue("g_isFinished", &isFninshed, sizeof(_bool))))
	//	return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_2x2"), m_pShader, "g_LuminanceTexture")))
		return;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.f;
	ViewPortDesc.Height = (_float)720.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
}

void CRenderer::Render_HDR()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_HDR"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BackBuffer"), m_pShader, "g_BackBufferTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Luminance"), m_pShader, "g_LuminanceTexture")))
		return;

	m_pShader->Begin(9);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_AvgLuminance()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Luminance"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_1x1"), m_pShader, "g_LuminanceTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_CopyLuminance"), m_pShader, "g_CopyLuminanceTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_BackBuffer"), m_pShader, "g_BackBufferTexture")))
		return;

	m_pShader->Begin(7);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Redner_LuminanceResult()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_ToneMapping"), m_pShader, "g_BackBufferTexture")))
		return;

	//if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
	//	return;
	//if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
	//	return;

	m_pShader->Begin(4);

	m_pVIBuffer->Render();
}

void CRenderer::Render_CopyLuminance()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_CopyLuminance"))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_1x1"), m_pShader, "g_LuminanceTexture")))
		return;

	//if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
	//	return;
	//if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
	//	return;

	m_pShader->Begin(8);

	m_pVIBuffer->Render();
	if (FAILED(m_pGameInstance->End_MRT()))
		return;

}



void CRenderer::Render_NonLight()
{
	for (auto& iter : m_RenderObject[RENDER_NONLIGHT])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_NONLIGHT].clear();

}

void CRenderer::Render_Blender()
{
	m_RenderObject[RENDER_BLENDER].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return dynamic_cast<CBlendObject*>(pSour)->Get_ViewZ() > dynamic_cast<CBlendObject*>(pDest)->Get_ViewZ();
		});

	for (auto& iter : m_RenderObject[RENDER_BLENDER])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_BLENDER].clear();
}

void CRenderer::Render_UI()
{
	for (auto& iter : m_RenderObject[RENDER_UI])
	{
		iter->Render();

		Safe_Release(iter);
	}
	m_RenderObject[RENDER_UI].clear();
}

#ifdef _DEBUG
void CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return;
	//if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_ShadowObjects"), m_pShader, m_pVIBuffer)))
	//	return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_CopyBackBuffer"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_HDR"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_Luminance"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_128"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_64"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_32"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_16"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_8"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_4"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_2"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_Debug(TEXT("MRT_1"), m_pShader, m_pVIBuffer)))
		return;
}
#endif // DEBUG

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{

#ifdef _DEBUG
	for (auto& iter : m_DebugComponents)
		Safe_Release(iter);
	m_DebugComponents.clear();
#endif // _DEBUG

	for (int i = 0; i < RENDER_END; ++i)
	{
		for (auto& iter : m_RenderObject[i])
			Safe_Release(iter);

		m_RenderObject[i].clear();
	}

	Safe_Release(m_pLightDepthStencilView);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
