#pragma once
#include "Base.h"

BEGIN(Engine)
class CMaterial :
    public CBase
{
public:
    typedef struct tMeterialDesc {
        _char strNeterialName[MAX_PATH];
        _bool isUVShader;

        _float fOpacity;
        _float4 vSpecularColor;
        _float fSpecularPower;
        _float fAssetShader;
        _float fDisableRDRS;
        _float fEngine;
        _float fIsClothShader;
        _float fSkinShader;
        _float fRough;
        _float fIsY3Shader;
        _float Imperfection;
        _float SPShader;

        _float fRTX;
        _float fRTY;
        _float fRDRMRS_X;
        _float fRDRMRS_Y;
        _float fImperfection_UV;
        _float fEngine_UV;
    } METERIAL_DESC;

    struct MATERIAL_BUFFER {
        _int g_isUVShader = { false };

        float Opacity = { 0.f };
        float fSpecularPower = { 0.f };

        float AssetShader = { 0.f };
        float DisableRDRT = { 0.f };
        float Engine = { 0.f };
        float IsOEClothShader = { 0.f };
        float SkinShader = { 0.f };
        float Rough = { 0.f };
        float IsY3Shader = { 0.f };
        float Imperfection = { 0.f };
        float SPShader = { 0.f };


        float g_fRTX = { 0.f };
        float g_fRTY = { 0.f };
        float g_fRDRMRS_X = { 0.f };
        float g_fRDRMRS_Y = { 0.f };
        float f_fImperfection_UV = { 0.f };
        float f_fEngine_UV = { 0.f };

        float padding1;
        float padding2;

        _vector vSpecularColor;
    };

private:
    CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CMaterial() = default;

public:
    const string Get_Name() const { return string(m_Info.strNeterialName); }

public:
    HRESULT Initialize(METERIAL_DESC* pDesc);
    HRESULT Bind_Shader(class CShader* pShader);
    void Bind_ComputeShader();

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    METERIAL_DESC m_Info;

    ID3D11Buffer* m_pMaterialBuffer = { nullptr };

private:
    HRESULT Ready_Buffer();

public:
    static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, METERIAL_DESC* pDesc);
    virtual void Free();
};
END