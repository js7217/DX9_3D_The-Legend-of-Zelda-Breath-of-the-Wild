#pragma once

#include "Base.h"

BEGIN(ENGINE)

class CTarget final : public CBase
{
private:
	explicit CTarget(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CTarget() = default;
public:	
	HRESULT Ready_Target(_uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor);
	HRESULT Clear_Target(_bool bClear = false);
	HRESULT SetUp_OnGraphicDev(_ulong dwIndex);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName);
	HRESULT Release_OnGraphicDev(_ulong dwIndex);
	
	HRESULT Ready_Debug_Buffer(_float fStartX, _float fStartY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_Buffer();
private:
	LPDIRECT3DDEVICE9			m_pDevice = nullptr;
	LPDIRECT3DTEXTURE9			m_pTarget_Texture = nullptr;
	LPDIRECT3DSURFACE9		m_pTarget_Surface = nullptr;
	LPDIRECT3DSURFACE9		m_pOld_Surface = nullptr;
private:
	D3DXCOLOR				m_ClearColor;

private:
	LPDIRECT3DVERTEXBUFFER9	m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9	m_pIB = nullptr;	

public:
	static CTarget* Create(LPDIRECT3DDEVICE9 pDevice, _uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor);
	virtual void Free();
};

END