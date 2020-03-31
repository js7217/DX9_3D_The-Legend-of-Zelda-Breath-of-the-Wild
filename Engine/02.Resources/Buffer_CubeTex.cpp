#include "Buffer_CubeTex.h"
#include "Transform.h"

CBuffer_CubeTex::CBuffer_CubeTex(LPDIRECT3DDEVICE9 pDevice)
	: CVIBuffer(pDevice)
{
}

CBuffer_CubeTex::CBuffer_CubeTex(const CBuffer_CubeTex & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CBuffer_CubeTex::Ready_VIBuffer()
{
	// For.Vertex Buffer
	m_iStride = sizeof(VTXCUBE);
	m_iNumVertices = 8;
	m_dwFVF = FVF_CUBE;

	// For.Index Buffer
	m_iPolygonSize = sizeof(POLYGON16);
	m_iNumPolygons = 12;
	m_eFormat = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_VIBuffer()))
		return E_FAIL;

	// 할당된 공간에 값을 채운다.
	VTXCUBE*			pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _vec3(-0.5f, 0.5f, -0.5f);
	m_pPosition[0] = pVertices[0].vPosition;
	D3DXVec3Normalize(&pVertices[0].vTexUV, &pVertices[0].vPosition);

	pVertices[1].vPosition = _vec3(0.5f, 0.5f, -0.5f);
	m_pPosition[1] = pVertices[1].vPosition;
	D3DXVec3Normalize(&pVertices[1].vTexUV, &pVertices[1].vPosition);

	pVertices[2].vPosition = _vec3(0.5f, -0.5f, -0.5f);
	m_pPosition[2] = pVertices[2].vPosition;
	D3DXVec3Normalize(&pVertices[2].vTexUV, &pVertices[2].vPosition);

	pVertices[3].vPosition = _vec3(-0.5f, -0.5f, -0.5f);
	m_pPosition[3] = pVertices[3].vPosition;
	D3DXVec3Normalize(&pVertices[3].vTexUV, &pVertices[3].vPosition);


	pVertices[4].vPosition = _vec3(-0.5f, 0.5f, 0.5f);
	m_pPosition[4] = pVertices[4].vPosition;
	D3DXVec3Normalize(&pVertices[4].vTexUV, &pVertices[4].vPosition);

	pVertices[5].vPosition = _vec3(0.5f, 0.5f, 0.5f);
	m_pPosition[5] = pVertices[5].vPosition;
	D3DXVec3Normalize(&pVertices[5].vTexUV, &pVertices[5].vPosition);

	pVertices[6].vPosition = _vec3(0.5f, -0.5f, 0.5f);
	m_pPosition[6] = pVertices[6].vPosition;
	D3DXVec3Normalize(&pVertices[6].vTexUV, &pVertices[6].vPosition);

	pVertices[7].vPosition = _vec3(-0.5f, -0.5f, 0.5f);
	m_pPosition[7] = pVertices[7].vPosition;
	D3DXVec3Normalize(&pVertices[7].vTexUV, &pVertices[7].vPosition);
	m_pVB->Unlock();

	m_pVB->Unlock();

	POLYGON16*		pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	// +X
	pIndices[0]._0 = 1;  pIndices[0]._1 = 5;  pIndices[0]._2 = 6;
	pIndices[1]._0 = 1;  pIndices[1]._1 = 6;  pIndices[1]._2 = 2;

	// -X
	pIndices[2]._0 = 4;  pIndices[2]._1 = 0;  pIndices[2]._2 = 3;
	pIndices[3]._0 = 4;  pIndices[3]._1 = 3;  pIndices[3]._2 = 7;

	// +Y
	pIndices[4]._0 = 4;  pIndices[4]._1 = 5;  pIndices[4]._2 = 1;
	pIndices[5]._0 = 4;  pIndices[5]._1 = 1;  pIndices[5]._2 = 0;

	// -Y
	pIndices[6]._0 = 3;  pIndices[6]._1 = 2;  pIndices[6]._2 = 6;
	pIndices[7]._0 = 3;  pIndices[7]._1 = 6;  pIndices[7]._2 = 7;

	// +Z
	pIndices[8]._0 = 5;  pIndices[8]._1 = 4;  pIndices[8]._2 = 7;
	pIndices[9]._0 = 5;  pIndices[9]._1 = 7;  pIndices[9]._2 = 6;

	// -Z
	pIndices[10]._0 = 0;  pIndices[10]._1 = 1;  pIndices[10]._2 = 2;
	pIndices[11]._0 = 0;  pIndices[11]._1 = 2;  pIndices[11]._2 = 3;


	m_pIB->Unlock();

	return NOERROR;
}

void CBuffer_CubeTex::Render_VIBuffer(const CTransform* pTransform)
{

	if (nullptr != pTransform)
	{
		_matrix		matTransform = *pTransform->Get_WorldMatrixPointer();

		// 정점의 위치에 행렬을 곱해서 정점의 위치를 변환하자.
		VTXCUBE*		pVertices = nullptr;

		m_pVB->Lock(0, 0, (void**)&pVertices, 0);

		for (size_t i = 0; i < m_iNumVertices; i++)
		{
			D3DXVec3TransformCoord(&pVertices[i].vPosition, &m_pPosition[i], &matTransform);
		}

		m_pVB->Unlock();
	}

	m_pDevice->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pDevice->SetFVF(m_dwFVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumPolygons);

}

CBuffer_CubeTex * CBuffer_CubeTex::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CBuffer_CubeTex*		pInstance = new CBuffer_CubeTex(pDevice);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MSG_BOX("CBuffer_CubeTex Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CBuffer_CubeTex::Clone_Component(void* pArg)
{
	return new CBuffer_CubeTex(*this);
}

void CBuffer_CubeTex::Free()
{
	
	CVIBuffer::Free();
}
