#include "AnimModel.h"
#include "GameInstance.h"

CAnimModel::CAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{pDevice, pContext}
{
}

CAnimModel::CAnimModel(const CAnimModel& rhs)
    :CGameObject{ rhs }
{
}

HRESULT CAnimModel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAnimModel::Initialize(void* pArg)
{
    GAMEOBJECT_DESC pDesc{};
    pDesc.fRotatePecSec = XMConvertToRadians(90.f);

    if (FAILED(CGameObject::Initialize(&pDesc)))
        return E_FAIL;

    m_strModelName = TEXT("Kiryu");

    if (FAILED(Add_Components()))
        return E_FAIL;

    //m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(rand() % 5, true));


    return S_OK;
}

void CAnimModel::Priority_Tick(const _float& fTimeDelta)
{
}

void CAnimModel::Tick(const _float& fTimeDelta)
{
    //_int iAnims = m_pModelCom->Get_NumAnimations();
    //if (m_pGameInstance->GetKeyState(DIK_UP) == TAP)
    //{
    //    m_iAnimIndex++;
    //    if (iAnims <= m_iAnimIndex)
    //        m_iAnimIndex = m_pModelCom->Get_NumAnimations() - 1;
    //}
    //if (m_pGameInstance->GetKeyState(DIK_DOWN) == TAP)
    //{
    //    m_iAnimIndex--;
    //    if (0 >= m_iAnimIndex)
    //        m_iAnimIndex = 0;
    //}

    CModel::ANIMATION_DESC desc{ m_iAnimIndex, true };

    m_pModelCom->Set_AnimationIndex(desc, 7.f);

    m_pModelCom->Play_Animation(fTimeDelta);
}

void CAnimModel::Late_Tick(const _float& fTimeDelta)
{
    m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CAnimModel::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    for (size_t i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE);

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CAnimModel::Set_Scaled(_float x, _float y, _float z)
{
    m_pTransformCom->Set_Scale(x, y, z);
}

void CAnimModel::Set_Rotation(_uint iAxis, _float vRadian, _float fTimeDelta)
{
    m_pTransformCom->Turn(m_pTransformCom->Get_State((CTransform::STATE)iAxis), vRadian * fTimeDelta);
}

const vector<class CAnimation*>& CAnimModel::Get_Animations()
{
    return m_pModelCom->Get_Animations();
}

const vector<class CBone*>& CAnimModel::Get_Bones()
{
    return m_pModelCom->Get_Bones();
}

void CAnimModel::Change_Model(wstring strModelName)
{
    Safe_Release(m_pModelCom);

    m_iAnimIndex = 0;
    m_strModelName = strModelName;

    wstring strComponentTag = TEXT("Prototype_Component_Model_") + m_strModelName;

    m_pModelCom = reinterpret_cast<CModel*>(m_pGameInstance->Add_Component_Clone(LEVEL_EDIT, strComponentTag));
}

void CAnimModel::Change_Animation(_uint iAnimIndex)
{
    m_iAnimIndex = iAnimIndex;
}

HRESULT CAnimModel::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    wstring strComponentTag = TEXT("Prototype_Component_Model_") + m_strModelName;

    if (FAILED(__super::Add_Component(LEVEL_EDIT, strComponentTag, TEXT("Com_Model"),
        reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CAnimModel::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

CAnimModel* CAnimModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAnimModel* pInstance = new CAnimModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CAnimModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAnimModel::Clone(void* pArg)
{
    CAnimModel* pInstance = new CAnimModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CAnimModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAnimModel::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
