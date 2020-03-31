#pragma once

#include "VIBuffer.h"
#include "NxTransform.h"

BEGIN(ENGINE)
class CQuadTree;
class CFrustum;
class ENGINE_DLL CBuffer_Terrain final : public CVIBuffer
{
private:
	explicit CBuffer_Terrain(LPDIRECT3DDEVICE9 pDevice);
	explicit CBuffer_Terrain(const CBuffer_Terrain& rhs);
	virtual ~CBuffer_Terrain() = default;
public:
	const _vec3*		Get_VtxPos(void) const { return m_pPos; }
	_ulong				Get_VtxCntX(void) const { return 257; }
	_ulong				Get_VtxCntZ(void) const { return 257; }
	void				Copy_Index(ENGINE::INDEX32* pIndex, const _ulong& dwTriCnt);
	_uint				Get_TriCnt(void) { return m_iNumVertices; }

	HRESULT Ready_VIBuffer(const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval);
	HRESULT Ready_VIBuffer(const _tchar* pHeightMap, const _float& fInterval);
	void Render_VIBuffer();
	_float Compute_HeightOnTerrain(const _vec3* pPosition);	
public:
	HRESULT Culling(CNxTransform* pTransform, CFrustum* pFrustum);
private:
	HRESULT		Create_NxTerrain();
private:
	CQuadTree*			m_pQuadTree = nullptr;

	_uint				m_iNumVerticesX = 0;
	_uint				m_iNumVerticesZ = 0;
	_float				m_fInterval = 0.f;
	BITMAPFILEHEADER	m_fh;
	BITMAPINFOHEADER	m_ih;
	_ulong*				m_pPixel = nullptr;
	_vec3*				m_pPos = nullptr;

private:
	POLYGON32*			m_pPolygonVertexIndex = nullptr;
	NxVec3*				m_pVertices = nullptr;
	NxU32*				m_pFaces = nullptr;
	NxVec3*				m_pNormals = nullptr;
	NxActor*			m_pTerrainActor = nullptr;
public:
	static CBuffer_Terrain* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iNumVerticesX, const _uint& iNumVerticesZ, const _float& fInterval = 1.f);
	static CBuffer_Terrain* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pHeightMap, const _float& fInterval = 1.f);
	virtual CComponent* Clone_Component(void* pArg);
protected:
	virtual void Free();


};

END