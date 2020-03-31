#include "stdafx.h"
#include "Npc.h"
#include "KeyMgr.h"

#include "NxPlayer.h"

CNpc::CNpc(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{

}

CNpc::CNpc(const CNpc & rhs)
	: CGameObject(rhs)
{

}


HRESULT CNpc::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CNpc::Ready_GameObject(void * pArg)
{
	_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();
	m_pPlayer = (CNxPlayer*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Player", 0);
	
	if (nullptr == m_pPlayer)
		return E_FAIL;
	
	return S_OK;
}

_int CNpc::Update_GameObject(const _double & TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bTalkEnd)
	{
		dynamic_cast<CNxPlayer*>(m_pPlayer)->Set_EndTalk();
		m_bTalkEnd = false;
	}

	return NO_EVENT;
}

_int CNpc::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	return NO_EVENT;
}

HRESULT CNpc::Render_GameObject()
{

	return NO_EVENT;
}

CGameObject * CNpc::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CNpc(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CNpc::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CNpc(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNpc::Free()
{
	CGameObject::Free();
}

