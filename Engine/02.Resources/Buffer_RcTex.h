#pragma once

#include "VIBuffer.h"

BEGIN(ENGINE)

class ENGINE_DLL CBuffer_RcTex final : public CVIBuffer
{
private:
	explicit CBuffer_RcTex(LPDIRECT3DDEVICE9 pDevice);
	explicit CBuffer_RcTex(const CBuffer_RcTex& rhs);
	virtual ~CBuffer_RcTex() = default;
public:
	LPDIRECT3DVERTEXBUFFER9 Get_VB() { return m_pVB; }
public:
	HRESULT Ready_VIBuffer();
	virtual HRESULT Ready_VIBuffer(_uint iNumInstance);
	void Render_VIBuffer();
	virtual void Render_Buffer(_uint iNumInstance, const CTransform* pTransform = nullptr);
private:
	LPDIRECT3DVERTEXBUFFER9			m_pVBTransform = nullptr;
	LPDIRECT3DVERTEXDECLARATION9	m_pDeclaration = nullptr;
public:
	static CBuffer_RcTex* Create(LPDIRECT3DDEVICE9 pDevice);
	static CBuffer_RcTex* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iNumInstance);
	virtual CComponent* Clone_Component(void* pArg);
protected:
	virtual void Free();


};

END