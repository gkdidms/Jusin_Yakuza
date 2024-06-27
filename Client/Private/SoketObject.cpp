#include "SoketObject.h"

CSoketObject::CSoketObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject{ pDevice, pContext }
{
}

CSoketObject::CSoketObject(const CSoketObject& rhs)
	:CPartObject{ rhs }
{
}

HRESULT CSoketObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSoketObject::Initialize(void* pArg)
{
	SOKETOBJECT_DESC* pDesc = static_cast<SOKETOBJECT_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSoketObject::Priority_Tick(const _float& fTimeDelta)
{
}

void CSoketObject::Tick(const _float& fTimeDelta)
{
}

void CSoketObject::Late_Tick(const _float& fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	//랜더러그룹에 넣어주는거는 각자 알아서 직접해야한다. (모델마다 필요한 그룹이 다를 수 있기때문)
}

HRESULT CSoketObject::Render()
{
	return S_OK;
}

void CSoketObject::Free()
{
	__super::Free();
}
