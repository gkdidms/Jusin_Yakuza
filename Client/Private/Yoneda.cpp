#include "Yoneda.h"

#include "GameInstance.h"
#include "Mesh.h"

#include "AI_Yoneda.h"


CYoneda::CYoneda(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext}
{
}

CYoneda::CYoneda(const CYoneda& rhs)
	: CMonster { rhs }
{
}

HRESULT CYoneda::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CYoneda::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CYoneda::Priority_Tick(const _float& fTimeDelta)
{
}

void CYoneda::Tick(const _float& fTimeDelta)
{
	m_pTree->Tick(fTimeDelta);

	Change_Animation(); //애니메이션 변경

	m_pModelCom->Play_Animation(fTimeDelta, m_pAnimCom, m_isAnimLoop);

	Synchronize_Root(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CYoneda::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CYoneda::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		_bool isRS = true;
		_bool isRD = true;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
			isRS = false;
		m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;

		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		if (pMesh->Get_AlphaApply())
			m_pShaderCom->Begin(1);     //블랜드
		else
			m_pShaderCom->Begin(0);		//디폴트

		m_pModelCom->Render(i);

		i++;
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CYoneda::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Model_Jimu"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_OBB;
	ColliderDesc.vExtents = _float3(0.8, 0.8, 0.8);
	ColliderDesc.vCenter = _float3(0, 0.f, 0);
	ColliderDesc.vRotation = _float3(0, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Anim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	//행동트리 저장
	CAI_Yoneda::AI_MONSTER_DESC AIDesc{};
	AIDesc.pAnim = m_pAnimCom;
	AIDesc.pState = &m_iState;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_Yoneda*>(m_pGameInstance->Add_BTNode(LEVEL_TEST, TEXT(""), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	return S_OK;
}

HRESULT CYoneda::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CYoneda::Change_Animation()
{
}

CYoneda* CYoneda::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CYoneda* pInstance = new CYoneda(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CYoneda::Clone(void* pArg)
{
	CYoneda* pInstance = new CYoneda(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CYoneda::Free()
{
	__super::Free();
}
