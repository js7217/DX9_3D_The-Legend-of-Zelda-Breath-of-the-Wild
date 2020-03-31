#include "NaviMesh.h"

//#define fY 0.25f

CNaviMesh::CNaviMesh(LPDIRECT3DDEVICE9 pDevice)
	:	CComponent(pDevice),
	m_dwCurrentIdx(0)
{
}

CNaviMesh::CNaviMesh(const CNaviMesh & rhs)
	: CComponent(rhs),
	m_dwCurrentIdx(rhs.m_dwCurrentIdx),
	m_vecCell(rhs.m_vecCell)
{
	for (auto& iter : m_vecCell)
		iter->AddRef();

}

HRESULT CNaviMesh::Ready_Component_Prototype(void* pArg)
{

	return S_OK;
}

HRESULT CNaviMesh::Ready_NaviMesh(wstring strPath, wstring strName)
{

	//Load_NaviMesh(32);		//Test
	//LoadforLineDat(strPath, strName);
	
	FAILED_CHECK_RETURN(Link_Cell(), E_FAIL);

	return S_OK;
}

void CNaviMesh::Render_NaviMesh()
{
	for (auto& iter : m_vecCell)
		iter->Render_Cell();
}

_bool CNaviMesh::Check_OnNaviMesh(const _vec3 * pTargetPos, const _vec3 * pTargetDir)
{
	_vec3		vEndPos = *pTargetPos + *pTargetDir;
	_vec3		vNormal = {};
	if (CCell::COMPARE_MOVE == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vNormal))
	{
		//벽이 아닐경우 네비매쉬의 높이값 추가
		return TRUE;
	}

	else if (CCell::COMPARE_STOP == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vNormal))
	{
		//벽일 경우 현재위치 리턴 -> 슬라이딩 벡터로 변환
		return FALSE;
	}

	return FALSE;
}

_vec3 CNaviMesh::MoveOn_NaviMesh(const _vec3 * pTargetPos, const _vec3 * pTargetDir)
{
	_vec3		vEndPos = *pTargetPos + *pTargetDir;
	_vec3 vNormal = {};
	if (CCell::COMPARE_MOVE == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vNormal))
	{
		//벽이 아닐경우 네비매쉬의 높이값 추가
		vEndPos.y = Compute_OnTerrain(pTargetPos, &m_dwCurrentIdx);
		return vEndPos;
	}

	else if (CCell::COMPARE_STOP == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vNormal))
	{
		//벽일 경우 현재위치 리턴 -> 슬라이딩 벡터로 변환
		_vec3 SlidVector = *pTargetDir - D3DXVec3Dot(pTargetDir, &vNormal) * vNormal;
		return *pTargetPos + SlidVector;
	}

	return _vec3(0.f, 0.f, 0.f);
}

_vec3 CNaviMesh::MoveOn_NaviMesh_Jump(const _vec3 * pTargetPos, const _vec3 * pTargetDir, const _float * fJump)
{
	_vec3		vEndPos = *pTargetPos + *pTargetDir;
	_vec3 vNormal = {};
	if (CCell::COMPARE_MOVE == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vNormal))
	{
		//벽이 아닐경우 네비매쉬의 높이값 추가
		vEndPos.y = Compute_OnTerrain(pTargetPos, &m_dwCurrentIdx);
		//vEndPos.y += 0.1f;
		vEndPos.y += *fJump;
		return vEndPos;
	}

	else if (CCell::COMPARE_STOP == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vNormal))
	{
		//벽일 경우 현재위치 리턴 -> 슬라이딩 벡터로 변환
		_vec3 SlidVector = *pTargetDir - D3DXVec3Dot(pTargetDir, &vNormal) * vNormal;
		return *pTargetPos + SlidVector;
	}

	return _vec3(0.f, 0.f, 0.f);
}

_vec3 CNaviMesh::MoveOn_NaviMesh_Dir(const _vec3 * pTargetPos, const _vec3 * pTargetDir, _float* fY)
{
	_vec3		vEndPos = *pTargetPos + *pTargetDir;
	_vec3 vNormal;

	if (CCell::COMPARE_MOVE == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vNormal))
	{
		//벽이 아닐경우 네비매쉬의 높이값 추가
		_vec3 vReturn = *pTargetDir;
		*fY = Compute_OnTerrain(pTargetPos, &m_dwCurrentIdx);

		return *pTargetDir;
	}

	else if (CCell::COMPARE_STOP == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vNormal))
	{
		//벽일 경우 현재위치 리턴 -> 슬라이딩 벡터로 변환
		_vec3 SlidVector = *pTargetDir - D3DXVec3Dot(pTargetDir, &vNormal) * vNormal;
		/// 방향 - 내적(방향, 부딫힌 선분의 법선 벡터) * 법선 벡터

		//_vec3 vReturn = -*pTargetDir;
		return SlidVector;
	}

	return _vec3(0.f, 0.f, 0.f);
}

_float CNaviMesh::MoveOn_Terrain(const _vec3 * pTargetPos, const _vec3 * pTargetDir)
{
	_vec3		vEndPos = *pTargetPos + *pTargetDir;
	_vec3 vNormal;

	if (CCell::COMPARE_MOVE == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vNormal))
	{
		vEndPos.y = Compute_OnTerrain(pTargetPos, &m_dwCurrentIdx);
		return vEndPos.y;
	}

	else if (CCell::COMPARE_STOP == m_vecCell[m_dwCurrentIdx]->Compare(&vEndPos, &m_dwCurrentIdx, &vNormal))
	{
		vEndPos.y = Compute_OnTerrain(pTargetPos, &m_dwCurrentIdx);
		return vEndPos.y;
	}

	return 0.f;
}



HRESULT CNaviMesh::Load_NaviMesh(_uint iMax)
{
	m_vecCell.reserve(iMax);

	CCell* pCell = nullptr;
	_uint Iterv = 8;
	_float fDown = 0.25f;
	_float fUp = 1.25f;
	
	for (_uint Z = 0; Z < iMax; ++Z)
	{
		for (_uint X = 0; X < iMax; ++X)
		{

			//pCell = CCell::Create(m_pDevice, m_vecCell.size(), &_vec3(X * 2.f, Z +1, (Z + 1) * 2.f), &_vec3((X + 1) * 2.f, Z, Z * 2.f), &_vec3(X * 2.f, Z, Z * 2.f));
			//NULL_CHECK_RETURN(pCell, E_FAIL);
			//m_vecCell.emplace_back(pCell);

			//pCell = CCell::Create(m_pDevice, m_vecCell.size(), &_vec3(X * 2.f, Z +1, (Z + 1) * 2.f), &_vec3((X + 1) * 2.f, Z + 1, (Z +1) * 2.f), &_vec3((X+1) * 2.f, Z, Z * 2.f));
			//NULL_CHECK_RETURN(pCell, E_FAIL);
			//m_vecCell.emplace_back(pCell);

			//가장 안쪽
			if (X > 14 && X < 18 && Z > 14 && Z < 18)
			{
				continue;
			}
			if (X > 11 && X < 21 && Z > 11 && Z < 21)
			{
				fUp = 3.5f;
				pCell = CCell::Create(m_pDevice, m_vecCell.size(),
					&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv), &_vec3(X * Iterv, fUp, Z * Iterv));
				NULL_CHECK_RETURN(pCell, E_FAIL);
				m_vecCell.emplace_back(pCell);

				pCell = CCell::Create(m_pDevice, m_vecCell.size(),
					&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv));
				NULL_CHECK_RETURN(pCell, E_FAIL);
				m_vecCell.emplace_back(pCell);
			}
			else if (X > 10 && X < 22 && Z > 10 && Z < 22)
			{
				fDown = 2.f;
				fUp = 3.5f;
				//////////////////////////////////////////////////////////////////
				if (X == 11 && Z == 11)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv), &_vec3(X * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}
				else if (X == 11 && Z == 21)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv), &_vec3(X * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}
				else if (X == 11)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv), &_vec3(X * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}

				if (Z == 11 && X == 11)
				{
					continue;
				}
				else if (Z == 11 && X == 21)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv), &_vec3(X * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}
				else if (Z == 11)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv), &_vec3(X * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}

				if (X == 21 && Z == 11)
				{
					continue;
				}
				else if (X == 21 && Z == 21)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv), &_vec3(X * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}
				else if (X == 21)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv), &_vec3(X * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}

				if (Z == 21 && X == 11)
				{
					continue;
				}
				else if (Z == 21 && X == 21)
				{
					continue;
				}
				else if (Z == 21)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv), &_vec3(X * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}
				/////////////////////////////////////////////////////////////////////
			}
			else if (X > 9 && X < 23 && Z > 9 && Z < 23)
			{
				//////////////////////////////////////////////////////////////////
				fDown = 0.25f;
				fUp = 2.f;

				if (X == 10 && Z == 10)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv), &_vec3(X * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}
				else if (X == 10 && Z == 22)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv), &_vec3(X * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}
				else if(X == 10)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv), &_vec3(X * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}

				if (Z == 10 && X == 10)
				{
					continue;
				}
				else if (Z == 10 && X == 22)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv), &_vec3(X * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}
				else if (Z == 10)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv), &_vec3(X * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}

				if (X == 22 && Z == 10)
				{
					continue;
				}
				else if (X == 22 && Z == 22)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv), &_vec3(X * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}
				else if (X == 22)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv), &_vec3(X * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fUp, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}

				if (Z == 22 && X == 10)
				{
					continue;
				}
				else if (Z == 22 && X == 22)
				{
					continue;
				}
				else if (Z == 22)
				{

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv), &_vec3(X * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);

					pCell = CCell::Create(m_pDevice, m_vecCell.size(),
						&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fUp, Z * Iterv));
					NULL_CHECK_RETURN(pCell, E_FAIL);
					m_vecCell.emplace_back(pCell);
				}				
				/////////////////////////////////////////////////////////////////////
			}
			else
			{
				fDown = 0.25f;

				pCell = CCell::Create(m_pDevice, m_vecCell.size(),
					&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv), &_vec3(X * Iterv, fDown, Z * Iterv));
				NULL_CHECK_RETURN(pCell, E_FAIL);
				m_vecCell.emplace_back(pCell);

				pCell = CCell::Create(m_pDevice, m_vecCell.size(),
					&_vec3(X * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, (Z + 1) * Iterv), &_vec3((X + 1) * Iterv, fDown, Z * Iterv));
				NULL_CHECK_RETURN(pCell, E_FAIL);
				m_vecCell.emplace_back(pCell);

			}
		
		}
	}

	return S_OK;
}

HRESULT CNaviMesh::LoadforLineDat(wstring strPath, wstring strName)
{
	wstring FullPath = strPath + strName;

	HANDLE hFile = CreateFile(FullPath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Load NaviMash Failed!");
		return E_FAIL;
	}
	
	CCell* pCell = nullptr;
	DWORD dwByte = 0;
	_int iSize = 0;
	ENGINE::NAVI tNavi;

	ReadFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);
	
	m_vecCell.reserve(iSize);

	while (true)
	{
		ReadFile(hFile, &tNavi.Index, sizeof(_ulong), &dwByte, nullptr);
		ReadFile(hFile, &tNavi.vPointA, sizeof(_vec3), &dwByte, nullptr);
		ReadFile(hFile, &tNavi.vPointB, sizeof(_vec3), &dwByte, nullptr);
		ReadFile(hFile, &tNavi.vPointC, sizeof(_vec3), &dwByte, nullptr);

		if (dwByte == 0)
			break;

		pCell = CCell::Create(m_pDevice, m_vecCell.size(), &tNavi.vPointA, &tNavi.vPointB, &tNavi.vPointC);
		NULL_CHECK_RETURN(pCell, E_FAIL);
		m_vecCell.emplace_back(pCell);

	}
	
	return S_OK;
}

HRESULT CNaviMesh::Link_Cell()
{
	for (_ulong i = 0; i < m_vecCell.size(); ++i)
	{
		for (_ulong j = 0; j < m_vecCell.size(); ++j)
		{
			if (i == j)
				continue;

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_AB) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_A),
					m_vecCell[i]->Get_Point(CCell::POINT_B),
					m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_AB, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_BC) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_B),
					m_vecCell[i]->Get_Point(CCell::POINT_C),
					m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_BC, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_CA) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_C),
					m_vecCell[i]->Get_Point(CCell::POINT_A),
					m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_CA, m_vecCell[j]);
				continue;
			}

		}
	}

	return S_OK;
}

_float CNaviMesh::Compute_OnTerrain(const _vec3 * pPos, _ulong * pCellIndex)
{
	D3DXPLANE d3Plane = {};

	D3DXPlaneFromPoints(&d3Plane,
		m_vecCell[(*pCellIndex)]->Get_Point(CCell::POINT_A),
		m_vecCell[(*pCellIndex)]->Get_Point(CCell::POINT_B),
		m_vecCell[(*pCellIndex)]->Get_Point(CCell::POINT_C));

	return (-d3Plane.a * pPos->x - d3Plane.c * pPos->z - d3Plane.d) / d3Plane.b;
}

CNaviMesh * CNaviMesh::Create(LPDIRECT3DDEVICE9 pDevice, wstring strPath, wstring strName)
{
	CNaviMesh*		pInstance = new CNaviMesh(pDevice);

	if (FAILED(pInstance->Ready_NaviMesh(strPath, strName)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CNaviMesh::Clone_Component(void * pArg)
{
	CNaviMesh*	pInstance = new CNaviMesh(*this);

	if (FAILED(pInstance->Ready_Component_Prototype(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNaviMesh::Free()
{
	CComponent::Free();

	for (auto iter : m_vecCell)
		Safe_Release(iter);

	m_vecCell.clear();
}
