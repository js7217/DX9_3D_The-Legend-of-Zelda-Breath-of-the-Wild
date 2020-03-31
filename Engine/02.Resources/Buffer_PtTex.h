#pragma once

#include "VIBuffer.h"

BEGIN(ENGINE)

class ENGINE_DLL CBuffer_PtTex final : public CVIBuffer
{
private:
	explicit CBuffer_PtTex(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBuffer_PtTex(const CBuffer_PtTex& rhs);
	virtual ~CBuffer_PtTex() = default;
public:
	virtual HRESULT Ready_VIBuffer() { return NOERROR; };
	virtual HRESULT Ready_VIBuffer_Clone(void* Points);
	virtual HRESULT Update_Buffer(const _float& fTimeDelta);
	virtual HRESULT ResetPos();
	virtual HRESULT ResetInfo();
	virtual void Render_Buffer(const CTransform* pTransform = nullptr);
public:
	void Environment(_matrix* pMatrix) { m_pMat = pMatrix; }
private:
	LPDIRECT3DVERTEXBUFFER9			m_pVBTransform = nullptr;
	LPDIRECT3DVERTEXDECLARATION9	m_pDeclaration = nullptr;
	_uint							m_PtCnt;
	PARTICLES*						particle = nullptr;
	PARTICLEPOINTER					particlePointer;
	_bool							m_bOnce = FALSE;
	_float							m_fStartTime = 0.f;
	_matrix*						m_pMat = nullptr;
public:
	static CBuffer_PtTex* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg = nullptr);
	virtual void Free();
};

END