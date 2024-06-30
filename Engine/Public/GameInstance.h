#pragma once

#include "Renderer.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "RandomManager.h"
#include "RenderTarget.h"

BEGIN(Engine)
class ENGINE_DLL CGameInstance :
    public CBase
{
    DECLARE_SINGLETON(CGameInstance)

private:
    CGameInstance();
    virtual ~CGameInstance() = default;

public:
    HRESULT Initialize_Engine(_uint iMaxLevelIndex, const ENGINE_DESC& Engine_Desc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
    void Tick(const _float& fTimeDelta);
    void Draw();
    void Draw_From_Tool();

    /* GraphicDevice */
public:
    HRESULT Clear_BackBuffer_View(_float4 vClearColor);
    HRESULT Clear_DepthStencil_View();
    HRESULT Present();

    /*Sound_Manager*/
public: 
    _bool PlaySound_W(const wstring pSoundKey, CHANNELID eID, float fVolume);
    void PlayBGM(const wstring pSoundKey, float fVolume);
    void StopSound(CHANNELID eID);
    void StopAll();
    void SetChannelVolume(CHANNELID eID, float fVolume);

    /* Timer */
public:
    _float		Get_TimeDelta(const _tchar* pTimerTag);
    void		Update_TimeDelta(const _tchar* pTimerTag);
    HRESULT		Ready_Timer(const _tchar* pTimerTag);
    void Set_TimeSpeed(const _tchar* pTimerTag, _float fSpeed); //���ǵ� ���� ���� Time ���� ���� / ������
    void Reset_TimeDelta(const _tchar* pTimerTag);

    /* Input_Device */
public:
    _byte	Get_DIKeyState(_ubyte byKeyID);
    _byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
    _long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
    KEYSTATE GetKeyState(_ubyte byKeyID); //Key Input ���ϱ� ���� ��� ex) GetKeyState(DIK_M) == HOLD; 
    KEYSTATE GetMouseState(MOUSEKEYSTATE eMouseState); // Mouse Input ���ϱ� ���� ��� ex) GetKeyState(DIM_LB) == HOLD; 

    /* Level_Manager */
public:
    HRESULT Open_Level(_uint iLevelIndex, class CLevel* pLevel);
    _uint Get_CurrentLevel(); // ���� ������ �ε��� ��ȯ

    /* GameObject_Manager */
public: 
    HRESULT Add_GameObject_Prototype(const wstring strGameObjectTag, class CGameObject* pGameObject); // GameObject Prototype ���� & ����
    HRESULT Add_GameObject(_uint iLevelIndex, const wstring strGameObjectTag, const wstring strLayerTag, void* pArg = nullptr); // GameObject Clone ���� & ����
    class CGameObject* Clone_Object(const wstring strGameObjectTag, void* pArg); // GameObject Clone ���� & ��ȯ (���� x)
    void Clear_Object(_uint iLevelIndex); // GameObject ����
    class CComponent* Get_GameObject_Component(_uint iLevelIndex, const wstring strLayerTag, const wstring strComponentTag, _uint iIndex = 0);
    class CGameObject* Get_GameObject(_uint iLevelIndex, const wstring strLayerTag, _uint iIndex);
    vector<CGameObject*> Get_GameObjects(_uint iLevelIndex, const wstring strLayerTag); // Layer�� ����� ��ü ����Ʈ(vector) ��ȯ

    /* Component_Manager */
public: 
    HRESULT Add_Component_Prototype(_uint iLevelIndex, const wstring strComponentTag, CComponent* pComponent); // Component Prototype ���� & ����
    CComponent* Add_Component_Clone(_uint iLevelIndex, const wstring strComponentTag, void* pArg = nullptr); // Component Clone ���� & ����

    /* Renderer */
public: 
    void Add_Renderer(CRenderer::RENDERER_STATE eRenderState, class CGameObject* pGameObject);
    void Set_HDR(_bool isHDR);
    void Set_HDRLight(_float fLight); // HDR �� ���� ���� 
    _bool isHDR();
    _float Get_HDRLight();
    void Set_SSAO(_bool isSSAO);
    _bool isSSAO();
    void Set_SSAORadiuse(_float fRadiuse);
    _float Get_SSAORadiuse();
    void Set_SSAOBlur(_float fBlur);
    _float Get_SSAOBlur();
    _float Get_SSAOBias();
    void Set_SSAOBias(_float fBias);

#ifdef _DEBUG
public:
    HRESULT Add_DebugComponent(class CComponent* pComponent);
    void Set_DebugView(_bool isDebugView);
    _bool isDebugView();
#endif // _DEBUG

    /* PipeLine */
public: 
    const _float4x4* Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState);
    _matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState);
    const _float4x4* Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState);
    _matrix Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState);
    const _float4* Get_CamPosition_Float4();
    _vector Get_CamPosition();
    void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix matTransform);
    const _float4* Get_ComLook_Float4();
    _vector Get_CamLook();
    const _float* Get_CamFar();
    void Set_CamFar(_float fFar);

    /* Font_Manager */
public: 
    HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
    HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor);
    HRESULT Perspective_Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fScale, const _float& fTimeDelta);
    HRESULT Blend_Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor);

    /* Light_Manager */
public:
    HRESULT Add_Light(const LIGHT_DESC& LightDesc);
    HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
    void    Edit_Light(int  iLightIndex, LIGHT_DESC lightDesc);
    void    Delete_Light(int iLightIndex);
    void    Delete_AllLights();

    /* Picking */
public: 
    _vector Picking(_bool* isSuccess);
    _bool Picking_UI(CTransform* pUITransform);
    _float Get_Z();
    float   FindObjID(_bool* isSuccess);

    /*RenderTarget_Manager*/
public: 
    HRESULT Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
    HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag);
    HRESULT Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSView = nullptr, _bool isClear = true); // isClear == false �� �� �ʱ�ȭ �ȵ�.
    HRESULT End_MRT();
    HRESULT Bind_RenderTargetSRV(const wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
    HRESULT Copy_Resource(const wstring& strTargetTag, ID3D11Texture2D* pDesc);
    HRESULT Create_Texture(const wstring& strTargetTag, const wstring& strSaveFilePath);
    /* Frustum*/
public:
    void Transform_ToLocalSpace(_fmatrix WorldMatrixInv);
    _bool isIn_WorldFrustum(_fvector vPosition, _float fRange = 0.f);
    _bool isIn_LocalFrustum(_fvector vPosition, _float fRange = 0.f);

#ifdef _DEBUG
public:
    HRESULT Ready_Debug(const wstring strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
    HRESULT Render_Debug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

    /* Convert_Manager */
public:
    void Get_FileNames(const wstring& strPath, vector<wstring>& vecOut);
    void Get_FileNames(const string& strPath, vector<string>& vecOut);

    wstring Get_FileName(const wstring& strPath);
    string Get_FileName(const string& strPath);

    void Get_FileExts(const wstring& strPath, vector<wstring>& vecOut);
    void Get_FileExts(const string& strPath, vector<string>& vecOut);
    wstring Get_FileExt(const wstring& strPath);
    string Get_FileExt(const string& strPath);

    void Get_DirectoryName(const wstring& strPath, vector<wstring>& vecOut);
    void Get_DirectoryName(const string& strPath, vector<string>& vecOut);
    wstring Get_Directory(const wstring& strPath);
    string Get_Directory(const string& strPath);

    _uint Get_NumFolders(const wstring& strPath);
    _uint Get_NumFolders(const string& strPath);
    _uint Get_NumFiles(const wstring& strPath);	
    _uint Get_NumFiles(const string& strPath);	

    _bool IsExists_Path(const wstring& strPath);		
    _bool IsExists_Path(const string& strPath);			

    wstring StringToWstring(const string& str);
    string WstringToString(const wstring& str);
    string Extract_String(const string& str, char cHead, char cTail);

public:
    template<typename T>
    T Get_Random(T Min, T Max);

private:
    class CGraphic_Device* m_pGraphic_Device = { nullptr };
    class CSoundMgr* m_pSound_Manager = { nullptr };
    class CInput_Device* m_pInput_Device = { nullptr };
    class CLevel_Manager* m_pLevel_Manager = { nullptr };
    class CGameObject_Manager* m_pGameObject_Manager = { nullptr };
    class CComponent_Manager* m_pComponent_Manager = { nullptr };
    class CTimer_Manager* m_pTimer_Manager = { nullptr };
    class CFont_Manager* m_pFont_Manager = { nullptr };
    class CRenderTarget_Manager* m_pRenderTarget_Manager = { nullptr };
    class CLight_Manager* m_pLight_Manager = { nullptr };
    class CRenderer* m_pRenderer = { nullptr };
    class CPipeLine* m_pPipeLine = { nullptr };
    class CPicking* m_pPicking = { nullptr };
    class CFrustum* m_pFrustum = { nullptr };
    class CConvert_Manager* m_pConvert_Manager = { nullptr };
    class CRandomManager* m_pRandom_Manager = { nullptr };


public:
    static void Release_Engine();
    virtual void Free() override;
};

END

template<typename T>
inline T CGameInstance::Get_Random(T Min, T Max)
{
    return m_pRandom_Manager->Get_Random(Min, Max);
}