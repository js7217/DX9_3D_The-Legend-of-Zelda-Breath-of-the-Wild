#pragma once

#include "Base.h"
#include "GraphicDev.h"
#include "TimeMgr.h"
#include "Management.h"
#include "SoundMgr.h"
#include "FontMgr.h"
#include "KeyMgr.h"
#include "DirectInput.h"

#include "EffectManager.h"
#include "Loading.h"
#include "Debug_Console.h"
#include "PhysXMgr.h"


class CMainApp	final	:	public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Ready_MainApp();
	_int Update_MainApp();
	_int LateUpdate_MainApp();
	HRESULT Render_MainApp();

private:
	HRESULT Ready_StartScene(SCENEID eID);
private:
	HRESULT Ready_DefaultSetting(CGraphicDev::WINMODE eMode, _uint iSizeX, _uint iSizeY);
	HRESULT Ready_Physx();
	HRESULT Ready_DefaultManger();
	HRESULT Ready_Component_Prototype();
	HRESULT Ready_GameObject_Prototpye();
	HRESULT Ready_ResourcePreLoad(SCENEID eID);

private:
	_double m_Time;
	_bool m_bDebug;

private:
	CGraphicDev*				m_pGraphicDev;
	LPDIRECT3DDEVICE9	m_pDevice;
	CPhysXMgr*					m_pPhysx;

private:
	CManagement*		m_pManagement;
	CRenderer*			m_pRenderer;
private:
	CLoading*				m_pLoading;
	CDebug_Console* m_pDebug;

private:
	CTimeMgr*		m_pTimeMgr;
	CSoundMgr*	m_pSoundMgr;
	CFontMgr*		m_pFontMgr;

private:
	_bool				m_bPhysxRender;

public:
	static CMainApp*		Create();
	virtual void Free();

};

