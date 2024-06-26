#include "CharacterData.h"  
#include "GameInstance.h"

CCharacterData::CCharacterData()
	:m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCharacterData::Initialize(wstring wstrModelName)
{
	wstring wstrFilePath = TEXT("../Bin/DataFiles/Character/");
	wstrFilePath += wstrModelName + TEXT("/");
	string strFilePath = m_pGameInstance->WstringToString(wstrFilePath);

	string strFileName = m_pGameInstance->WstringToString(wstrModelName) + "_AlphaMeshes.dat";

	string strFileFullPath = strFilePath + strFileName;

	if (!fs::exists(strFileFullPath))
	{
		cout << "_AlphaMeshes Yes!!" << endl;
	}

	strFileName = m_pGameInstance->WstringToString(wstrModelName) + "_LoopAnimations.dat";
	strFileFullPath = strFilePath + strFileName;
	if (!fs::exists(strFileFullPath))
	{
		cout << "_LoopAnimations Yes!!" << endl;
	}

	strFileName = m_pGameInstance->WstringToString(wstrModelName) + "_AnimationEvents.dat";
	strFileFullPath = strFilePath + strFileName;
	if (!fs::exists(strFileFullPath))
	{
		cout << "_AnimationEvents Yes!!" << endl;
	}

	strFileName = m_pGameInstance->WstringToString(wstrModelName) + "_Colliders.dat";
	strFileFullPath = strFilePath + strFileName;
	if (!fs::exists(strFileFullPath))
	{
		cout << "_Colliders Yes!!" << endl;
	}

	return S_OK;
}

CCharacterData* CCharacterData::Create(wstring wstrModelName)
{
	CCharacterData* pInstnace = new CCharacterData();


	if (FAILED(pInstnace->Initialize(wstrModelName)))
	{
		MSG_BOX("Failed To Created : CCharacterData");
		Safe_Release(pInstnace);
	}

	return pInstnace;
}

void CCharacterData::Free()
{
	Safe_Release(m_pGameInstance);
}
