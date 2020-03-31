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
	class CMesh_Dynamic;
}

class CObj_YabusameTarget_B : public CDgnObj
{
	

private:
	explicit CObj_YabusameTarget_B(LPDIRECT3DDEVICE9 pDevice);
	explicit CObj_YabusameTarget_B(const CObj_YabusameTarget_B& rhs);
	virtual ~CObj_YabusameTarget_B() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	//Component
	CPhysXMgr*				m_pPhysXMgr = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;
	_bool					m_bDraw = false;

private:
	list<CGameObject*>		m_BrkList;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CDgnObj* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CDgnObj* Clone_GameObject(void* pArg);
	virtual void Free();



};

