#include "CarChase.h"

#include "GameInstance.h"

#include "CarChase_Monster.h"
#include "CarChase_Reactor.h"

#include "Highway_Taxi.h"

CCarChase::CCarChase()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CCarChase::~CCarChase()
{
}

HRESULT CCarChase::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CARCHASE_DESC* pDesc = static_cast<CARCHASE_DESC*>(pArg);
	m_Info = pDesc->Info;

	m_iState = START;

	return S_OK;
}

//ƽ�� ���鼭 ������Ʈ ������ �����Ѵ�.
_bool CCarChase::Tick()
{
	if (m_iState == START)
		m_iState = Start() ? RUNNING : START;
	else if (m_iState == RUNNING)
		m_iState = Running() ? STAGE_END : RUNNING;
	else
		return End();
		
	return false;
}

//���� ���� & �÷��̾� �ڸ� �̵�
_bool CCarChase::Start()
{
	vector<STAGE_MONSTER_INFO> MonsterInfo = m_Info.MonsterInfo;

	for (auto& iter : MonsterInfo)
	{
		CCarChase_Reactor::HIGHWAY_IODESC Desc{};
		Desc.fRotatePecSec = XMConvertToRadians(180.f);
		Desc.fSpeedPecSec = 10.f;
		Desc.iNaviNum = 0;
		Desc.iStageDir = m_Info.iStageDir; // �������� ���� ����
		Desc.iLineDir = iter.iMonsterDir; // ���� ��, ��, �� ����
		Desc.iObjectIndex = iter.iObjectIndex;
		Desc.iNaviRouteNum = iter.iMonsterLine;
		memcpy(Desc.iMonsterWeaponType, iter.iWeaponType, sizeof(_int) * 2);

		wstring strGameObject = TEXT("");

		if (iter.iReactorType == CCarChase_Monster::REACTOR_VAN)
			strGameObject = TEXT("Prototype_GameObject_ReactorVan");
		else if (iter.iReactorType == CCarChase_Monster::REACTOR_SEDAN)
			strGameObject = TEXT("Prototype_GameObject_ReactorSedan");
		else if (iter.iReactorType == CCarChase_Monster::REACTOR_BIKE)
			strGameObject = TEXT("Prototype_GameObject_ReactorBike");

		if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), strGameObject, TEXT("Layer_Reactor"), &Desc)))
			return false;
	}

	CHighway_Taxi* pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Taxi"), 0));
	
	pPlayer->Set_Dir(m_Info.iStageDir);

	if (m_Info.iPlayerLine != m_Info.iPrePlayerLine)
		pPlayer->Set_NavigationRouteIndex(m_Info.iPlayerLine);

	return true;
}

//���� ���������� �Ѿ�� ���� ���ǹ� ����
_bool CCarChase::Running()
{
	vector<CGameObject*> Reactors = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Reactor"));
	//vector<STAGE_MONSTER_INFO> MonsterInfo = m_Info.MonsterInfo;

	/*_uint iCount = { 0 };
	for (auto& Reactor : Reactors)
	{
		find_if(MonsterInfo.begin(), MonsterInfo.end(), [&](STAGE_MONSTER_INFO& Info)->_bool {
			if (Info.iObjectIndex == Reactor->Get_ObjID())
			{
				if (Reactor->isObjectDead())
					iCount++;

				return true;
			}
		});
	}

	if (MonsterInfo.size() == iCount)
		return true;*/

	if (Reactors.size() <= 0)
		return true;

	return false;
}

_bool CCarChase::End()
{
	return true;
}

CCarChase* CCarChase::Create(void* pArg)
{
	CCarChase* pInstance = new CCarChase();

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCarChase::Free()
{
	Safe_Release(m_pGameInstance);
}
