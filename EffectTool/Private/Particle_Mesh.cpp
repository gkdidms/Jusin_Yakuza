#include "Particle_Mesh.h"
#include "GameInstance.h"
#include "VIBuffer_Instance_Mesh.h"

CParticle_Mesh::CParticle_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CEffect{ pDevice , pContext }
{
}

CParticle_Mesh::CParticle_Mesh(const CParticle_Mesh& rhs)
    :CEffect{ rhs },
    m_BufferInstance{ rhs.m_BufferInstance },
    m_strModelTag{rhs.m_strModelTag }
{
    
}

HRESULT CParticle_Mesh::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CParticle_Mesh::Initialize_Prototype(string strFilePath)
{
    if (FAILED(Load_Data(strFilePath)))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Mesh::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        PARTICLE_MESH_DESC* pDesc = static_cast<PARTICLE_MESH_DESC*>(pArg);

        m_strModelTag = pDesc->strModelTag;


        if (nullptr == pDesc->pWorldMatrix)
        {
            PARTICLE_MESH_DESC* pDesc = static_cast<PARTICLE_MESH_DESC*>(pArg);
            m_BufferInstance = pDesc->BufferInstance;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vStartPos));
        }
        else
        {
            m_pWorldMatrix = pDesc->pWorldMatrix;
            m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(m_pWorldMatrix));
        }
    }

    m_BufferInstance.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CParticle_Mesh::Priority_Tick(const _float& fTimeDelta)
{
}

void CParticle_Mesh::Tick(const _float& fTimeDelta)
{
    m_fCurTime += fTimeDelta;
    if (!m_BufferInstance.isLoop)
    {
        _float fTotalTime = m_fStartTime + m_BufferInstance.vLifeTime.y;
        if (m_fCurTime > fTotalTime)
            m_isDead = true;
    }

    if (m_fCurTime >= m_fStartTime)
    {
        if (m_iAction & iAction[ACTION_SPREAD])
        {
            m_pVIBufferCom->Spread(fTimeDelta); 
        }
        if (m_iAction & iAction[ACTION_DROP])
        {
            m_pVIBufferCom->Drop(fTimeDelta);
        }
        if (m_iAction & iAction[ACTION_SIZEUP])
        {
            m_pVIBufferCom->SizeUp_Time(fTimeDelta);
        }
        if (m_iAction & iAction[ACTION_SIZEDOWN])
        {
            m_pVIBufferCom->SizeDown_Time(fTimeDelta);
        }
        if (m_iAction & iAction[ACTION_NOBILLBOARD])
        {
            m_pVIBufferCom->RotSpread(fTimeDelta);
        }
    }


}

void CParticle_Mesh::Late_Tick(const _float& fTimeDelta)
{
    switch (m_eType)
    {
         case Client::CEffect::TYPE_MESH:
         {
             if (m_BufferInstance.isLoop)
             {
                 m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
             }
             else
             {
                 if (m_fCurTime >= m_fStartTime && !m_isDead)
                 {
                     m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
                 }
             }
             break;
         }
         case Client::CEffect::TYPE_MESHEFFECT:
         {
             if (m_BufferInstance.isLoop)
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
             break;
         }
    }
}

HRESULT CParticle_Mesh::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    

    for (size_t i = 0; i < m_BufferInstance.iNumInstance; i++)
    {
        m_pShaderCom->Begin(0);

        m_pVIBufferCom->Render(i);
    }

    return S_OK;
}

HRESULT CParticle_Mesh::Save_Data(const string strDirectory)
{
    return E_NOTIMPL;
}

HRESULT CParticle_Mesh::Load_Data(const string strDirectory)
{
    return E_NOTIMPL;
}

HRESULT CParticle_Mesh::Add_Components()
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, m_strModelTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_VIBuffer_Instance_Mesh"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &m_BufferInstance)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxInstance_Mesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(m_iCurrentLevel, m_TextureTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Mesh::Bind_ShaderResources()
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
    if (FAILED(m_pShaderCom->Bind_RawValue("g_lifeAlpha", &m_fLifeAlpha, sizeof(_float2))))
        return E_FAIL;

    _float Radian = XMConvertToRadians(m_fRotate++);
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRadian", &Radian, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

CParticle_Mesh* CParticle_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Mesh* pInstance = new CParticle_Mesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CParticle_Mesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CParticle_Mesh* CParticle_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string strFilePath)
{
    CParticle_Mesh* pInstance = new CParticle_Mesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strFilePath)))
    {
        MSG_BOX("Failed To Created : CParticle_Mesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Mesh::Clone(void* pArg)
{
    CParticle_Mesh* pInstance = new CParticle_Mesh(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CParticle_Mesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Mesh::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
