#pragma once

#include "GameObject.h"
#include "NxTransform.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CMesh_Static;
}

class CDgnObj :	public CGameObject
{
public:
	explicit CDgnObj(LPDIRECT3DDEVICE9 pDevice);
	explicit CDgnObj(const CDgnObj& rhs);
	virtual ~CDgnObj() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

protected:
	void TimeStop_Sound(_double CurTime);
	_double m_SoundTime = 0.0;

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

