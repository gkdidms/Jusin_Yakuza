#include "WPHYakuza.h"

#include "GameInstance.h"
#include "Mesh.h"

#include "SocketCollider.h"
#include "Collision_Manager.h"

#include "AI_WPHYakuza.h"

CWPHYakuza::CWPHYakuza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster { pDevice, pContext }
{
}

CWPHYakuza::CWPHYakuza(const CWPHYakuza& rhs)
    : CMonster { rhs }
{
}

HRESULT CWPHYakuza::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWPHYakuza::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_wstrModelName = TEXT("Jimu");

    if (FAILED(Add_CharacterData()))
        return E_FAIL;

	m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.2f);

    return S_OK;
}

void CWPHYakuza::Priority_Tick(const _float& fTimeDelta)
{
}

void CWPHYakuza::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWPHYakuza::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWPHYakuza::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		_bool isCloth = true;
		string strMeshName = string(pMesh->Get_Name());
		if (strMeshName.find("hair") != string::npos || strMeshName.find("face") != string::npos ||
			strMeshName.find("foot") != string::npos || strMeshName.find("body") != string::npos ||
			strMeshName.find("eye") != string::npos)
			isCloth = false;

		m_pShaderCom->Bind_RawValue("g_isCloth", &isCloth, sizeof(_bool));

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

HRESULT CWPHYakuza::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, m_wstrModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.5, 0.8, 0.5);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Anim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom[DEFAULT]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_SyncAnim"),
		TEXT("Com_SyncAnim"), reinterpret_cast<CComponent**>(&m_pAnimCom[CUTSCENE]))))
		return E_FAIL;

	//행동트리 저장
	CAI_WPHYakuza::AI_MONSTER_DESC AIDesc{};
	AIDesc.pState = &m_iState;
	memcpy(AIDesc.pAnim, m_pAnimCom, sizeof(CAnim*) * ANIM_TYPE_END);
	AIDesc.pCurrentAnimType = &m_iCurrentAnimType;
	AIDesc.pThis = this;

	m_pTree = dynamic_cast<CAI_WPHYakuza*>(m_pGameInstance->Add_BTNode(m_iCurrentLevel, TEXT("Prototype_BTNode_WPHYakuza"), &AIDesc));
	if (nullptr == m_pTree)
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

void CWPHYakuza::Change_Animation()
{
	m_isAnimLoop = false;

	__super::Change_Animation();

	switch (m_iState)
	{
	case MONSTER_IDLE:
	{
		m_strAnimName = "e_wph_stand_btl";
		m_isAnimLoop = true;
		m_isDown = false;
		break;
	}
	case MONSTER_HEAVY_ATTACK:
	{
		m_strAnimName = "e_wph_atk_heavy";
		break;
	}
	default:
		break;
	}

	if (FAILED(Setup_Animation()))
		return;
}

CWPHYakuza* CWPHYakuza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWPHYakuza* pInstance = new CWPHYakuza(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
        Safe_Release(pInstance);

    return pInstance;
}

CGameObject* CWPHYakuza::Clone(void* pArg)
{
    CWPHYakuza* pInstance = new CWPHYakuza(*this);

    if (FAILED(pInstance->Initialize(pArg)))
        Safe_Release(pInstance);

    return pInstance;
}

void CWPHYakuza::Free()
{
    __super::Free();
}
