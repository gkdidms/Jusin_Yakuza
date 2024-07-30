#include "Reactor_Van.h"

#include "GameInstance.h"

#include "CarChase_Van.h"

CReactor_Van::CReactor_Van(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCarChase_Reactor{ pDevice, pContext }
{
}

CReactor_Van::CReactor_Van(const CReactor_Van& rhs)
	: CCarChase_Reactor{ rhs }
{
}

HRESULT CReactor_Van::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CReactor_Van::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	return S_OK;
}

void CReactor_Van::Priority_Tick(const _float& fTimeDelta)
{
}

void CReactor_Van::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CReactor_Van::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CReactor_Van::Ready_Monster(_int* pMonsterTypes)
{
	for (size_t i = 0; i < 3; ++i)
	{
		CCarChase_Van::CARCHASE_MONSTER_DESC Desc{};
		Desc.iWeaponType = i == 0 ? CCarChase_Monster::DRV : pMonsterTypes[i - 1];
		Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();

		if (FAILED(m_pGameInstance->Add_GameObject(m_iCurrentLevel, TEXT("Prototype_GameObject_CarChaseVan"), TEXT("Layer_Monster"), &Desc)))
			return E_FAIL;
	}
}

void CReactor_Van::Change_Animation()
{
	//벤에 관한 애니메이션 넣기
	if (m_isObjectDead)
		m_strAnimName = "w_mngcar_c_van_ded_1";

	if (FAILED(Setup_Animation()))
		return;
}

HRESULT CReactor_Van::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Van"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CReactor_Van* CReactor_Van::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReactor_Van* pInstance = new CReactor_Van(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CReactor_Van::Clone(void* pArg)
{
	CReactor_Van* pInstance = new CReactor_Van(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CReactor_Van::Free()
{
	__super::Free();
}
