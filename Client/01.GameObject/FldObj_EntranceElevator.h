#pragma once

#include "DgnObj.h"
#include "NxTransform.h"
#include "Debug_Console.h"
#include "Management.h"

namespace ENGINE
{
	class CKeyMgr;
	class CShader;
	class CTexture;
	class CRenderer;
	class CMesh_Static;
}

class CFldObj_EntranceElevator : public CDgnObj
{
	

private:
	explicit CFldObj_EntranceElevator(LPDIRECT3DDEVICE9 pDevice);
	explicit CFldObj_EntranceElevator(const CFldObj_EntranceElevator& rhs);
	virtual ~CFldObj_EntranceElevator() = default;

public:
	void Set_MoveElevator() { m_bIsPlayAnimation = true; }
	void Set_WarpPoint(const _vec3& vPoint) { m_vWarpPoint = vPoint; }

public:
	const _vec3& Get_WarpPoint() { return m_vWarpPoint; }
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void	Move_Elevator(const _double & TimeDelta);

private:
	//Component
	CManagement*			m_pManagement = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;
	_bool					m_bDraw = false;
	_bool					m_bIsPlayAnimation = false;
	_bool					m_bIsMove = false;
	_double					m_dwTimer = 10.0;
	_double					m_dwTimeDelta = 0.0;
	NxShape*				m_pTriggerShape = nullptr;
	_vec3					m_vWarpPoint = { 0.f, 0.f, 0.f };
private:
	//Manager
	CDebug_Console*		m_pDebug = nullptr;
	CPhysXMgr*				m_pPhysx = nullptr;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CDgnObj* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CDgnObj* Clone_GameObject(void* pArg);
	virtual void Free();



};

