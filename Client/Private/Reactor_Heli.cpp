#include "Reactor_Heli.h"

#include "GameInstance.h"

#include "CarChase_Van.h"

CReactor_Heli::CReactor_Heli(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCarChase_Reactor{ pDevice, pContext }
{
}

CReactor_Heli::CReactor_Heli(const CReactor_Heli& rhs)
	: CCarChase_Reactor{ rhs }
{
}

HRESULT CReactor_Heli::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CReactor_Heli::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strAnimName = "w_mngcar_e_van_wpr_aim_st_1";

	return S_OK;
}

void CReactor_Heli::Priority_Tick(const _float& fTimeDelta)
{
}

void CReactor_Heli::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_isObjectDead = Check_Dead();
}

void CReactor_Heli::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CReactor_Heli::Ready_Monster(_int* pMonsterTypes)
{
	for (size_t i = 0; i < 3; ++i)
	{
		if (i > 0 && pMonsterTypes[i - 1] == -1)
			break;

		CCarChase_Van::CARCHASE_MONSTER_DESC Desc{};
		Desc.iWeaponType = i == 0 ? CCarChase_Monster::DRV : pMonsterTypes[i - 1];
		Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Desc.pBoneMatrix = m_pModelCom->Get_BoneTransformationMatrix("anm_root");
		Desc.iLineDir = m_iLineDir;
		Desc.iObjectIndex = m_iObjectIndex + i;

		CCarChase_Monster* pMonster = dynamic_cast<CCarChase_Monster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CarChaseVan"), &Desc));
		if (nullptr == pMonster)
			return E_FAIL;

		m_Monsters.emplace_back(pMonster);
	}

	return S_OK;
}

void CReactor_Heli::Change_Animation()
{
	if (m_isObjectDead)
		m_strAnimName = "w_mngcar_c_van_ded_1";

	//벤에 관한 애니메이션 넣기
	if (m_strAnimName == "w_mngcar_e_van_wpr_aim_st_1")
		m_iAnim = 1;
	if (m_strAnimName == "w_mngcar_c_van_ded_1")
		m_iAnim = 0;

	if (m_iAnim == 0 && m_pModelCom->Get_AnimFinished())
		m_isDead = true;
}

HRESULT CReactor_Heli::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Heli"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CReactor_Heli* CReactor_Heli::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReactor_Heli* pInstance = new CReactor_Heli(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CReactor_Heli::Clone(void* pArg)
{
	CReactor_Heli* pInstance = new CReactor_Heli(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CReactor_Heli::Free()
{
	__super::Free();
	
}
