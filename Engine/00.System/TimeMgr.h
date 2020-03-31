#pragma once

#ifndef __TIMEMGR_H__
#define __TIMEMGR_H__

#include "Base.h"

BEGIN(ENGINE)

class ENGINE_DLL CTimeMgr : public CBase
{
	DECLARE_SINGLETON(CTimeMgr)

private:
 	explicit CTimeMgr();
	virtual ~CTimeMgr() = default;

public:
	_bool Check_SlowTime() { return m_bSlowTime; }
	void Set_TimeStop(_bool bCheck);
	void Set_SlowTime(_bool bCheck) { m_bSlowTime = bCheck; }
	const _double& Get_DeltaTime();

public:
	void Init_Time();
	_double Update_Time();

private:
	_bool	m_bTimeStop = FALSE;
	_bool  m_bSlowTime = FALSE;
	_double m_DeltaTime = 0.0; //시간 / fps   = 수행시간
	_double m_SlowTime = 0.0;

	LARGE_INTEGER m_OldTime;		//측정 시작 시간(Time)
	LARGE_INTEGER m_CurTime;		//측정 후 지나간 시간(Time)
	LARGE_INTEGER m_CpuClock;	//CPU 진동수(Frequency)

public:
	virtual void Free();
};

END

#endif // !__TIMEMGR_H__
