#pragma once

// ���� ������ ���� �迭���� ������ Ŭ�������� �θ�.
// �������۸� ������ ��ü���� �θ��.

#include "Component.h"

BEGIN(ENGINE)

class CTransform;
class CPicking;
class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pDevice);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	HRESULT Ready_VIBuffer();	
	virtual _bool Picking_ToBuffer(_vec3* pOut, const CTransform* pTransformCom, const CPicking* pPickingCom) { return true; }

protected:
	LPDIRECT3DVERTEXBUFFER9			m_pVB = nullptr;
	_uint							m_iStride = 0; // ���� �ϳ��� ũ��.
	_uint							m_iNumVertices; // �������� ����.
	_ulong							m_dwFVF = 0; // ���� ����� �����ϴ� fvf����ü.
	_vec3*							m_pPosition = nullptr;

protected:
	LPDIRECT3DINDEXBUFFER9			m_pIB = nullptr;
	_uint							m_iPolygonSize = 0;
	_uint							m_iNumPolygons = 0;
	D3DFORMAT						m_eFormat = D3DFORMAT(0);
	void*							m_pIndices = nullptr;

public:
	virtual CComponent* Clone_Component(void* pArg) = 0;
	virtual void Free();
};

END