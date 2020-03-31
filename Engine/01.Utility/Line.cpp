#include "Line.h"

USING(ENGINE)

CLine::CLine()
{
}

CLine::~CLine()
{
}

_vec3 CLine::Get_Normal()
{
	return _vec3(m_vNormal.x, 0.f, m_vNormal.y);
}

HRESULT CLine::Ready_Line(const _vec2 * pPointA, const _vec2 * pPointB)
{
	m_vPoint[POINT_START] = *pPointA;
	m_vPoint[POINT_FINISH] = *pPointB;

	m_vDirection = m_vPoint[POINT_FINISH] - m_vPoint[POINT_START];
	m_vNormal = _vec2(m_vDirection.y * -1.f, m_vDirection.x);
	D3DXVec2Normalize(&m_vNormal, &m_vNormal);

	return S_OK;
}

CLine::COMPARE CLine::Compare(const _vec2 * pEndPos)
{
	_vec2 vDest = *pEndPos - m_vPoint[POINT_START];
	D3DXVec2Normalize(&vDest, &vDest);

	float fDot = D3DXVec2Dot(&vDest, &m_vNormal);

	

	if (0 <= fDot)
		return COMPARE_OUT;
	else
		return COMPARE_IN;
}

CLine * CLine::Create(const _vec2 * pPointA, const _vec2 * pPointB)
{
	CLine*		pInstance = new CLine;

	if (FAILED(pInstance->Ready_Line(pPointA, pPointB)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLine::Free(void)
{
}
