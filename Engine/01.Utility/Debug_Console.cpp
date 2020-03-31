#include "Debug_Console.h"

IMPLEMENT_SINGLETON(CDebug_Console)

CDebug_Console::CDebug_Console()
	: m_TimeAcc(1.0), m_bClientEnd(FALSE), m_bPlayer(FALSE)
{
	ZeroMemory(m_vPlayerPos, sizeof(_vec3));
}

void CDebug_Console::Set_ClientEnd(_bool bCheck)
{
	m_bClientEnd = bCheck;
}

HRESULT CDebug_Console::Ready_DebugConsole()
{
	m_bClientEnd = TRUE;
	InitializeCriticalSection(&m_Crt);
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

	FILE *stream;
	AllocConsole();
	freopen_s(&stream, "CONIN$", "rb", stdin);
	freopen_s(&stream, "CONOUT$", "wb", stdout);
	freopen_s(&stream, "CONOUT$", "wb", stderr);

	ios::sync_with_stdio();
	//_bool bb = AttachConsole(ATTACH_PARENT_PROCESS); //DETACHED_PROCESS

	//if (bClientEnd)
	//	Free();

	return S_OK;
}

void CDebug_Console::Update_DebugConsole()
{
	_bool bDebug = TRUE;
	while (bDebug)
	{
		system("cls");
		m_iSwitch = 0;

		cout << "[ DEBUG CONSOLE :: -1 to Exit Console ]" << endl;
		cout << " 1 :: Time Manager " << endl;
		cout << " 2 :: Player Controll " << endl;
		cout << "Select Console >>  ";
		cin >> m_iSwitch;
		cout << endl;

		if (cin.fail())
		{
			cin.clear();
			cin.ignore(256, '\n');
			m_iSwitch = 0;
		}

		system("cls");
		switch (m_iSwitch)
		{
		case 0:
			break;
		case 1:
			Time_Console();
			break;
		case 2:
			Player_Console();
			break;

		case -1:
			bDebug = FALSE;
			break;

		default:
			m_iSwitch = 0;
			break;
		}
	}
	m_iSwitch = 0;
	m_bPlayer = FALSE;
	m_TimeAcc = 1.0;
}

void CDebug_Console::Time_Console()
{
	//cout << "Time Controll Console [ Current Acceleration Time = " << m_TimeAcc << " ]" << endl;
	m_iSwitch = 0;
	while (true)
	{
		system("cls");
		cout << "[ Notice : -1 to Exit  ]" << endl;
		printf("Time Controll Console [ Current Acceleration Time =  %3.4f ]\n", m_TimeAcc);
		cout << " Acceleration Time : ";
		cin >> m_TimeAcc;
		cout << endl;

		if (m_TimeAcc < 0)
		{
			m_TimeAcc = 1.0;
			break;
		}

	}
}

void CDebug_Console::Player_Console()
{
	m_iSwitch = 0;
	while (true)
	{
		system("cls");
		m_iSwitch = 0;
		cout << "[ Notice : -1 to Exit  ]" << endl;
		cout << "Player Controll Console Mode !" << endl;
		printf("Current Player Position [ X : %3.4f, Y : %3.4f, Z : %3.4f ]\n", m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z);
		cout << "1 :: Player Position Mode" << endl;
		cout << "Select Mode >>  ";
		cin >> m_iSwitch;
		cout << endl;

		switch (m_iSwitch)
		{
		case 0:
			break;
		case 1:
			Player_Position();
			break;

		default:
			break;
		}


		if (m_iSwitch < 0)
		{
			m_iSwitch = 0;
			break;
		}
	}
	m_iSwitch = 0;
}

void CDebug_Console::Player_Position()
{
	m_bPlayer = TRUE;
	m_TimeAcc = 0.0;

	printf("Current Player Position [ X : %3.4f, Y : %3.4f, Z : %3.4f ]\n", m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z);
	cout << " Insert Pos X >>  ";
	cin >> m_vPlayerPos.x;
	cout << endl;
	Sleep(500);

	cout << " Insert Pos Y >>  ";
	cin >> m_vPlayerPos.y;
	cout << endl;
	Sleep(500);

	cout << " Insert Pos Z >>  ";
	cin >> m_vPlayerPos.z;
	cout << endl;
	Sleep(500);

	m_iSwitch = 0;
	m_TimeAcc = 1.0;
	m_bPlayer = FALSE;
}

_uint CDebug_Console::Thread_Main(void * pArg)
{
	CDebug_Console* pDebug = (CDebug_Console*)pArg;

	EnterCriticalSection(pDebug->Get_Crt());

	pDebug->Update_DebugConsole();

	LeaveCriticalSection(pDebug->Get_Crt());

	return 0;
}

CDebug_Console * CDebug_Console::Create()
{
	CDebug_Console* pInstance = new CDebug_Console;

	if (FAILED(pInstance->Ready_DebugConsole()))
		Safe_Release(pInstance);

	return pInstance;
}

void CDebug_Console::Free()
{
	//FreeConsole();
	if (m_bClientEnd)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		DeleteCriticalSection(&m_Crt);
	}

}
