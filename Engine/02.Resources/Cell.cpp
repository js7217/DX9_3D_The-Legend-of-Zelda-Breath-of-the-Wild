#include "Cell.h"

CCell::CCell(LPDIRECT3DDEVICE9 pDevice)
	: m_pD3Line(nullptr),
	m_pGraphicDev(pDevice)
{
	ZeroMemory(m_pNeighbor, sizeof(CCell*) * NEIGHBOR_END);
	m_pGraphicDev->AddRef();
}

CCell::~CCell()
{
}

HRESULT CCell::Ready_Cell(const _ulong & dwIndex, const _vec3 * pPointA, const _vec3 * pPointB, const _vec3 * pPointC)
{
	m_dwIndex = dwIndex;

	m_vPoint[POINT_A] = *pPointA;
	m_vPoint[POINT_B] = *pPointB;
	m_vPoint[POINT_C] = *pPointC;

	m_pLine[LINE_AB] = CLine::Create(&_vec2(m_vPoint[POINT_A].x, m_vPoint[POINT_A].z),
		&_vec2(m_vPoint[POINT_B].x, m_vPoint[POINT_B].z));
	m_pLine[LINE_BC] = CLine::Create(&_vec2(m_vPoint[POINT_B].x, m_vPoint[POINT_B].z),
		&_vec2(m_vPoint[POINT_C].x, m_vPoint[POINT_C].z));
	m_pLine[LINE_CA] = CLine::Create(&_vec2(m_vPoint[POINT_C].x, m_vPoint[POINT_C].z),
		&_vec2(m_vPoint[POINT_A].x, m_vPoint[POINT_A].z));

#ifdef _DEBUG
	FAILED_CHECK_RETURN(D3DXCreateLine(m_pGraphicDev, &m_pD3Line), E_FAIL);
#endif // _DEBUG

	return S_OK;
}

_bool CCell::Compare_Point(const _vec3 * pFirstPoint, const _vec3 * pSecondPoint, CCell * pCell)
{
	if (m_vPoint[POINT_A] == *pFirstPoint)
	{
		if (m_vPoint[POINT_B] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_AB] = pCell;
			return TRUE;
		}

		if (m_vPoint[POINT_C] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_CA] = pCell;
			return TRUE;
		}

	}

	if (m_vPoint[POINT_B] == *pFirstPoint)
	{
		if (m_vPoint[POINT_A] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_AB] = pCell;
			return TRUE;
		}

		if (m_vPoint[POINT_C] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_BC] = pCell;
			return TRUE;
		}

	}

	if (m_vPoint[POINT_C] == *pFirstPoint)
	{
		if (m_vPoint[POINT_A] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_CA] = pCell;
			return TRUE;
		}

		if (m_vPoint[POINT_B] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_BC] = pCell;
			return TRUE;
		}
	}

	return FALSE;
}

CCell::COMPARE CCell::Compare(const _vec3 * pEndPos, _ulong * pCellIndex, _vec3* vNormal)
{
	for (_ulong i = 0; i < LINE_END; ++i)
	{
		if (CLine::COMPARE_OUT == m_pLine[i]->Compare(&_vec2(pEndPos->x, pEndPos->z)))
		{
			*vNormal = m_pLine[i]->Get_Normal();
			if (nullptr == m_pNeighbor[i])
				return COMPARE_STOP;

			else
			{
				*pCellIndex = *m_pNeighbor[i]->Get_Index();
				return COMPARE_MOVE;
			}
		}
	}

	return COMPARE_MOVE;
}

void CCell::Render_Cell()
{
	_vec3 vPoint[4] = { m_vPoint[POINT_A], m_vPoint[POINT_B], m_vPoint[POINT_C], m_vPoint[POINT_A] };

	_matrix matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	for (_ulong i = 0; i < 4; ++i)
	{
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matView);

		if (vPoint[i].z <= 0.1f)
			vPoint[i].z = 0.1f;
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matProj);
	}

	m_pD3Line->SetWidth(3.f);

	m_pGraphicDev->EndScene();
	m_pGraphicDev->BeginScene();

	m_pD3Line->Begin();

	_matrix matTemp;
	D3DXMatrixIdentity(&matTemp);

	m_pD3Line->DrawTransform(vPoint, 4, &matTemp, D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

	m_pD3Line->End();
}

CCell * CCell::Create(LPDIRECT3DDEVICE9 pDevice, const _ulong & dwIndex, const _vec3 * pPointA, const _vec3* pPointB, const _vec3 * pPointC)
{
	CCell* pInstance = new CCell(pDevice);

	if (FAILED(pInstance->Ready_Cell(dwIndex, pPointA, pPointB, pPointC)))
		ENGINE::Safe_Release(pInstance);

	return pInstance;
}

void CCell::Free()
{
	for (_ulong i = 0; i < LINE_END; ++i)
		Safe_Release(m_pLine[i]);

	Safe_Release(m_pD3Line);
	Safe_Release(m_pGraphicDev);
}
