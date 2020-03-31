#pragma once

#include "Base.h"
#include "ObjectManager.h"
#include "PhysXMgr.h"

BEGIN(ENGINE)

class ENGINE_DLL CScene abstract : public CBase
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CScene() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(const _double & TimeDelta);
	virtual _int LateUpdate_Scene(const _double & TimeDelta);
	virtual HRESULT Render_Scene();

protected:
	HRESULT SetUp_NxScene(CPhysXMgr::PHYSX_SCENE eScene);

protected:
	LPDIRECT3DDEVICE9 m_pDevice;
protected:
	CObjectManager*	m_pObjectMgr;

public:
	virtual void Free();
	
};

END

