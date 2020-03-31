#include "Management.h"
#include "GraphicDev.h"
#include "Scene.h"
#include "Inventory_UI.h"

IMPLEMENT_SINGLETON(CManagement)

#define END_EVENT -1

CManagement::CManagement()
	:	m_pCurrenctScene(nullptr),
	m_pObjectMgr(GET_INSTANCE(CObjectManager)), m_pCompMgr(GET_INSTANCE(CComponentManager))
{
	m_bFieldLoad = FALSE;
	Safe_AddRef(m_pObjectMgr);
	Safe_AddRef(m_pCompMgr);

	for (_int i = 0; i < 4; ++i)
		m_bDungeonOpen[i] = false;

	for (_int i = 0; i < 5; ++i)
		m_bKokkoEnter[i] = FALSE;
}

HRESULT CManagement::SetUp_CurrentScene(CScene * pScene, _uint iSceneNum)
{
	if (nullptr == pScene)
		return E_FAIL;

	if (nullptr != m_pCurrenctScene)
	{
		_ulong dwRefCnt = 0;

		if (dwRefCnt = Safe_Release(m_pCurrenctScene))
		{
			Safe_Release(pScene);
			MSG_BOX("Scene Change FAILED!");
			return E_FAIL;
		}

	}

	m_pCurrenctScene = pScene;
	m_iSceneNum = iSceneNum;
	Safe_AddRef(m_pCurrenctScene);
	
	//인벤토리 weeapon 날렸다 다시 만든다. ㅈㅅ 인벤 아이템 콜라이더가 안붙어서 똥을 쌌습니다. 지은이 이강동
	CInventory_UI* pInven_UI = (CInventory_UI*)m_pObjectMgr->Get_GameObject(4, L"Layer_Inven", 0);
	if (pInven_UI == nullptr)
		return S_OK;
	pInven_UI->Set_IsResetWeapon(true);

	return S_OK;
}

HRESULT CManagement::Ready_Engine(_uint iNumScene)
{
	if (nullptr == m_pObjectMgr || nullptr == m_pCompMgr)
		return E_FAIL;

	if (FAILED(m_pObjectMgr->Reserve_Object_Manager(iNumScene)))
		return E_FAIL;
	
	if (FAILED(m_pCompMgr->Reserve_Component_Manager(iNumScene)))
		return E_FAIL;
	

	return S_OK;
}
		  
_int CManagement::Update_Management(const _double & TimeDelta)
{
	if (nullptr == m_pCurrenctScene)
		return END_EVENT;

	_int iExit = 0;

	iExit = m_pCurrenctScene->Update_Scene(TimeDelta);
	if (iExit & 0x80000000)
		return END_EVENT;

	return iExit;
}

_int CManagement::LateUpdate_Management(const _double & TimeDelta)
{
	if (nullptr == m_pCurrenctScene)
		return END_EVENT;

	_int iExit = 0;

	iExit = m_pCurrenctScene->LateUpdate_Scene(TimeDelta);
	if (iExit & 0x80000000)
		return END_EVENT;

	return iExit;
}

HRESULT CManagement::Render_Management()
{
	if (nullptr == m_pCurrenctScene)
		return E_FAIL;

	if (FAILED(m_pCurrenctScene->Render_Scene()))
		return E_FAIL;

	return S_OK;
}

HRESULT CManagement::Clear_Prototype(_uint iSceneID)
{
	if (nullptr == m_pObjectMgr || nullptr == m_pCompMgr)
		return E_FAIL;

	if (FAILED(m_pObjectMgr->Clear_Prototype(iSceneID)))
		return E_FAIL;

	if (FAILED(m_pCompMgr->Clear_Prototype(iSceneID)))
		return E_FAIL;

	return S_OK;
}

HRESULT CManagement::Set_DungeonOpen(_int iNum)
{
	if (iNum < 0)
		return E_FAIL;

	m_bDungeonOpen[iNum] = TRUE;
	return S_OK;
}

HRESULT CManagement::Add_GameObject_Prototype(_uint iSceneID, const _tchar * pProtoTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObjectMgr)
		return E_FAIL;

	return m_pObjectMgr->Add_GameObject_Prototype(iSceneID, pProtoTag, pPrototype);
}

HRESULT CManagement::Add_GameObject(_uint iProtoSceneID, const _tchar * pProtoTag, _uint iSceneID, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObjectMgr)
		return E_FAIL;

	return m_pObjectMgr->Add_GameObject(iProtoSceneID, pProtoTag, iSceneID, pLayerTag, pArg);
}

HRESULT CManagement::Add_Component_Prototype(_uint iSceneID, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pCompMgr)
		return E_FAIL;

	return m_pCompMgr->Add_Component_Prototype(iSceneID, pPrototypeTag, pPrototype);
}

HRESULT CManagement::Clone_Component(_uint iSceneID, const _tchar * pPrototypeTag, CComponent ** ppOutComponent, void * pArg)
{
	if (nullptr == m_pCompMgr)
		return E_FAIL;

	*ppOutComponent = m_pCompMgr->Clone_Component(iSceneID, pPrototypeTag, pArg);

	if (nullptr == *ppOutComponent)
		return E_FAIL;
	
	return S_OK;
}

HRESULT CManagement::Release_Engine()
{
	_ulong dwRefCnt = 0;

	if (dwRefCnt = GET_INSTANCE(CManagement)->Destroy_Instance())
		MSG_BOX("Management Destroy Failed!");

	if(dwRefCnt = GET_INSTANCE(CObjectManager)->Destroy_Instance())
		MSG_BOX("Object Manger Destroy Failed!");

	if (dwRefCnt = GET_INSTANCE(CComponentManager)->Destroy_Instance())
		MSG_BOX("Component Manger Destroy Failed!");

	if (dwRefCnt = GET_INSTANCE(CLightManager)->Destroy_Instance())
		MSG_BOX("CLight_Manager Release Failed");

	if (dwRefCnt = GET_INSTANCE(CTarget_Manager)->Destroy_Instance())
		MSG_BOX("CTarget_Manager Release Failed");

	if (dwRefCnt = GET_INSTANCE(CPhysXMgr)->Destroy_Instance())
		MSG_BOX("CTarget_Manager Release Failed");

	if (dwRefCnt = GET_INSTANCE(CGraphicDev)->Destroy_Instance())
		MSG_BOX("CGraphic Device Destroy Failed!");

	return S_OK;
}

void CManagement::Free()
{
	Safe_Release(m_pCompMgr);
	Safe_Release(m_pObjectMgr);
	Safe_Release(m_pCurrenctScene);
	
}
