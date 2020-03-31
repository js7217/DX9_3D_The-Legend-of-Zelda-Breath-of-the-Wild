#pragma once

#include "NxTransform.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CMesh_Static;
	class CMesh_Dynamic;
	class CFrustum;
	class CGameObject;
}

class CObj_Log_TreeApple :	public CGameObject
{
public:
	explicit CObj_Log_TreeApple(LPDIRECT3DDEVICE9 pDevice);
	explicit CObj_Log_TreeApple(const CObj_Log_TreeApple& rhs);
	virtual ~CObj_Log_TreeApple() = default;

public:
	void Set_GrabObject(CGameObject* pObj);
	void Set_AgainGrabObject(CGameObject* pObj) { m_pGrabObject = pObj; }

public:
	_bool& Get_bCheckHit() { return m_bCheckHit; }
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void 	Move_Target();
	_bool	Check_Dead();

private:
	void TimeStop_Sound(_double CurTime);
	_double m_SoundTime = 0.0;

public:
	HRESULT Create_NxTransform(const _matrix matWorld);
private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;
private:
	CGameObject*			m_pGrabObject = nullptr;
	CGameObject*			m_pGiant = nullptr;
	_matrix					m_matRotX_M90;
	_matrix					m_matTrans;
	CMesh_Dynamic*			m_pGrabMesh = nullptr;
	_vec3					m_vRHandLook;
	NxShape*				m_pShape;

public:
	_float		m_fTime = 0.f;
	_float		m_fFlowTime = 0.f;
	_float		m_fBlinkTime = 0.f;
	_bool		m_bBlinkTime = FALSE;
	_bool				m_bIsCheckDead = false;
	_bool				m_bCheckHit = FALSE;
	_double				m_TimeStop = 0;

private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();



};

