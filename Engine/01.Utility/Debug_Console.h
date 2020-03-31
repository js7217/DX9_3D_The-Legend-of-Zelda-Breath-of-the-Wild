#pragma once

#include "Base.h"

BEGIN(ENGINE)

class ENGINE_DLL CDebug_Console : public CBase
{
	DECLARE_SINGLETON(CDebug_Console)

private:
	explicit CDebug_Console();
	virtual ~CDebug_Console() = default;

public:
	CRITICAL_SECTION*	Get_Crt() { return &m_Crt; }
	_double Get_TimeAcc() { return m_TimeAcc; }
	_vec3		Get_PlayerPos() { return m_vPlayerPos; }
	void Set_PlayerPos(_vec3 vPos)
	{		m_vPlayerPos = vPos;	}
	_bool Get_bPlayer() { return m_bPlayer; }
	void Set_bPlayer(_bool bCheck) { m_bPlayer = bCheck; }
	void Set_ClientEnd(_bool bCheck);
	
public:
	HRESULT Ready_DebugConsole();
	void Update_DebugConsole();

private:
	void Time_Console();
	void Player_Console();
	void Player_Position();

public:
	static _uint CALLBACK Thread_Main(void* pArg);

private:
	_bool m_bClientEnd;
	_double m_TimeAcc;

private:
	_bool m_bPlayer;
	_vec3 m_vPlayerPos;

private:
	HANDLE						m_hThread;
	CRITICAL_SECTION	m_Crt;

public:
	_int m_iSwitch;


public:
	static CDebug_Console* Create();
	virtual void Free();
};

END