#include "stdafx.h"
#include "FldObj_MiniGame_LostKokko.h"

CFldObj_MiniGame_LostKokko::CFldObj_MiniGame_LostKokko(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
	, m_pManagement(GET_INSTANCE(CManagement))
{

}

CFldObj_MiniGame_LostKokko::CFldObj_MiniGame_LostKokko(const CFldObj_MiniGame_LostKokko & rhs)
	: CGameObject(rhs)
	, m_pManagement(rhs.m_pManagement)
{

}

HRESULT CFldObj_MiniGame_LostKokko::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CFldObj_MiniGame_LostKokko::Ready_GameObject(void * pArg)
{	
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	m_pQuestUI = (CGameObject*)pArg;

	CQuest_Koko_UI* pQuest = dynamic_cast<CQuest_Koko_UI*>(m_pQuestUI);
	return S_OK;
}

_int CFldObj_MiniGame_LostKokko::Update_GameObject(const _double & TimeDelta)
{
	if (dynamic_cast<CQuest_Koko_UI*>(m_pQuestUI)->Get_End())
		return END_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	return NO_EVENT;
}

_int CFldObj_MiniGame_LostKokko::LateUpdate_GameObject(const _double & TimeDelta)
{

	return NO_EVENT;
}

HRESULT CFldObj_MiniGame_LostKokko::Render_GameObject()
{
	return NO_EVENT;
}

HRESULT CFldObj_MiniGame_LostKokko::Add_Component()
{
	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	tState.cName = "LostKokko";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_STATIC;
	tState.NxFlag = NX_SF_DISABLE_RAYCASTING | NX_TRIGGER_ENABLE;
	tState.vLocalPos = { 0.f, -0.5f, 0.f };
	tState.vStartPos = { 129.f, 6.f, 75.f };
	tState.vAngle = { 0.f, 40.f, 0.f };
	tState.vDimensions = { 7.5f, 0.5f, 10.f };
	tState.iMaterial = 0;
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	return S_OK;
}

CGameObject * CFldObj_MiniGame_LostKokko::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CFldObj_MiniGame_LostKokko(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CFldObj_MiniGame_LostKokko::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CFldObj_MiniGame_LostKokko(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CFldObj_MiniGame_LostKokko::Free()
{
	Safe_Release(m_pTransform);

	CGameObject::Free();
}