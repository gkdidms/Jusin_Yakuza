#include "Group.h"

#include "GameInstance.h"

#ifdef _TOOL
#include "Object_Manager.h"
#endif // _TOOL

#include "Image_Texture.h"
#include "Text.h"
#include "Btn.h"
#include "UI_Effect.h"	

CGroup::CGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Object{ pDevice, pContext}
{
}

CGroup::CGroup(const CGroup& rhs)
	: CUI_Object{ rhs }
{
	for (auto& iter : rhs.m_PartObjects)
	{
		m_PartObjects.push_back(iter);
	}

}

void CGroup::Remove_PartObject(_uint iIndex)
{
	auto iter = m_PartObjects.begin();

	Safe_Release(m_PartObjects[iIndex]);

	m_PartObjects.erase(iter + iIndex);

}

HRESULT CGroup::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGroup::Initialize_Prototype(ifstream& in)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Load_Groupbinary(in)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGroup::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CGroup::Priority_Tick(const _float& fTimeDelta)
{
	for (auto& pObject : m_PartObjects)
		pObject->Priority_Tick(fTimeDelta);
}

void CGroup::Tick(const _float& fTimeDelta)
{
	for (auto& pObject : m_PartObjects)
		pObject->Tick(fTimeDelta);
}

void CGroup::Late_Tick(const _float& fTimeDelta)
{
	for (auto& pObject : m_PartObjects)
		pObject->Late_Tick(fTimeDelta);
}

HRESULT CGroup::Render()
{
//	for (auto& pObject : m_PartObjects)
	//	pObject->Render();

	return S_OK;
}


HRESULT CGroup::Save_binary(const string strDirectory)
{

	return S_OK;
}

HRESULT CGroup::Save_Groupbinary(ofstream& out)
{
	_int size = m_PartObjects.size();
	out.write((char*)&size, sizeof(_int));

	_float4x4 GroupWorld = *m_pTransformCom->Get_WorldFloat4x4();
	out.write((char*)&GroupWorld, sizeof(_float4x4));

	for (auto& pObject : m_PartObjects)
	{
		pObject->Save_Groupbinary(out);
	}

	return S_OK;
}

HRESULT CGroup::Load_Groupbinary(ifstream& in)
{
	_int strTexturelength;
	char charBox[MAX_PATH] = {};
	in.read((char*)&strTexturelength, sizeof(_int));
	in.read((char*)&charBox, strTexturelength);
	m_strName = charBox;

	_int size;
	in.read((char*)&size, sizeof(_int));
	_float4x4 GroupWorld = {};
	in.read((char*)&GroupWorld, sizeof(_float4x4));
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&GroupWorld));

	for (_int i = 0; i < size; i++)
	{
		_uint Type;
		in.read((char*)&Type, sizeof(_uint));

		switch (Type)
		{
		case 0:
		{
			CImage_Texture::tUITextureDesc pDesc{};
			pDesc.iTypeIndex = Type;

			_int strTexturelength;
			char charBox[MAX_PATH] = {};
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strName = charBox;

			in.read((char*)&pDesc.isParent, sizeof(_bool));

			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strParentName = charBox;

			string path;
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFilePath = m_pGameInstance->StringToWstring(path);

			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFileName = m_pGameInstance->StringToWstring(path);

			in.read((char*)&pDesc.fStartUV, sizeof(_float2));
			in.read((char*)&pDesc.fEndUV, sizeof(_float2));
			in.read((char*)&pDesc.vColor, sizeof(_float4));
			in.read((char*)&pDesc.isColor, sizeof(_bool));
			in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
			in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

			in.read((char*)&pDesc.bAnim, sizeof(_bool));
			in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
			in.read((char*)&pDesc.vStartPos, sizeof(_float3));
			in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
			in.read((char*)&pDesc.isReverse, sizeof(_bool));

			pDesc.isLoad = true;
			pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();

			CUI_Texture* pImage = dynamic_cast<CUI_Texture*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Image_Texture"), &pDesc));
			if (nullptr == pImage)
				return E_FAIL;

			m_PartObjects.emplace_back(pImage);
		}
		break;

		case 1:
		{
			CBtn::BTN_DESC pDesc{};
			pDesc.iTypeIndex = Type;

			_int strTexturelength;
			char charBox[MAX_PATH] = {};
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strName = charBox;

			in.read((char*)&pDesc.isParent, sizeof(_bool));

			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strParentName = charBox;

			string path;
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFilePath = m_pGameInstance->StringToWstring(path);

			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFileName = m_pGameInstance->StringToWstring(path);

			in.read((char*)&pDesc.fStartUV, sizeof(_float2));
			in.read((char*)&pDesc.fEndUV, sizeof(_float2));
			in.read((char*)&pDesc.vColor, sizeof(_float4));
			in.read((char*)&pDesc.isColor, sizeof(_bool));
			in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
			in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

			in.read((char*)&pDesc.bAnim, sizeof(_bool));
			in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
			in.read((char*)&pDesc.vStartPos, sizeof(_float3));
			in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
			in.read((char*)&pDesc.isReverse, sizeof(_bool));


			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strClickFilePath = m_pGameInstance->StringToWstring(path);

			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strClickFileName = m_pGameInstance->StringToWstring(path);

			in.read((char*)&pDesc.ClickStartUV, sizeof(_float2));
			in.read((char*)&pDesc.ClickEndUV, sizeof(_float2));

			pDesc.isLoad = true;

			pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
			CBtn* pBtn = dynamic_cast<CBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Btn"), &pDesc));
			if (nullptr == pBtn)
				return E_FAIL;

			m_PartObjects.emplace_back(pBtn);
		}
		break;

		case 2:
		{
			CText::TEXT_DESC pDesc{};
			pDesc.iTypeIndex = Type;

			_int strTexturelength;
			char charBox[MAX_PATH] = {};
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strName = charBox;

			in.read((char*)&pDesc.isParent, sizeof(_bool));

			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strParentName = charBox;

			string path;

			in.read((char*)&pDesc.vColor, sizeof(_float4));
			in.read((char*)&pDesc.isColor, sizeof(_bool));
			in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
			in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

			in.read((char*)&pDesc.bAnim, sizeof(_bool));
			in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
			in.read((char*)&pDesc.vStartPos, sizeof(_float3));
			in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
			in.read((char*)&pDesc.isReverse, sizeof(_bool));


			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strText = m_pGameInstance->StringToWstring(path);

			pDesc.isLoad = true;
			pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
			CText* pText = dynamic_cast<CText*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Text"), &pDesc));
			if (nullptr == pText)
				return E_FAIL;

			m_PartObjects.emplace_back(pText);
		}
		break;

		case 4:
		{
			CUI_Effect::UI_EFFECT_DESC pDesc{};
			pDesc.iTypeIndex = Type;

			_int strTexturelength;
			char charBox[MAX_PATH] = {};
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strName = charBox;

			in.read((char*)&pDesc.isParent, sizeof(_bool));

			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			pDesc.strParentName = charBox;


			string path;
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFilePath = m_pGameInstance->StringToWstring(path);

			ZeroMemory(charBox, MAX_PATH);
			in.read((char*)&strTexturelength, sizeof(_int));
			in.read((char*)&charBox, strTexturelength);
			path = charBox;
			pDesc.strTextureFileName = m_pGameInstance->StringToWstring(path);

			in.read((char*)&pDesc.fStartUV, sizeof(_float2));
			in.read((char*)&pDesc.fEndUV, sizeof(_float2));
			in.read((char*)&pDesc.vColor, sizeof(_float4));
			in.read((char*)&pDesc.isColor, sizeof(_bool));
			in.read((char*)&pDesc.iShaderPass, sizeof(_uint));
			in.read((char*)&pDesc.WorldMatrix, sizeof(_float4x4));

			in.read((char*)&pDesc.bAnim, sizeof(_bool));
			in.read((char*)&pDesc.fAnimTime, sizeof(_float2));
			in.read((char*)&pDesc.vStartPos, sizeof(_float3));
			in.read((char*)&pDesc.fControlAlpha, sizeof(_float2));
			in.read((char*)&pDesc.isReverse, sizeof(_bool));


			in.read((char*)&pDesc.vLifeTime, sizeof(_float3));
			in.read((char*)&pDesc.fSpeed, sizeof(_float));

			pDesc.isLoad = true;
			pDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
			CUI_Effect* pUIEffect = dynamic_cast<CUI_Effect*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIEffect"), &pDesc));
			if (nullptr == pUIEffect)
				return E_FAIL;

			m_PartObjects.emplace_back(pUIEffect);
		}
		break;

		default:
			break;
		}

	}

	return S_OK;
}

HRESULT CGroup::Show_UI()
{
	for (auto& UIObject : m_PartObjects)
	{
		UIObject->Show_UI();
	}
	return S_OK;
}

HRESULT CGroup::Close_UI()
{
	for (auto& UIObject : m_PartObjects)
	{
		UIObject->Close_UI();
	}
	return S_OK;
}

_bool CGroup::Check_AnimFin()
{
	for (auto& UIObject : m_PartObjects)
	{
		if (!UIObject->Check_AnimFin())
			return false;
	}
	return true;
}



CGroup* CGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGroup* pInstance = new CGroup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGroup* CGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& in)
{
	CGroup* pInstance = new CGroup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(in)))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CGroup::Clone(void* pArg)
{
	CGroup* pInstance = new CGroup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CGroup::Free()
{
	__super::Free();

	for (auto& pObject : m_PartObjects)
		Safe_Release(pObject);
}
