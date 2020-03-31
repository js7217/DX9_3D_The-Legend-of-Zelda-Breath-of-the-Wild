#include "Scene.h"

#define NO_EVENT 0
#define END_EVENT -1

CScene::CScene(LPDIRECT3DDEVICE9 pDevice)
	:	m_pDevice(pDevice), m_pObjectMgr(GET_INSTANCE(CObjectManager))
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pObjectMgr);
}

HRESULT CScene::Ready_Scene()
{

	return S_OK;
}

_int CScene::Update_Scene(const _double & TimeDelta)
{
	
	return m_pObjectMgr->Update_Object_Manager(TimeDelta);
}

_int CScene::LateUpdate_Scene(const _double & TimeDelta)
{
	
	return m_pObjectMgr->LateUpdate_Object_Manager(TimeDelta);
}

HRESULT CScene::Render_Scene()
{

	return S_OK;
}

HRESULT CScene::SetUp_NxScene(CPhysXMgr::PHYSX_SCENE eScene)
{
	if (FAILED(GET_INSTANCE(CPhysXMgr)->SetUp_NxScene(eScene)))
		return E_FAIL;

	return S_OK;
}

void CScene::Free()
{
	Safe_Release(m_pObjectMgr);
	Safe_Release(m_pDevice);
}

