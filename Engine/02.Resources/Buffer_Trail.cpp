#include "Buffer_Trail.h"

CBuffer_Trail::CBuffer_Trail(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CBuffer_Trail::CBuffer_Trail(const CBuffer_Trail & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CBuffer_Trail::Ready_VIBuffer()
{
	return NOERROR;
}

HRESULT CBuffer_Trail::Ready_VIBuffer(_int iSize)
{
	m_iNumVertices = (iSize - 2) * 10;

	m_iStride = sizeof(VTXTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;

	m_iNumPolygons = m_iNumVertices - 2;
	m_iPolygonSize = sizeof(POLYGON16);
	m_eFormat = D3DFMT_INDEX16;

	_int _isize = m_iNumVertices / 10 / 2;
	m_vUnderPos = new _vec3[_isize + 1];
	m_vUpPos = new _vec3[_isize + 1];
	m_vecCatMul_Under.resize(_isize);
	m_vecCatMul_Up.resize(_isize);

	if (FAILED(CVIBuffer::Ready_VIBuffer()))
		return E_FAIL;

	VTXTEX*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	_uint Idx = m_iNumVertices / 2 - 1;
	_float fUV = 1 / (_float)Idx;
	_uint iMul = 0;

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = _vec3(0.f, 0.f, 0.f);
		if (i % 2 == 0)
		{
			pVertices[i].vTexUV = _vec2(fUV * iMul, 0.f);
		}
		else
		{
			pVertices[i].vTexUV = _vec2(fUV * iMul, 1.f);

			iMul++;
		}
	}

	for (size_t i = 0; i < m_iNumVertices; ++i)
		m_pPosition[i] = pVertices[i].vPosition;

	m_pVB->Unlock();

	POLYGON16*		pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	for (_uint i = 0; i < m_iNumPolygons; ++i)
	{
		_uint Idx = i % 2;
		if (Idx == 0)
		{
			pIndices[i]._0 = i;
			pIndices[i]._1 = i + 1;
			pIndices[i]._2 = i + 2;
		}
		if (Idx == 1)
		{
			pIndices[i]._0 = i + 1;
			pIndices[i]._1 = i;
			pIndices[i]._2 = i + 2;
		}
	}

	m_pIB->Unlock();

	return NOERROR;
}

void CBuffer_Trail::Render_Buffer()
{
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pDevice->SetFVF(m_dwFVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumPolygons);
}

void CBuffer_Trail::Update_Buffer_Trail(list<_vec3>* pUnderList, list<_vec3>* pUpList)
{
	if (m_bOnce == false)
	{
		_uint iSize = 0;
		iSize = _int(pUnderList->size() + pUpList->size());

		if (FAILED(Ready_VIBuffer(iSize)))
			return;

		m_bOnce = true;
	}

	Update_Cat_Mull_Rom(pUnderList, pUpList);
	Calculate_Cat_Mull_Pos();
}

void CBuffer_Trail::Update_Cat_Mull_Rom(list<_vec3>* pUnderList, list<_vec3>* pUpList)
{
	_int iIndex = 0;

	for (auto& pUnder : *pUnderList)
	{
		m_vUnderPos[iIndex] = pUnder;
		iIndex++;
	}
	iIndex = 0;
	for (auto& pUp : *pUpList)
	{
		m_vUpPos[iIndex] = pUp;
		iIndex++;
	}

	_int iSize = (_int)pUnderList->size() - 1;
	iIndex = 0;

	for (_int i = 0; i < iSize; i++)
	{
		if (i == 0)
		{
			//under
			m_vecCatMul_Under[0].vPos_0 = m_vUnderPos[0];
			m_vecCatMul_Under[0].vPos_1 = m_vUnderPos[0];
			m_vecCatMul_Under[0].vPos_2 = m_vUnderPos[1];
			m_vecCatMul_Under[0].vPos_3 = m_vUnderPos[2];

			//Up
			m_vecCatMul_Up[0].vPos_0 = m_vUpPos[0];
			m_vecCatMul_Up[0].vPos_1 = m_vUpPos[0];
			m_vecCatMul_Up[0].vPos_2 = m_vUpPos[1];
			m_vecCatMul_Up[0].vPos_3 = m_vUpPos[2];

		}
		else if (i == iSize - 1)
		{
			//under

			m_vecCatMul_Under[i].vPos_0 = m_vUnderPos[i - 1];
			m_vecCatMul_Under[i].vPos_1 = m_vUnderPos[i];
			m_vecCatMul_Under[i].vPos_2 = m_vUnderPos[i + 1];
			m_vecCatMul_Under[i].vPos_3 = m_vUnderPos[i + 1];

			//Up
			m_vecCatMul_Up[i].vPos_0 = m_vUpPos[i - 1];
			m_vecCatMul_Up[i].vPos_1 = m_vUpPos[i];
			m_vecCatMul_Up[i].vPos_2 = m_vUpPos[i + 1];
			m_vecCatMul_Up[i].vPos_3 = m_vUpPos[i + 1];
		}
		else
		{
			//under
			m_vecCatMul_Under[i].vPos_0 = m_vUnderPos[i - 1];
			m_vecCatMul_Under[i].vPos_1 = m_vUnderPos[i];
			m_vecCatMul_Under[i].vPos_2 = m_vUnderPos[i + 1];
			m_vecCatMul_Under[i].vPos_3 = m_vUnderPos[i + 2];

			//Up
			m_vecCatMul_Up[i].vPos_0 = m_vUpPos[i - 1];
			m_vecCatMul_Up[i].vPos_1 = m_vUpPos[i];
			m_vecCatMul_Up[i].vPos_2 = m_vUpPos[i + 1];
			m_vecCatMul_Up[i].vPos_3 = m_vUpPos[i + 2];
		}
	}

	iIndex = 0;
}

void CBuffer_Trail::Calculate_Cat_Mull_Pos()
{
	_int iSize = _int(m_vecCatMul_Under.size());

	VTXTEX*        pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	_int iIndex = 0;
	for (_int i = 0; i < iSize; ++i)
	{
		for (_int j = 0; j < 10; ++j)
		{
			D3DXVec3CatmullRom(&pVertices[iIndex].vPosition, &m_vecCatMul_Under[i].vPos_0, &m_vecCatMul_Under[i].vPos_1, &m_vecCatMul_Under[i].vPos_2, &m_vecCatMul_Under[i].vPos_3, 0.1f * j);
			iIndex++;
			D3DXVec3CatmullRom(&pVertices[iIndex].vPosition, &m_vecCatMul_Up[i].vPos_0, &m_vecCatMul_Up[i].vPos_1, &m_vecCatMul_Up[i].vPos_2, &m_vecCatMul_Up[i].vPos_3, 0.1f * j);
			iIndex++;
		}
		iIndex = iIndex;
	}

	for (size_t i = 0; i < m_iNumVertices; ++i)
		m_pPosition[i] = pVertices[i].vPosition;

	m_pVB->Unlock();
}

CBuffer_Trail * CBuffer_Trail::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBuffer_Trail*	pInstance = new CBuffer_Trail(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_Trail Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}


CComponent * CBuffer_Trail::Clone_Component(void* pArg)
{
	return new CBuffer_Trail(*this);
}

void CBuffer_Trail::Free()
{
	Safe_Delete_Array(m_vUnderPos);
	Safe_Delete_Array(m_vUpPos);
	Safe_Delete_Array(m_pPosition);
	Safe_Delete_Array(m_pIndices);
	m_vecCatMul_Under.clear();
	m_vecCatMul_Under.shrink_to_fit();

	m_vecCatMul_Up.clear();
	m_vecCatMul_Up.shrink_to_fit();

	CVIBuffer::Free();
}
