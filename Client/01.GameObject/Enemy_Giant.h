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
	class CGameObject;
}

class CNxPlayer;
class CEnemy_Giant : public CGameObject
{

	enum ACTIONSTATE
	{
		STATE_READY, STATE_IDLE,
		STATE_SLEEP_LOOP, STATE_SLEEP_END,
		STATE_SEATCH_TREE, STATE_WALK, 
		STATE_BREAK_TREE_START, STATE_BREAK_TREE_END,
		STATE_RUN, STATE_ATTACK_TREE, STATE_ATTACK_THROW,
		STATE_CRY,
		STATE_DAMAGE_EYE_START, STATE_DAMAGE_EYE_LOOP, STATE_DAMAGE_EYE_END,
		STATE_CATCH_TREE_START, STATE_CATCH_TREE_END,
		STATE_ATTACK_HANDCLAP, STATE_ATTACK_HIPDROP,
		STATE_THROW_STONE_START, STATE_THROW_STONE_END,
		STATE_DEAD_STAND,
		STATE_END
	};

	enum SINGLEANIMATION
	{
		IDLE, SLEEP_LOOP, SLEEP_END, SEARCH_BALLTE, WALK, BREAK_TREE_START, BREAK_TREE_END, RUN, ATTACK_TREE, ATTACK_THROW, CRY,
		DAMAGE_EYE_START, DAMAGE_EYE_LOOP, DAMAGE_EYE_END, CATCH_TREE_START, CATCH_TREE_END, ATTACK_HANDCLAP, ATTACK_HIPDROP,
		THROW_STONE_START, THROW_STONE_END, DEAD_STAND, SLEEP_LOOP2
	};

	enum ANIMATIONSEQ
	{
		SEQ_START, SEQ_LOOP, SEQ_END
	};

private:
	explicit CEnemy_Giant(LPDIRECT3DDEVICE9 pDevice);
	explicit CEnemy_Giant(const CEnemy_Giant& rhs);
	virtual ~CEnemy_Giant() = default;

public:
	void	Set_WakeUp() {	m_bIsWakeUp = true; m_eActionState = STATE_SLEEP_END;	}
	void	Set_EyeHit() { m_eActionState = STATE_DAMAGE_EYE_START; }
	void	Set_Target();
	void	Set_LogPos(const _vec3& vLogPos) { m_vLogPos = vLogPos; }
	void	Set_Catch() { m_bIsCatchTree = true; }
	void	Set_Damage(_int iDamage) {	m_iHp -= iDamage;	}
	void	Set_Dead() { m_eActionState = STATE_DEAD_STAND; }

public:
	_bool		Get_WakeUp() { return m_bIsWakeUp; }
	_matrix		Get_WorldMat() { return m_matWorld; }
	_bool		Get_CatchTree() { return m_bIsCatchTree; }
	_bool		Get_bLog() { return m_bLog; }
	_bool		Get_CatchStone() { return m_bIsCatchStone; }
	_vec3		Get_TargetDir() { return m_vTargetDir; }
	NxVec3		Get_PlayerPos() { return m_vPlayerPos; }
	_int		Get_Hp() { return m_iHp; }
	_bool		Get_Dead()
	{
		if (-1 == End_GameObject())
			return true;
		else
			return false;
	}
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void		Create_Shapes();
	void		Update_Shapes();

private:
	void		Set_ActionState();
	void		ActionSequence(const _double& TimeDelta);

private:
	void		Sleep();
	void		Sleep_End();
	void		Search_Battle();
	void		Walk();
	void		Wake_Up();
	void		Idle();
	void		Break_Tree_Start();
	void		Break_Tree_End();
	void		Run();
	void		Attack_Tree();
	void		Attack_Throw();
	void		Cry();
	void		Damage_Eye_Start();
	void		Damage_Eye_Loop();
	void		Damage_Eye_End();
	void		Catch_Tree_Start();
	void		Catch_Tree_End();
	void		Attack_HandClap();
	void		Attack_HipDrop();
	void		Throw_Stone_Start();
	void		Throw_Stone_End();
	void		Dead_Stand();

private:
	void		Actual_Rotation(_float fAngle, _float fRotAngle = 5.f);
	_bool		Decide_Rotation(const _vec3& vTargetPos, _float fRotAngle = 5.f);
	_bool		Walk_Distance();
	_bool		Check_Front_Player();
	_bool		Check_Front_Tree(const _vec3& vTreePos, _float fRadian = 0.4f);
private:
	//Component
	CManagement*			m_pManagement = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Dynamic*			m_pMesh = nullptr;
	CFrustum*				m_pFrustumCom = nullptr;
	CTexture*				m_pDisolve = nullptr;
	_bool					m_bDraw = false;
	_double					m_dwTimeDelta = 0.0;
	_double					m_dwDisolveTime = 0.0;
	NxMat34					m_NxMatrix;
	_matrix					m_matWorld;
private:
	//Manager
	CDebug_Console*			m_pDebug = nullptr;
	CPhysXMgr*				m_pPhysx = nullptr;

private:
	_bool					m_bIsAction[STATE_END];
	ANIMATIONSEQ			m_eAnimationSeq;
	ACTIONSTATE				m_eActionState;

private:
	_float					m_fTime = 0.f;
	_float					m_fCryRadial = 0.f;
	_bool					m_bCryRadial = FALSE;
	_bool					m_bIsWakeUp = false;
	_bool					m_bIsCatchTree = false;
	_bool					m_bIsCatchStone = false;
	_bool					m_bSearchTree = false;
	_bool					m_bFront = false;
	_bool					m_bLog = false;
	_bool					m_bIsThrowStone = false;				// 돌 던지는 패턴은 언제나 한번 공격이 이뤄져야 가능하게끔 할 것이다.
	_float					m_fRotationAngle = 0.0f;
	_float					m_fDist = 0.f;
	_float					m_fRotSpeed = 0.f;
	_vec3					m_vTreePos = { 0.f, 0.f, 0.f };
	_vec3					m_vTargetDir = { 0.f, 0.f, 0.f };
	NxVec3					m_vPlayerPos;
	CNxPlayer*				m_pPlayer = nullptr;
	_vec3					m_vLogPos = { 0.f, 0.f, 0.f };		// 잡고있는 Log를 떨궜을때 찾아야하는 Pos값.
	_int					m_iThrowCnt = 2;
	_uint					m_iMaxHp = 100;
	_uint					m_iHp = 100;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

