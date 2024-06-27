#include "GameInstance.h"

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "GameObject_Manager.h"
#include "Component_Manager.h"
#include "RenderTarget_Manager.h"
#include "Light_Manager.h"
#include "Timer_Manager.h"
#include "Font_Manager.h"
#include "Renderer.h"
#include "Picking.h"
#include "SoundMgr.h"
#include "Frustum.h"
#include "Convert_Manager.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(_uint iMaxLevelIndex, const ENGINE_DESC& Engine_Desc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(Engine_Desc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pSound_Manager = CSoundMgr::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(Engine_Desc.hInst, Engine_Desc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create(iMaxLevelIndex);
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pGameObject_Manager = CGameObject_Manager::Create(iMaxLevelIndex);
	if (nullptr == m_pGameObject_Manager)
		return E_FAIL;

	m_pRandom_Manager = CRandomManager::Create();
	if (nullptr == m_pRandom_Manager)
		return E_FAIL;

	m_pRenderTarget_Manager = CRenderTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iMaxLevelIndex);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, Engine_Desc.hWnd);
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pConvert_Manager = CConvert_Manager::Create();
	if (nullptr == m_pConvert_Manager)
		return E_FAIL;




	return S_OK;
}

void CGameInstance::Tick(const _float& fTimeDelta)
{
	m_pInput_Device->Update_InputDev();

	m_pGameObject_Manager->Priority_Tick(fTimeDelta);

	m_pGameObject_Manager->Tick(fTimeDelta);

	m_pPipeLine->Tick();

	m_pFrustum->Tick();

	m_pGameObject_Manager->Late_Tick(fTimeDelta);

	m_pLevel_Manager->Tick(fTimeDelta);
}

void CGameInstance::Draw()
{
	m_pRenderer->Draw();
}

void CGameInstance::Draw_From_Tool()
{
	m_pRenderer->Draw();
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();
}


HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pLevel)
{
	return m_pLevel_Manager->Open_Level(iLevelIndex, pLevel);
}

_uint CGameInstance::Get_CurrentLevel()
{
	return m_pLevel_Manager->Get_CurrentLevel();
}

_bool CGameInstance::PlaySound_W(const wstring pSoundKey, CHANNELID eID, float fVolume)
{
	return m_pSound_Manager->PlaySound_W(pSoundKey, eID, fVolume);
}

void CGameInstance::PlayBGM(const wstring pSoundKey, float fVolume)
{
	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopSound(CHANNELID eID)
{
	m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

HRESULT CGameInstance::Add_GameObject_Prototype(const wstring strGameObjectTag, CGameObject* pGameObject)
{
	return m_pGameObject_Manager->Add_GameObject_Prototype(strGameObjectTag, pGameObject);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const wstring strGameObjectTag, const wstring strLayerTag, void* pArg)
{
	return m_pGameObject_Manager->Add_GameObject(iLevelIndex, strGameObjectTag, strLayerTag, pArg);
}

CGameObject* CGameInstance::Clone_Object(const wstring strGameObjectTag, void* pArg)
{
	return m_pGameObject_Manager->Clone_Object(strGameObjectTag, pArg);
}

void CGameInstance::Clear_Object(_uint iLevelIndex)
{
	m_pRenderer->Clear();
	m_pGameObject_Manager->Clear(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);
}

CComponent* CGameInstance::Get_GameObject_Component(_uint iLevelIndex, const wstring strLayerTag, const wstring strComponentTag, _uint iIndex)
{
	return m_pGameObject_Manager->Get_GameObject_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CGameObject* CGameInstance::Get_GameObject(_uint iLevelIndex, const wstring strLayerTag, _uint iIndex)
{
	return m_pGameObject_Manager->Get_GameObject(iLevelIndex, strLayerTag, iIndex);
}

vector<CGameObject*> CGameInstance::Get_GameObjects(_uint iLevelIndex, const wstring strLayerTag)
{
	return m_pGameObject_Manager->Get_GameObjects(iLevelIndex, strLayerTag);
}

void CGameInstance::Add_Renderer(CRenderer::RENDERER_STATE eRenderState, CGameObject* pGameObject)
{
	m_pRenderer->Add_Renderer(eRenderState, pGameObject);
}

void CGameInstance::Set_HDR(_bool isHDR)
{
	m_pRenderer->Set_HDR(isHDR);
}

void CGameInstance::Set_HDRLight(_float fLight)
{
	m_pRenderer->Set_HDRLight(fLight);
}

_bool CGameInstance::isHDR()
{
	return m_pRenderer->isHDR();
}

_float CGameInstance::Get_HDRLight()
{
	return m_pRenderer->Get_HDRLight();
}

void CGameInstance::Set_SSAO(_bool isSSAO)
{
	m_pRenderer->Set_SSAO(isSSAO);
}

_bool CGameInstance::isSSAO()
{
	return m_pRenderer->isSSAO();
}

void CGameInstance::Set_SSAORadiuse(_float fRadiuse)
{
	m_pRenderer->Set_SSAORadiuse(fRadiuse);
}

_float CGameInstance::Get_SSAORadiuse()
{
	return m_pRenderer->Get_SSAORadiuse();
}

void CGameInstance::Set_SSAOBlur(_float fBlur)
{
	m_pRenderer->Set_SSAOBlur(fBlur);
}

_float CGameInstance::Get_SSAOBlur()
{
	return m_pRenderer->Get_SSAOBlur();
}

_float CGameInstance::Get_SSAOBias()
{
	return m_pRenderer->Get_SSAOBias();
}

void CGameInstance::Set_SSAOBias(_float fBias)
{
	m_pRenderer->Set_SSAOBias(fBias);
}

#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugComponent(CComponent* pComponent)
{
	return m_pRenderer->Add_DebugComponent(pComponent);
}

void CGameInstance::Set_DebugView(_bool isDebugView)
{
	m_pRenderer->Set_DebugView(isDebugView);
}
_bool CGameInstance::isDebugView()
{
	return m_pRenderer->isDebugView();
}
#endif // _DEBUG



HRESULT CGameInstance::Add_Component_Prototype(_uint iLevelIndex, const wstring strComponentTag, CComponent* pComponent)
{
	return m_pComponent_Manager->Add_Component_Prototype(iLevelIndex, strComponentTag, pComponent);
}

CComponent* CGameInstance::Add_Component_Clone(_uint iLevelIndex, const wstring strComponentTag, void* pArg)
{
	return m_pComponent_Manager->Add_Component_Clone(iLevelIndex, strComponentTag, pArg);
}

const _float4x4* CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

const _float4x4* CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

const _float4* CGameInstance::Get_CamPosition_Float4()
{
	return m_pPipeLine->Get_CamPosition_Float4();
}

_vector CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix matTransform)
{
	m_pPipeLine->Set_Transform(eState, matTransform);
}

const _float4* CGameInstance::Get_ComLook_Float4()
{
	return m_pPipeLine->Get_ComLook_Float4();
}

_vector CGameInstance::Get_CamLook()
{
	return m_pPipeLine->Get_CamLook();
}

const _float* CGameInstance::Get_CamFar()
{
	return m_pPipeLine->Get_CamFar();
}

void CGameInstance::Set_CamFar(_float fFar)
{
	m_pPipeLine->Set_CamFar(fFar);
}

_float CGameInstance::Get_TimeDelta(const _tchar* pTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

void CGameInstance::Update_TimeDelta(const _tchar* pTimerTag)
{
	m_pTimer_Manager->Update_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const _tchar* pTimerTag)
{
	return m_pTimer_Manager->Ready_Timer(pTimerTag);
}

void CGameInstance::Set_TimeSpeed(const _tchar* pTimerTag, _float fSpeed)
{
	m_pTimer_Manager->Set_TimeSpeed(pTimerTag, fSpeed);
}

void CGameInstance::Reset_TimeDelta(const _tchar* pTimerTag)
{
	m_pTimer_Manager->Reset_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor)
{
	return m_pFont_Manager->Render_Font(strFontTag, strText, vPosition, vColor);
}

HRESULT CGameInstance::Perspective_Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fScale, const _float& fTimeDelta)
{
	return m_pFont_Manager->Perspective_Render(strFontTag, strText, vPosition, vColor, fScale, fTimeDelta);
}

HRESULT CGameInstance::Blend_Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor)
{
	return m_pFont_Manager->Blend_Render(strFontTag, strText, vPosition, vColor);
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

KEYSTATE CGameInstance::GetKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->GetKeyState(byKeyID);
}

KEYSTATE CGameInstance::GetMouseState(MOUSEKEYSTATE eMouseState)
{
	return m_pInput_Device->GetMouseState(eMouseState);
}


_vector CGameInstance::Picking(_bool* isSuccess)
{
	return m_pPicking->Picking(isSuccess);
}

_float CGameInstance::Get_Z()
{
	return m_pPicking->Get_Z();
}

float CGameInstance::FindObjID(_bool* isSuccess)
{
	return m_pPicking->FindObjID(isSuccess);
}

HRESULT CGameInstance::Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pRenderTarget_Manager->Add_RenderTarget(strRenderTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag)
{
	return m_pRenderTarget_Manager->Add_MRT(strMRTTag, strRenderTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSView, _bool isClear)
{
	return m_pRenderTarget_Manager->Begin_MRT(strMRTTag, pDSView, isClear);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pRenderTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RenderTargetSRV(const wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pRenderTarget_Manager->Bind_RenderTargetSRV(strTargetTag, pShader, pConstantName);
}

HRESULT CGameInstance::Copy_Resource(const wstring& strTargetTag, ID3D11Texture2D* pDesc)
{
	return m_pRenderTarget_Manager->Copy_Resource(strTargetTag, pDesc);
}

HRESULT CGameInstance::Create_Texture(const wstring& strTargetTag, const wstring& strSaveFilePath)
{
	return m_pRenderTarget_Manager->Create_Texture(strTargetTag, strSaveFilePath);
}

void CGameInstance::Transform_ToLocalSpace(_fmatrix WorldMatrixInv)
{
	return m_pFrustum->Transform_ToLocalSpace(WorldMatrixInv);
}

_bool CGameInstance::isIn_WorldFrustum(_fvector vPosition, _float fRange)
{
	return m_pFrustum->isIn_WorldFrustum(vPosition, fRange);
}

_bool CGameInstance::isIn_LocalFrustum(_fvector vPosition, _float fRange)
{
	return m_pFrustum->isIn_LocalFrustum(vPosition, fRange);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_Debug(const wstring strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pRenderTarget_Manager->Ready_Debug(strRenderTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_Debug(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pRenderTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif // _DEBUG

void CGameInstance::Get_FileNames(const wstring& strPath, vector<wstring>& vecOut)
{
	m_pConvert_Manager->Get_FileNames(strPath, vecOut);
}

void CGameInstance::Get_FileNames(const string& strPath, vector<string>& vecOut)
{
	m_pConvert_Manager->Get_FileNames(strPath, vecOut);
}

wstring CGameInstance::Get_FileName(const wstring& strPath)
{
	return m_pConvert_Manager->Get_FileName(strPath);
}

string CGameInstance::Get_FileName(const string& strPath)
{
	return m_pConvert_Manager->Get_FileName(strPath);
}

void CGameInstance::Get_FileExts(const wstring& strPath, vector<wstring>& vecOut)
{
	m_pConvert_Manager->Get_FileExts(strPath, vecOut);
}

void CGameInstance::Get_FileExts(const string& strPath, vector<string>& vecOut)
{
	m_pConvert_Manager->Get_FileExts(strPath, vecOut);
}

wstring CGameInstance::Get_FileExt(const wstring& strPath)
{
	return m_pConvert_Manager->Get_FileExt(strPath);
}

string CGameInstance::Get_FileExt(const string& strPath)
{
	return m_pConvert_Manager->Get_FileExt(strPath);
}

void CGameInstance::Get_DirectoryName(const wstring& strPath, vector<wstring>& vecOut)
{
	m_pConvert_Manager->Get_DirectoryName(strPath, vecOut);
}

void CGameInstance::Get_DirectoryName(const string& strPath, vector<string>& vecOut)
{
	m_pConvert_Manager->Get_DirectoryName(strPath, vecOut);
}

wstring CGameInstance::Get_Directory(const wstring& strPath)
{
	return m_pConvert_Manager->Get_Directory(strPath);
}

string CGameInstance::Get_Directory(const string& strPath)
{
	return m_pConvert_Manager->Get_Directory(strPath);
}

_uint CGameInstance::Get_NumFolders(const wstring& strPath)
{
	return m_pConvert_Manager->Get_NumFolders(strPath);
}

_uint CGameInstance::Get_NumFolders(const string& strPath)
{
	return m_pConvert_Manager->Get_NumFolders(strPath);
}

_uint CGameInstance::Get_NumFiles(const wstring& strPath)
{
	return m_pConvert_Manager->Get_NumFiles(strPath);
}

_uint CGameInstance::Get_NumFiles(const string& strPath)
{
	return m_pConvert_Manager->Get_NumFiles(strPath);
}

_bool CGameInstance::IsExists_Path(const wstring& strPath)
{
	return m_pConvert_Manager->IsExists_Path(strPath);
}

_bool CGameInstance::IsExists_Path(const string& strPath)
{
	return  m_pConvert_Manager->IsExists_Path(strPath);
}

wstring CGameInstance::StringToWstring(const string& str)
{
	return  m_pConvert_Manager->StringToWstring(str);
}

string CGameInstance::WstringToString(const wstring& str)
{
	return  m_pConvert_Manager->WstringToString(str);
}

string CGameInstance::Extract_String(const string& str, char cHead, char cTail)
{
	return  m_pConvert_Manager->Extract_String(str, cHead, cTail);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

void CGameInstance::Edit_Light(int iLightIndex, LIGHT_DESC lightDesc)
{
	return m_pLight_Manager->Edit_Light(iLightIndex, lightDesc);
}

void CGameInstance::Delete_Light(int iLightIndex)
{
	return m_pLight_Manager->Delete_Light(iLightIndex);
}

void CGameInstance::Delete_AllLights()
{
	return m_pLight_Manager->Delete_AllLights();
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->Free();

	DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGameObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pPicking);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pConvert_Manager);
	Safe_Release(m_pRandom_Manager);
}
