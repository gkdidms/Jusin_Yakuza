#include "GameObject.h"

#include "GameInstance.h"
#include "Component.h"


CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice {pDevice},
	m_pContext {pContext},
	m_pGameInstance {CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject& rhs)
	: m_pDevice{rhs.m_pDevice},
	m_pContext{rhs.m_pContext},
	m_pGameInstance{rhs.m_pGameInstance},
	m_pBufferSRVTexture{rhs.m_pBufferSRVTexture},
	m_pBufferUAVTexture{rhs.m_pBufferUAVTexture},
	m_pTextureOptionBuffer{rhs.m_pTextureOptionBuffer}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
		m_iObjectIndex = pDesc->iObjectIndex;
	};

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;

	m_Components.emplace(L"Com_Transform", m_pTransformCom);
	Safe_AddRef(m_pTransformCom);

	m_Casecade = { 0.f, 6.f, 18.f, 100.f };

	m_iCurrentLevel = m_pGameInstance->Get_CurrentLevel();



	return S_OK;
}

void CGameObject::Priority_Tick(const _float& fTimeDelta)
{
	
}

void CGameObject::Tick(const _float& fTimeDelta)
{
}

void CGameObject::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

CComponent* CGameObject::Get_Component(wstring strComponentTag)
{
	CComponent* pComponent = Find_Component(strComponentTag);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring strComponentPrototypeTag, const wstring strComponentTag, class CComponent** pComponent, void* pArg)
{
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	*pComponent = m_pGameInstance->Add_Component_Clone(iLevelIndex, strComponentPrototypeTag, pArg);
	if (nullptr == *pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, *pComponent);

	Safe_AddRef(*pComponent);

	return S_OK;
}

void CGameObject::Bind_TextureUAV(_uint iType)
{
	m_pContext->CSSetUnorderedAccessViews(iType + 1, 1, &m_pBufferUAVTexture[iType], nullptr);
}

HRESULT CGameObject::Bind_TextureSRV(CShader* m_pShader, const _char* pName, _uint iType)
{
	return m_pShader->Bind_SRV(pName, m_pBufferSRVTexture[iType]);
}

CComponent* CGameObject::Find_Component(const wstring StrComponentTag)
{
	auto Pair = m_Components.find(StrComponentTag);

	if (m_Components.end() == Pair)
		return nullptr;

	return Pair->second;
}

HRESULT CGameObject::Ready_Buffer()
{
	for (size_t i = 0; i < TEXTURE_TYPE_END; ++i)
	{
		// UAV와 SRV를 지원하는 텍스처 생성
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = 1280;
		textureDesc.Height = 720;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // 예: 16비트 플로트 포맷
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ID3D11Texture2D* pTexture = nullptr;
		if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture)))
			return E_FAIL;

		// SRV 생성
		ID3D11ShaderResourceView* pSRV = nullptr;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, &pSRV)))
			return E_FAIL;

		m_pBufferSRVTexture.emplace_back(pSRV);

		// UAV 생성
		ID3D11UnorderedAccessView* pUAV = nullptr;
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = textureDesc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pDevice->CreateUnorderedAccessView(pTexture, &uavDesc, &pUAV)))
			return E_FAIL;

		m_pBufferUAVTexture.emplace_back(pUAV);

		Safe_Release(pTexture);
	}

	D3D11_BUFFER_DESC BufferDesc{};
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = sizeof(TEXTURE_OPTION);
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = sizeof(TEXTURE_OPTION);

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pTextureOptionBuffer)))
		return E_FAIL;

	return S_OK;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pTransformCom);

	for (auto& pSRV : m_pBufferSRVTexture)
		Safe_Release(pSRV);

	for (auto& pUAV : m_pBufferUAVTexture)
		Safe_Release(pUAV);

	Safe_Release(m_pTextureOptionBuffer);
}
