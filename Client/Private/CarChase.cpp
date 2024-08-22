#include "CarChase.h"

#include "GameInstance.h"
#include "UIManager.h"

#include "CarChase_Monster.h"
#include "CarChase_Reactor.h"
#include "CarChaseCamera.h"

#include "Highway_Taxi.h"
#include "Highway_Kiryu.h"

CCarChase::CCarChase()
	: m_pGameInstance{ CGameInstance::GetInstance() },
	m_pUIManager{ CUIManager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pUIManager);
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

//���� ���� & �÷��̾� dir / ī�޶� dir ����
_bool CCarChase::Start()
{
	vector<STAGE_MONSTER_INFO> MonsterInfo = m_Info.MonsterInfo;
	CHighway_Taxi* pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Taxi"), 0));

	//������ ��ġ�� �÷��̾� ��������Ʈ �ε����� -10��ŭ
	_int iWaypoint = m_Info.iStageDir == DIR_F ? pPlayer->Get_CurrentWaypointIndex() + 3: pPlayer->Get_CurrentWaypointIndex() - 10;

	if (iWaypoint < 0)
	{
		//0���� �۴ٸ�
		iWaypoint = pPlayer->Get_WaypointSize() + iWaypoint;
	}

	//���� ����
	for (auto& iter : MonsterInfo)
	{
		CCarChase_Reactor::HIGHWAY_IODESC Desc{};
		Desc.fRotatePecSec = XMConvertToRadians(180.f);
		Desc.fSpeedPecSec = 10.f;
		Desc.iNaviNum = 0;
		Desc.iStageDir = m_Info.iStageDir; // �������� ���� ����
		Desc.iLineDir = iter.iMonsterDir; // ���� ��, ��, �� ����
		Desc.iObjectIndex = iter.iObjectIndex; // ������Ʈ id ��
		Desc.iNaviRouteNum = iter.iMonsterLine; // ���Ͱ� ����� line�� index
		Desc.iWaypointIndex = iWaypoint; // ������ waypoint
		memcpy(Desc.iMonsterWeaponType, iter.iWeaponType, sizeof(_int) * 2); // ���� ���� (Ż �͸��� ���Ͱ� �ٸ���)

		wstring strGameObject = TEXT("");

		if (iter.iReactorType == CCarChase_Monster::REACTOR_VAN)
			strGameObject = TEXT("Prototype_GameObject_ReactorVan");
		else if (iter.iReactorType == CCarChase_Monster::REACTOR_SEDAN)
			strGameObject = TEXT("Prototype_GameObject_ReactorSedan");
		else if (iter.iReactorType == CCarChase_Monster::REACTOR_BIKE)
			strGameObject = TEXT("Prototype_GameObject_ReactorBike");
		else if (iter.iReactorType == CCarChase_Monster::REACTOR_HELI)
			strGameObject = TEXT("Prototype_GameObject_ReactorHeli");

		if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), strGameObject, TEXT("Layer_Reactor"), &Desc)))
			return false;
	}

	Set_TaxiStageDir();

	//ī�޶� �������� ���� �־��ֱ�
	CCarChaseCamera* pCamera = dynamic_cast<CCarChaseCamera*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"), CAMERA_CARCHASE));
	pCamera->Set_StageDir(m_Info.iStageDir);

	return true;
}

//���� ���������� �Ѿ�� ���� ���ǹ� ����
_bool CCarChase::Running()
{
	vector<CGameObject*> Reactors = m_pGameInstance->Get_GameObjects(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Reactor"));
	vector<STAGE_MONSTER_INFO> MonsterInfo = m_Info.MonsterInfo;

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
	{
		for (auto& pReactor : Reactors)
			pReactor->Set_Dead();

		return true;
	}*/

	//Set_TaxiStageDir();				//�޸��� �߿��� ������ ��ġ�� �ٲ� �� �����Ƿ�, ���� ü����

	if (Reactors.size() <= 0)
		return true;

	return false;
}

//�÷��̾� �ڸ� �̵�
_bool CCarChase::End()
{
	// ���� �������� ����� ���� �������� ������ �ٸ��ٸ�
	if (m_Info.iPlayerLine != m_Info.iNextPlayerLine)
	{
		CHighway_Taxi* pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Taxi"), 0));
		pPlayer->Set_NavigationRouteIndex(m_Info.iNextPlayerLine);
		pPlayer->Sit_Swap();
	}

	return true;
}

void CCarChase::Set_TaxiStageDir()
{
	CHighway_Taxi* pPlayer = dynamic_cast<CHighway_Taxi*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Taxi"), 0));

	//�÷��̾� �������� ���� �� �־���
	if (nullptr == m_pUIManager->Get_Target())
		pPlayer->Set_Dir(m_Info.iStageDir);
	else
	{
		_vector vTaxiPos = XMLoadFloat3(pPlayer->Get_Kiryu()->Get_Pos());
		_vector vTargetPos = m_pUIManager->Get_Target()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		_vector vDir = XMVector3Normalize(vTargetPos- vTaxiPos);
		_vector vTaxiLook = XMLoadFloat3(pPlayer->Get_Kiryu()->Get_Right());

		_float fDot = acos(XMVectorGetX(XMVector3Dot(vDir, vTaxiLook)));

		_uint iDir = m_Info.iStageDir;

		//const float PI = 3.14159265359f;
		//const float FRONT_ANGLE = PI / 8;          // 22.5��
		//const float FRONT_DIAGONAL_ANGLE = 3 * PI / 8;  // 67.5��
		//const float SIDE_ANGLE = 5 * PI / 8;       // 112.5��
		//const float BACK_DIAGONAL_ANGLE = 7 * PI / 8;  // 157.5��

		const float FRONT_ANGLE = XMConvertToRadians(10.f);
		const float FRONT_DIAGONAL_ANGLE = XMConvertToRadians(45.f);
		const float SIDE_ANGLE = XMConvertToRadians(160.f);
		const float BACK_DIAGONAL_ANGLE = XMConvertToRadians(170.f);

		if (fDot <= FRONT_ANGLE) {
			// �� (Front)
			iDir = 0;
		}
		else if (fDot <= FRONT_DIAGONAL_ANGLE) {
			// �մ밢�� (Front Diagonal)
			iDir = 1;
		}
		else if (fDot <= SIDE_ANGLE) {
			// �� (Side)
			iDir = 2;
		}
		else if (fDot <= BACK_DIAGONAL_ANGLE) {
			// �޴밢�� (Back Diagonal)
			iDir = 3;
		}
		else {
			// �� (Back)
			iDir = 4;
		}

		// 0 ��, 1 �� �밢, 2 ��, 3 �� �밢, 4 ��
		pPlayer->Set_Dir(iDir);
	}
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
	Safe_Release(m_pUIManager);
}
