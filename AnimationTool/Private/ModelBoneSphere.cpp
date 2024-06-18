#include "ModelBoneSphere.h"
#include "GameInstance.h"

CModelBoneSphere::CModelBoneSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{pDevice, pContext}
{
}

CModelBoneSphere::CModelBoneSphere(const CModelBoneSphere& rhs)
    :CGameObject{ rhs }
{
}

HRESULT CModelBoneSphere::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CModelBoneSphere::Initialize(void* pArg)
{
    GAMEOBJECT_DESC pDesc{};
    pDesc.fRotatePecSec = XMConvertToRadians(90.f);

    if (FAILED(CGameObject::Initialize(&pDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CModelBoneSphere::Priority_Tick(const _float& fTimeDelta)
{
}

void CModelBoneSphere::Tick(const _float& fTimeDelta)
{
    if (m_pGameInstance->GetKeyState(DIK_UP) == TAP)
        m_iTextureIndex = 1;
    if (m_pGameInstance->GetKeyState(DIK_DOWN) == TAP)
        m_iTextureIndex = 0;
}

void CModelBoneSphere::Late_Tick(const _float& fTimeDelta)
{
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CModelBoneSphere::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    for (size_t i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex);

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CModelBoneSphere::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    wstring strComponentTag = TEXT("Prototype_Component_Model_Bone_Sphere");

    if (FAILED(__super::Add_Component(LEVEL_EDIT, strComponentTag, TEXT("Com_Model"),
        reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Texture_BoneSphere"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelBoneSphere::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

CModelBoneSphere* CModelBoneSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CModelBoneSphere* pInstance = new CModelBoneSphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CModelBoneSphere");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CModelBoneSphere::Clone(void* pArg)
{
    CModelBoneSphere* pInstance = new CModelBoneSphere(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CModelBoneSphere");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModelBoneSphere::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
