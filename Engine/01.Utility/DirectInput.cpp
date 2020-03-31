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
	//DI 컴 객체 생성
	FAILED_CHECK_RETURN(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8,
		(void**)&m_pInputSDK, 
		NULL), E_FAIL);

	//키보드 객체 생성
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr), E_FAIL);

	//생성된 키보드 객체에 대한 정보를 컴 객체에게 전달
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	//장치에 대한 독점권을 설정(클라이언트가 표시되는 상태에서 키 입력 여부를 결정
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	//장치에 대한 Access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();

	// 마우스 객체 생성 //
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr), E_FAIL);

	//생성된 마우스 객체의 정보를 컴 객체에게 전달
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	//장치에 대한 독점권 설정(클라이언트 표시되는 상태에서의 키입력)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	//장치에 대한 Access 버전을 받아오는 함수
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
