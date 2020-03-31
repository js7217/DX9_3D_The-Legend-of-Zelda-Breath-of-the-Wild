#pragma once

#include "Base.h"

class CLight :	public CBase
{
private:
	explicit CLight(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CLight() = default;

public:
	void	Set_Activate(_bool bAct) { m_bIsActivate = bAct; }
	_bool	Get_Activate() { return m_bIsActivate; }

public:
	HRESULT Ready_Light(const D3DLIGHT9& LightInfo);
	HRESULT Render_Light(LPD3DXEFFECT pEffect);

private:
	LPDIRECT3DDEVICE9	m_pDevice;
	D3DLIGHT9			m_tLightInfo;
	_uint				m_iIndex = 0;
	_bool				m_bIsActivate = true;

private:
	LPDIRECT3DVERTEXBUFFER9	m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9	m_pIB = nullptr;

public:
	const D3DLIGHT9* Get_LightInfo(void) const { return &m_tLightInfo; }

public:
	static CLight* Create(LPDIRECT3DDEVICE9 pDevice, const D3DLIGHT9 & LightInfo);
	virtual void Free();

};

