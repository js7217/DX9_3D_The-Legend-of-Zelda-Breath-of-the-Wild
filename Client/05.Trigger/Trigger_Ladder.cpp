#include "stdafx.h"
#include "Trigger_Ladder.h"
#include "KeyMgr.h"

CTriger_Ladder::CTriger_Ladder(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{

}

CTriger_Ladder::CTriger_Ladder(CTriger_Ladder & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTriger_Ladder::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CTriger_Ladder::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransform->Create_LadderActor_Up();
	m_pTransform->Create_LadderActor_Down();
	m_pTransform->Get_Actor()->getShapes()[0]->setName("Trigger");

	return S_OK;
}

_int CTriger_Ladder::Update_GameObject(const _double & TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	return NO_EVENT;
}

_int CTriger_Ladder::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	//if (GET_INSTANCE(CKeyMgr)->Key_Down(dwKEY_Q))
	//{
	//	m_pTransform->Set_NxPos_Direction(&NxVec3(0.f, 0.1f, 0.f));
	//}
	//if (GET_INSTANCE(CKeyMgr)->Key_Down(dwKEY_E))
	//{
	//	m_pTransform->Set_NxPos_Direction(&NxVec3(0.f, -0.1f, 0.f));
	//}
	
	return NO_EVENT;
}

HRESULT CTriger_Ladder::Add_Component(void * pArg)
{
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));
	
	if (pArg != nullptr)
		memcpy(&tState, pArg, sizeof(COLL_STATE));

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;
	
	return S_OK;
}

CGameObject * CTriger_Ladder::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CTriger_Ladder(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CTriger_Ladder::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CTriger_Ladder(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTriger_Ladder::Free()
{
	Safe_Release(m_pTransform);

	CGameObject::Free();
}
