#pragma once

#include "Base.h"

class CLight;
class ENGINE_DLL CLightManager :	public CBase
{
	DECLARE_SINGLETON(CLightManager)

private:
	explicit CLightManager();
	virtual ~CLightManager() = default;

public:
	void	Set_Activate(_uint iIndex, _bool bAct);
	_bool	Get_Activate(_uint iIndex);

public:
	HRESULT Add_Light(LPDIRECT3DDEVICE9 pDevice, const D3DLIGHT9 & LightInfo);
	HRESULT Render_Light_Manager(LPD3DXEFFECT pEffect);

public:
	const D3DLIGHT9* Get_LightInfo(const _uint& iIndex = 0);

private:
	CLight* Get_Light(const _uint& iIndex = 0);

private:
	list<CLight*> m_LightList;
	typedef list<CLight*> LIGHTLIST;
	
public:
	virtual void Free();

};

