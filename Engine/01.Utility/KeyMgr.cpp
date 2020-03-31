#include "KeyMgr.h"

IMPLEMENT_SINGLETON(CKeyMgr)

CKeyMgr::CKeyMgr()
{
}

CKeyMgr::~CKeyMgr()
{
	Free();
}

void CKeyMgr::KeyInput()
{
	m_dwCurKey = 0;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		m_dwCurKey |= dwKEY_UP;				// 0001
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		m_dwCurKey |= dwKEY_DOWN;			//0002
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_dwCurKey |= dwKEY_LEFT;			//0004
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_dwCurKey |= dwKEY_RIGHT;			//0008

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		m_dwCurKey |= dwKEY_RETURN;			// 0010
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		m_dwCurKey |= dwKEY_SPACE;				//0020
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		m_dwCurKey |= dwKEY_LBUTTON;			//0040
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		m_dwCurKey |= dwKEY_RBUTTON;		//0080

	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		m_dwCurKey |= dwKEY_ESCAPE;			//0100
	if (GetAsyncKeyState(0x5A) & 0x8000)
		m_dwCurKey |= dwKEY_Z;			//0200
	if (GetAsyncKeyState(0x58) & 0x8000)
		m_dwCurKey |= dwKEY_X;			//0400
	if (GetAsyncKeyState(0x43) & 0x8000)
		m_dwCurKey |= dwKEY_C;			//0800

	
	if (GetAsyncKeyState(VK_F1) & 0x8000)
		m_dwCurKey |= dwKEY_F1;			//1000
	if (GetAsyncKeyState(VK_F2) & 0x8000)
		m_dwCurKey |= dwKEY_F2;			//2000
	if (GetAsyncKeyState(VK_F3) & 0x8000)
		m_dwCurKey |= dwKEY_F3;			//4000
	if (GetAsyncKeyState(VK_F4) & 0x8000)
		m_dwCurKey |= dwKEY_F4;			//8000

	if (GetAsyncKeyState(0x57) & 0x8000)
		m_dwCurKey |= dwKEY_W;			//10000
	if (GetAsyncKeyState(0x41) & 0x8000)
		m_dwCurKey |= dwKEY_A;			//20000
	if (GetAsyncKeyState(0x53) & 0x8000)
		m_dwCurKey |= dwKEY_S;			//40000
	if (GetAsyncKeyState(0x44) & 0x8000)
		m_dwCurKey |= dwKEY_D;			//80000

	if (GetAsyncKeyState(0x51) & 0x8000)
		m_dwCurKey |= dwKEY_Q;			//100000
	if (GetAsyncKeyState(0x45) & 0x8000)
		m_dwCurKey |= dwKEY_E;			//200000
	if (GetAsyncKeyState(0x52) & 0x8000)
		m_dwCurKey |= dwKEY_R;			//400000
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		m_dwCurKey |= dwKEY_Shift;			//800000


	if (GetAsyncKeyState(VK_CONTROL) * 0x8000)
		m_dwCurKey |= dwKEY_LCntl;		//2000000
	if (GetAsyncKeyState('0') * 0x8000)
		m_dwCurKey |= dwKEY_0;		//1000000
	if (GetAsyncKeyState('1') * 0x8000)
		m_dwCurKey |= dwKEY_1;		// 4000000
	if (GetAsyncKeyState('2') * 0x8000)
		m_dwCurKey |= dwKEY_2;		// 8000000

	if (GetAsyncKeyState('3') * 0x8000)
		m_dwCurKey |= dwKEY_3;		// 10000000
	if (GetAsyncKeyState('4') * 0x8000)
		m_dwCurKey |= dwKEY_4;		// 8000000
	if (GetAsyncKeyState('5') * 0x8000)
		m_dwCurKey |= dwKEY_5;		// 10000000
	if (GetAsyncKeyState(VK_TAB) * 0x8000)
		m_dwCurKey |= dwKEY_TAB;		// 10000000
	
}

bool CKeyMgr::Key_Down(DWORD dwKey)
{
	//전에 누른적 없고 현재 누르고있을 때 TRUE 반환
	if (!(m_dwKeyDown & dwKey) && m_dwCurKey & dwKey)
	{
		m_dwKeyDown |= dwKey;
		return true;
	}
	//전에 누른적 있고 현재 누르지 않았을 때 FALSE 반환 
	//(위에 상태에서 이어질 때 == 위에 상태와 반대일 떄)
	else if (m_dwKeyDown & dwKey && !(m_dwCurKey & dwKey))
	{
		m_dwKeyDown ^= dwKey;
		return false;
	}

	return false;
}

bool CKeyMgr::Key_Up(DWORD dwKey)
{
	//전에 누른적 있고 현재 안눌었을 떄 TRUE 반환
	if (m_dwKeyUP & dwKey && !(m_dwCurKey & dwKey))
	{
		m_dwKeyUP ^= dwKey;
		return true;
	}
	//전에 누른적 없고 현재 눌렀을 때 FALSE 반환
	else if (!(m_dwKeyUP & dwKey) && m_dwCurKey & dwKey)
	{
		m_dwKeyUP |= dwKey;
		return false;
	}

	return false;
}

bool CKeyMgr::Key_Press(DWORD dwKey)
{
	if (m_dwCurKey & dwKey)
		return true;

	return false;
}

//한개의 키를 누른 후 그 다음키를 이어서 눌렀을 때 작동
bool CKeyMgr::Key_Combined(DWORD dwFirst, DWORD dwSecond)
{
	if (Key_Press(dwSecond) && Key_Press(dwFirst))
		return true;
	if (Key_Press(dwFirst) && Key_Press(dwSecond))
		return true;

	return false;
}

DWORD CKeyMgr::Key_Press_Comb(DWORD dwFirst, DWORD dwSecond)
{
	
	if (Key_Press(dwSecond) && Key_Down(dwFirst))
		return 2;
	else if (Key_Press(dwFirst))
		return 1;

	return 0;	//FALSE
}

void CKeyMgr::Free(void)
{
}
