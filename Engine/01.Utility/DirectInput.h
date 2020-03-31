#ifndef DirectInput_h__
#define DirectInput_h__

#include "Base.h"

BEGIN(ENGINE)

class ENGINE_DLL CInputDev :
	public CBase 
{
	DECLARE_SINGLETON(CInputDev)

public:
	enum MOUSE_KEYSTATE {DIM_LB, DIM_RB, DIM_MB, DIM_END};
	enum MOUSE_MOVESTATE {DIMS_X, DIMS_Y, DIMS_Z, DIMS_END};

private:
	explicit CInputDev();
	virtual ~CInputDev();

public:
	_byte	Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte	Get_DIMouseState(MOUSE_KEYSTATE eMouse) { return m_tMouseState.rgbButtons[eMouse]; }

	_long	Get_DIMouseMove(MOUSE_MOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}


public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Set_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK;
private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard;
	LPDIRECTINPUTDEVICE8	m_pMouse;

private:
	_byte					m_byKeyState[256];
	DIMOUSESTATE			m_tMouseState;

public:
	virtual void	Free(void);

};


END

#endif // DirectInput_h__
