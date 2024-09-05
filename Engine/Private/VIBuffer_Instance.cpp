#include "VIBuffer_Instance.h"
#include "ComputeShader.h"
#include "GameInstance.h"
CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& rhs)
	: CVIBuffer{ rhs }
{
	//m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance);
	//m_pContext->CopyResource(m_pVBInstance, rhs.m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		INSTANCE_DESC* pDesc = static_cast<INSTANCE_DESC*>(pArg);

		m_InstanceDesc = pDesc;
		
	}

	return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{

	ID3D11Buffer* pVertices[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint pStrideVertices[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint pStartVertices[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertices, pStrideVertices, pStartVertices);
	m_pContext->IASetIndexBuffer(m_pIB, m_GIFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_Primitive_Topology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_InstanceDesc->iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Compute_Render()
{
	CComputeShader::COMPUTE_DESC ComputeDesc{};	
	ComputeDesc.OutputBuffer = mOutputUAV;
	ComputeDesc.NumElement = m_InstanceDesc->iNumInstance;

	//m_pComputeShader->Render(&ComputeDesc);

	m_pContext->Flush();




	COMPUTEMATRIX* pCopyBlends = new COMPUTEMATRIX[m_InstanceDesc->iNumInstance];
	ZeroMemory(pCopyBlends, sizeof(VTXMATRIX) * m_InstanceDesc->iNumInstance);

	D3D11_BUFFER_DESC	InstanceBufferDesc = {};

	InstanceBufferDesc.ByteWidth = sizeof(COMPUTEMATRIX) * m_InstanceDesc->iNumInstance;
	InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	InstanceBufferDesc.MiscFlags = 0;
	InstanceBufferDesc.StructureByteStride = sizeof(COMPUTEMATRIX);

	ID3D11Buffer* Instance = { nullptr };


	m_InitialData.pSysMem = pCopyBlends;

	if (FAILED(m_pDevice->CreateBuffer(&InstanceBufferDesc, &m_InitialData, &Instance)))
		return E_FAIL;

	Safe_Delete_Array(pCopyBlends);	

	m_pContext->CopyResource(Instance, m_pOutBuffer);




	//�̰����� m_pBlendSortBuffer�� ���۸� m_pVBInstance ���۷� �ٲ���
	D3D11_MAPPED_SUBRESOURCE		SubResourceOutput{};
	D3D11_MAPPED_SUBRESOURCE		SubResourceRender{};

	m_pContext->Map(Instance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceOutput);
	m_pContext->Map(m_pRenderBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceRender);

	COMPUTEMATRIX* pOutputVertices = (COMPUTEMATRIX*)SubResourceOutput.pData;		
	VTXMATRIX* pRenderVertices = (VTXMATRIX*)SubResourceRender.pData;

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		//�θ�Ʈ������ ������ �ǵڿ� ���� �߰��̳� �ȵ̳� ����
		memcpy(&pRenderVertices[i], &pOutputVertices[i], sizeof(VTXMATRIX));
	}

	m_pContext->Unmap(Instance, 0);
	m_pContext->Unmap(m_pRenderBuffer, 0);	

	Safe_Release(Instance);

	ID3D11Buffer* pVertices[] = {
		m_pVB,
		m_pRenderBuffer//��ǻƮ ���̵� ��ġ�� ���� �ٲ�(������ ����, �̰� ������)
	};

	_uint pStrideVertices[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint pStartVertices[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertices, pStrideVertices, pStartVertices);
	m_pContext->IASetIndexBuffer(m_pIB, m_GIFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_Primitive_Topology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_InstanceDesc->iNumInstance, 0, 0, 0);


	return S_OK;
}

void CVIBuffer_Instance::Spread(_float fTimeDelta)
{

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		pVertices[i].vLifeTime.y += fTimeDelta;
		//x�� ����,y �� current
		_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];


		_vector			vDir = XMLoadFloat4(&pVertices[i].vDirection);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);

				XMStoreFloat4(&pVertices[i].vDirection, XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));

				if (!m_InstanceDesc->isAttach)//�׻� �ٿ��ٴҲ�?
				{
					pVertices[i].vTranslation.x += XMVectorGetX(WorlPosition);
					pVertices[i].vTranslation.y += XMVectorGetY(WorlPosition);
					pVertices[i].vTranslation.z += XMVectorGetZ(WorlPosition);
				}

				pVertices[i].vLifeTime.y = 0.f;

				pVertices[i].vRectSize.x = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);	//ũ��
				pVertices[i].vRectSize.y = m_pGameInstance->Get_Random(0.f, 360.f);//ȸ��
			}
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::RotSpread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		pVertices[i].vLifeTime.y += fTimeDelta;

		// �޽� �ӷ� �ε�
		_vector MeshSpeed = XMLoadFloat3(&m_pMeshSpeed[i]);

		// �߷� ����
		_vector Gravity = XMVectorSet(0.f, -9.81f * m_InstanceDesc->GravityScale, 0.f, 0.f);

		// �ӵ� ũ�� ���
		float speedMagnitude = XMVectorGetX(XMVector3Length(MeshSpeed));

		// �� �࿡ ���� �ӵ� ���� �ε�
		float speedX = XMVectorGetX(MeshSpeed);
		float speedY = XMVectorGetY(MeshSpeed);
		float speedZ = XMVectorGetZ(MeshSpeed);

		// �� �࿡ ���� ���� ��� (���� ���� ��� * �ӵ��� ����)
		float airResistanceX = 0.5f * m_InstanceDesc->CrossArea * speedX * speedX / m_pWeight[i];
		float airResistanceY = 0.5f * m_InstanceDesc->CrossArea * speedY * speedY / m_pWeight[i];
		float airResistanceZ = 0.5f * m_InstanceDesc->CrossArea * speedZ * speedZ / m_pWeight[i];

		// ���� ���� ����
		_vector airResistance = XMVectorSet(
			speedX > 0 ? -airResistanceX : airResistanceX,
			speedY > 0 ? -airResistanceY : airResistanceY,
			speedZ > 0 ? -airResistanceZ : airResistanceZ,
			0.f
		);

		// ���� ������ y���� �߷º��� ũ�� �ʵ��� ����
		float gravityMagnitude = fabsf(XMVectorGetY(Gravity)) * m_pWeight[i];
		float airResistanceYMagnitude = fabsf(XMVectorGetY(airResistance));

		if (airResistanceYMagnitude > gravityMagnitude) {
			float scale = gravityMagnitude / airResistanceYMagnitude;
			airResistance = XMVectorScale(airResistance, scale);
		}

		// ���ӵ� ��� (�߷� + ���� ���� �������� �ۿ�)
		_vector VerAcc = XMVectorAdd(Gravity, airResistance);

		// ���ο� �ӷ� ���
		_vector NewMeshSpeed = XMVectorAdd(MeshSpeed, XMVectorScale(VerAcc, fTimeDelta));

		// ���ο� �޽� �ӷ� ����
		XMStoreFloat3(&m_pMeshSpeed[i], NewMeshSpeed);

		// �¿� �յ� ��鸲�� ���� ���� �Լ� �߰�
		float oscillationX = 0.0f;
		float oscillationZ = 0.0f;

		// �ӵ��� �پ��� ��ü�� �������� ������ �Ǹ� �¿� �յ� ��鸲 ����
		if (XMVectorGetY(NewMeshSpeed) < 0.f) {
			oscillationX = m_pAmplitude[i].x * sinf(m_pFrequency[i].x * pVertices[i].vLifeTime.y);
			oscillationZ = m_pAmplitude[i].z * cosf(m_pFrequency[i].z * pVertices[i].vLifeTime.y);
			NewMeshSpeed = XMVectorAdd(NewMeshSpeed, XMVectorSet(oscillationX, 0.f, oscillationZ, 0.f));
		}

		// ���� �� ���� �Ÿ� ���
		_vector Distance = XMVectorScale(NewMeshSpeed, fTimeDelta);

		// ���� ��ġ�� � �Ÿ��� ������ ���ο� ��ġ ���
		_vector newTranslation = XMVectorAdd(XMLoadFloat4(&pVertices[i].vTranslation), Distance);
		XMStoreFloat4(&pVertices[i].vTranslation, newTranslation);





		//ȸ��. �غ�
		_vector vRight = XMLoadFloat4(&pVertices[i].vRight);
		_vector vUp = XMLoadFloat4(&pVertices[i].vUp);
		_vector vLook= XMLoadFloat4(&pVertices[i].vLook);

		_matrix		RotationXMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f ,0.f , 0.f), m_pOriginalAngleVelocity[i].x *fTimeDelta);
		_matrix		RotationYMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f ,1.f , 0.f), m_pOriginalAngleVelocity[i].y * fTimeDelta);
		_matrix		RotationZMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f ,0.f , 0.f), m_pOriginalAngleVelocity[i].z * fTimeDelta);

		vRight = XMVector3TransformNormal(vRight, RotationXMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationYMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationZMatrix);

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight));
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp));
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook));


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{

				_vector WorldPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

				//m_pOriginalOffsets[i] = _float3(m_InstanceDesc->vOffsetPos.x, m_InstanceDesc->vOffsetPos.y, m_InstanceDesc->vOffsetPos.z); // Loop�� ���� �������ش�.
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f);
				XMStoreFloat4(&pVertices[i].vDirection, vDir);

				_float MeshSpeedX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.x, m_InstanceDesc->vMaxSpeed.x);
				_float MeshSpeedY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.y, m_InstanceDesc->vMaxSpeed.y);
				_float MeshSpeedZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.z, m_InstanceDesc->vMaxSpeed.z);

				m_pMeshSpeed[i] = _float3(MeshSpeedX* pVertices[i].vDirection.x, MeshSpeedY* pVertices[i].vDirection.y, MeshSpeedZ* pVertices[i].vDirection.z);



				if (!m_InstanceDesc->isAttach)//�׻� �ٿ��ٴҲ�?
				{
					pVertices[i].vTranslation.x += XMVectorGetX(WorldPosition);
					pVertices[i].vTranslation.y += XMVectorGetY(WorldPosition);
					pVertices[i].vTranslation.z += XMVectorGetZ(WorldPosition);
				}

				_float StartRotX =XMConvertToRadians (m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
				_float StartRotY = XMConvertToRadians( m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
				_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

				_matrix StartRot = XMMatrixRotationX(StartRotX)* XMMatrixRotationY(StartRotY)* XMMatrixRotationZ(StartRotZ);

				XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
				XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
				XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

				_float AngleVelocityX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.x, m_InstanceDesc->HighAngleVelocity.x));
				_float AngleVelocityY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.y, m_InstanceDesc->HighAngleVelocity.y));
				_float AngleVelocityZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.z, m_InstanceDesc->HighAngleVelocity.z));

				m_pOriginalAngleVelocity[i] = _float3(AngleVelocityX, AngleVelocityY, AngleVelocityZ);



				m_pWeight[i] = m_pGameInstance->Get_Random(m_InstanceDesc->fWeight.x, m_InstanceDesc->fWeight.y);

				_float FrequencyX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.x, m_InstanceDesc->vMaxFrequency.x);
				_float FrequencyY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.y, m_InstanceDesc->vMaxFrequency.y);
				_float FrequencyZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.z, m_InstanceDesc->vMaxFrequency.z);

				m_pFrequency[i] = _float3(FrequencyX, FrequencyY, FrequencyZ);

				_float AmplitudeX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.x, m_InstanceDesc->vMaxAmplitude.x);
				_float AmplitudeY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.y, m_InstanceDesc->vMaxAmplitude.y);
				_float AmplitudeZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.z, m_InstanceDesc->vMaxAmplitude.z);

				m_pAmplitude[i] = _float3(AmplitudeX, AmplitudeY, AmplitudeZ);

			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);


}

void CVIBuffer_Instance::FallSpread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		pVertices[i].vRectSize.y = 1.f;	

		pVertices[i].vLifeTime.y += fTimeDelta;
		//x�� ����,y �� current
		_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];
		
		_vector			vDir = (XMLoadFloat4(&pVertices[i].vDirection));

		_vector Hor = XMVectorSetY(vDir, 0.f);
		//���򰡼ӵ� = -1/2 �� ���� �ӵ���(ũ��� 1 ����)
		_vector HorAcc = -0.5f * m_InstanceDesc->CrossArea * XMVectorMultiply(Hor, Hor);

		_vector HorVel = Hor * m_pSpeeds[i] + HorAcc * fTimeDelta;
		if (XMVectorGetX(vDir) > 0.f)
		{
			if (XMVectorGetX(HorVel) < 0.f)
				HorVel = XMVectorSetX(HorVel, 0.f);
		}
		else
		{
			if (XMVectorGetX(HorVel) >= 0.f)
				HorVel = XMVectorSetX(HorVel, 0.f);
		}
		if (XMVectorGetZ(vDir) > 0.f)
		{
			if (XMVectorGetZ(HorVel) < 0.f)
				HorVel = XMVectorSetZ(HorVel, 0.f);
		}
		else
		{
			if (XMVectorGetZ(HorVel) >= 0.f)
				HorVel = XMVectorSetZ(HorVel, 0.f);
		}

		//���� � �Ÿ� = �ʱ� ���� �ӵ� �� �ð� + 1 / 2 �� ���ӵ� �� �ð���
		_vector HorDistance = HorVel * fTimeDelta;

		_vector Ver = XMVectorSet(0.f, XMVectorGetY(vDir), 0.f, 0.f);

		//�������ӵ� = �߷� - ���� ���� = 9.81 - 1/2 �� ���� �ӵ���
		_vector Gravity = XMVectorSet(0.f, -9.81f, 0.f, 0.f);
		_vector VerAcc = Gravity + (-0.5f * XMVectorMultiply(Ver, Ver));

		//���� � �Ÿ� = �ʱ� ���� �ӵ� �� �ð� + 1 / 2 �� ���ӵ� �� �ð���
		_vector VerVel = Ver * m_pSpeeds[i] + VerAcc * m_InstanceDesc->GravityScale * fTimeDelta;
		_vector VerDistance = VerVel * fTimeDelta;

		//��ü � �Ÿ� = (���� � �Ÿ� + ���� � �Ÿ�) / 2
		_vector FinDistance = (HorDistance + VerDistance);

		_vector OriginPosition = XMVectorSet(pVertices[i].vTranslation.x, pVertices[i].vTranslation.y, pVertices[i].vTranslation.z, 1.f);

		_vector FinalPosition = OriginPosition + FinDistance;

		XMStoreFloat4(&pVertices[i].vTranslation, FinalPosition);


		//ȸ��. �غ�
		_vector vRight = XMLoadFloat4(&pVertices[i].vRight);
		_vector vUp = XMLoadFloat4(&pVertices[i].vUp);
		_vector vLook = XMLoadFloat4(&pVertices[i].vLook);

		_matrix		RotationXMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].x * fTimeDelta);
		_matrix		RotationYMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pOriginalAngleVelocity[i].y * fTimeDelta);
		_matrix		RotationZMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].z * fTimeDelta);

		vRight = XMVector3TransformNormal(vRight, RotationXMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationYMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationZMatrix);

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight));
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp));
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook));

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vRectSize.y = 0.f;

				_vector WorldPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				_vector			vDir = XMVector4Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));
				XMStoreFloat4(&pVertices[i].vDirection, vDir);
				pVertices[i].vLifeTime.y = 0.f;

				if (!m_InstanceDesc->isAttach)//�׻� �ٿ��ٴҲ�?
				{
					pVertices[i].vTranslation.x += XMVectorGetX(WorldPosition);
					pVertices[i].vTranslation.y += XMVectorGetY(WorldPosition);
					pVertices[i].vTranslation.z += XMVectorGetZ(WorldPosition);
				}

				_float StartRotX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
				_float StartRotY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
				_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

				_matrix StartRot = XMMatrixRotationRollPitchYaw(StartRotX, StartRotY, StartRotZ);

				pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
				pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
				pVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

				XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
				XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
				XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

				_float AngleVelocityX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.x, m_InstanceDesc->HighAngleVelocity.x));
				_float AngleVelocityY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.y, m_InstanceDesc->HighAngleVelocity.y));
				_float AngleVelocityZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.z, m_InstanceDesc->HighAngleVelocity.z));

				m_pOriginalAngleVelocity[i] = _float3(AngleVelocityX, AngleVelocityY, AngleVelocityZ);

				m_pSpeeds[i] = m_pGameInstance->Get_Random(m_InstanceDesc->vSpeed.x, m_InstanceDesc->vSpeed.y);

				pVertices[i].vRectSize.y = m_pGameInstance->Get_Random(0.f, 360.f);//ȸ��
			}
			else
			{
				pVertices[i].vRectSize.x = 0.f;
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::BloodSpread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_uint LiveNum = 0;
	if (0 != m_InstanceDesc->fDelay)
	{
		m_iCurrentTime += fTimeDelta;//�ð��߰� 
		LiveNum = m_iCurrentTime / m_InstanceDesc->fDelay;

		if (LiveNum > m_InstanceDesc->iNumInstance)
			m_iCurrentTime = 0.f;
	}

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		if (0 != m_InstanceDesc->fDelay && LiveNum <= i)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vRectSize.y = 0.f;

				pVertices[i].vLifeTime.y = 0.f;

				_vector WorldPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				_vector			vDir = XMVector4Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));
				XMStoreFloat4(&pVertices[i].vDirection, vDir);

				if (!m_InstanceDesc->isAttach)//�׻� �ٿ��ٴҲ�?
				{
					pVertices[i].vTranslation.x += XMVectorGetX(WorldPosition);
					pVertices[i].vTranslation.y += XMVectorGetY(WorldPosition);
					pVertices[i].vTranslation.z += XMVectorGetZ(WorldPosition);
				}

				_float StartRotX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
				_float StartRotY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
				_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

				_matrix StartRot = XMMatrixRotationRollPitchYaw(StartRotX, StartRotY, StartRotZ);

				pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
				pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
				pVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

				XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
				XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
				XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

				_float AngleVelocityX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.x, m_InstanceDesc->HighAngleVelocity.x));
				_float AngleVelocityY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.y, m_InstanceDesc->HighAngleVelocity.y));
				_float AngleVelocityZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.z, m_InstanceDesc->HighAngleVelocity.z));

				m_pOriginalAngleVelocity[i] = _float3(AngleVelocityX, AngleVelocityY, AngleVelocityZ);

				m_pSpeeds[i] = m_pGameInstance->Get_Random(m_InstanceDesc->vSpeed.x, m_InstanceDesc->vSpeed.y);

			}
			else
			{
				pVertices[i].vRectSize.x = 0.f;
			}
			continue;
		}

		pVertices[i].vRectSize.y = 1.f;

		pVertices[i].vLifeTime.y += fTimeDelta;
		//x�� ����,y �� current
		_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

		_vector			vDir = (XMLoadFloat4(&pVertices[i].vDirection));

		_vector Hor = XMVectorSetY(vDir, 0.f);
		//���򰡼ӵ� = -1/2 �� ���� �ӵ���(ũ��� 1 ����)
		_vector HorAcc = -0.5f * m_InstanceDesc->CrossArea * XMVectorMultiply(Hor, Hor);

		_vector HorVel = Hor * m_pSpeeds[i] + HorAcc * fTimeDelta;
		if (XMVectorGetX(vDir) > 0.f)
		{
			if (XMVectorGetX(HorVel) < 0.f)
				HorVel = XMVectorSetX(HorVel, 0.f);
		}
		else
		{
			if (XMVectorGetX(HorVel) >= 0.f)
				HorVel = XMVectorSetX(HorVel, 0.f);
		}
		if (XMVectorGetZ(vDir) > 0.f)
		{
			if (XMVectorGetZ(HorVel) < 0.f)
				HorVel = XMVectorSetZ(HorVel, 0.f);
		}
		else
		{
			if (XMVectorGetZ(HorVel) >= 0.f)
				HorVel = XMVectorSetZ(HorVel, 0.f);
		}

		//���� � �Ÿ� = �ʱ� ���� �ӵ� �� �ð� + 1 / 2 �� ���ӵ� �� �ð���
		_vector HorDistance = HorVel * fTimeDelta;

		_vector Ver = XMVectorSet(0.f, XMVectorGetY(vDir), 0.f, 0.f);

		//�������ӵ� = �߷� - ���� ���� = 9.81 - 1/2 �� ���� �ӵ���
		_vector Gravity = XMVectorSet(0.f, -9.81f, 0.f, 0.f);
		_vector VerAcc = Gravity + (-0.5f * XMVectorMultiply(Ver, Ver));

		//���� � �Ÿ� = �ʱ� ���� �ӵ� �� �ð� + 1 / 2 �� ���ӵ� �� �ð���
		_vector VerVel = Ver * m_pSpeeds[i] + VerAcc * m_InstanceDesc->GravityScale * fTimeDelta;
		_vector VerDistance = VerVel * fTimeDelta;

		//��ü � �Ÿ� = (���� � �Ÿ� + ���� � �Ÿ�) / 2
		_vector FinDistance = (HorDistance + VerDistance);

		_vector OriginPosition = XMVectorSet(pVertices[i].vTranslation.x, pVertices[i].vTranslation.y, pVertices[i].vTranslation.z, 1.f);

		_vector FinalPosition = OriginPosition + FinDistance;

		XMStoreFloat4(&pVertices[i].vTranslation, FinalPosition);


		//ȸ��. �غ�
		_vector vRight = XMLoadFloat4(&pVertices[i].vRight);
		_vector vUp = XMLoadFloat4(&pVertices[i].vUp);
		_vector vLook = XMLoadFloat4(&pVertices[i].vLook);

		_matrix		RotationXMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].x * fTimeDelta);
		_matrix		RotationYMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pOriginalAngleVelocity[i].y * fTimeDelta);
		_matrix		RotationZMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].z * fTimeDelta);

		vRight = XMVector3TransformNormal(vRight, RotationXMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationYMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationZMatrix);

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight));
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp));
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook));
		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				pVertices[i].vRectSize.y = 0.f;
			}
		}
	
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::MeshSpread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		// ���� �ð� ������Ʈ
		pVertices[i].vLifeTime.y += fTimeDelta; // ���� �ð� ������Ʈ

		// ���� ���� ����ȭ
		XMVECTOR vDir = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vDirection));
		XMVECTOR vNewDir = vDir;

		// ���� �ӵ� �ε�
		XMVECTOR vSpeed = XMLoadFloat3(&m_pMeshSpeed[i]);

		// �߷� ���
		XMVECTOR Gravity = XMVectorSet(0.f, -m_InstanceDesc->GravityScale * m_pWeight[i], 0.f, 0.f);	

		// ���� ���� ��� (�ӵ��� ������ ���)
		float airResistance = 0.5f * m_InstanceDesc->CrossArea * pow(XMVectorGetY(vSpeed), 2);
		XMVECTOR VerAcc = Gravity - XMVectorSet(0.f, airResistance, 0.f, 0.f);

		// ���ο� ���⿡ ���� ���ӵ� �߰�
		vNewDir = XMVectorAdd(vNewDir, VerAcc * fTimeDelta);

		// ���� �ӵ� �� �Ÿ� ���
		XMVECTOR VerVel = vNewDir * fTimeDelta;

		// ���ο� ���� ����
		XMStoreFloat4(&pVertices[i].vDirection, vNewDir);

		// ���� �ӵ����� Y ���� �ӵ� ����
		float currentSpeedY = XMVectorGetY(vNewDir);
		float currentSpeedXZ = sqrt(pow(XMVectorGetX(vSpeed), 2) + pow(XMVectorGetZ(vSpeed), 2));

		// XZ ���������� �̵� ��� (�ӵ��� ���)
		float xOffset = currentSpeedXZ * sin(m_pFrequency[i].x * pVertices[i].vLifeTime.y) * m_pAmplitude[i].x;
		float zOffset = currentSpeedXZ * cos(m_pFrequency[i].z * pVertices[i].vLifeTime.y) * m_pAmplitude[i].z;

		// ���� ��ġ�� � �Ÿ��� ������ ���ο� ��ġ ���
		XMVECTOR OriginPosition = XMLoadFloat4(&pVertices[i].vTranslation);
		XMVECTOR newPosition = OriginPosition + VerVel + XMVectorSet(xOffset, 0.0f, zOffset, 0.0f);
		XMStoreFloat4(&pVertices[i].vTranslation, newPosition);

		// ��鸲 ȿ�� �߰�
		float shakeX = m_pAmplitude[i].x * sin(m_pFrequency[i].x * pVertices[i].vLifeTime.y);
		float shakeY = m_pAmplitude[i].y * sin(m_pFrequency[i].y * pVertices[i].vLifeTime.y);
		float shakeZ = m_pAmplitude[i].z * sin(m_pFrequency[i].z * pVertices[i].vLifeTime.y);

		// ���ο� ��ġ�� ��鸲 �߰�
		XMVECTOR shakeOffset = XMVectorSet(shakeX, shakeY, shakeZ, 0.0f);
		newPosition = newPosition + shakeOffset;
		XMStoreFloat4(&pVertices[i].vTranslation, newPosition);

		// Y ������ ������ �� �ӵ� ����
		if (XMVectorGetY(vNewDir) < 0.f) {
			pVertices[i].vDirection.y = 0.f; // �� �̻� ������� �ʵ��� ����
		}







		//ȸ��. �غ�
		_vector vRight = XMLoadFloat4(&pVertices[i].vRight);
		_vector vUp = XMLoadFloat4(&pVertices[i].vUp);
		_vector vLook = XMLoadFloat4(&pVertices[i].vLook);

		_matrix		RotationXMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].x * fTimeDelta);
		_matrix		RotationYMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pOriginalAngleVelocity[i].y * fTimeDelta);
		_matrix		RotationZMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_pOriginalAngleVelocity[i].z * fTimeDelta);

		vRight = XMVector3TransformNormal(vRight, RotationXMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationYMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationZMatrix);

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight));
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp));
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook));


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc->isLoop)
			{
				//m_pOriginalOffsets[i] = _float3(m_InstanceDesc->vOffsetPos.x, m_InstanceDesc->vOffsetPos.y, m_InstanceDesc->vOffsetPos.z); // Loop�� ���� �������ش�.
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f);
				XMStoreFloat4(&pVertices[i].vDirection, vDir);

				_float StartRotX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
				_float StartRotY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
				_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

				_matrix StartRot = XMMatrixRotationX(StartRotX) * XMMatrixRotationY(StartRotY) * XMMatrixRotationZ(StartRotZ);

				XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
				XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
				XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

				_float AngleVelocityX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.x, m_InstanceDesc->HighAngleVelocity.x));
				_float AngleVelocityY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.y, m_InstanceDesc->HighAngleVelocity.y));
				_float AngleVelocityZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.z, m_InstanceDesc->HighAngleVelocity.z));

				m_pOriginalAngleVelocity[i] = _float3(AngleVelocityX, AngleVelocityY, AngleVelocityZ);

				_float MeshSpeedX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.x, m_InstanceDesc->vMaxSpeed.x);
				_float MeshSpeedY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.y, m_InstanceDesc->vMaxSpeed.y);
				_float MeshSpeedZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinSpeed.z, m_InstanceDesc->vMaxSpeed.z);

				m_pMeshSpeed[i] = _float3(MeshSpeedX, MeshSpeedY, MeshSpeedZ);

				m_pWeight[i] = m_pGameInstance->Get_Random(m_InstanceDesc->fWeight.x, m_InstanceDesc->fWeight.y);

				_float FrequencyX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.x, m_InstanceDesc->vMaxFrequency.x);
				_float FrequencyY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.y, m_InstanceDesc->vMaxFrequency.y);
				_float FrequencyZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinFrequency.z, m_InstanceDesc->vMaxFrequency.z);

				m_pFrequency[i] = _float3(FrequencyX, FrequencyY, FrequencyZ);

				_float AmplitudeX = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.x, m_InstanceDesc->vMaxAmplitude.x);
				_float AmplitudeY = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.y, m_InstanceDesc->vMaxAmplitude.y);
				_float AmplitudeZ = m_pGameInstance->Get_Random(m_InstanceDesc->vMinAmplitude.z, m_InstanceDesc->vMaxAmplitude.z);

				m_pAmplitude[i] = _float3(AmplitudeX, AmplitudeY, AmplitudeZ);

			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::IntervalSpread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_uint LiveNum = 0;
	_uint Next = 0;

	_bool LastFin = false;

	if (0 != m_InstanceDesc->fDelay)
	{
		m_iCurrentTime += fTimeDelta;//�ð��߰� 
		LiveNum = m_iCurrentTime / m_InstanceDesc->fDelay;	

	}

	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

	_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{

		if (pVertices[i].vLifeTime.y > pVertices[i].vLifeTime.x)
		{
			pVertices[i].vRectSize.x = 0.f;
		}

		if (i < LiveNum && pVertices[i].vLifeTime.y <= pVertices[i].vLifeTime.x)
		{
			if (false == m_pIsPlay[i])
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);

				XMStoreFloat4(&pVertices[i].vDirection, XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));

				pVertices[i].vRectSize.x = m_pOriginalSize[i];

				if (!m_InstanceDesc->isAttach)//�׻� �ٿ��ٴҲ�?
				{
					pVertices[i].vTranslation.x += XMVectorGetX(WorlPosition);
					pVertices[i].vTranslation.y += XMVectorGetY(WorlPosition);
					pVertices[i].vTranslation.z += XMVectorGetZ(WorlPosition);
				}
				m_pIsPlay[i] = true;
			}

			pVertices[i].vLifeTime.y += fTimeDelta;
			//x�� ����,y �� current

			_vector			vDir = XMLoadFloat4(&pVertices[i].vDirection);

			XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);
		}
		else
		{
			pVertices[i].vRectSize.x = 0.f;
		}

	}

	if (m_InstanceDesc->isLoop)
	{
		if (LiveNum > m_InstanceDesc->iNumInstance)
		{
			Reset();
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instance::Reset()
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;	

		//x�� ����,y �� current
		_vector WorlPosition = XMLoadFloat4x4(m_pCurrentWorldMatrix).r[3];

		pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
		_vector			vDir = XMVector4Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_pOriginalOffsets[i]), 0.f));
		XMStoreFloat4(&pVertices[i].vDirection, vDir);
		pVertices[i].vLifeTime.y = 0.f;

		if (!m_InstanceDesc->isAttach)//�׻� �ٿ��ٴҲ�?
		{
			pVertices[i].vTranslation.x += XMVectorGetX(WorlPosition);
			pVertices[i].vTranslation.y += XMVectorGetY(WorlPosition);
			pVertices[i].vTranslation.z += XMVectorGetZ(WorlPosition);
		}

		_float StartRotX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.x, m_InstanceDesc->HighStartRot.x));
		_float StartRotY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.y, m_InstanceDesc->HighStartRot.y));
		_float StartRotZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowStartRot.z, m_InstanceDesc->HighStartRot.z));

		_matrix StartRot = XMMatrixRotationX(StartRotX) * XMMatrixRotationY(StartRotY) * XMMatrixRotationZ(StartRotZ);

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(StartRot.r[0]));
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(StartRot.r[1]));
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(StartRot.r[2]));

		_float AngleVelocityX = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.x, m_InstanceDesc->HighAngleVelocity.x));
		_float AngleVelocityY = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.y, m_InstanceDesc->HighAngleVelocity.y));
		_float AngleVelocityZ = XMConvertToRadians(m_pGameInstance->Get_Random(m_InstanceDesc->LowAngleVelocity.z, m_InstanceDesc->HighAngleVelocity.z));

		m_pOriginalAngleVelocity[i] = _float3(AngleVelocityX, AngleVelocityY, AngleVelocityZ);

		m_pSpeeds[i] = m_pGameInstance->Get_Random(m_InstanceDesc->vSpeed.x, m_InstanceDesc->vSpeed.y);

		pVertices[i].vRectSize.y = m_pGameInstance->Get_Random(0.f, 360.f);//ȸ��

		if(0!=m_InstanceDesc->fDelay)
		{
			m_pIsPlay[i] = false;
			m_iCurrentTime = 0.f;
		}
	}

	m_isReset = true;
	m_pContext->Unmap(m_pVBInstance, 0);
}

_bool CVIBuffer_Instance::LifeTime_Check()
{

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

		if (pVertices[i].vLifeTime.y <pVertices[i].vLifeTime.x)
		{
			return false;
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);
	

	return true;
}

void CVIBuffer_Instance::SizeUp_Time(_float fTimeDelta)
{
	if(!m_isReset)
	{
		D3D11_MAPPED_SUBRESOURCE		SubResource{};

		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
		{
			VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

			//x�� ����,y �� current

			pVertices[i].vRectSize.x = (1.f + (pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x)) * m_pOriginalSize[i];

			//if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
			//{
			//	if (true == m_InstanceDesc->isLoop)
			//	{
			//		pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
			//		pVertices[i].vLifeTime.y = 0.f;
			//		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc->vOffsetPos), 0.f);
			//		XMStoreFloat4(&pVertices[i].vDirection, vDir);
			//		pVertices[i].vRectSize.x = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);
			//	}
			//}

		}

		m_pContext->Unmap(m_pVBInstance, 0);
	}
}

void CVIBuffer_Instance::SizeDown_Time(_float fTimeDelta)
{
	if (!m_isReset)
	{
		D3D11_MAPPED_SUBRESOURCE		SubResource{};

		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
		{
			VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

			//x�� ����,y �� current
			pVertices[i].vRectSize.x = (1.f - (pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x)) * m_pOriginalSize[i];

			//if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
			//{
			//	if (true == m_InstanceDesc->isLoop)
			//	{
			//		pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
			//		pVertices[i].vLifeTime.y = 0.f;
			//		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc->vOffsetPos), 0.f);
			//		XMStoreFloat4(&pVertices[i].vDirection, vDir);
			//		pVertices[i].vRectSize.x = m_pGameInstance->Get_Random(m_InstanceDesc->vRectSize.x, m_InstanceDesc->vRectSize.y);
			//	}
			//}

		}

		m_pContext->Unmap(m_pVBInstance, 0);
	}
}
//
//void CVIBuffer_Instance::Leaf_Fall(_float fTimeDelta)
//{
//	bool allInstancesDead = true;
//	D3D11_MAPPED_SUBRESOURCE      SubResource{};
//	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
//	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
//	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
//	{
//
//		pVertices[i].vLifeTime.y += fTimeDelta;
//
//		_float fRatio = pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x;
//		_vector vRight, vUp, vLook, vPos;
//		vRight = XMLoadFloat4(&pVertices[i].vRight);
//		vUp = XMLoadFloat4(&pVertices[i].vUp);
//		vLook = XMLoadFloat4(&pVertices[i].vLook);
//		vPos = XMLoadFloat4(&pVertices[i].vTranslation);
//
//		if (fRatio <= 0.2f)
//		{
//			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
//			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
//			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
//			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
//			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
//		}
//		else if (fRatio > 0.2f && fRatio <= 0.4f)
//		{
//			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vUp), XMConvertToRadians(m_pSpeeds[i]));
//			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
//			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
//			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
//			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
//		}
//		else if (fRatio > 0.4f && fRatio <= 0.6f)
//		{
//			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
//			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
//			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
//			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
//			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
//		}
//		else if (fRatio > 0.6f && fRatio <= 0.8f)
//		{
//			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vUp), XMConvertToRadians(m_pSpeeds[i]));
//			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
//			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
//			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
//			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
//		}
//		else
//		{
//			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
//			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
//			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
//			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
//			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
//		}
//
//		_vector vDir = XMVector4Normalize(vLook);
//		vPos += vDir * m_pSpeeds[i] * fTimeDelta;
//
//
//
//		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight));
//		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp) );
//		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook) );
//		XMStoreFloat4(&pVertices[i].vTranslation, vPos);	
//
//		pVertices[i].vTranslation.y -=0.98f * m_InstanceDesc->GravityScale* fTimeDelta;
//
//		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
//		{
//			if (true == m_InstanceDesc->isLoop)
//			{
//				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
//				pVertices[i].vLifeTime.y = 0.f;
//			}
//			else
//			{
//				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
//			}
//		}
//
//	}
//
//	m_pContext->Unmap(m_pVBInstance, 0);
//
//
//
//
//}

void CVIBuffer_Instance::Compute_Sort()
{
	//���ۿ� ���� �ص�.
	D3D11_MAPPED_SUBRESOURCE		SubResourceOrigin{};
	D3D11_MAPPED_SUBRESOURCE		SubResourceCopy{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceOrigin);	
	//m_pContext->Map(m_pOutBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceCopy);	

	VTXMATRIX* pVertices = (VTXMATRIX*)SubResourceOrigin.pData;

	COMPUTEMATRIX* pCopyBlends = new COMPUTEMATRIX[m_InstanceDesc->iNumInstance];
	ZeroMemory(pCopyBlends, sizeof(VTXMATRIX) * m_InstanceDesc->iNumInstance);

	_vector CamPos = m_pGameInstance->Get_CamPosition();

	ZeroMemory(pCopyBlends, sizeof(COMPUTEMATRIX) * m_InstanceDesc->iNumInstance);//������ �ʱ�ȭ�� ���

	for (size_t i = 0; i < m_InstanceDesc->iNumInstance; i++)
	{
		memcpy(&pCopyBlends[i], &pVertices[i], sizeof(VTXMATRIX));	

		memcpy(&pCopyBlends[i].vCamPos, &CamPos, sizeof(_float4));
	}

//	m_pContext->Unmap(m_pOutBuffer, 0);
	m_pContext->Unmap(m_pVBInstance, 0);

	m_InitialData.pSysMem = pCopyBlends;

	if (FAILED(m_pDevice->CreateBuffer(&m_OutBufferDesc, &m_InitialData, &m_pOutBuffer)))
		return ;
	//���̴����� �б�,����.
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pOutBuffer, &uavDesc, &mOutputUAV)))
		return ;


}

void CVIBuffer_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pOriginalPositions);
		Safe_Delete_Array(m_pOriginalOffsets);
		Safe_Delete_Array(m_pOriginalSize);
		Safe_Delete_Array(m_pOriginalAngleVelocity);
		Safe_Delete_Array(m_pMeshSpeed);
		Safe_Delete_Array(m_pWeight);
		Safe_Delete_Array(m_pFrequency);
		Safe_Delete_Array(m_pAmplitude);
		Safe_Delete_Array(m_pIsPlay);
	}

	Safe_Release(m_pComputeShader);
	Safe_Release(m_pRenderBuffer);
	Safe_Release(m_pOutBuffer);
	Safe_Release(mOutputUAV);
	Safe_Release(m_pVBInstance);
}
