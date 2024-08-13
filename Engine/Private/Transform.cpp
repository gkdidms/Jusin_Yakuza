#include "Transform.h"

#include "GameInstance.h"

#include "GameObject.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent{ rhs },
	m_WorldMatrix{ rhs.m_WorldMatrix },
	m_fSpeedPerSec{ rhs.m_fSpeedPerSec },
	m_fRotationPerSec{ rhs.m_fRotationPerSec }
{
}

_vector CTransform::Get_State(STATE eState)
{
	return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix matWorld = XMLoadFloat4x4(&m_WorldMatrix);

	_bool isNan = XMVector4IsNaN(vState);
	if (isNan)
		int a = 0;

	matWorld.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, matWorld);
}

void CTransform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE_RIGHT, Get_State(STATE_RIGHT) * fScaleX);
	Set_State(STATE_UP, Get_State(STATE_UP) * fScaleY);
	Set_State(STATE_LOOK, Get_State(STATE_LOOK) * fScaleZ);
}

void CTransform::Set_Scale(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fScaleZ);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		TRANSFORM_DESC* pTransformDesc = (TRANSFORM_DESC*)pArg;

		m_fSpeedPerSec = pTransformDesc->fSpeedPecSec;
		m_fRotationPerSec = pTransformDesc->fRotatePecSec;
	}

	return S_OK;
}

HRESULT CTransform::Bind_ShaderMatrix(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

HRESULT CTransform::Bind_ShaderOldMatrix(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_OldWorldMatrix);
}

void CTransform::Go_Straight(const _float& fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3NormalizeEst(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (isnan(vPosition.m128_f32[0]))	return;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Straight_CustumSpeed(const _float& fSpeed, const _float& fTimeDelta, CNavigation* pNavi)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * fSpeed * fTimeDelta;

	if (nullptr != pNavi)
	{
		if (!pNavi->isMove(vPosition))
		{
			// �����̵�����
			_vector vNormal = XMVector3Normalize(pNavi->Get_SlidingNormal());

			_float fDot = XMVectorGetX(XMVector3Dot(vLook, vNormal));
			vNormal = vNormal * fDot * -1.f;
			_vector vSliding = XMVector3Normalize(vLook + vNormal);

			// ���� �����̵� ���Ϳ� ���� �븻 ���� ������ ������ ���
			float fDotProduct = XMVectorGetX(XMVector3Dot(vSliding, vNormal));
			float fAngle = acosf(fDotProduct / (XMVectorGetX(XMVector3Length(vSliding)) * XMVectorGetX(XMVector3Length(vNormal))));
			fAngle = XMConvertToDegrees(fAngle);

			_vector vPos = Get_State(CTransform::STATE_POSITION);

			if (fAngle <= 2.0f || fAngle >= 88.0f)
			{
				const float rotationStep = XMConvertToRadians(30.0f); // 3��
				const int numSteps = static_cast<int>(360.0f / 30.0f); // 360�� / 3�� = 120ȸ��

				bool validMovementFound = false;

				for (int i = 0; i < numSteps; ++i)
				{
					// ȸ�� ��� ����
					_matrix rotationMatrix = XMMatrixRotationY(rotationStep * i);
					_vector vNewSliding = XMVector3TransformNormal(vSliding, rotationMatrix);

					vPos = Get_State(CTransform::STATE_POSITION);
					vPos += vNewSliding * fTimeDelta * fSpeed;

					if (true == pNavi->isMove(vPos))
					{
						vSliding = vNewSliding;
						Set_State(CTransform::STATE_POSITION, vPos);
						Set_State(CTransform::STATE_LOOK, vSliding);
						return;
					}

				}
				return;
			}
			else
			{
				vPos += vSliding * fTimeDelta * fSpeed;
				if (true == pNavi->isMove(vPos))
				{
					Set_State(CTransform::STATE_POSITION, vPos);
					Set_State(CTransform::STATE_LOOK, vSliding);
					return;
				}
			}


			return;
		}
	}

	if (isnan(vPosition.m128_f32[0]))	return;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Move_Custum(const _float4& vDir, const _float& fSpeed, const _float& fTimeDelta, CNavigation* pNavi)
{
	_vector vPosition = Get_State(STATE_POSITION);
	vPosition += XMLoadFloat4(&vDir) * fSpeed * fTimeDelta;

	if (nullptr != pNavi)
	{
		if (!pNavi->isMove(vPosition))
		{
			// �����̵�����
			_vector vNormal = XMVector3Normalize(pNavi->Get_SlidingNormal());

			XMVECTOR	vDirection = XMLoadFloat4(&vDir);
			_float fDot = XMVectorGetX(XMVector3Dot(vDirection, vNormal));
			vNormal = vNormal * fDot * -1.f;
			_vector vSliding = XMVector3Normalize(vDirection + vNormal);

			// ���� �����̵� ���Ϳ� ���� �븻 ���� ������ ������ ���
			float fDotProduct = XMVectorGetX(XMVector3Dot(vSliding, vNormal));
			float fAngle = acosf(fDotProduct / (XMVectorGetX(XMVector3Length(vSliding)) * XMVectorGetX(XMVector3Length(vNormal))));
			fAngle = XMConvertToDegrees(fAngle);

			_vector vPos = Get_State(CTransform::STATE_POSITION);

			if (fAngle <= 2.0f || fAngle >= 88.0f)
			{
				const float rotationStep = XMConvertToRadians(30.0f); // 3��
				const int numSteps = static_cast<int>(360.0f / 30.0f); // 360�� / 3�� = 120ȸ��

				bool validMovementFound = false;

				for (int i = 0; i < numSteps; ++i)
				{
					// ȸ�� ��� ����
					_matrix rotationMatrix = XMMatrixRotationY(rotationStep * i);
					_vector vNewSliding = XMVector3TransformNormal(vSliding, rotationMatrix);

					vPos = Get_State(CTransform::STATE_POSITION);
					vPos += vNewSliding * fTimeDelta * fSpeed;

					if (true == pNavi->isMove(vPos))
					{
						vSliding = vNewSliding;
						Set_State(CTransform::STATE_POSITION, vPos);
						return;
					}

				}
				return;
			}
			else
			{
				vPos += vSliding * fTimeDelta * fSpeed;
				if (true == pNavi->isMove(vPos))
				{
					Set_State(CTransform::STATE_POSITION, vPos);
					return;
				}
			}


			return;
		}

	}

	if (isnan(vPosition.m128_f32[0]))	return;

	Set_State(STATE_POSITION, vPosition);
}


void CTransform::Go_Backward(const _float& fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (isnan(vPosition.m128_f32[0]))	return;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(const _float& fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (isnan(vPosition.m128_f32[0]))	return;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(const _float& fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (isnan(vPosition.m128_f32[0]))	return;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(const _float& fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (isnan(vPosition.m128_f32[0]))	return;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Down(const _float& fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (isnan(vPosition.m128_f32[0]))	return;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::LookAt(_fvector vTargetPosition, _bool isDir)
{
	_float3 m_vScale = Get_Scaled();

	_vector vLook = isDir ? XMVector3Normalize(vTargetPosition) : vTargetPosition - Get_State(STATE_POSITION);
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	if (XMVector4IsNaN(XMVector4Normalize(vRight)) || XMVector4IsNaN(XMVector4Normalize(vUp)) || XMVector4IsNaN(XMVector4Normalize(vLook)))
	{
		return;
	}

	if (IsZeroVector(vRight) || IsZeroVector(vUp) || IsZeroVector(vLook))
	{
		return;
	}

	Set_State(STATE_RIGHT, XMVector4Normalize(vRight) * m_vScale.x);
	Set_State(STATE_UP, XMVector4Normalize(vUp) * m_vScale.y);
	Set_State(STATE_LOOK, XMVector4Normalize(vLook) * m_vScale.z);
}


/*
<���� �߰�>
isDir�� true�� ��� 
1��° ������ vTargetPosition�� ���⺤�ͷ� ���´�. 
*/
void CTransform::LookAt_For_LandObject(_fvector vTargetPosition, _bool isDir)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = isDir ? XMVector3Normalize(vTargetPosition) : XMVector3Normalize(vTargetPosition - vPosition);

	// Y�� ���� ����
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	// Right ���� ���
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	// Look ���� ����
	vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	if (XMVector3IsNaN(XMVector3Normalize(vRight)) || XMVector3IsNaN(XMVector3Normalize(vUp)) || XMVector3IsNaN(XMVector3Normalize(vLook)))
	{
		return;
	}

	if (IsZeroVector(vRight) || IsZeroVector(vUp) || IsZeroVector(vLook))
	{
		return;
	}


	Set_State(STATE_RIGHT, vRight * Get_Scaled().x);
	Set_State(STATE_LOOK, vLook * Get_Scaled().z);
}

void CTransform::LookForCamera(_fvector vCamLook, _float fRadian)
{
	_float3 m_vScale = Get_Scaled();

	_vector vChangeCamLook = XMVectorSetY(vCamLook, 0.f);
	_vector vLook = XMVector4Normalize(vChangeCamLook);
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, XMVector4Normalize(vRight) * m_vScale.x);
	Set_State(STATE_UP, XMVector4Normalize(vUp) * m_vScale.y);
	Set_State(STATE_LOOK, XMVector4Normalize(vLook) * m_vScale.z);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	if (isnan(vRight.m128_f32[0]) || isnan(vRight.m128_f32[0]) || isnan(vRight.m128_f32[0]))
	{
		return;
	}

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);
	 

	if (isnan(vRight.m128_f32[0]) || isnan(vRight.m128_f32[0]) || isnan(vRight.m128_f32[0]))
	{
		return;
	}


	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Change_Rotation(_fvector vAxis, _float fRadian)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);


	if (isnan(vRight.m128_f32[0]) || isnan(vRight.m128_f32[0]) || isnan(vRight.m128_f32[0]))
	{
		return;
	}


	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

_vector CTransform::Get_Rotation(_fvector vAxis, _float fRadian)
{
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	return XMVector3Normalize(vLook);
}

void CTransform::Change_Rotation_Quaternion(const _float4& vQuaternion)
{
	// �� ���͵��� �����´ٰ� ����
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vUp = Get_State(STATE_UP);
	_vector vLook = Get_State(STATE_LOOK);

	// ���ʹϾ��� XMMATRIX�� ��ȯ
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&vQuaternion));

	// �� ���͵��� ȸ����Ų��
	vRight = XMVector3TransformNormal(vRight, rotationMatrix);
	vUp = XMVector3TransformNormal(vUp, rotationMatrix);
	vLook = XMVector3TransformNormal(vLook, rotationMatrix);


	if (isnan(vRight.m128_f32[0]) || isnan(vRight.m128_f32[0]) || isnan(vRight.m128_f32[0]))
	{
		return;
	}


	// ����� �� ���͵��� �����Ѵ�
	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTransform");
		Safe_Release(pInstance);
	}
		
	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
