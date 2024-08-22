#include "../Public/CineCamera.h"

#include "GameInstance.h"
#include "SystemManager.h"

CCineCamera::CCineCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCineCamera::CCineCamera(const CCineCamera& rhs)
	: CCamera{ rhs }
{
}

HRESULT CCineCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCineCamera::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		CINE_CAMERA_DESC* pDesc = static_cast<CINE_CAMERA_DESC*>(pArg);
		Load_CamBin(pDesc->iFileNum);
	}

	/* ī�޶� �ʱ�ȭ */
	m_iCurCamIndex = 0;
	/* ù ������ stay�� ���� */
	m_eCurCamState = CAM_STATE_STAY;
	m_fStayDeltaTime = 0;
	m_fStayTime = m_vCamerasObjDesc[0].fStayTime;
	m_fLerpDeltaTime = 0;
	
	CAMERA_DESC		cameraDesc;
	cameraDesc.fFovY = m_vCamerasObjDesc[0].fFovY;
	cameraDesc.fAspect = m_vCamerasObjDesc[0].fAspect;
	cameraDesc.fNear = m_vCamerasObjDesc[0].fNear;
	cameraDesc.fFar = m_vCamerasObjDesc[0].fFar;
	cameraDesc.vFocus = m_vCamerasObjDesc[0].vFocus;
	cameraDesc.vEye = m_vCamerasObjDesc[0].vEye;

	m_vEye = XMLoadFloat4(&cameraDesc.vEye);
	m_vFocus = XMLoadFloat4(&cameraDesc.vFocus);
	m_bLastLerp = false;
	
	if (FAILED(__super::Initialize(&cameraDesc)))
		return E_FAIL;

	Set_Start();

	return S_OK;
}

void CCineCamera::Priority_Tick(const _float& fTimeDelta)
{
}

void CCineCamera::Tick(const _float& fTimeDelta)
{
	if (CSystemManager::GetInstance()->Get_Camera() == CAMERA_CINEMACHINE)
	{
		Start_Lerp(fTimeDelta);
		Cam_Move_Handle_Setting(fTimeDelta);
		Cam_Lerp(fTimeDelta);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vEye);
		m_pTransformCom->LookAt(m_vFocus);

		__super::Tick(fTimeDelta);
	}
}

void CCineCamera::Late_Tick(const _float& fTimeDelta)
{
}

HRESULT CCineCamera::Render()
{
	return S_OK;
}

HRESULT CCineCamera::Load_CamBin(int iFileNum)
{
	m_vCamerasObjDesc.clear();

	CAMERAOBJ_IO		camIODesc;
	Import_Bin_Cam_Data_OnTool(&camIODesc, iFileNum);

	m_bFirstLerp = camIODesc.bFirstLerp;

	for (int i = 0; i < camIODesc.iCameraNum; i++)
	{
		CAMERAOBJ_DESC		cameraDesc;
		cameraDesc = camIODesc.pCamObjDesc[i];

		m_vCamerasObjDesc.push_back(cameraDesc);
	}

	m_iCurCamIndex = 0;

	Safe_Delete_Array(camIODesc.pCamObjDesc);

	return S_OK;
}

void CCineCamera::Setting_Start_Cinemachine()
{
	m_iCurCamIndex = 0;

	m_vEye = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex].vEye);
	m_vFocus = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex].vFocus);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vEye);
	m_pTransformCom->LookAt(m_vFocus);


	/* ù ������ stay�� ���� */
	m_eCurCamState = CAM_STATE_STAY;
	m_fStayDeltaTime = 0;
	m_fStayTime = m_vCamerasObjDesc[0].fStayTime;
	m_fLerpDeltaTime = 0;
	m_bLastLerp = false;

	Set_Start();
}

HRESULT CCineCamera::Import_Bin_Cam_Data_OnTool(CAMERAOBJ_IO* camData, int iFileNum)
{
	char fullPath[MAX_PATH];
	strcpy_s(fullPath, "../../Client/Bin/DataFiles/CameraData/");

	strcat_s(fullPath, "Camera_Data");
	strcat_s(fullPath, "_");

	string strNum = to_string(iFileNum);
	char cLevel[20];
	strcpy_s(cLevel, strNum.c_str());
	strcat_s(fullPath, cLevel);
	strcat_s(fullPath, ".dat");

	ifstream in(fullPath, ios::binary);

	if (!in.is_open()) {
		MSG_BOX("���� ���� ����");
		return E_FAIL;
	}

	in.read((char*)&camData->iCameraNum, sizeof(int));

	in.read((char*)&camData->bFirstLerp, sizeof(bool));

	camData->pCamObjDesc = new CAMERAOBJ_DESC[camData->iCameraNum];

	for (int i = 0; i < camData->iCameraNum; i++)
	{
		in.read((char*)&camData->pCamObjDesc[i], sizeof(CAMERAOBJ_DESC));
	}

	in.close();

	return S_OK;
}

void CCineCamera::Mouse_Fix()
{
	POINT	pt{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

void CCineCamera::Cam_Move_Handle_Setting(const _float& fTimeDelta)
{
	/* ���°� ���� ���� */
	if (m_vCamerasObjDesc.size() > m_iCurCamIndex && true == m_bStart)
	{
		if (CAM_STATE_STAY == m_eCurCamState)
		{
			m_fStayDeltaTime += fTimeDelta;

			if (m_fStayDeltaTime > m_fStayTime)
			{
				m_iCurCamIndex++;

				if (m_vCamerasObjDesc.size() == m_iCurCamIndex)
				{
					Set_Ending();
					return;
				}

				/* ���� �ٲٱ� */
				Change_CamDesc();

				if (true == m_vCamerasObjDesc[m_iCurCamIndex-1].bLerp)
				{
					/* ���������ؼ� �̵� */
					m_eCurCamState = CAM_STATE_LERP;
					m_fLerpDeltaTime = 0;
					m_fStayDeltaTime = 0;
					m_fLerpTime = m_vCamerasObjDesc[m_iCurCamIndex - 1].fMoveTime;
				}
				else
				{
					/* �������� ���Ҷ� */
					m_eCurCamState = CAM_STATE_STAY;
					m_fStayDeltaTime = 0;
					m_fLerpDeltaTime = 0;
					m_fStayTime = m_vCamerasObjDesc[m_iCurCamIndex].fStayTime;


					XMVECTOR		camEye, camFocus;
					camEye = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex].vEye);
					camFocus = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex].vFocus);
					m_pTransformCom->LookAt(camFocus);
					m_vEye = camEye;
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, camEye);
				}
			}
		}
		else if (CAM_STATE_LERP == m_eCurCamState)
		{
			m_fLerpDeltaTime += fTimeDelta;

			if (m_fLerpDeltaTime > m_fLerpTime)
			{
				/* Lerp�Ҷ� m_icurindex++ ���� -> Lerp �ϱ� �� stay �Ŀ� ++ ���༭ �ʿ���� */
				if (m_vCamerasObjDesc.size() == m_iCurCamIndex)
				{
					Set_Ending();
					return;
				}

				/* ���� �ٲٱ� */
				Change_CamDesc();

				/* Lerp�� �� �ӹ����� */
				m_eCurCamState = CAM_STATE_STAY;
				m_fStayDeltaTime = 0;
				m_fStayTime = m_vCamerasObjDesc[m_iCurCamIndex].fStayTime;

				XMVECTOR		camEye, camFocus;
				//camEye = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex].vEye);
				camFocus = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex].vFocus);
				m_pTransformCom->LookAt(camFocus);
				//m_vEye = camEye;
				//m_pTransformCom->Set_State(CTransform::STATE_POSITION, camEye);

				m_fLerpDeltaTime = 0;
				
			}
		}
	}
	
}

void CCineCamera::Cam_Lerp(const _float& fTimeDelta)
{
	
	if (CAM_STATE_LERP == m_eCurCamState && false == m_bLastLerp && true == m_bStart)
	{
		/* �ó׸ӽ� �������� �۵� */
		XMVECTOR		vPreEye, vCurEye;
		XMVECTOR		vPreFocus, vCurFocus;
		vPreEye = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex - 1].vEye);
		vCurEye = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex].vEye);
		/*m_vEye = XMVectorLerp(vPreEye, vCurEye, fTime);*/
		m_vEye = Positioning_Using_Linear_Interpolation(vPreEye, vCurEye, m_vCamerasObjDesc[m_iCurCamIndex - 1].fMoveSpeed);


		vPreFocus = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex - 1].vFocus);
		vCurFocus = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex].vFocus);
		/*m_vFocus = XMVectorLerp(vPreFocus, vCurFocus, fTime);*/
		m_vFocus = Positioning_Using_Linear_Interpolation(vPreFocus, vCurFocus, m_vCamerasObjDesc[m_iCurCamIndex - 1].fMoveSpeed);
	}
	else if (CAM_STATE_LERP == m_eCurCamState && true == m_bLastLerp)
	{
		/* �÷��̾�� ���ư��� lerp�ϴ� �κ� */
		XMVECTOR		vPreEye, vCurEye;
		XMVECTOR		vPreFocus, vCurFocus;
		vPreEye = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex - 1].vEye);
		vCurEye = m_pGameInstance->Get_GameObject(LEVEL_RUNMAP, TEXT("Layer_Camera"), 0)->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		/*m_vEye = XMVectorLerp(vPreEye, vCurEye, fTime);*/
		m_vEye = Positioning_Using_Linear_Interpolation(vPreEye, vCurEye, m_vCamerasObjDesc[m_iCurCamIndex - 1].fMoveSpeed);

		vPreFocus = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex - 1].vFocus);
		vCurFocus = XMVectorSet(0,0,0,1);
		/*m_vFocus = XMVectorLerp(vPreFocus, vCurFocus, fTime);*/
		m_vFocus = Positioning_Using_Linear_Interpolation(vPreFocus, vCurFocus, m_vCamerasObjDesc[m_iCurCamIndex - 1].fMoveSpeed);

		m_fLerpDeltaTime += fTimeDelta;

		if (m_fLerpDeltaTime > m_fLerpTime)
		{
			/* �÷��̾�� ���ư� �� ī�޶� �÷��̾� ī�޶�� ���� */
			m_eCurCamState = CAM_STATE_END;
			m_iCurCamIndex = 0;
			m_vCamerasObjDesc.clear();
			CSystemManager::GetInstance()->Set_Camera(CAMERA_PLAYER);
		}
	}
}

void CCineCamera::Change_CamDesc()
{
	/* ī�޶� ���� ���� */
	m_fFovY = m_vCamerasObjDesc[m_iCurCamIndex].fFovY;
	m_fAspect = m_vCamerasObjDesc[m_iCurCamIndex].fAspect;
	m_fNear = m_vCamerasObjDesc[m_iCurCamIndex].fNear;
	m_fFar = m_vCamerasObjDesc[m_iCurCamIndex].fFar;
}

void CCineCamera::Set_Start()
{
	if (true == m_bFirstLerp)
	{
		/* ���������ϸ鼭 cinemachine ������ �̵� */
		/* 0��°�� �ӵ��� �ð��� �ޱ� ���� ���� ���� */
		m_bStart = false;
		m_fLerpDeltaTime = 0;
		m_fLerpTime = m_vCamerasObjDesc[0].fMoveTime;
		m_fMoveSpeed = m_vCamerasObjDesc[0].fMoveSpeed;

		m_vStartEye = m_pGameInstance->Get_GameObject(LEVEL_RUNMAP, TEXT("Layer_Camera"), 0)->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		m_vStartFocus = XMVectorSet(0, 0, 0, 1);
	}
	else
	{
		/* �ٷ� cinemachine ���� */
		m_bStart = true;
	}
}

void CCineCamera::Set_Ending()
{
	if (false == m_vCamerasObjDesc[m_iCurCamIndex - 1].bLerp)
	{
		/* ���� ���� ���Ҷ� */
		m_eCurCamState = CAM_STATE_END;
		m_iCurCamIndex = 0;
		m_vCamerasObjDesc.clear();
		CSystemManager::GetInstance()->Set_Camera(CAMERA_PLAYER);
	}
	else if (true == m_vCamerasObjDesc[m_iCurCamIndex - 1].bLerp)
	{
		/* ���� ���� �Ҷ� */
		m_bLastLerp = true;
		m_eCurCamState = CAM_STATE_LERP;
		m_fLerpDeltaTime = 0;
		m_fLerpTime = m_vCamerasObjDesc[m_iCurCamIndex - 1].fMoveTime;
	}
}

void CCineCamera::Start_Lerp(const _float& fTimeDelta)
{
	if (false == m_bStart)
	{
		m_eCurCamState = CAM_STATE_LERP;

		m_fLerpDeltaTime += fTimeDelta;

		if (m_fLerpDeltaTime > m_fLerpTime)
		{

 			m_iCurCamIndex++;

			/* ��¥ ù��° ī�޶� ���� */
			m_eCurCamState = CAM_STATE_STAY;
			m_fStayDeltaTime = 0;
			m_fStayTime = m_vCamerasObjDesc[m_iCurCamIndex].fStayTime;
			m_fLerpDeltaTime = 0;
			m_bLastLerp = false;

			/* ���� �ٲٱ� */
			Change_CamDesc();

			m_bStart = true;

			/* �ؿ��� �����ϸ� �ȵż� return���� ������ */
			return;
		}

		XMVECTOR		vPreEye, vCurEye;
		XMVECTOR		vPreFocus, vCurFocus;
		vPreEye = m_vStartEye;
		vCurEye = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex + 1].vEye);
		m_vEye = Positioning_Using_Linear_Interpolation(m_vStartEye, vCurEye, m_fMoveSpeed);


		vPreFocus = m_vStartFocus;
		vCurFocus = XMLoadFloat4(&m_vCamerasObjDesc[m_iCurCamIndex + 1].vFocus);
		m_vFocus = Positioning_Using_Linear_Interpolation(vPreFocus, vCurFocus, m_fMoveSpeed);
	}
}

XMVECTOR CCineCamera::Positioning_Using_Linear_Interpolation(XMVECTOR vCurPos, XMVECTOR vTargetPos, float fSpeed)
{
	XMVECTOR		vResult;
	float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vCurPos));
	float fLerpFactor = (fDistance > 0) ? (fSpeed * m_fLerpDeltaTime / m_fLerpTime) : 1.0f;

	fLerpFactor = min(fLerpFactor, 1.0f);

	vResult = XMVectorLerp(vCurPos, vTargetPos, fLerpFactor);

	return vResult;
}

CCineCamera* CCineCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCineCamera* pInstance = new CCineCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCineCamera::Clone(void* pArg)
{
	CCineCamera* pInstance = new CCineCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCineCamera::Free()
{

	m_vCamerasObjDesc.clear();

	__super::Free();
}
