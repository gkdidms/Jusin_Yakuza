#include "CarChase_CATBullet.h"

#include "GameInstance.h"

#include "Mesh.h"

#include "Highway_Kiryu.h"
#include "Highway_Taxi.h"

CCarChase_CATBullet::CCarChase_CATBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice, pContext }
{
}

CCarChase_CATBullet::CCarChase_CATBullet(const CCarChase_CATBullet& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CCarChase_CATBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarChase_CATBullet::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);
	m_pParentMatrix = pDesc->pParentMatrix;

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pParentMatrix));

	m_pTarget = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_iCurrentLevel, TEXT("Layer_Taxi"), 0))->Get_Kiryu();
	if (nullptr == m_pTarget)
		return E_FAIL;

	return S_OK;
}

void CCarChase_CATBullet::Priority_Tick(const _float& fTimeDelta)
{
}

void CCarChase_CATBullet::Tick(const _float& fTimeDelta)
{
	//플레이어를 타겟으로 따라다님
	//m_pTransformCom->LookAt_For_LandObject(XMLoadFloat4((_float4*)&m_pTarget->Get_ModelMatrix()->m[3]));
	_vector vLook = XMVector3Normalize(XMLoadFloat4((_float4*)&m_pTarget->Get_ModelMatrix()->m[3]) - XMLoadFloat4((_float4*)&m_WorldMatrix.m[3]));
	vLook = XMVectorSetW(vLook, 0.f);
	m_pTransformCom->LookAt(vLook, true);

	m_pTransformCom->Go_Straight_CustumSpeed(m_fSpeed, fTimeDelta);

	_matrix ParentMatrix = XMMatrixIdentity();
	ParentMatrix.r[3] = XMVectorSetY(XMLoadFloat4x4(m_pParentMatrix).r[3], XMVectorGetY(XMLoadFloat4x4(m_pParentMatrix).r[3]) + 2.f);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * ParentMatrix);
}

void CCarChase_CATBullet::Late_Tick(const _float& fTimeDelta)
{
	m_pGameInstance->Add_Renderer(CRenderer::RENDER_NONBLENDER, this);
}

HRESULT CCarChase_CATBullet::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	int i = 0;
	for (auto& pMesh : m_pModelCom->Get_Meshes())
	{
		if (nullptr != m_pMaterialCom)
		{
			if (FAILED(m_pMaterialCom->Bind_Shader(m_pShaderCom, m_pModelCom->Get_MaterialName(pMesh->Get_MaterialIndex()))))
				return E_FAIL;
		}

		_bool fFar = m_pGameInstance->Get_CamFar();
		m_pShaderCom->Bind_RawValue("g_fFar", &fFar, sizeof(_float));

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		_bool isRS = true;
		_bool isRD = true;
		_bool isRM = true;
		_bool isRT = true;
		_bool isMulti = true;
		_float fRDCount = 1.f;

		if (m_pGameInstance->Find_String(pMesh->Get_Name(), "shirts") ||
			m_pGameInstance->Find_String(pMesh->Get_Name(), "jacket") ||
			m_pGameInstance->Find_String(pMesh->Get_Name(), "pants"))
			fRDCount = 50.f;
		m_pShaderCom->Bind_RawValue("g_RDCount", &fRDCount, sizeof(_float));


		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MultiDiffuseTexture", i, aiTextureType_SHININESS)))
			isMulti = false;
		m_pShaderCom->Bind_RawValue("g_isMulti", &isMulti, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RSTexture", i, aiTextureType_SPECULAR)))
			isRS = false;
		m_pShaderCom->Bind_RawValue("g_isRS", &isRS, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("IsY3Shader", &isRS, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RDTexture", i, aiTextureType_OPACITY)))
			isRD = false;
		m_pShaderCom->Bind_RawValue("g_isRD", &isRD, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RMTexture", i, aiTextureType_METALNESS)))
			isRM = false;
		m_pShaderCom->Bind_RawValue("g_isRM", &isRM, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RTTexture", i, aiTextureType_EMISSIVE)))
			isRT = false;
		m_pShaderCom->Bind_RawValue("g_isRT", &isRT, sizeof(_bool));

		m_pShaderCom->Begin(0);		//디폴트

		m_pModelCom->Render(i);

		i++;
	}

	return S_OK;
}

HRESULT CCarChase_CATBullet::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_RcktGunBullet"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Material_RcktGunBullet"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::COLLIDER_AABB;
	ColliderDesc.vExtents = _float3(0.3, 0.3, 0.3);
	ColliderDesc.vCenter = _float3(0, ColliderDesc.vExtents.y, 0);

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarChase_CATBullet::Bind_ResourceData()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CCarChase_CATBullet* CCarChase_CATBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarChase_CATBullet* pInstance = new CCarChase_CATBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CCarChase_CATBullet::Clone(void* pArg)
{
	CCarChase_CATBullet* pInstance = new CCarChase_CATBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCarChase_CATBullet::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pColliderCom);
}
