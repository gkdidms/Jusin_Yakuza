#include "MonsterGroup.h"

#include "GameInstance.h"

#include "Monster.h"

CMonsterGroup::CMonsterGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMonsterGroup::CMonsterGroup(const CMonsterGroup& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CMonsterGroup::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterGroup::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonsterGroup::Priority_Tick(const _float& fTimeDelta)
{
}

void CMonsterGroup::Tick(const _float& fTimeDelta)
{
}

void CMonsterGroup::Late_Tick(const _float& fTimeDelta)
{
}

/* 
군집 될 몬스터들을 만든다 
군집 될 몬스터의 수는 총 3마리
전투 타입은 랜덤으로 결정한다.
*/
HRESULT CMonsterGroup::Ready_Monsters()
{
	
	return S_OK;
}

CMonsterGroup* CMonsterGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterGroup* pInstance = new CMonsterGroup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CMonsterGroup::Clone(void* pArg)
{
	CMonsterGroup* pInstance = new CMonsterGroup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMonsterGroup::Free()
{
	__super::Free();
}
