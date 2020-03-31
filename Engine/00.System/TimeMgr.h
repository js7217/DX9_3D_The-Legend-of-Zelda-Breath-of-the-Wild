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
	_double m_DeltaTime = 0.0; //�ð� / fps   = ����ð�
	_double m_SlowTime = 0.0;

	LARGE_INTEGER m_OldTime;		//���� ���� �ð�(Time)
	LARGE_INTEGER m_CurTime;		//���� �� ������ �ð�(Time)
	LARGE_INTEGER m_CpuClock;	//CPU ������(Frequency)

public:
	virtual void Free();
};

END

#endif // !__TIMEMGR_H__
