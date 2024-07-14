#include "CharacterData.h"  
#include "GameInstance.h"
#include "SystemManager.h"

#include "Effect.h"
#include "Model.h"
#include "LandObject.h"

CCharacterData::CCharacterData()
	:m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCharacterData::Initialize(CLandObject* pCharacter)
{
	m_pCharacter = pCharacter;

	wstring wstrFilePath = TEXT("../Bin/DataFiles/Character/");
	wstrFilePath += m_pCharacter->Get_ModelName() + TEXT("/");

	// 알파 적용할 메시 로드
	string strFilePath = m_pGameInstance->WstringToString(wstrFilePath);
	string strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_AlphaMeshes.dat";
	string strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_AlphaMeshes(strFileFullPath)))
		return E_FAIL;

	// 루프애니메이션 정보 로드
	strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_LoopAnimations.dat";
	strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_LoopAnimations(strFileFullPath)))
		return E_FAIL;
	
	// 애니메이션 이벤트 정보 로드
	strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_AnimationEvents.dat";
	strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_AnimationEvents(strFileFullPath)))
		return E_FAIL;

	// 콜라이더 정보 로드
	strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_Colliders.dat";
	strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_Colliders(strFileFullPath)))
		return E_FAIL;

	// 이펙트 정보는 레이어와 오브젝트가 전부 생성된 뒤에 뼈 정보를 꺼내와서 넘겨줘야한다.
	strFileName = m_pGameInstance->WstringToString(m_pCharacter->Get_ModelName()) + "_EffectState.dat";
	strFileFullPath = strFilePath + strFileName;
	if (FAILED(Load_EffectState(strFileFullPath)))
		return E_FAIL;
	
	return S_OK;
}

void CCharacterData::Set_CurrentAnimation(string strAnimName)
{
	m_CurrentEvents.clear();

	auto lower_bound_iter = m_AnimationEvents.lower_bound(strAnimName);
	auto upper_bound_iter = m_AnimationEvents.upper_bound(strAnimName);

	if (lower_bound_iter == upper_bound_iter && lower_bound_iter != m_AnimationEvents.end())
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

HRESULT CCharacterData::Load_AlphaMeshes(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
		cout << "_AlphaMeshes Yes!!" << endl;

		ifstream in(strFilePath, ios::binary);

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
}

HRESULT CCharacterData::Load_LoopAnimations(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
		cout << "_LoopAnimations Yes!!" << endl;

		ifstream in(strFilePath, ios::binary);

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
}

HRESULT CCharacterData::Load_AnimationEvents(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
		cout << "_AnimationEvents Yes!!" << endl;

		ifstream in(strFilePath, ios::binary);

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
}

HRESULT CCharacterData::Load_Colliders(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
		cout << "_Colliders Yes!!" << endl;

		ifstream in(strFilePath, ios::binary);

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
			if (2 == ColliderDesc.iType)
				in.read((char*)&ColliderDesc.vValue, sizeof(_float));
			else
				in.read((char*)&ColliderDesc.vValue, sizeof(_float3));

			in.read((char*)&ColliderDesc.iActionType, sizeof(_int));
			in.read((char*)&ColliderDesc.iPartType, sizeof(_int));
			in.read((char*)&ColliderDesc.isAlways, sizeof(_bool));

			m_Colliders.emplace(iBoneIndex, ColliderDesc);
		}

		in.close();
	}
}

HRESULT CCharacterData::Load_EffectState(string strFilePath)
{
	if (fs::exists(strFilePath))
	{
		cout << "_EffectState Yes!!" << endl;

		ifstream in(strFilePath, ios::binary);

		if (!in.is_open()) {
			MSG_BOX("EffectState 개방 실패");
			return E_FAIL;
		}

		_uint iNumEffectState = { 0 };
		in >> iNumEffectState;

		for (size_t i = 0; i < iNumEffectState; i++)
		{
			string strBoneName = "";
			in >> strBoneName;

			string strEffectName = "";
			in >> strEffectName;

			Create_Effect(strBoneName, strEffectName);

			m_Effects.emplace(strBoneName, m_pGameInstance->StringToWstring(strEffectName));
		}

		in.close();

	}
}
		
void CCharacterData::Create_Effect(string& strBoneName, string& strEffectName)
{
	const _float4x4* pBoneMatrix = { nullptr };

	CModel* pModel = reinterpret_cast<CModel*>(m_pCharacter->Get_Component(TEXT("Com_Model")));
	pBoneMatrix = pModel->Get_BoneCombinedTransformationMatrix(strBoneName.c_str());

	CEffect::EFFECT_DESC Desc{};
	Desc.pWorldMatrix = pBoneMatrix;
	m_pGameInstance->Add_GameObject(LEVEL_TEST, m_pGameInstance->StringToWstring(strEffectName), TEXT("Layer_Effect"), &Desc);
}


CCharacterData* CCharacterData::Create(CLandObject* pCharacter)
{
	CCharacterData* pInstnace = new CCharacterData();

	if (FAILED(pInstnace->Initialize(pCharacter)))
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
