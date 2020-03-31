#include "VIBuffer.h"

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{

}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iStride(rhs.m_iStride)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_dwFVF(rhs.m_dwFVF)
	, m_iNumPolygons(rhs.m_iNumPolygons)
	, m_iPolygonSize(rhs.m_iPolygonSize)
	, m_eFormat(rhs.m_eFormat)	
	, m_pPosition(rhs.m_pPosition)
	, m_pIndices(rhs.m_pIndices)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Ready_VIBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	// 정점 생성
	if (FAILED(m_pDevice->CreateVertexBuffer(m_iStride * m_iNumVertices, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	m_pPosition = new _vec3[m_iNumVertices];
	ZeroMemory(m_pPosition, sizeof(_vec3) * m_iNumVertices);

	// 인덱스 버퍼 생성
	if (FAILED(m_pDevice->CreateIndexBuffer(m_iPolygonSize * m_iNumPolygons, 0, m_eFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return NOERROR;

	if (D3DFMT_INDEX16 == m_eFormat)
	{
		m_pIndices = new INDEX16[m_iNumPolygons];
		ZeroMemory(m_pIndices, sizeof(INDEX16) * m_iNumPolygons);
	}
	else
	{
		m_pIndices = new INDEX32[m_iNumPolygons];
		ZeroMemory(m_pIndices, sizeof(INDEX32) * m_iNumPolygons);
	}

	return NOERROR;
}

void CVIBuffer::Free()
{
	if (!m_isClone)
	{
		Safe_Delete_Array(m_pPosition);
		Safe_Delete_Array(m_pIndices);
	}

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	CComponent::Free();
}
