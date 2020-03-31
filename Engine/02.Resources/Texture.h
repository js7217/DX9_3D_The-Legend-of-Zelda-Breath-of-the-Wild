#pragma once

#include "Component.h"

BEGIN(ENGINE)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TEXTURETYPE {TYPE_GENERAL, TYPE_CUBE, TYPE_END};
private:
	explicit CTexture(LPDIRECT3DDEVICE9 pDevice);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;
public:
	HRESULT Ready_Texture(TEXTURETYPE eType, const _tchar* pFileName, const _uint& iCnt);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName, const _uint& iIndex = 0);
private:
	vector<IDirect3DBaseTexture9*>			m_vecTexture;
	typedef vector<IDirect3DBaseTexture9*>	VECTEXTURE;
private:
	_tchar									m_szFileName[MAX_PATH] = L"";
public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pDevice, TEXTURETYPE eType, const _tchar* pFileName, const _uint& iCnt = 1);
	virtual CComponent* Clone_Component(void* pArg);
protected:
	virtual void Free();
};

END