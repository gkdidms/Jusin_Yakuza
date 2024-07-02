
#include "..\Public\Input_Device.h"



Engine::CInput_Device::CInput_Device(void)
{

}

HRESULT Engine::CInput_Device::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{

	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;

	// Ű���� ��ü ����
	if (FAILED((m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr))))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();

	m_vecKey.reserve(DIK_END);

	for (int i = 0; i < 256; ++i)
	{
		m_vecKey.push_back(tKeyInfo{ KEYSTATE::NONE,false });
	}

	for (int i = 0; i < DIM_END; ++i)
		m_vecMouseState.push_back(KEYSTATE::NONE);

	return S_OK;
}

void Engine::CInput_Device::Update_InputDev(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);

	for (size_t i = 0; i < 256; i++)
	{
		if (m_byKeyState[i] & 0x80)
		{
			if (m_vecKey[i].IsPrePush)
			{
				//�������� �����־�����
				m_vecKey[i].eState = KEYSTATE::HOLD;
			}
			else
			{
				m_vecKey[i].eState = KEYSTATE::TAP;
			}
			//���������� �����־����� true
			m_vecKey[i].IsPrePush = true;
		}
		else// Ű�� �ȴ����־�����
		{
			if (m_vecKey[i].IsPrePush)
			{
				// ������ �����־���.
				m_vecKey[i].eState = KEYSTATE::AWAY;
			}
			else
			{
				m_vecKey[i].eState = KEYSTATE::NONE;
			}
			m_vecKey[i].IsPrePush = false;
		}
	}

	for (int i = 0; i < DIM_END; ++i)
	{
		if (m_tMouseState.rgbButtons[i] & 0x80)
		{
			if (m_vecMouseState[i] == KEYSTATE::NONE)
				m_vecMouseState[i] = KEYSTATE::TAP;
			else
				m_vecMouseState[i] = KEYSTATE::HOLD;
		}
		else
		{
			if (m_vecMouseState[i] == KEYSTATE::HOLD || m_vecMouseState[i] == KEYSTATE::TAP)
				m_vecMouseState[i] = KEYSTATE::AWAY;
			else
				m_vecMouseState[i] = KEYSTATE::NONE;
		}
	}
}

CInput_Device * CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device*		pInstance = new CInput_Device();

	if (FAILED(pInstance->Ready_InputDev(hInst, hWnd)))
	{
		MSG_BOX("Failed to Created : CInput_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

