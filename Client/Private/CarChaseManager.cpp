#include "CarChaseManager.h"

#include "GameInstance.h"

#include "CarChase.h"
#include "CarChase_Monster.h"
#include "CarChase_Reactor.h"

#include "Highway_Taxi.h"

void CCarChaseManager::Set_Coll()
{
}

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

	//�ӽ÷� �÷��̾ �����Ѵ�.
	CHighway_Taxi::HIGHWAY_TEXI_DESC Desc{};
	Desc.fRotatePecSec = XMConvertToRadians(180.f);
	Desc.fSpeedPecSec = 10.f;
	Desc.iNaviNum = 0;
	Desc.iNaviRouteNum = LINE_A;
	Desc.iObjectIndex = 999999;
	if (FAILED(m_pGameInstance->Add_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Prototype_GameObject_Taxi"), TEXT("Layer_Taxi"), &Desc)))
		return E_FAIL;

	return S_OK;
}

void CCarChaseManager::Tick()
{
	if (m_isFinished)
		return;

	if (m_isStart)
	{
		//������ �� ����Ʈ â �ߵ��� ���� 
	}

	//���� ���������� �Ѿ��
	if (m_Stages[m_iCurrentStage]->Tick())
	{
		m_iCurrentStage++;

		if (m_iCurrentStage >= m_Stages.size())
		{
			m_isFinished = true;
			return;
		}
	}
}

/*
Stage�� DIR_F, DIR_B	 �� ��� ������ ���� ������ 
DIR_F, DIR_B �� �ȴ�.
*/
HRESULT CCarChaseManager::Ready_Stage()
{
	//1 ��������
	vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_1 = {
		CCarChase::STAGE_MONSTER_INFO{
			100000,
			LINE_D,
			DIR_M,
			CCarChase_Monster::REACTOR_HELI,
			{ CCarChase_Monster::RKT, CCarChase_Monster::GAT }
		}
	};

	CCarChase::STAGE_INFO StageInfo_1 = CCarChase::STAGE_INFO{
		LINE_A,
		LINE_B,
		DIR_R,
		MonsterInfo_1
	};

	CCarChase::CARCHASE_DESC Desc{};
	Desc.Info = StageInfo_1;

	CCarChase* pStage1 = CCarChase::Create(&Desc);
	if (nullptr == pStage1)
		return E_FAIL;
	
	m_Stages.emplace_back(pStage1);

	//2 ��������
	vector<CCarChase::STAGE_MONSTER_INFO> MonsterInfo_2 = {
		CCarChase::STAGE_MONSTER_INFO{
			200000,
			LINE_C,
			DIR_F,
			CCarChase_Monster::REACTOR_VAN,
			{ CCarChase_Monster::WPR, -1}
		},
		CCarChase::STAGE_MONSTER_INFO{
			200001,
			LINE_D,
			DIR_F,
			CCarChase_Monster::REACTOR_VAN,
			{ CCarChase_Monster::WPR, -1 }
		},
	};

	CCarChase::STAGE_INFO StageInfo_2 = CCarChase::STAGE_INFO{
		LINE_B,
		LINE_A,
		DIR_F,
		MonsterInfo_2
	};

	Desc.Info = StageInfo_2;
	CCarChase* pStage2 = CCarChase::Create(&Desc);
	if (nullptr == pStage2)
		return E_FAIL;

	m_Stages.emplace_back(pStage2);

	////3 ��������
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
	//	LINE_C,
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

	for (auto& pStage : m_Stages)
		Safe_Release(pStage);
}
