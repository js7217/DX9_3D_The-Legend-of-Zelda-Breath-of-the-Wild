#pragma once

#include "Base.h"
#include "Frustum.h"
#include "NxTransform.h"

// 내 지형 전체를 의미하는 사각형하나. 
// 사분할한 자식들을 보유한다.

BEGIN(ENGINE)

class CFrustum;


class CQuadTree final : public CBase
{
public:
	enum NEIGHBOR {NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END};
	enum CORNER {CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END};
	enum CHILD {CHILD_LT, CHILD_RT, CHILD_RB, CHILD_LB, CHILD_END};
private:
	explicit CQuadTree(LPDIRECT3DDEVICE9 pGraphic_Device);	
	virtual ~CQuadTree() = default;
public:
	HRESULT Ready_QuadTree(const _vec3* pPositions, const _uint& iNumVerticesX, const _uint& iNumVerticesZ);
	HRESULT Ready_QuadTree(const _vec3* pPositions, _uint iLT, _uint iRT, _uint iRB, _uint iLB);
	HRESULT Culling(CFrustum* pFrustum, CNxTransform* pTransform, INDEX32* pIndices, _uint& iNumPolygon);
private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;
	CQuadTree*				m_pChild[4];
	CQuadTree*				m_pNeighbor[NEIGHBOR_END];
	const _vec3*			m_pPositions = nullptr;
private:
	_uint					m_iCornerIdx[4];
	_uint					m_iCenterIdx = 0;
	_float					m_fRadius;
private:
	HRESULT Make_Child();
	HRESULT Make_Neighbor();
	_bool LevelofDetail();

public:
	static CQuadTree* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _vec3* pPositions, const _uint& iNumVerticesX, const _uint& iNumVerticesZ);
	static CQuadTree* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _vec3* pPositions, _uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void Free();
};

END