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

class CStone_Treasure_Box : public CDgnObj
{
public:
	enum BOXTYPE { STONE, IRON };

private:
	explicit CStone_Treasure_Box(LPDIRECT3DDEVICE9 pDevice);
	explicit CStone_Treasure_Box(const CStone_Treasure_Box& rhs);
	virtual ~CStone_Treasure_Box() = default;
public:
	ITEM Get_ItemInfo() { return m_tItemInfo; }
	_int Get_iNum() { return m_iNum; }
	void Set_Magnet();
public:
	void	Set_Animation(_uint iAniNum) { m_pMesh->SetUp_AnimationSet(iAniNum); }
	void	Set_CurrentAniIndex(_uint iAniIndex) { m_iCurrentAni = iAniIndex; }
	bool&	Get_bIsOpened() { return m_bIsOpened; }

public:
	HRESULT	Add_MeshComponent(BOXTYPE eType);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	//Component
	CManagement*			m_pManagement = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*					m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Dynamic*			m_pMesh = nullptr;

private:
	//Manager
	CDebug_Console*		m_pDebug = nullptr;
	CPhysXMgr*			m_pPhysx = nullptr;
	_int				m_iNum = 0;
	ITEM				m_tItemInfo;
	_float		m_fTime = 0.f;
	_float		m_fFlowTime = 0.f;
	_float		m_fBlinkTime = 0.f;
	_bool		m_bBlinkTime = FALSE;
private:
	//Animate State
	_uint m_iCurAniState = 0;
	_uint m_iPreAniState = 0;
	_uint m_iCurrentAni;
	_bool m_bIsOpened = false;
	_double		m_dTimeDelta = 0;
	

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT Create_Capsule_Coll(NxVec3 nxPos);

public:
	static CDgnObj* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CDgnObj* Clone_GameObject(void* pArg);
	virtual void Free();



};

