#pragma once

//#include "MyDefine.h"
#include "GameObject.h"

namespace ENGINE
{
	class CShader;
	class CTexture;
	class CRenderer;
	class CTransform;
	class CBuffer_RcTex;
}
class CNpc;
class CMainQuest : public CGameObject
{
public:
	explicit CMainQuest(LPDIRECT3DDEVICE9 pDevice);
	explicit CMainQuest(CMainQuest& rhs);
	virtual ~CMainQuest() = default;
public:
	void	Set_Npc(CNpc* pNpc);
	void	Set_Hinox(_bool bCheck) { m_bIsHinox = bCheck; }
	void	Set_Maracus(_bool bCheck) { m_bIsMaracus = bCheck; }

public:
	_bool	Get_bIsStart() { return m_bIsQuestStart; }
	_bool	Get_QuestClear();
	_bool	Get_bIsMissionClear() { return m_bMissionClear; }
public:
	virtual void Set_IsDraw(_bool bCheck, _int iNum) { m_bIsDraw = bCheck; m_iPage = iNum; }
	_bool Get_IsDraw() { return m_bIsDraw; }
	virtual HRESULT Ready_GameObject_Prototype(); // For.Prototype
	virtual HRESULT Ready_GameObject(void* pArg); // For.Game Instance
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void ScriptRender();
	void MissionClearRender();
	void KeyInput();
private:
	CTransform*			m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*					m_pShader = nullptr;
	CTexture*					m_pTexture = nullptr;
	CTexture*			m_pTexBtn_One = nullptr;
	CTexture*			m_pTex_Arrow = nullptr;
	CTexture*			m_pTex_EnterArrow = nullptr;
	CTexture*			m_pTexName = nullptr;
	CTexture*			m_pTexMissionWin = nullptr;
	CTexture*			m_pTexComplete = nullptr;
	CTexture*			m_pTexCompleteName = nullptr;
	CTexture*			m_pTexMoneyBar = nullptr;
	CTexture*			m_pTexReward = nullptr;
	CTexture*			m_pTexNum = nullptr;
	CBuffer_RcTex*		m_pBuffer = nullptr;

	_float						m_fArrowMove = 0.f;

	_float						m_fIEnterSpeed;
	_float						m_fScale_EnterArrow = 1.f;

private: // In.ViewPort Pos
	_float						m_fX, m_fY, m_fSizeX, m_fSizeY;
	_bool						m_bIsDraw = false;
	_int						m_iPage = 0;
	CNpc*						m_pNpc = nullptr;
	_short						m_nScriptNum = 0;
	_short						m_nScriptNum_Apply = 0;

	_float						m_fQuestNameAlpha = 0.f;
	_float						m_fAlphaSpeed = 0.f;
	_float						m_fAlphaTime = 0.f;

	_bool					m_bIsDialog = false;
	_bool					m_bIsQuestStart = false;

	_bool					m_bIsMaracus = false;
	_bool					m_bIsHinox = false;
	_bool					m_bMissionClear = false;
	_float					m_fMinScale = 1.f;
	_bool					m_bIsEndAlpha = false;
	_float					m_fEndAlpha = 1.f;


	_bool						m_bIsReward = false;
	_int						m_iMoneyPre = 0;
	_int						m_iPlayerMoney = 0;
	_float						m_fMoneyPlusTime = 0.f;
	_float						m_fPlusUp = 0.f;
	_float						m_fRewardTime = 0.f;
	_bool						m_bIsPlusUp = false;
public:
	static CMainQuest* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

