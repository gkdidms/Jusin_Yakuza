#include "UI_Effect.h"
#include "GameInstance.h"
CUI_Effect::CUI_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CUI_Effect::CUI_Effect(const CUI_Effect& rhs)
	: CUI_Texture{ rhs }
{
}

HRESULT CUI_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Effect::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UI_EFFECT_DESC* pDesc = static_cast<UI_EFFECT_DESC*>(pArg);
	m_vLifeTime = pDesc->vLifeTime;
	m_fSpeed = pDesc->fSpeed;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Effect::Priority_Tick(const _float& fTimeDelta)
{
}

void CUI_Effect::Tick(const _float& fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_vLifeTime.x += m_fSpeed * fTimeDelta;
}

void CUI_Effect::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	//종료시간 넘으면 죽음
#ifdef _TOOL
	if (m_vLifeTime.x >= m_vLifeTime.z)
		m_vLifeTime.x = 0.f;
#else
	if (m_vLifeTime.x >= m_vLifeTime.z)
		m_isDead = true;
#endif // _TOOL
}

HRESULT CUI_Effect::Render()
{
	if (FAILED(Bind_ResourceData()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iShaderPass);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Effect::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Effect::Bind_ResourceData()
{
	if (FAILED(m_pTransformCom->Bind_ShaderMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLifeTime", &m_vLifeTime, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))	
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Effect::Save_binary(const string strDirectory)
{
	string Directory = strDirectory;

	Directory = Directory + m_strName + ".dat";
	ofstream out(Directory, ios::binary);

	out.write((char*)&m_iTypeIndex, sizeof(_uint));

	_int strTexturelength = m_strName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strName.c_str(), strTexturelength);


	out.write((char*)&m_isParent, sizeof(_bool));

	strTexturelength = m_strParentName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strParentName.c_str(), strTexturelength);

	string FilePath = m_pGameInstance->WstringToString(m_strTextureFilePath);
	strTexturelength = FilePath.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FilePath.c_str(), strTexturelength);

	string FileName = m_pGameInstance->WstringToString(m_strTextureName);
	strTexturelength = FileName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FileName.c_str(), strTexturelength);

	out.write((char*)&m_fStartUV, sizeof(_float2));

	out.write((char*)&m_fEndUV, sizeof(_float2));

	out.write((char*)&m_vColor, sizeof(_float4));

	out.write((char*)&m_isColor, sizeof(_bool));

	out.write((char*)&m_iShaderPass, sizeof(_uint));

	_float4x4 WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();

	out.write((char*)&WorldMatrix, sizeof(_float4x4));


	out.write((char*)&m_isAnim, sizeof(_bool));

	_float2 AnimTime = { 0.f , m_fAnimTime.y };
	out.write((char*)&AnimTime, sizeof(_float2));

	out.write((char*)&m_vStartPos, sizeof(_float3));

	out.write((char*)&m_fControlAlpha, sizeof(_float2));

	out.write((char*)&m_isReverse, sizeof(_bool));

	//개별저장
	_float3 LifeTime{};
	LifeTime.x = 0.f;
	LifeTime.y = m_vLifeTime.y;
	LifeTime.z = m_vLifeTime.z;
	out.write((char*)&LifeTime, sizeof(_float3));

	out.write((char*)&m_fSpeed, sizeof(_float));

	out.close();

	return S_OK;
}

HRESULT CUI_Effect::Save_Groupbinary(ofstream& out)
{
	out.write((char*)&m_iTypeIndex, sizeof(_uint));

	_int strTexturelength = m_strName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strName.c_str(), strTexturelength);


	out.write((char*)&m_isParent, sizeof(_bool));

	strTexturelength = m_strParentName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(m_strParentName.c_str(), strTexturelength);

	string FilePath = m_pGameInstance->WstringToString(m_strTextureFilePath);
	strTexturelength = FilePath.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FilePath.c_str(), strTexturelength);

	string FileName = m_pGameInstance->WstringToString(m_strTextureName);
	strTexturelength = FileName.length();
	out.write((char*)&strTexturelength, sizeof(_int));
	out.write(FileName.c_str(), strTexturelength);

	out.write((char*)&m_fStartUV, sizeof(_float2));

	out.write((char*)&m_fEndUV, sizeof(_float2));

	out.write((char*)&m_vColor, sizeof(_float4));

	out.write((char*)&m_isColor, sizeof(_bool));

	out.write((char*)&m_iShaderPass, sizeof(_uint));

	_float4x4 WorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();

	out.write((char*)&WorldMatrix, sizeof(_float4x4));


	out.write((char*)&m_isAnim, sizeof(_bool));

	_float2 AnimTime = { 0.f , m_fAnimTime.y };
	out.write((char*)&AnimTime, sizeof(_float2));

	out.write((char*)&m_vStartPos, sizeof(_float3));

	out.write((char*)&m_fControlAlpha, sizeof(_float2));

	out.write((char*)&m_isReverse, sizeof(_bool));
	//개별저장
	_float3 LifeTime{};
	LifeTime.x = 0.f;
	LifeTime.y = m_vLifeTime.y;
	LifeTime.z = m_vLifeTime.z;
	out.write((char*)&LifeTime, sizeof(_float3));

	out.write((char*)&m_fSpeed, sizeof(_float));

	return S_OK;
}

CUI_Effect* CUI_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Effect* pInstance = new CUI_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject* CUI_Effect::Clone(void* pArg)
{
	CUI_Effect* pInstance = new CUI_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CUI_Effect::Free()
{
	__super::Free();
}
