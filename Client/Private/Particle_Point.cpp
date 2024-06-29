#include "Particle_Point.h"
#include "GameInstance.h"


CParticle_Point::CParticle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CEffect{ pDevice , pContext}
{
}

CParticle_Point::CParticle_Point(const CParticle_Point& rhs)
    :CEffect{ rhs },
    m_BufferInstance{rhs.m_BufferInstance }
{
}

HRESULT CParticle_Point::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CParticle_Point::Initialize_Prototype(string strFilePath)
{
    if (FAILED(Load_Data(strFilePath)))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Point::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        PARTICLE_POINT_DESC* pDesc = static_cast<PARTICLE_POINT_DESC*>(pArg);
        m_BufferInstance = pDesc->BufferInstance;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vStartPos));
    }

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CParticle_Point::Priority_Tick(const _float& fTimeDelta)
{
}

void CParticle_Point::Tick(const _float& fTimeDelta)
{

    m_fCurTime += fTimeDelta;
    if (!m_BufferInstance.isLoop)
    {
        _float fTotalTime = m_fStartTime + m_BufferInstance.vLifeTime.y;
        if (m_fCurTime > fTotalTime)
            m_isDead = true;
    }

    if(m_fCurTime>= m_fStartTime)
    {
        if (m_iAction & iAction[ACTION_SPREAD])
        {
            m_pVIBufferCom->Spread(fTimeDelta);
        }
        if (m_iAction & iAction[ACTION_DROP])
        {
            m_pVIBufferCom->Drop(fTimeDelta);
        }
    }



}

void CParticle_Point::Late_Tick(const _float& fTimeDelta)
{
    Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

    if (5 != m_iShaderPass)
        m_pVIBufferCom->Blend_Sort();


    if(m_BufferInstance.isLoop)
    {
        m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
    }
    else
    {
        if (m_fCurTime >= m_fStartTime && !m_isDead)
        {
            m_pGameInstance->Add_Renderer(CRenderer::RENDER_EFFECT, this);
        }
    }

}

HRESULT CParticle_Point::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

       m_pShaderCom->Begin(m_iShaderPass);

       if (5!=m_iShaderPass)
            m_pVIBufferCom->Render();
       else
           m_pVIBufferCom->Render();

       

    return S_OK;
}

void* CParticle_Point::Get_Instance()
{
    return &m_BufferInstance;
}


HRESULT CParticle_Point::Save_Data(const string strDirectory)
{
    string Directory = strDirectory;
    string ParticleTag = m_pGameInstance->WstringToString(m_ParticleTag);
    string TextureTag = m_pGameInstance->WstringToString(m_TextureTag);

    string headTag = "Prototype_GameObject_Particle_Point_";
    Directory += "/"+ headTag + ParticleTag + ".dat";

    ofstream out(Directory, ios::binary);

    out.sync_with_stdio(false);

    out.write((char*)&m_eType, sizeof(_uint));

    _int strlength = ParticleTag.length();
    out.write((char*)&strlength, sizeof(_int));
    out.write(ParticleTag.c_str(), strlength);

    _int strTexturelength = TextureTag.length();
    out.write((char*)&strTexturelength, sizeof(_int));
    out.write(TextureTag.c_str(), strTexturelength);

    out.write((char*)&m_iShaderPass, sizeof(_int));

    out.write((char*)&m_fStartTime, sizeof(_float));

    out.write((char*)&m_vStartPos, sizeof(_float4));
    
    out.write((char*)&m_iAction, sizeof(_uint));

    out.write((char*)&m_vStartColor, sizeof(_float4));

    out.write((char*)&m_vEndColor, sizeof(_float4));

    out.write((char*)&m_BufferInstance.iNumInstance, sizeof(_uint));
    out.write((char*)&m_BufferInstance.vOffsetPos, sizeof(_float3));
    out.write((char*)&m_BufferInstance.vPivotPos, sizeof(_float3));
    out.write((char*)&m_BufferInstance.bRadius, sizeof(_bool));
    out.write((char*)&m_BufferInstance.vRange, sizeof(_float3));
    out.write((char*)&m_BufferInstance.fRadius, sizeof(_float));
    out.write((char*)&m_BufferInstance.vSize, sizeof(_float2));
    out.write((char*)&m_BufferInstance.vRectSize, sizeof(_float2));
    out.write((char*)&m_BufferInstance.vSpeed, sizeof(_float2));
    out.write((char*)&m_BufferInstance.vLifeTime, sizeof(_float2));
    out.write((char*)&m_BufferInstance.isLoop, sizeof(_bool));

    out.flush();

    out.close();

    return S_OK;

}

HRESULT CParticle_Point::Load_Data(const string strDirectory)
{
    string Directory = strDirectory;

    ifstream in(Directory, ios::binary);
    
    in.sync_with_stdio(false);

    if (!in.is_open()) {
        MSG_BOX("파일 개방 실패");
        return E_FAIL;
    }

    string strFileName = m_pGameInstance->Get_FileName(Directory);

    in.read((char*)&m_eType, sizeof(_uint));

    _int strlength ;
    char charparticleTag[MAX_PATH] = {};

    in.read((char*)&strlength, sizeof(_int));   

    in.read(charparticleTag, strlength);
    string tag = charparticleTag;
    m_ParticleTag = m_pGameInstance->StringToWstring(tag);


    _int strTexturelength;
    char charTextureTag[MAX_PATH] = {};

    in.read((char*)&strTexturelength, sizeof(_int));

    in.read(charTextureTag, strTexturelength);
    string textag = charTextureTag;
    m_TextureTag = m_pGameInstance->StringToWstring(textag);


    in.read((char*)&m_iShaderPass, sizeof(_int));

    in.read((char*)&m_fStartTime, sizeof(_float));

    in.read((char*)&m_vStartPos, sizeof(_float4));

    in.read((char*)&m_iAction, sizeof(_uint));

    in.read((char*)&m_vStartColor, sizeof(_float4));

    in.read((char*)&m_vEndColor, sizeof(_float4));

    in.read((char*)&m_BufferInstance.iNumInstance, sizeof(_uint));
    in.read((char*)&m_BufferInstance.vOffsetPos, sizeof(_float3));
    in.read((char*)&m_BufferInstance.vPivotPos, sizeof(_float3));
    in.read((char*)&m_BufferInstance.bRadius, sizeof(_bool));
    in.read((char*)&m_BufferInstance.vRange, sizeof(_float3));
    in.read((char*)&m_BufferInstance.fRadius, sizeof(_float));
    in.read((char*)&m_BufferInstance.vSize, sizeof(_float2));
    in.read((char*)&m_BufferInstance.vRectSize, sizeof(_float2));
    in.read((char*)&m_BufferInstance.vSpeed, sizeof(_float2));
    in.read((char*)&m_BufferInstance.vLifeTime, sizeof(_float2));
    in.read((char*)&m_BufferInstance.isLoop, sizeof(_bool));

    in.close();

    return S_OK;
}

HRESULT CParticle_Point::Add_Components()
{
    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &m_BufferInstance)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_TEST, m_TextureTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Point::Bind_ShaderResources()
{

    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vStartColor", &m_vStartColor, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vEndColor", &m_vEndColor, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CParticle_Point* CParticle_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Point* pInstance = new CParticle_Point(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CParticle_Point");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CParticle_Point* CParticle_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string strFilePath)
{
    CParticle_Point* pInstance = new CParticle_Point(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strFilePath)))
    {
        MSG_BOX("Failed To Created : CParticle_Point");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Point::Clone(void* pArg)
{
    CParticle_Point* pInstance = new CParticle_Point(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CParticle_Point");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Point::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
