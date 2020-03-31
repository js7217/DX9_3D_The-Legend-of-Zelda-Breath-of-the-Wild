#pragma once

#include "Base.h"

BEGIN(ENGINE)

class CAnimationCtrl final : public CBase
{
private:
	explicit CAnimationCtrl(LPD3DXANIMATIONCONTROLLER pAniCtrl);
	explicit CAnimationCtrl(const CAnimationCtrl& rhs);
	virtual ~CAnimationCtrl() = default;

public:
	HRESULT Ready_AnimationCtrl();
	HRESULT SetUp_AnimationSet(_uint iIndex);
	HRESULT SetUp_AnimationSet(const char* pName);
	HRESULT Play_Animation(const _double& TimeDelta, _float fAdjSpeed = 1.f);
	_bool	IsAnimationSetEnd(_float fInterpolatedValue = 0.0f);
	_uint	Get_CurrentAniTrack() { return m_iCurrentTrack; }

private:
	LPD3DXANIMATIONCONTROLLER	m_pAniCtrl = nullptr;
	_uint		m_iCurrentTrack = 0;
	_uint		m_iNewTrack = 1;
	_uint		m_iOldIndex = 99;
	_double		m_TimeAcc = 0.0;
	_double		m_dPeriod = 0.0;

public:
	static CAnimationCtrl* Create(LPD3DXANIMATIONCONTROLLER pAniCtrl);
	static CAnimationCtrl* Create(const CAnimationCtrl& rhs);
	virtual void Free();

};

END