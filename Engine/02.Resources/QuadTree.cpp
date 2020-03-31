#include "QuadTree.h"


CQuadTree::CQuadTree(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	ZeroMemory(m_pChild, sizeof(CQuadTree*) * CHILD_END);
	ZeroMemory(m_pNeighbor, sizeof(CQuadTree*) * NEIGHBOR_END);

	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CQuadTree::Ready_QuadTree(const _vec3* pPositions, const _uint& iNumVerticesX, const _uint& iNumVerticesZ)
{
	m_pPositions = pPositions;

	m_iCornerIdx[CORNER_LT] = iNumVerticesX * iNumVerticesZ - iNumVerticesX;
	m_iCornerIdx[CORNER_RT] = iNumVerticesX * iNumVerticesZ - 1;
	m_iCornerIdx[CORNER_RB] = iNumVerticesX - 1;
	m_iCornerIdx[CORNER_LB] = 0;

	for (size_t i = 0; i < CORNER_END; ++i)
		m_iCenterIdx += m_iCornerIdx[i];
	m_iCenterIdx /= 4;

	_vec3		vDir = m_pPositions[m_iCornerIdx[CORNER_LT]] - m_pPositions[m_iCenterIdx];

	m_fRadius = D3DXVec3Length(&vDir);

	if (FAILED(Make_Child()))
		return E_FAIL;

	if (FAILED(Make_Neighbor()))
		return E_FAIL;
	
	return NOERROR;
}

HRESULT CQuadTree::Ready_QuadTree(const _vec3 * pPositions, _uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_pPositions = pPositions;

	m_iCornerIdx[CORNER_LT] = iLT;
	m_iCornerIdx[CORNER_RT] = iRT;
	m_iCornerIdx[CORNER_RB] = iRB;
	m_iCornerIdx[CORNER_LB] = iLB;

	for (size_t i = 0; i < CORNER_END; ++i)
		m_iCenterIdx += m_iCornerIdx[i];
	m_iCenterIdx >>= 2;

	_vec3		vDir = m_pPositions[m_iCornerIdx[CORNER_LT]] - m_pPositions[m_iCenterIdx];

	m_fRadius = D3DXVec3Length(&vDir);

	if (FAILED(Make_Child()))
		return E_FAIL;



	return NOERROR;
}

HRESULT CQuadTree::Culling(CFrustum * pFrustum, CNxTransform* pTransform, INDEX32* pIndices, _uint& iNumPolygon)
{
	// 가장 작은 사각형일때(자식없다)
	// if (1 == m_iCornerIdx[CORNER_RT] - m_iCornerIdx[CORNER_LT])

	// 그려야할 조건을 만족한다면.
	if (true == LevelofDetail())
	{
		_bool		isDraw[4] = { true, true, true, true };

		if(nullptr != m_pNeighbor[NEIGHBOR_LEFT])
			isDraw[0] = m_pNeighbor[NEIGHBOR_LEFT]->LevelofDetail();
		if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
			isDraw[1] = m_pNeighbor[NEIGHBOR_TOP]->LevelofDetail();
		if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
			isDraw[2] = m_pNeighbor[NEIGHBOR_RIGHT]->LevelofDetail();
		if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
			isDraw[3] = m_pNeighbor[NEIGHBOR_BOTTOM]->LevelofDetail();

		// 절두체 안에 각각삼각형(좌하, 우상)의 세점이 절두체 안에 있으면 그린다.
		_bool		isIn[4];

		isIn[0] = pFrustum->LocalPt_InFrustum(&m_pPositions[m_iCornerIdx[CORNER_LT]], pTransform, 0.0f);
		isIn[1] = pFrustum->LocalPt_InFrustum(&m_pPositions[m_iCornerIdx[CORNER_RT]], pTransform, 0.0f);
		isIn[2] = pFrustum->LocalPt_InFrustum(&m_pPositions[m_iCornerIdx[CORNER_RB]], pTransform, 0.0f);
		isIn[3] = pFrustum->LocalPt_InFrustum(&m_pPositions[m_iCornerIdx[CORNER_LB]], pTransform, 0.0f);


		if (true == isDraw[0] &&
			true == isDraw[1] &&
			true == isDraw[2] &&
			true == isDraw[3])
		{
			// 현재 쿼드트리의 오른쪽 위삼각혀을ㅇ 그려?
			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				pIndices[iNumPolygon]._0 = m_iCornerIdx[CORNER_LT];
				pIndices[iNumPolygon]._1 = m_iCornerIdx[CORNER_RT];
				pIndices[iNumPolygon]._2 = m_iCornerIdx[CORNER_RB];
				++iNumPolygon;
			}

			// 현재 쿼드트리의 왼쪽 아래삼각혀을ㅇ 그려?
			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				pIndices[iNumPolygon]._0 = m_iCornerIdx[CORNER_LT];
				pIndices[iNumPolygon]._1 = m_iCornerIdx[CORNER_RB];
				pIndices[iNumPolygon]._2 = m_iCornerIdx[CORNER_LB];
				++iNumPolygon;
			}
			return NOERROR;
		}

		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCornerIdx[CORNER_LT] + m_iCornerIdx[CORNER_LB]) >> 1;
		iTC = (m_iCornerIdx[CORNER_LT] + m_iCornerIdx[CORNER_RT]) >> 1;
		iRC = (m_iCornerIdx[CORNER_RT] + m_iCornerIdx[CORNER_RB]) >> 1;
		iBC = (m_iCornerIdx[CORNER_LB] + m_iCornerIdx[CORNER_RB]) >> 1;

		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{
			if (false == isDraw[NEIGHBOR_LEFT])
			{
				pIndices[iNumPolygon]._0 = m_iCornerIdx[CORNER_LT];
				pIndices[iNumPolygon]._1 = m_iCenterIdx;
				pIndices[iNumPolygon]._2 = iLC;
				++iNumPolygon;

				pIndices[iNumPolygon]._0 = iLC;
				pIndices[iNumPolygon]._1 = m_iCenterIdx;
				pIndices[iNumPolygon]._2 = m_iCornerIdx[CORNER_LB];
				++iNumPolygon;
			}

			else
			{

				pIndices[iNumPolygon]._0 = m_iCornerIdx[CORNER_LT];
				pIndices[iNumPolygon]._1 = m_iCenterIdx;
				pIndices[iNumPolygon]._2 = m_iCornerIdx[CORNER_LB];
				++iNumPolygon;
			}

			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				pIndices[iNumPolygon]._0 = m_iCornerIdx[CORNER_LB];
				pIndices[iNumPolygon]._1 = m_iCenterIdx;
				pIndices[iNumPolygon]._2 = iBC;
				++iNumPolygon;

				pIndices[iNumPolygon]._0 = iBC;
				pIndices[iNumPolygon]._1 = m_iCenterIdx;
				pIndices[iNumPolygon]._2 = m_iCornerIdx[CORNER_RB];
				++iNumPolygon;
			}
			else
			{
				pIndices[iNumPolygon]._0 = m_iCornerIdx[CORNER_LB];
				pIndices[iNumPolygon]._1 = m_iCenterIdx;
				pIndices[iNumPolygon]._2 = m_iCornerIdx[CORNER_RB];
				++iNumPolygon;
			}
		}

		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			if (false == isDraw[NEIGHBOR_TOP])
			{
				pIndices[iNumPolygon]._0 = m_iCornerIdx[CORNER_LT];
				pIndices[iNumPolygon]._1 = iTC;
				pIndices[iNumPolygon]._2 = m_iCenterIdx;
				++iNumPolygon;

				pIndices[iNumPolygon]._0 = m_iCenterIdx;
				pIndices[iNumPolygon]._1 = iTC;
				pIndices[iNumPolygon]._2 = m_iCornerIdx[CORNER_RT];
				++iNumPolygon;
			}
			else
			{
				pIndices[iNumPolygon]._0 = m_iCornerIdx[CORNER_LT];
				pIndices[iNumPolygon]._1 = m_iCornerIdx[CORNER_RT];
				pIndices[iNumPolygon]._2 = m_iCenterIdx;
				++iNumPolygon;
			}

			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				pIndices[iNumPolygon]._0 = m_iCornerIdx[CORNER_RT];
				pIndices[iNumPolygon]._1 = iRC;
				pIndices[iNumPolygon]._2 = m_iCenterIdx;
				++iNumPolygon;

				pIndices[iNumPolygon]._0 = m_iCenterIdx;
				pIndices[iNumPolygon]._1 = iRC;
				pIndices[iNumPolygon]._2 = m_iCornerIdx[CORNER_RB];
				++iNumPolygon;
			}
			else
			{
				pIndices[iNumPolygon]._0 = m_iCornerIdx[CORNER_RT];
				pIndices[iNumPolygon]._1 = m_iCornerIdx[CORNER_RB];
				pIndices[iNumPolygon]._2 = m_iCenterIdx;
				++iNumPolygon;
			}
		}

		return NOERROR;
	}

	// 충돌을 비교하고. 충돌했다라면. 
	if (true == pFrustum->LocalPt_InFrustum(&m_pPositions[m_iCenterIdx], pTransform, m_fRadius))
	{ 
		for (size_t i = 0; i < CHILD_END; ++i)
		{
			if (nullptr != m_pChild[i])
				m_pChild[i]->Culling(pFrustum, pTransform, pIndices, iNumPolygon);
		}	
	}

	return NOERROR;
}

HRESULT CQuadTree::Make_Child()
{
	if (1 == m_iCornerIdx[CORNER_RT] - m_iCornerIdx[CORNER_LT])
		return NOERROR;	

	_uint	iLC, iTC, iRC, iBC;

	iLC = (m_iCornerIdx[CORNER_LT] + m_iCornerIdx[CORNER_LB]) >> 1;
	iTC = (m_iCornerIdx[CORNER_RT] + m_iCornerIdx[CORNER_LT]) >> 1;
	iRC = (m_iCornerIdx[CORNER_RT] + m_iCornerIdx[CORNER_RB]) >> 1;
	iBC = (m_iCornerIdx[CORNER_RB] + m_iCornerIdx[CORNER_LB]) >> 1;
		
	m_pChild[CHILD_LT] = CQuadTree::Create(m_pGraphic_Device, m_pPositions, m_iCornerIdx[CORNER_LT], iTC, m_iCenterIdx, iLC);
	m_pChild[CHILD_RT] = CQuadTree::Create(m_pGraphic_Device, m_pPositions, iTC, m_iCornerIdx[CORNER_RT], iRC, m_iCenterIdx);
	m_pChild[CHILD_RB] = CQuadTree::Create(m_pGraphic_Device, m_pPositions, m_iCenterIdx, iRC, m_iCornerIdx[CORNER_RB], iBC);
	m_pChild[CHILD_LB] = CQuadTree::Create(m_pGraphic_Device, m_pPositions, iLC, m_iCenterIdx, iBC, m_iCornerIdx[CORNER_LB]);

	return NOERROR;
}

HRESULT CQuadTree::Make_Neighbor()
{
	if (1 == m_iCornerIdx[CORNER_RT] - m_iCornerIdx[CORNER_LT])
		return NOERROR;

	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RT];
	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_LB];

	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LT];
	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_RB];

	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_RT];
	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LB];

	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_LT];
	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RB];

	if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RT];
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_LB];
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
	{
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_RT];
	}

	for (size_t i = 0; i < CHILD_END; ++i)
	{
		if (nullptr != m_pChild[i])
			m_pChild[i]->Make_Neighbor();
	}

	return NOERROR;
}

_bool CQuadTree::LevelofDetail()
{
	if (1 == m_iCornerIdx[CORNER_RT] - m_iCornerIdx[CORNER_LT])
		return true;


	_matrix			matView;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3			vCamPosition;
	memcpy(&vCamPosition, &matView.m[3][0], sizeof(_vec3));

	_float		fDistance = D3DXVec3Length(&(vCamPosition - m_pPositions[m_iCenterIdx]));

	_float		fWidth = D3DXVec3Length(&(m_pPositions[m_iCornerIdx[CORNER_RT]] - m_pPositions[m_iCornerIdx[CORNER_LT]]));

	return fDistance * 0.05f > fWidth;
}

CQuadTree * CQuadTree::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _vec3* pPositions, const _uint& iNumVerticesX, const _uint& iNumVerticesZ)
{
	CQuadTree* pInstance = new CQuadTree(pGraphic_Device);

	if (FAILED(pInstance->Ready_QuadTree(pPositions, iNumVerticesX, iNumVerticesZ)))
	{
		MSG_BOX("CQuadTree Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CQuadTree * CQuadTree::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _vec3 * pPositions, _uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree* pInstance = new CQuadTree(pGraphic_Device);

	if (FAILED(pInstance->Ready_QuadTree(pPositions, iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("CQuadTree Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CQuadTree::Free()
{
	for (size_t i = 0; i < CHILD_END; ++i)
		Safe_Release(m_pChild[i]);

	Safe_Release(m_pGraphic_Device);
}
