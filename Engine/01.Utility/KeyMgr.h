#pragma once

#ifndef __KEYMANAGER_H__
#define __KEYMANAGER_H__

#include "Base.h"

BEGIN(ENGINE)

const DWORD dwKEY_UP			 = 0x00000001;
const DWORD dwKEY_DOWN	 = 0x00000002;
const DWORD dwKEY_LEFT		 = 0x00000004;
const DWORD dwKEY_RIGHT	 = 0x00000008;

const DWORD dwKEY_RETURN	 = 0x00000010;
const DWORD dwKEY_SPACE		 = 0x00000020;
const DWORD dwKEY_LBUTTON	 = 0x00000040;
const DWORD dwKEY_RBUTTON	 = 0x00000080;

const DWORD dwKEY_ESCAPE		= 0x00000100;
const DWORD dwKEY_Z				= 0x00000200;
const DWORD dwKEY_X				= 0x00000400;
const DWORD dwKEY_C				= 0x00000800;

const DWORD dwKEY_F1			= 0x00001000;
const DWORD dwKEY_F2			= 0x00002000;
const DWORD dwKEY_F3			= 0x00004000;
const DWORD dwKEY_F4			= 0x00008000;

const DWORD dwKEY_W				= 0x00010000;
const DWORD dwKEY_A				= 0x00020000;
const DWORD dwKEY_S				= 0x00040000;
const DWORD dwKEY_D				= 0x00080000;

const DWORD dwKEY_Q				= 0x00100000;
const DWORD dwKEY_E				= 0x00200000;
const DWORD dwKEY_R				= 0x00400000;
const DWORD dwKEY_Shift		= 0x00800000;

const DWORD dwKEY_LCntl			= 0x01000000;
const DWORD dwKEY_0				= 0x02000000;
const DWORD dwKEY_1				= 0x04000000;
const DWORD dwKEY_2				= 0x08000000;

const DWORD dwKEY_3				= 0x10000000;
const DWORD dwKEY_4				= 0x20000000;
const DWORD dwKEY_5				= 0x40000000;
const DWORD dwKEY_TAB			= 0x80000000;

class ENGINE_DLL CKeyMgr :
	public CBase
{
	DECLARE_SINGLETON(CKeyMgr)

private:
	explicit CKeyMgr();
	virtual ~CKeyMgr();

public:
	void KeyInput();
	bool Key_Down(DWORD dwKey);
	bool Key_Up(DWORD dwKey);
	bool Key_Press(DWORD dwKey);
	bool Key_Combined(DWORD dwFirst, DWORD dwSecond);
	DWORD Key_Press_Comb(DWORD dwFirst, DWORD dwSecond);

private:
	DWORD m_dwCurKey;
	DWORD m_dwKeyDown;
	DWORD m_dwKeyUP;

public:
	virtual void Free(void);

};

END

#endif // !__KEYMANAGER_H__