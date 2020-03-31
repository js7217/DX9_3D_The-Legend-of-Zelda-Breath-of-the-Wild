#include "AnimationCtrl.h"

CAnimationCtrl::CAnimationCtrl(LPD3DXANIMATIONCONTROLLER pAniCtrl)
	: m_pAniCtrl(pAniCtrl)
{
	m_pAniCtrl->AddRef();
}

CAnimationCtrl::CAnimationCtrl(const CAnimationCtrl & rhs)
{
	if (FAILED(rhs.m_pAniCtrl->CloneAnimationController(rhs.m_pAniCtrl->GetMaxNumAnimationOutputs(),
		rhs.m_pAniCtrl->GetMaxNumAnimationSets(), 
		rhs.m_pAniCtrl->GetMaxNumTracks(), 
		rhs.m_pAniCtrl->GetMaxNumEvents(), &m_pAniCtrl)))
		return;
}

HRESULT CAnimationCtrl::Ready_AnimationCtrl()
{
	return NOERROR;
}

HRESULT CAnimationCtrl::SetUp_AnimationSet(_uint iIndex)
{
	if (nullptr == m_pAniCtrl)
		return E_FAIL;

	if (m_iOldIndex == iIndex)
		return NOERROR;
	
	m_iNewTrack = (m_iCurrentTrack == 0 ? 1 : 0);

	LPD3DXANIMATIONSET		pAnimationSet = nullptr;

	if (FAILED(m_pAniCtrl->GetAnimationSet(iIndex, &pAnimationSet)))
		return E_FAIL;

	m_dPeriod = pAnimationSet->GetPeriod();

	if (FAILED(m_pAniCtrl->SetTrackAnimationSet(m_iNewTrack, pAnimationSet)))
		return E_FAIL;

	Safe_Release(pAnimationSet);

	m_pAniCtrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniCtrl->UnkeyAllTrackEvents(m_iNewTrack);

	// 기존 재생되던 트랙에 대한 설정.
	m_pAniCtrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_TimeAcc + 0.2);
	m_pAniCtrl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_TimeAcc, 0.2, D3DXTRANSITION_LINEAR);
	m_pAniCtrl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_TimeAcc, 0.2, D3DXTRANSITION_LINEAR);

	// 새롭게 재생되어야할 트랙에 대한 설정.
	m_pAniCtrl->SetTrackEnable(m_iNewTrack, TRUE);
	m_pAniCtrl->KeyTrackSpeed(m_iNewTrack, 1.f, m_TimeAcc, 0.2, D3DXTRANSITION_LINEAR);
	m_pAniCtrl->KeyTrackWeight(m_iNewTrack, 0.9f, m_TimeAcc, 0.2, D3DXTRANSITION_LINEAR);


	m_pAniCtrl->SetTrackPosition(m_iNewTrack, 0.0);
	m_TimeAcc = 0.0;

	m_pAniCtrl->ResetTime();

	m_iOldIndex = iIndex;
	m_iCurrentTrack = m_iNewTrack;

	return NOERROR;
}

HRESULT CAnimationCtrl::SetUp_AnimationSet(const char * pName)
{
	return NOERROR;
}

HRESULT CAnimationCtrl::Play_Animation(const _double& TimeDelta, _float fAdjSpeed)
{
	m_pAniCtrl->AdvanceTime(TimeDelta * (_double)fAdjSpeed, nullptr);

	m_TimeAcc += TimeDelta;

	return NOERROR;
}

_bool CAnimationCtrl::IsAnimationSetEnd(_float fInterpolatedValue)
{
	D3DXTRACK_DESC		TrackDesc;
	ZeroMemory(&TrackDesc, sizeof(D3DXTRACK_DESC));

	m_pAniCtrl->GetTrackDesc(m_iCurrentTrack, &TrackDesc);

	if (TrackDesc.Position >= m_dPeriod + fInterpolatedValue) // 보간
	{
		return true;
	}

	return false;
}

CAnimationCtrl * CAnimationCtrl::Create(LPD3DXANIMATIONCONTROLLER pAniCtrl)
{
	CAnimationCtrl* pInstance = new CAnimationCtrl(pAniCtrl);

	if (FAILED(pInstance->Ready_AnimationCtrl()))
	{
		MSG_BOX("CAnimationCtrl Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimationCtrl * CAnimationCtrl::Create(const CAnimationCtrl & rhs)
{
	CAnimationCtrl* pInstance = new CAnimationCtrl(rhs);

	if (FAILED(pInstance->Ready_AnimationCtrl()))
	{
		MSG_BOX("CAnimationCtrl Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimationCtrl::Free()
{
	Safe_Release(m_pAniCtrl);
}
