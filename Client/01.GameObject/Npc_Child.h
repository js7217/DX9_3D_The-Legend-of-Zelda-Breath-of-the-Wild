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

class CNpc_Child :	public CNpc
{
	enum ACTIONSTATE
	{
		STATE_READY, STATE_CATCHFISH, STATE_CATCHFISH_WAIT, STATE_SONG, STATE_TALK,
		STATE_END
	};

	enum SINGLEANIMATION
	{
		CATCHFISH, CATCHFISH_WAIT, SONG, TALK
	};
public:
	explicit CNpc_Child(LPDIRECT3DDEVICE9 pDevice);
	explicit CNpc_Child(const CNpc_Child& rhs);
	virtual ~CNpc_Child() = default;


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
	void					CatchFish();
	void					CatchFish_Wait();
	void					Song();
	void					Talk();
private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Dynamic*			m_pMesh = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;

private:
	_bool					m_bDraw = false;
	_double				m_SingTime = 0.0;


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

