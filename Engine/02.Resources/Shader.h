#pragma once

#include "Component.h"

BEGIN(ENGINE)

class ENGINE_DLL CShader :
	public CComponent
{
private:
	explicit CShader(LPDIRECT3DDEVICE9 pDevice);
	explicit CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	LPD3DXEFFECT Get_EffectHandle() const 
	{	return m_pEffect;	}
	
public:
	HRESULT Ready_Shader(const _tchar* pFileName);
private:
	LPD3DXEFFECT	m_pEffect;
public:
	static CShader* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar * pFileName);
	virtual CComponent* Clone_Component(void* pArg);
protected:
	virtual void Free();


};

END