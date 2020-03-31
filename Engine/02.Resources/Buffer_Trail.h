#pragma once

#include "VIBuffer.h"

BEGIN(ENGINE)

class ENGINE_DLL CBuffer_Trail final : public CVIBuffer
{
public:
	typedef struct tagCat_Mull
	{
		_vec3 vPos_0;
		_vec3 vPos_1;
		_vec3 vPos_2;
		_vec3 vPos_3;
	}CATMULLROM;
private:
	explicit CBuffer_Trail(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBuffer_Trail(const CBuffer_Trail& rhs);
	virtual ~CBuffer_Trail() = default;
public:
	HRESULT Ready_VIBuffer();
	HRESULT Ready_VIBuffer(_int iSize);
	void Render_Buffer();
public:
	void Update_Buffer_Trail(list<_vec3>* pUnderList, list<_vec3>* pUpList);
	void Update_Cat_Mull_Rom(list<_vec3>* pUnderList, list<_vec3>* pUpList);
	void Calculate_Cat_Mull_Pos();
public:
	static CBuffer_Trail* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
public:
	virtual void Free();
private:
	_vec3*					m_vUnderPos = nullptr;
	_vec3*					m_vUpPos = nullptr;
	vector<CATMULLROM>		m_vecCatMul_Under;
	vector<CATMULLROM>		m_vecCatMul_Up;
	_bool					m_bOnce = false;
};

END