#pragma once

#include "Base.h"
#include "Component.h"
#include "Management.h"
#include "SoundMgr.h"

BEGIN(ENGINE)

class ENGINE_DLL CGameObject :	public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pDevice);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;
public:
	void Erase_Component(const _tchar * pComponentTag);

public:
	void Set_OnHand(_bool bCheck) { m_bOnHand = bCheck; }
	void Set_GameObject(_bool bDead) { m_bEndEvent = bDead; }
	void Set_GrabState(_bool bCheck) { m_bGrabState = bCheck; }
	void Set_Throw_Item(_bool bCheck) { m_bThrow = bCheck; }
	void Set_PowerThrow_Item(_bool bCheck) { m_bPowerThrow = bCheck; }
	void Set_RenderState(_bool bCheck) { m_bRender = bCheck; }
	void Set_HitVeolocity(NxVec3 vVelocity) { m_vHitVelocity = vVelocity; }
	void Set_EnterWater(_bool bCheck) { m_bCheckWater = bCheck; }
	void Set_WaterY(_float fY) { m_fWaterY = fY; }
	void Set_Magnet(_bool bCheck) { m_bMagnetCatch = bCheck; }

public:
	_bool Get_OnHand() { return m_bOnHand; }
	_bool Get_GrabState() { return m_bGrabState; }
	_bool Get_RenderState() { return m_bRender; }
	_vec3 Get_vPosition() { return m_vPosition; }
public:
	_matrix Get_Transform(_D3DTRANSFORMSTATETYPE eType) const
	{		_matrix matTransform;
		m_pDevice->GetTransform(eType, &matTransform);
		return matTransform;	}
	CComponent* Get_Component(const _tchar* pComponentTag);
	
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual void Render_RimLight() {}
	virtual void Render_Shadow(_int iPass = 1);
	HRESULT Compute_ViewZ(CTransform * pTransform);

protected:
	virtual _int End_GameObject();

protected:
	LPDIRECT3DDEVICE9		m_pDevice;
	_float	m_fViewZ = 0.f;
	_bool	m_bEndEvent = FALSE;
	NxVec3	m_vHitVelocity = {0.f, 0.f, 0.f};
	_float		m_fWaterY = 0.f;
	_vec3		m_vPosition = { 0.f, 0.f, 0.f };
protected:
	_bool m_bGrabState = FALSE;
	_bool m_bThrow = FALSE;
	_bool m_bPowerThrow = FALSE;
	_bool m_bOnHand = FALSE;
	_bool m_bRender = TRUE;
	_bool m_bCheckWater = FALSE;

public:
	NxVec3		m_NxVelocity;
	_bool		m_bMagnetCatch = FALSE;
	_bool		m_bTimeStop = FALSE;

protected:
	_bool m_bIsMonster;

protected:
	map<const _tchar*, CComponent*>	m_MapComponent;
	typedef map<const _tchar*, CComponent*> COMPONENTS;

protected:
	HRESULT Add_Component(_uint iSCeneID, const _tchar * pProtoTag, const _tchar * pComponentTag, CComponent ** ppOutComponent, void * pArg = nullptr);
	HRESULT Add_Component_Mag(_uint iSCeneID, const _tchar * pProtoTag, const _tchar * pComponentTag, CComponent ** ppOutComponent, void * pArg = nullptr);

private:
	CComponent* Find_Component(const _tchar* pComponentTag);

public:
	virtual CGameObject* Clone_GameObject(void* pArg) = 0;
	virtual void Free();

};

END
