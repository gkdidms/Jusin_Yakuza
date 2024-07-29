#include "AI_CarChase.h"

#include "GameInstance.h"

#include "Player.h"
#include "CarChase_Monster.h"

CAI_CarChase::CAI_CarChase()
    : CBTNode{},
    m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

CAI_CarChase::CAI_CarChase(const CAI_CarChase& rhs)
    : CBTNode{ rhs },
    m_pGameInstance{ rhs.m_pGameInstance }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CAI_CarChase::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAI_CarChase::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    AI_CARCHASE_DESC* pDesc = static_cast<AI_CARCHASE_DESC*>(pArg);
    m_pAnimCom = pDesc->pAnim;
    m_pThis = pDesc->pThis;

    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

    return S_OK;
}

void CAI_CarChase::Tick(const _float& fTimeDelta)
{
    this->Execute();
}

_float CAI_CarChase::DistanceFromPlayer()
{
	_vector vPlayerPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vThisPos = m_pThis->m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDistance = XMVector3Length(vPlayerPos - vThisPos).m128_f32[0]; // 거리 측정	

	return fDistance;
}

_bool CAI_CarChase::isBehine()
{
	//몬스터를 기준으로 플레이어가 뒤에 존재하는지 확인하기.
	_vector vThisLook = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	_vector vThisPos = m_pThis->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	_vector vToPlayer = XMVector3Normalize(vPlayerPos - vThisPos);

	_float fDot = XMVector3Dot(vThisLook, vToPlayer).m128_f32[0];
	_float fAngle = acos(fDot);

	//플레이어의 시야각을 180도로 두고 계산한다.
	//뒤에 있는지 없는지 체크.
	return  fAngle > (XMConvertToRadians(180.f) * 0.5f);
}

void CAI_CarChase::LookAtPlayer()
{
	_vector vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	m_pThis->Get_TransformCom()->LookAt_For_LandObject(vPlayerPos);
}

_bool CAI_CarChase::Find_PlayerCurrentAnimationName(string strAnimName)
{
	string strPlayerAnimName = m_pPlayer->Get_CurrentAnimationName();
	if (strPlayerAnimName.find(strAnimName) == string::npos)
		return false;

	return true;
}

void CAI_CarChase::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pRoot);
}
