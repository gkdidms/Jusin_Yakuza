#include "TestMap.h"

#include "GameInstance.h"

CTestMap::CTestMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTestMap::CTestMap(const CTestMap& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CTestMap::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTestMap::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Componenets()))
        return E_FAIL;

    return S_OK;
}

void CTestMap::Priority_Tick(const _float& fTimeDelta)
{
}

void CTestMap::Tick(const _float& fTimeDelta)
{
}

void CTestMap::Late_Tick(const _float& fTimeDelta)
{
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CTestMap::Render()
{
    if (FAILED(Bind_ResourceData()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CTestMap::Add_Componenets()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Mesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Map"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestMap::Bind_ResourceData()
{
    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

CTestMap* CTestMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTestMap* pInstance = new CTestMap(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CTestMap");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestMap::Clone(void* pArg)
{
    CTestMap* pInstance = new CTestMap(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CTestMap");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestMap::Free()
{
    __super::Free();
}
