#pragma once

#include "Base.h"
#include "ObjectManager.h"
#include "ComponentManager.h"
#include "LightManager.h"
#include "Target_Manager.h"

BEGIN(ENGINE)

class CScene;
class ENGINE_DLL CManagement :	public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit CManagement();
	virtual ~CManagement() = default;

public:
	_bool Get_DungeonOut() { return m_bIsDungeonOut; }
	void  Set_DungeonOut(_bool bCheck) { m_bIsDungeonOut = bCheck; }
	_bool	Get_DeadGiant() { return m_bIsDeadGiant; }
	void	Set_DeadGiant() { m_bIsDeadGiant = true; }
	void	Set_CurrentSceneNum(_uint iNum) { m_iSceneNum = iNum; }
	void	Set_WarpPoint(const _vec3& vPoint) { m_vWarpPoint = vPoint; }
	HRESULT	Set_DungeonOpen(_int iNum);
	void	Set_KokkoEnter(_int iNum) { m_bKokkoEnter[iNum] = TRUE; }
	void	Set_CompleteQuest() { m_bKokkoQuest = TRUE; }
	void	Set_ClearMainQuest() { m_bClearMainQuest = TRUE; }
	void	Set_InitialCinematic() {m_bInitialGameStartCinematic = TRUE;}
public:
	_uint			Get_CurrentSceneNum() { return m_iSceneNum; }
	_bool&			Get_FieldLoadState() { return m_bFieldLoad; }
	const _vec3&	Get_WarpPoint() { return m_vWarpPoint; }
	_bool&			Get_DungeonOpen(_int iNum) { return m_bDungeonOpen[iNum]; }
	_bool&			Get_KokkoEnter(_int iNum) { return m_bKokkoEnter[iNum]; }
	_bool&			Get_bKokkoQuest() { return m_bKokkoQuest; }
	_bool&			Get_InitialCinematic() { return m_bInitialGameStartCinematic; }
	_bool&			Get_bClearMainQuest() { return m_bClearMainQuest; }
public:
	HRESULT SetUp_CurrentScene(CScene* pScene, _uint iSceneNum);
	HRESULT Ready_Engine(_uint iNumScene);
	_int Update_Management(const _double & TimeDelta);
	_int LateUpdate_Management(const _double & TimeDelta);
	HRESULT Render_Management();
	HRESULT Clear_Prototype(_uint iSceneID);

public:	//Object Manager;
	HRESULT Add_GameObject_Prototype(_uint iSceneID, const _tchar* pProtoTag, CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iProtoSceneID, const _tchar* pProtoTag, _uint iSceneID, const _tchar* pLayerTag, void* pArg = nullptr);
	
public:	//Component Manager;
	HRESULT Add_Component_Prototype(_uint iSceneID, const _tchar* pPrototypeTag, CComponent* pPrototype);
	HRESULT Clone_Component(_uint iSceneID, const _tchar* pPrototypeTag, CComponent** ppOutComponent, void* pArg = nullptr);

public:
	static HRESULT Release_Engine();

private:
	CScene*	m_pCurrenctScene;
	_uint		m_iSceneNum;
	_bool		m_bFieldLoad = FALSE;
	_vec3		m_vWarpPoint = { 0.f, 0.f, 0.f };
	_bool		m_bDungeonOpen[4];
	_bool		m_bKokkoQuest = FALSE;
	_bool		m_bKokkoEnter[5];
	_bool		m_bIsDeadGiant = false;
	_bool		m_bInitialGameStartCinematic = FALSE;
	_bool		m_bClearMainQuest = false;
	_bool		m_bIsDungeonOut = false;
private:
	CObjectManager*			m_pObjectMgr;
	CComponentManager*		m_pCompMgr;

public:
	virtual void Free();

};

END