#pragma once

#include "Npc.h"
#include "NxPlayer.h"
#include "NxTransform.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CMesh_Dynamic;
	class CFrustum;
}

class CNpc_Kokko_Papa :	public CNpc
{
public:
	enum ACTIONSTATE
	{
		STATE_READY, STATE_NORMAL_TALK, STATE_SAD_TALK, STATE_SAD_TALK_WAIT, STATE_SAD_TALK_L_ST, STATE_SAD_TALK_L,
		STATE_SURPRISE_TALK, STATE_SURPRISE_WAIT, STATE_JOY_TALK, STATE_JOY_WAIT,
		STATE_END
	};

	enum SINGLEANIMATION
	{
		NORMAL_TALK, 
		SAD_TALK, SAD_TALK_WAIT, 
		SAD_TALK_L_ST, SAD_TALK_L, 
		SURPRISE_TALK, SURPRISE_WAIT, 
		JOY_TALK, JOY_WAIT
	};

public:
	void	Set_AnimationState(ACTIONSTATE eActionState);
	void	Set_Disable_Collision(const _bool& bIsColl);
public:
	explicit CNpc_Kokko_Papa(LPDIRECT3DDEVICE9 pDevice);
	explicit CNpc_Kokko_Papa(const CNpc_Kokko_Papa& rhs);
	virtual ~CNpc_Kokko_Papa() = default;


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
	void					Normal_Talk();
	void					Sad_Talk();
	void					Sad_Talk_Wait();
	void					Sad_Talk_L_st();
	void					Sad_Talk_L();
	void					Surprise_Talk();
	void					Surprise_Wait();
	void					Joy_Talk();
	void					Joy_Wait();

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

