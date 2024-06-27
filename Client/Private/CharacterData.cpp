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

	if (fs::exists(strFileFullPath))
	{
		cout << "_AlphaMeshes Yes!!" << endl;

		ifstream in(strFileFullPath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("AlphaMeshes 개방 실패");
			return E_FAIL;
		}

		_uint iNumMeshes = { 0 };
		in.read((char*)&iNumMeshes, sizeof(_uint));

		m_AlphaMeshes.reserve(iNumMeshes);

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			_uint iMeshIndex = { 0 };
			in.read((char*)&iMeshIndex, sizeof(_uint));

			m_AlphaMeshes.push_back(iMeshIndex);
		}

		in.close();

	}

	strFileName = m_pGameInstance->WstringToString(wstrModelName) + "_LoopAnimations.dat";
	strFileFullPath = strFilePath + strFileName;
	if (fs::exists(strFileFullPath))
	{
		cout << "_LoopAnimations Yes!!" << endl;

		ifstream in(strFileFullPath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("LoopAnimations 개방 실패");
			return E_FAIL;
		}

		_uint iNumAnimations = { 0 };
		in.read((char*)&iNumAnimations, sizeof(_uint));

		m_LoopAnimations.reserve(iNumAnimations);

		for (size_t i = 0; i < iNumAnimations; i++)
		{
			_uint iAnimationIndex = { 0 };
			in.read((char*)&iAnimationIndex, sizeof(_uint));

			m_LoopAnimations.push_back(iAnimationIndex);
		}

		in.close();
	}

	strFileName = m_pGameInstance->WstringToString(wstrModelName) + "_AnimationEvents.dat";
	strFileFullPath = strFilePath + strFileName;
	if (fs::exists(strFileFullPath))
	{
		cout << "_AnimationEvents Yes!!" << endl;

		ifstream in(strFileFullPath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("AnimationEvents 개방 실패");
			return E_FAIL;
		}

		_uint iNumEvents = { 0 };

		in >> iNumEvents;

		for (size_t i = 0; i < iNumEvents; i++)
		{
			string strAnimName = "";
			ANIMATION_EVENT EventDesc{};

			in >> strAnimName;
			in >> EventDesc.iType;
			in >> EventDesc.fPlayPosition;
			in >> EventDesc.strChannelName;
			in >> EventDesc.iBoneIndex;

			m_AnimationEvents.emplace(strAnimName, EventDesc);
		}

		in.close();

	}

	strFileName = m_pGameInstance->WstringToString(wstrModelName) + "_Colliders.dat";
	strFileFullPath = strFilePath + strFileName;
	if (fs::exists(strFileFullPath))
	{
		cout << "_Colliders Yes!!" << endl;

		ifstream in(strFileFullPath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("Colliders 개방 실패");
			return E_FAIL;
		}

		_uint iNumColliders = { 0 };
		in.read((char*)&iNumColliders, sizeof(_uint));

		for (size_t i = 0; i < iNumColliders; i++)
		{
			_uint iBoneIndex = { 0 };

			COLLIDER_STATE ColliderDesc{};

			in.read((char*)&iBoneIndex, sizeof(_uint));
			in.read((char*)&ColliderDesc.iType, sizeof(_uint));
			in.read((char*)&ColliderDesc.vCenter, sizeof(_float3));

			// 2는 Sphere라서 Vlaue가 Radius라 float1개만 쓴다
			if(2 == ColliderDesc.iType)
				in.read((char*)&ColliderDesc.vValue, sizeof(_float));
			else
				in.read((char*)&ColliderDesc.vValue, sizeof(_float3));


			m_Colliders.emplace(iBoneIndex, ColliderDesc);
		}

		in.close();
	}

	return S_OK;
}

void CCharacterData::Set_CurrentAnimation(string strAnimName)
{
	m_CurrentEvents.clear();

	auto lower_bound_iter = m_AnimationEvents.lower_bound(strAnimName);
	auto upper_bound_iter = m_AnimationEvents.upper_bound(strAnimName);

	if (lower_bound_iter == upper_bound_iter)
	{
		m_CurrentEvents.push_back((*lower_bound_iter).second);
	}
	else
	{
		for (; lower_bound_iter != upper_bound_iter; ++lower_bound_iter)
		{
			m_CurrentEvents.push_back((*lower_bound_iter).second);
		}
	}

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
