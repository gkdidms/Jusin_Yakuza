#include "CutSceneCamera.h"

#include "GameInstance.h"
#include "FileTotalMgr.h"
#include "SystemManager.h"

#include "Player.h"

CCutSceneCamera::CCutSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext },
	m_pSystemManager { CSystemManager::GetInstance() }
{
	Safe_AddRef(m_pSystemManager);
}

CCutSceneCamera::CCutSceneCamera(const CCutSceneCamera& rhs)
	: CCamera{ rhs },
	m_pSystemManager { rhs.m_pSystemManager }
{
	Safe_AddRef(m_pSystemManager);
}

HRESULT CCutSceneCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCutSceneCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCutSceneCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CCutSceneCamera::Tick(const _float& fTimeDelta)
{
	if (m_pSystemManager->Get_Camera() != CAMERA_CUTSCENE) return;

	ShowCursor(false);

	//Compute_ViewMatrix();

	// �÷��̾�� ���� Ÿ�̸Ӹ� ����Ѵ�
	__super::Tick(m_pGameInstance->Get_TimeDelta(TEXT("Timer_Player")));
}

void CCutSceneCamera::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CCutSceneCamera::Render()
{
	return S_OK;
}

void CCutSceneCamera::Compute_ViewMatrix()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), 0));

	CModel* pPlayerCamModelCom = static_cast<CModel*>(pPlayer->Get_Component(TEXT("Com_Model_Cam")));
	CModel* pPlayerModelCom = static_cast<CModel*>(pPlayer->Get_Component(TEXT("Com_Model")));

	// Blender���� ���� ���� ��ȯ ���
	_matrix matCameraMatrix = XMLoadFloat4x4(pPlayerCamModelCom->Get_BoneCombinedTransformationMatrix("Camera"));
	_matrix matPlayerWorld = pPlayer->Get_TransformCom()->Get_WorldMatrix();

	// ī�޶� vector_c_n ���� �ٿ�����Ѵ�.
	_matrix matVectorBoneWorld = XMLoadFloat4x4(pPlayerModelCom->Get_BoneCombinedTransformationMatrix("vector_c_n"));

	// Blender�� ��ǥ�踦 DirectX�� ��ǥ��� ��ȯ�ϱ� ���� ȸ�� ���
	_matrix rotationMatrixX = XMMatrixRotationX(XMConvertToRadians(fConvertX));
	_matrix rotationMatrixY = XMMatrixRotationY(XMConvertToRadians(fConvertY));
	_matrix rotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(fConvertZ));

	// Blender�� �� ��ȯ ��İ� �÷��̾��� ���� ��ȯ ����� �����ϰ� ��ǥ�� ��ȯ�� ����
	_matrix viewMatrix = rotationMatrixX * rotationMatrixY * rotationMatrixZ * matVectorBoneWorld * matCameraMatrix * matPlayerWorld;

	// ���� �� ����� ���
	_matrix viewMatrixInv = XMMatrixInverse(nullptr, viewMatrix);

	// �� ����� ���������ο� ����
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, viewMatrix);

	auto KeyFrames = pPlayerCamModelCom->Get_CurrentKeyFrameIndices(32);
	_uint iKeyFrameIndex = KeyFrames->front();
	Set_FoV(pPlayerCamModelCom->Get_FoV(pPlayerCamModelCom->Get_AnimationName(32), iKeyFrameIndex));
}

CCutSceneCamera* CCutSceneCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCutSceneCamera* pInstance = new CCutSceneCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCutSceneCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCutSceneCamera::Clone(void* pArg)
{
	CCutSceneCamera* pInstance = new CCutSceneCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCutSceneCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCutSceneCamera::Free()
{
	__super::Free();

	Safe_Release(m_pSystemManager);
}
