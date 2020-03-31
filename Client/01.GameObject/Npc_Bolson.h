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

class CNpc_Bolson :	public CNpc
{
	enum ACTIONSTATE
	{
		STATE_READY, STATE_IDLE, STATE_STAND_END, STATE_STAND_ST, STATE_DANCE, STATE_END
	};

	enum SINGLEANIMATION
	{
		IDLE, STAND_END, STAND_ST, DANCE
	};

	enum ANIMATIONSEQ
	{
		SEQ_START, SEQ_LOOP, SEQ_END
	};

public:
	explicit CNpc_Bolson(LPDIRECT3DDEVICE9 pDevice);
	explicit CNpc_Bolson(const CNpc_Bolson& rhs);
	virtual ~CNpc_Bolson() = default;


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
	void					Idle();
	void					Stand_End();
	void					Stand_Start();
	void					Dance();
private:
	void		Actual_Rotation(_float fAngle, _float fRotAngle = 5.f);
	_bool		Decide_Rotation(const _vec3& vTargetPos, _float fRotAngle = 5.f);

private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Dynamic*			m_pMesh = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;
	_bool					m_bDraw = false;

private:
	_bool					m_bIsAction[STATE_END];
	ANIMATIONSEQ			m_eAnimationSeq;
	ACTIONSTATE				m_eActionState;

private:
	_bool					m_bIsAnimation = true;
	_double					m_dwTimeDelta;
	_double					m_dwTimer = 0.0;
private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CNpc* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CNpc* Clone_GameObject(void* pArg);
	virtual void Free();



};

