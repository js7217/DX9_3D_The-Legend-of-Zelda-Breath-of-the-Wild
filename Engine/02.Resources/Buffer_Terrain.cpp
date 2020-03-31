#include "Buffer_Terrain.h"
#include "Stream.h"
#include "PhysXMgr.h"

#include "Frustum.h"
#include "QuadTree.h"


CBuffer_Terrain::CBuffer_Terrain(LPDIRECT3DDEVICE9 pDevice)
	: CVIBuffer(pDevice)
{

}

CBuffer_Terrain::CBuffer_Terrain(const CBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_pPixel(rhs.m_pPixel)
	, m_pPos(rhs.m_pPos)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_fInterval(rhs.m_fInterval)
	, m_pPolygonVertexIndex(rhs.m_pPolygonVertexIndex)
	, m_pVertices(rhs.m_pVertices)
	, m_pFaces(rhs.m_pFaces)
	, m_pNormals(rhs.m_pNormals)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	Create_NxTerrain();
}

void CBuffer_Terrain::Copy_Index(ENGINE::INDEX32 * pIndex, const _ulong & dwTriCnt)
{
	ENGINE::INDEX32*		pOriginIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pOriginIndex, 0);

	memcpy(pOriginIndex, pIndex, sizeof(INDEX32) * dwTriCnt);

	m_pIB->Unlock();

	m_iNumPolygons = dwTriCnt;
}

HRESULT CBuffer_Terrain::Ready_VIBuffer(const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval)
{
	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	m_fInterval = fInterval;

	m_iNumVertices = iNumVerticesX * iNumVerticesZ;
	m_iStride = sizeof(VTXNORTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;
	m_iNumPolygons = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2;

	m_iPolygonSize = sizeof(POLYGON32);
	m_eFormat = D3DFMT_INDEX32;
	
	if (FAILED(CVIBuffer::Ready_VIBuffer()))
		return E_FAIL;


	VTXNORTEX*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	for (_uint i = 0; i < iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < iNumVerticesX; j++)
		{
			_uint		iIndex = i * iNumVerticesX + j;

			pVertices[iIndex].vPosition = _vec3(j * m_fInterval, 0.0f, i * m_fInterval);
			pVertices[iIndex].vNormal = _vec3(0.0f, 0.0f, 0.0f);
			pVertices[iIndex].vTexUV = _vec2(j / (iNumVerticesX - 1.f) * 30.f, i / (iNumVerticesZ - 1.f) * 30.f);
		}
	}
	



	POLYGON32*		pIndices = nullptr;
	
	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	_uint			iPolygonIndex = 0;

	for (_uint i = 0; i < iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < iNumVerticesX - 1; j++)
		{
			_uint iIndex = i * iNumVerticesX + j;

			// 우.상
			pIndices[iPolygonIndex]._0 = iIndex + iNumVerticesX;
			pIndices[iPolygonIndex]._1 = iIndex + iNumVerticesX + 1;
			pIndices[iPolygonIndex]._2 = iIndex + 1;

			_vec3		vSour, vDest;
			_vec3		vNormal;

			vSour = pVertices[pIndices[iPolygonIndex]._1].vPosition - pVertices[pIndices[iPolygonIndex]._0].vPosition;
			vDest = pVertices[pIndices[iPolygonIndex]._2].vPosition - pVertices[pIndices[iPolygonIndex]._1].vPosition;

			D3DXVec3Normalize(&vNormal, D3DXVec3Cross(&vNormal, &vSour, &vDest));

			pVertices[pIndices[iPolygonIndex]._0].vNormal += vNormal;
			pVertices[pIndices[iPolygonIndex]._1].vNormal += vNormal;
			pVertices[pIndices[iPolygonIndex]._2].vNormal += vNormal;
			++iPolygonIndex;

			// 좌.하
			pIndices[iPolygonIndex]._0 = iIndex + iNumVerticesX;
			pIndices[iPolygonIndex]._1 = iIndex + 1;
			pIndices[iPolygonIndex]._2 = iIndex;

			vSour = pVertices[pIndices[iPolygonIndex]._1].vPosition - pVertices[pIndices[iPolygonIndex]._0].vPosition;
			vDest = pVertices[pIndices[iPolygonIndex]._2].vPosition - pVertices[pIndices[iPolygonIndex]._1].vPosition;

			D3DXVec3Normalize(&vNormal, D3DXVec3Cross(&vNormal, &vSour, &vDest));

			pVertices[pIndices[iPolygonIndex]._0].vNormal += vNormal;
			pVertices[pIndices[iPolygonIndex]._1].vNormal += vNormal;
			pVertices[pIndices[iPolygonIndex]._2].vNormal += vNormal;
			++iPolygonIndex;
		}
	}
	
	for (_uint i = 0; i < m_iNumVertices; ++i)
		D3DXVec3Normalize(&pVertices[i].vNormal, &pVertices[i].vNormal);

	m_pVB->Unlock();
	m_pIB->Unlock();


	return NOERROR;
}

HRESULT CBuffer_Terrain::Ready_VIBuffer(const _tchar * pHeightMap, const _float & fInterval)
{
	_uint		iNumVerticesX, iNumVerticesZ;

	HANDLE		hFile = 0;
	_ulong		dwByte = 0;

	hFile = CreateFile(pHeightMap, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	// 파일정보
	ReadFile(hFile, &m_fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);

	// 이미지정보
	ReadFile(hFile, &m_ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	iNumVerticesX = m_ih.biWidth;
	iNumVerticesZ = m_ih.biHeight;

	//m_pPosition = new _vec3[iNumVerticesX * iNumVerticesZ];

	m_pPixel = new _ulong[iNumVerticesX * iNumVerticesZ];

	// 픽셀정보
	ReadFile(hFile, m_pPixel, sizeof(_ulong) * (iNumVerticesX * iNumVerticesZ), &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	m_fInterval = fInterval;

	m_iNumVertices = iNumVerticesX * iNumVerticesZ;
	m_iStride = sizeof(VTXNORTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;
	m_iNumPolygons = (iNumVerticesX - 1) * (iNumVerticesZ - 1) * 2;

	m_iPolygonSize = sizeof(POLYGON32);
	m_eFormat = D3DFMT_INDEX32;
	m_pPos = new _vec3[m_iNumVertices];
	if (FAILED(CVIBuffer::Ready_VIBuffer()))
		return E_FAIL;

	VTXNORTEX*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);
	_uint		iHeightIdx = (iNumVerticesX) * (iNumVerticesZ);
	for (_uint i = 0; i < iNumVerticesZ; i++)
	{
		iHeightIdx = (iNumVerticesX) * (iNumVerticesZ);
		iHeightIdx -= (iNumVerticesX * (i + 1)) + 1;

		for (_uint j = 0; j < iNumVerticesX; j++)
		{
			iHeightIdx++;
			_uint		iIndex = i * iNumVerticesX + j;

			pVertices[iIndex].vPosition = _vec3(j * m_fInterval, (m_pPixel[iHeightIdx] & 0x000000ff) / 7.f, i * m_fInterval);
			m_pPos[iIndex] = pVertices[iIndex].vPosition;
			m_pPosition[iIndex] = pVertices[iIndex].vPosition;
			pVertices[iIndex].vNormal = _vec3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _vec2(j / (iNumVerticesX - 1.f), i / (iNumVerticesZ - 1.f));
		}
	}


	POLYGON32*		pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	_uint			iPolygonIndex = 0;


	for (_uint i = 0; i < iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < iNumVerticesX - 1; j++)
		{
			_uint iIndex = i * iNumVerticesX + j;

			// 우.상
			pIndices[iPolygonIndex]._0 = iIndex + iNumVerticesX;
			pIndices[iPolygonIndex]._1 = iIndex + iNumVerticesX + 1;
			pIndices[iPolygonIndex]._2 = iIndex + 1;

			_vec3		vSour, vDest;
			_vec3		vNormal;

			vSour = pVertices[pIndices[iPolygonIndex]._1].vPosition - pVertices[pIndices[iPolygonIndex]._0].vPosition;
			vDest = pVertices[pIndices[iPolygonIndex]._2].vPosition - pVertices[pIndices[iPolygonIndex]._1].vPosition;

			D3DXVec3Normalize(&vNormal, D3DXVec3Cross(&vNormal, &vSour, &vDest));

			pVertices[pIndices[iPolygonIndex]._0].vNormal += vNormal;
			pVertices[pIndices[iPolygonIndex]._1].vNormal += vNormal;
			pVertices[pIndices[iPolygonIndex]._2].vNormal += vNormal;
			++iPolygonIndex;

			// 좌.하
			pIndices[iPolygonIndex]._0 = iIndex + iNumVerticesX;
			pIndices[iPolygonIndex]._1 = iIndex + 1;
			pIndices[iPolygonIndex]._2 = iIndex;

			vSour = pVertices[pIndices[iPolygonIndex]._1].vPosition - pVertices[pIndices[iPolygonIndex]._0].vPosition;
			vDest = pVertices[pIndices[iPolygonIndex]._2].vPosition - pVertices[pIndices[iPolygonIndex]._1].vPosition;

			D3DXVec3Normalize(&vNormal, D3DXVec3Cross(&vNormal, &vSour, &vDest));

			pVertices[pIndices[iPolygonIndex]._0].vNormal += vNormal;
			pVertices[pIndices[iPolygonIndex]._1].vNormal += vNormal;
			pVertices[pIndices[iPolygonIndex]._2].vNormal += vNormal;
			++iPolygonIndex;
		}
	}

	for (size_t i = 0; i < m_iNumVertices; ++i)
		D3DXVec3Normalize(&pVertices[i].vNormal, &pVertices[i].vNormal);

	m_pVB->Unlock();

	m_pPolygonVertexIndex = new POLYGON32[m_iNumPolygons];
	memcpy(m_pPolygonVertexIndex, pIndices, sizeof(POLYGON32) * m_iNumPolygons);

	m_pIB->Unlock();

	m_pVertices = new NxVec3[m_iNumVertices];
	iHeightIdx = (iNumVerticesX) * (iNumVerticesZ);
	for (NxU32 y = 0; y < iNumVerticesZ; ++y)
	{
		iHeightIdx = (iNumVerticesX) * (iNumVerticesZ);
		iHeightIdx -= (iNumVerticesX * (y + 1)) + 1;
		for (NxU32 x = 0; x < iNumVerticesX; ++x)
		{
			iHeightIdx++;
			m_pVertices[x + y * iNumVerticesX] = NxVec3(NxF32(x) - (NxF32(iNumVerticesX - 1)*0.5f), (m_pPixel[iHeightIdx] & 0x000000ff) / 7.f, NxF32(y) - (NxF32(iNumVerticesX - 1)*0.5f));
		}
	}

	m_pFaces = new NxU32[m_iNumPolygons * 3];
	NxU32 k = 0;

	for (NxU32 j = 0; j < iNumVerticesZ - 1; j++)
	{
		for (NxU32 i = 0; i < iNumVerticesX - 1; i++)
		{
			// Create first triangle
			m_pFaces[k++] = i + j*iNumVerticesX;
			m_pFaces[k++] = i + (j + 1)*iNumVerticesX;
			m_pFaces[k++] = i + 1 + (j + 1)*iNumVerticesX;

			// Create second triangle
			m_pFaces[k++] = i + j*iNumVerticesX;
			m_pFaces[k++] = i + 1 + (j + 1)*iNumVerticesX;
			m_pFaces[k++] = i + 1 + j*iNumVerticesX;
		}
	}

	m_pNormals = new NxVec3[m_iNumVertices];
	NxBuildSmoothNormals(m_iNumPolygons, m_iNumVertices, m_pVertices, m_pFaces, NULL, m_pNormals, true);
	

	m_pQuadTree = CQuadTree::Create(m_pDevice, m_pPosition, m_iNumVerticesX, m_iNumVerticesZ);


	return NOERROR;
}

void CBuffer_Terrain::Render_VIBuffer()
{
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pDevice->SetFVF(m_dwFVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumPolygons);
}

_float CBuffer_Terrain::Compute_HeightOnTerrain(const _vec3 * pPosition)
{
	_ulong dwIndex = _ulong(pPosition->z / m_fInterval) * m_iNumVerticesX + _ulong(pPosition->x / m_fInterval);

	_float	fRatioX = (pPosition->x - m_pPosition[dwIndex + m_iNumVerticesX].x) / m_fInterval;
	_float	fRatioZ = (m_pPosition[dwIndex + m_iNumVerticesX].z - pPosition->z) / m_fInterval;

	D3DXPLANE			Plane;

	if (fRatioX > fRatioZ)
	{
		D3DXPlaneFromPoints(&Plane, &m_pPosition[dwIndex + m_iNumVerticesX], &m_pPosition[dwIndex + m_iNumVerticesX + 1], &m_pPosition[dwIndex + 1]);

		// ax + by + cz + d == 0
		// y = (-ax + -cz + -d) / b
		return ((Plane.a * pPosition->x * -1.f) + (Plane.c * pPosition->z * -1.f) + Plane.d * -1.f) / Plane.b;


	}
	else
	{
		D3DXPlaneFromPoints(&Plane, &m_pPosition[dwIndex + m_iNumVerticesX], &m_pPosition[dwIndex + 1], &m_pPosition[dwIndex]);

		return ((Plane.a * pPosition->x * -1.f) + (Plane.c * pPosition->z * -1.f) + Plane.d * -1.f) / Plane.b;
	}	
}

HRESULT CBuffer_Terrain::Culling(CNxTransform * pTransform, CFrustum * pFrustum)
{

	if (nullptr == m_pPosition)
		return E_FAIL;

	INDEX32*		pIndices = (INDEX32*)m_pIndices;

	_uint			iNumPolygons = 0;

	_matrix			matIdentity;
	D3DXMatrixIdentity(&matIdentity);

	m_pQuadTree->Culling(pFrustum->Make_LocalPlane(pTransform), pTransform, pIndices, iNumPolygons);

	INDEX32*	pOriginalIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pOriginalIndices, 0);

	memcpy(pOriginalIndices, pIndices, sizeof(INDEX32) * iNumPolygons);

	m_pIB->Unlock();

	m_iNumPolygons = iNumPolygons;

	return NOERROR;
}

HRESULT CBuffer_Terrain::Create_NxTerrain()
{
	NxTriangleMeshDesc terrainDesc;
	terrainDesc.numVertices = m_iNumVertices;
	terrainDesc.numTriangles = m_iNumPolygons;
	terrainDesc.pointStrideBytes = sizeof(NxVec3);
	terrainDesc.triangleStrideBytes = 3 * sizeof(NxU32);
	terrainDesc.points = m_pVertices;
	terrainDesc.triangles = m_pFaces;
	terrainDesc.flags = 0;

	terrainDesc.heightFieldVerticalAxis = NX_Y;
	terrainDesc.heightFieldVerticalExtent = -1000.f;

	NxInitCooking();

	MemoryWriteBuffer buf;
	_bool bStatus = NxCookTriangleMesh(terrainDesc, buf);
	if (!bStatus)
	{
		MessageBox(0, L"Terrain Cooking Failed", L"System Error", MB_OK);
		return E_FAIL;
	}

	MemoryReadBuffer readBuffer(buf.data);
	NxTriangleMesh* pTerrainMesh = GET_INSTANCE(CPhysXMgr)->Get_NxPhysicsSDK()->createTriangleMesh(readBuffer);

	NxTriangleMeshShapeDesc TerrainShapeDesc;
	TerrainShapeDesc.meshData = pTerrainMesh;

	TerrainShapeDesc.shapeFlags &= ~NX_SF_VISUALIZATION;
	TerrainShapeDesc.shapeFlags |= NX_SF_FEATURE_INDICES /*| NX_SF_DISABLE_RAYCASTING*/;

	TerrainShapeDesc.localPose.t = NxVec3((m_iNumVerticesX - 1) * 0.5f, 0.f, (m_iNumVerticesZ - 1) * 0.5f);
	TerrainShapeDesc.name = "Terrain";

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&TerrainShapeDesc);
	ActorDesc.name = "Terrain";

	m_pTerrainActor = GET_INSTANCE(CPhysXMgr)->Get_NxScene()->createActor(ActorDesc);

//	m_pTerrainActor->setName("Terrain");
	//m_pTerrainActor->getShapes()[0]->setSkinWidth(0.025f);
	
	NxCloseCooking();
	return S_OK;
}

CBuffer_Terrain * CBuffer_Terrain::Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval)
{
	CBuffer_Terrain*	pInstance = new CBuffer_Terrain(pDevice);

	if (FAILED(pInstance->Ready_VIBuffer(iNumVerticesX, iNumVerticesZ, fInterval)))
	{
		MessageBox(0, L"CBuffer_Terrain Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;

}

CBuffer_Terrain * CBuffer_Terrain::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar * pHeightMap, const _float & fInterval)
{
	CBuffer_Terrain*	pInstance = new CBuffer_Terrain(pDevice);

	if (FAILED(pInstance->Ready_VIBuffer(pHeightMap, fInterval)))
	{
		MessageBox(0, L"CBuffer_Terrain Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}


CComponent * CBuffer_Terrain::Clone_Component(void* pArg)
{
	return new CBuffer_Terrain(*this);
}

void CBuffer_Terrain::Free()
{
	if (false == m_isClone)
	{	
		NX_DELETE_ARRAY(m_pFaces);
		NX_DELETE_ARRAY(m_pNormals);
		Safe_Delete_Array(m_pPixel);
		Safe_Delete_Array(m_pPolygonVertexIndex);
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pPos);
		Safe_Release(m_pQuadTree);
		GET_INSTANCE(CPhysXMgr)->Get_NxScene()->releaseActor(*m_pTerrainActor);
	}

	CVIBuffer::Free();
}
