#include "stdafx.h"
#include "DgnObj.h"
#include "KeyMgr.h"

CDgnObj::CDgnObj(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{

}

CDgnObj::CDgnObj(const CDgnObj & rhs)
	: CGameObject(rhs)
{

}

HRESULT CDgnObj::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CDgnObj::Ready_GameObject(void * pArg)
{
	

	return S_OK;
}

_int CDgnObj::Update_GameObject(const _double & TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	return NO_EVENT;
}

_int CDgnObj::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	return NO_EVENT;
}

HRESULT CDgnObj::Render_GameObject()
{

	return NO_EVENT;
}

void CDgnObj::TimeStop_Sound(_double CurTime)
{
	if (CurTime >= 1)
	{
		GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"BitaLock_Timer03.wav", CSoundMgr::DGN_OBJECT1);
	}
}

CGameObject * CDgnObj::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CDgnObj(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CDgnObj::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CDgnObj(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CDgnObj::Free()
{
	CGameObject::Free();
}

