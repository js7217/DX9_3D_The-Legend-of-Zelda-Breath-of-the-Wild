#include "DirectInput.h"

IMPLEMENT_SINGLETON(CInputDev)

CInputDev::CInputDev()
	: m_pInputSDK(nullptr), m_pKeyBoard(nullptr), m_pMouse(nullptr)
{
}

CInputDev::~CInputDev()
{
	Free();
}

HRESULT CInputDev::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	//DI �� ��ü ����
	FAILED_CHECK_RETURN(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8,
		(void**)&m_pInputSDK, 
		NULL), E_FAIL);

	//Ű���� ��ü ����
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr), E_FAIL);

	//������ Ű���� ��ü�� ���� ������ �� ��ü���� ����
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	//��ġ�� ���� �������� ����(Ŭ���̾�Ʈ�� ǥ�õǴ� ���¿��� Ű �Է� ���θ� ����
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	//��ġ�� ���� Access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();

	// ���콺 ��ü ���� //
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr), E_FAIL);

	//������ ���콺 ��ü�� ������ �� ��ü���� ����
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	//��ġ�� ���� ������ ����(Ŭ���̾�Ʈ ǥ�õǴ� ���¿����� Ű�Է�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	//��ġ�� ���� Access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();

	return S_OK;
}

void CInputDev::Set_InputDev(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

void CInputDev::Free(void)
{
	ENGINE::Safe_Release(m_pKeyBoard);
	ENGINE::Safe_Release(m_pMouse);
	ENGINE::Safe_Release(m_pInputSDK);
}
