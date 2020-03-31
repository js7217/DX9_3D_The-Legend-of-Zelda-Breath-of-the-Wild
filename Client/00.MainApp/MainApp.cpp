#include "stdafx.h"
#include "MainApp.h"
#include "Scene_Logo.h"
#include "Logo_BackGround.h"

CMainApp::CMainApp()
	: m_pDevice(nullptr), m_pGraphicDev(nullptr), m_pRenderer(nullptr),
	m_pManagement(GET_INSTANCE(CManagement)),
	m_pTimeMgr(GET_INSTANCE(CTimeMgr)), m_pSoundMgr(GET_INSTANCE(CSoundMgr)),
	m_pFontMgr(GET_INSTANCE(CFontMgr)), m_pDebug(GET_INSTANCE(CDebug_Console))
{
	m_Time = 0.0;
	m_bDebug = FALSE;
	m_bPhysxRender = FALSE;
	Safe_AddRef(m_pManagement);
	Safe_AddRef(m_pTimeMgr);
	Safe_AddRef(m_pSoundMgr);
	Safe_AddRef(m_pFontMgr);
	Safe_AddRef(m_pDebug);
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(Ready_DefaultSetting(CGraphicDev::MODE_FULL, WINCX, WINCY)))
		return E_FAIL;

	if (FAILED(Ready_Physx()))
		return E_FAIL;

	//Sigleton
	if (FAILED(Ready_DefaultManger()))
		return E_FAIL;

	if (FAILED(Ready_Component_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_GameObject_Prototpye()))
		return E_FAIL;

	if (FAILED(Ready_ResourcePreLoad(SCENE_COMPONENT)))
		return E_FAIL;

#ifdef _DEBUG
	while (!m_pLoading->Get_Finished())
		continue;
#endif // _DEBUG
	
#if !_DEBUG
	Sleep(5000);
#endif

	if (FAILED((Ready_StartScene(SCENE_LOGO))))
		return E_FAIL;

	if(m_pLoading->Get_Finished())
		Safe_Release(m_pLoading);

	return S_OK;
}

int CMainApp::Update_MainApp()
{
	if (nullptr == m_pManagement || nullptr == m_pDevice)
		return END_EVENT;

	////////////UPDATE_DEFAULT_MANAGER////////////

	//TimeDelta
	m_Time = m_pTimeMgr->Update_Time();

#ifdef _DEBUG
	if (m_pDebug != nullptr)
		m_Time *= m_pDebug->Get_TimeAcc();

#endif // _DEBUG

	//KeyManager & Mouse Input Update
	GET_INSTANCE(CInputDev)->Set_InputDev();
	GET_INSTANCE(CKeyMgr)->KeyInput();
	GET_INSTANCE(CEffectManager)->Update_Effect(m_Time);
	m_pSoundMgr->Update_SoundSystem();
		
	//////////////////////////////////////
	
	return m_pManagement->Update_Management(m_Time);
}

int CMainApp::LateUpdate_MainApp()
{ 
	if (nullptr == m_pManagement || nullptr == m_pDevice)
		return END_EVENT;

#ifdef _DEBUG
	//Create Debug Console
	if (!m_bDebug && GET_INSTANCE(CKeyMgr)->Key_Down(dwKEY_F4))
	{
		m_pDebug->Ready_DebugConsole();
		Safe_AddRef(m_pDebug);
		m_bDebug = TRUE;
	}

#endif // _DEBUG
	if (GET_INSTANCE(CKeyMgr)->Key_Down(dwKEY_LCntl))
	{
		if (!m_bPhysxRender)
			m_bPhysxRender = TRUE;
		else
			m_bPhysxRender = FALSE;
	}
	
	GET_INSTANCE(CEffectManager)->LateUpdate_Effect(m_Time);
	return m_pManagement->LateUpdate_Management(m_Time);
}

HRESULT CMainApp::Render_MainApp()
{
	if (nullptr == m_pGraphicDev)
		return E_FAIL;

	m_pGraphicDev->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Get_Time(m_Time);
		m_pRenderer->Render_Renderer();
	}

	if (FAILED(m_pManagement->Render_Management()))
		return E_FAIL;


	//Update Physx 
	if (m_pPhysx != nullptr)
		m_pPhysx->Update_PhysxManager(m_Time);

	if (nullptr != m_pPhysx && m_bPhysxRender)	//Render_Physx_Collider
		m_pPhysx->Render_PhysxCollider();

	m_pPhysx->Render_PhysxEnd();
	m_pGraphicDev->Render_End();

	return S_OK;
}

HRESULT CMainApp::Ready_StartScene(SCENEID eID)
{
	if (nullptr == m_pManagement || nullptr == m_pDevice)
		return E_FAIL;

	CScene* pScene = nullptr;

	switch (eID)
	{
	case SCENE_LOGO:
		pScene = CScene_Logo::Create(m_pDevice);
		break;
	}

	if (nullptr == pScene)
		return E_FAIL;

	if (FAILED(m_pManagement->SetUp_CurrentScene(pScene, SCENE_LOGO)))
		return E_FAIL;

	Safe_Release(pScene);

	return S_OK;
}

HRESULT CMainApp::Ready_DefaultSetting(CGraphicDev::WINMODE eMode, _uint iSizeX, _uint iSizeY)
{
	//Graphic Device
	m_pGraphicDev = GET_INSTANCE(CGraphicDev);

	if (nullptr == m_pGraphicDev)
		return E_FAIL;

	Safe_AddRef(m_pGraphicDev);
	m_pGraphicDev->Ready_GraphicDev(g_hWnd, eMode, iSizeX, iSizeY, &m_pDevice);

	//Management
	if (FAILED(m_pManagement->Ready_Engine(SCENE_END)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Physx()
{
	//Physx Manager
	m_pPhysx = GET_INSTANCE(CPhysXMgr);
	Safe_AddRef(m_pPhysx);

	if (FAILED(m_pPhysx->SetUp_NxPhysX(m_pDevice)))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_DefaultManger()
{
	//Time Manager
	m_pTimeMgr->Init_Time();
	m_pTimeMgr->Update_Time();

	// Sound Manager
	if (FAILED(m_pSoundMgr->Ready_SoundManger()))
		return E_FAIL;

	//Font Manager
	if (FAILED(m_pFontMgr->Ready_Font(m_pDevice, L"Font_Default", L"¹ÙÅÁ", 15, 20, FW_HEAVY)))
		return E_FAIL;

	//Font Manager
	if (FAILED(m_pFontMgr->Ready_Font(m_pDevice, L"Font_Nanum", L"NanumSquare", 15, 25, FW_HEAVY)))
		return E_FAIL;
	
	//Mouse Input
	if (FAILED(GET_INSTANCE(CInputDev)->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;

	// EffectManager
	GET_INSTANCE(CEffectManager)->SetGraphicDevice(m_pDevice);

	return S_OK;
}

HRESULT CMainApp::Ready_GameObject_Prototpye()
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ResourcePreLoad(SCENEID eID)
{
	m_pLoading = CLoading::Create(m_pDevice, eID);
	NULL_CHECK_RETURN(m_pLoading, E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Ready_Component_Prototype()
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_COMPONENT, L"Component_Renderer", m_pRenderer = CRenderer::Create(m_pDevice))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp;

	if (FAILED(pInstance->Ready_MainApp()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMainApp::Free()
{
	//Manager
	Safe_Release(m_pTimeMgr);
	m_pTimeMgr->Destroy_Instance();
	Safe_Release(m_pSoundMgr);
	m_pSoundMgr->Destroy_Instance();
	Safe_Release(m_pFontMgr);
	m_pFontMgr->Destroy_Instance();

	GET_INSTANCE(CKeyMgr)->Destroy_Instance();
	GET_INSTANCE(CInputDev)->Destroy_Instance();
	GET_INSTANCE(CEffectManager)->Destroy_Instance();

	Safe_Release(m_pDebug);
	m_pDebug->Destroy_Instance();

	//Component
	Safe_Release(m_pRenderer);

	//Utility
	Safe_Release(m_pManagement);
	Safe_Release(m_pPhysx);

	//System
	Safe_Release(m_pDevice);
	Safe_Release(m_pGraphicDev);

	//Release Engine	
	CManagement::Release_Engine();
}
