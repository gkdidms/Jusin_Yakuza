#include "Highway_Van.h"

#include "GameInstance.h"

CHighway_Van::CHighway_Van(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CHighway{ pDevice, pContext }
{
}

CHighway_Van::CHighway_Van(const CHighway_Van& rhs)
	: CHighway{ rhs }
{
}

HRESULT CHighway_Van::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHighway_Van::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CHighway_Van::Priority_Tick(const _float& fTimeDelta)
{
}

void CHighway_Van::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CHighway_Van::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CHighway_Van::Change_Animation()
{
	//벤에 관한 애니메이션 넣기

	if (m_isObjectDead)
		m_strAnimName = "w_mngcar_c_van_ded_1";

	if (FAILED(Setup_Animation()))
		return;
}

HRESULT CHighway_Van::Add_Components()
{
	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Shader_VtxAnim"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Model_Van"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_ReactorHighwayAnim"),
		TEXT("Com_Anim"), reinterpret_cast<CComponent**>(&m_pAnimCom))))
		return E_FAIL;

	return S_OK;
}

CHighway_Van* CHighway_Van::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHighway_Van* pInstance = new CHighway_Van(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CHighway_Van::Clone(void* pArg)
{
	CHighway_Van* pInstance = new CHighway_Van(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHighway_Van::Free()
{
	__super::Free();
}
