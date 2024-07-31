#include "CarChaseManager.h"

#include "GameInstance.h"

#include "CarChase.h"
#include "CarChase_Monster.h"
#include "Highway_Taxi.h"

CCarChaseManager::CCarChaseManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice },
	m_pContext{ pContext },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCarChaseManager::Initialize()
{
	Ready_Stage();

	////임시로 플레이어를 생성한다.
	//CHighway_Taxi::HIGHWAY_TEXI_DESC Desc{};
	//Desc.fRotatePecSec = XMConvertToRadians(180.f);
	//Desc.fSpeedPecSec = 10.f;
	//Desc.iNaviNum = 0;
	//Desc.iNaviRouteNum = 0;
	//Desc.iObjectIndex = 999999;
	//if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Taxi"), TEXT("Layer_Texi"), &Desc)))
	//	return E_FAIL;

	return S_OK;
}

void CCarChaseManager::Tick()
{
	//다음 스테이지로 넘어가기
	if (m_Stages[m_iCurrentStage]->Tick())
	{
		m_iCurrentStage++;
	}
}

HRESULT CCarChaseManager::Ready_Stage()
{
	//1 스테이지
	vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_1 = {
		CCarChase::STAGE_MONSTER_INFO{
			100000,
			LINE_B,
			CCarChase_Monster::REACTOR_VAN,
			{ CCarChase_Monster::WPR, -1}
		},
	};

	CCarChase::STAGE_INFO StageInfo_1 = CCarChase::STAGE_INFO{
		LINE_A,
		LINE_C,
		MonsterInfo_1
	};

	CCarChase::CARCHASE_DESC Desc{};
	Desc.Info = StageInfo_1;

	CCarChase* pStage1 = CCarChase::Create(&Desc);
	if (nullptr == pStage1)
		return E_FAIL;
	
	m_Stages.emplace_back(pStage1);

	////2 스테이지
	//vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_2 = {
	//	CCarChase::STAGE_MONSTER_INFO{
	//		200000,
	//		LINE_B,
	//		CCarChase_Monster::REACTOR_VAN,
	//		{ CCarChase_Monster::WPR, -1}
	//	},
	//	CCarChase::STAGE_MONSTER_INFO{
	//		200001,
	//		LINE_D,
	//		CCarChase_Monster::REACTOR_VAN,
	//		{ CCarChase_Monster::WPR, -1 }
	//	},
	//};

	//CCarChase::STAGE_INFO StageInfo_2 = CCarChase::STAGE_INFO{
	//	LINE_C,
	//	LINE_D,
	//	MonsterInfo_2
	//};

	//Desc.Info = StageInfo_2;
	//CCarChase* pStage2 = CCarChase::Create(&Desc);
	//if (nullptr == pStage2)
	//	return E_FAIL;

	//m_Stages.emplace_back(pStage2);

	////3 스테이지
	//vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_3 = {
	//	CCarChase::STAGE_MONSTER_INFO{
	//		300000,
	//		LINE_A,
	//		CCarChase_Monster::REACTOR_VAN,
	//		{ CCarChase_Monster::WPR, -1}
	//	},
	//	CCarChase::STAGE_MONSTER_INFO{
	//		300001,
	//		LINE_B,
	//		CCarChase_Monster::REACTOR_VAN,
	//		{ CCarChase_Monster::WPR, -1}
	//	},
	//};

	//CCarChase::STAGE_INFO StageInfo_3 = CCarChase::STAGE_INFO{
	//	LINE_D,
	//	LINE_END,
	//	MonsterInfo_3
	//};

	//Desc.Info = StageInfo_3;
	//CCarChase* pStage3 = CCarChase::Create(&Desc);
	//if (nullptr == pStage3)
	//	return E_FAIL;

	//m_Stages.emplace_back(pStage3);
}

CCarChaseManager* CCarChaseManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarChaseManager* pInstanec = new CCarChaseManager(pDevice, pContext);

	if (FAILED(pInstanec->Initialize()))
		Safe_Release(pInstanec);

	return pInstanec;
}

void CCarChaseManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
