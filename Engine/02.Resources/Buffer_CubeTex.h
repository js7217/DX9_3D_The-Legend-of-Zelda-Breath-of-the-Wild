#pragma once

#include "VIBuffer.h"

BEGIN(ENGINE)

class ENGINE_DLL CBuffer_CubeTex final : public CVIBuffer
{
private:
	explicit CBuffer_CubeTex(LPDIRECT3DDEVICE9 pDevice);
	explicit CBuffer_CubeTex(const CBuffer_CubeTex& rhs);
	virtual ~CBuffer_CubeTex() = default;
public:
	HRESULT Ready_VIBuffer();
	virtual void Render_VIBuffer(const CTransform* pTransform = nullptr);
public:
	static CBuffer_CubeTex* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent* Clone_Component(void* pArg);
protected:
	virtual void Free();
};

END