#include "stdafx.h"
#include "Trigger_BowlingZone.h"

CTrigger_BowlingZone::CTrigger_BowlingZone(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CTrigger_BowlingZone::CTrigger_BowlingZone(CTrigger_BowlingZone & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrigger_BowlingZone::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CTrigger_BowlingZone::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	NxQuat q;
	q.fromAngleAxis(-45.f, NxVec3(0.f, 1.f, 0.f));
	m_pTransform->Get_Actor()->setGlobalOrientationQuat(q);
	m_pTransform->Get_Actor()->getShapes()[0]->setName("Trigger");
	
	return S_OK;
}

_int CTrigger_BowlingZone::Update_GameObject(const _double & TimeDelta)
{
	if (CGameObject::End_GameObject())
		return END_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	return NO_EVENT;
}


HRESULT CTrigger_BowlingZone::Add_Component(void * pArg)
{
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	if (pArg != nullptr)
		memcpy(&tState, pArg, sizeof(COLL_STATE));

	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	m_pTransform->Get_Actor()->getShapes()[0]->setFlag(NX_TRIGGER_ENABLE, TRUE);
	m_pTransform->Get_Actor()->getShapes()[0]->setFlag(NX_SF_DISABLE_RAYCASTING, TRUE);

	return S_OK;
}

CGameObject * CTrigger_BowlingZone::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CTrigger_BowlingZone(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CTrigger_BowlingZone::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CTrigger_BowlingZone(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTrigger_BowlingZone::	Free()
{
	Safe_Release(m_pTransform);

	CGameObject::Free();
}


