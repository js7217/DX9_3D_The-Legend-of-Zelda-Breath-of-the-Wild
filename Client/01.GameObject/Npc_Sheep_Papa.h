#pragma once

#include "Npc.h"
#include "NxTransform.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CMesh_Dynamic;
	class CFrustum;
}

class CNpc_Sheep_Papa :	public CNpc
{
	enum ACTIONSTATE
	{
		STATE_READY, STATE_SHEEPCARE,
		STATE_END
	};

	enum SINGLEANIMATION
	{
		SHEEPCARE, SHEEPCARE2
	};
public:
	explicit CNpc_Sheep_Papa(LPDIRECT3DDEVICE9 pDevice);
	explicit CNpc_Sheep_Papa(const CNpc_Sheep_Papa& rhs);
	virtual ~CNpc_Sheep_Papa() = default;


public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void					Set_ActionState();
	void					ActionSequence(const _double& TimeDelta);

private:
	void					SheepCare();
private:
	void					Buy();
	void					Talk();
	void					Talk_Wait();
private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Dynamic*			m_pMesh = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;
	_bool					m_bDraw = false;

private:
	_bool					m_bIsAction[STATE_END];
	ACTIONSTATE				m_eActionState;

private:
	_double					m_dwTimeDelta;
private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CNpc* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CNpc* Clone_GameObject(void* pArg);
	virtual void Free();



};

