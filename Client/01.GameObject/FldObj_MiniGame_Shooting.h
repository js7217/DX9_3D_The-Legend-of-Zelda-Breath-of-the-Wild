#pragma once

#include "NxTransform.h"

namespace ENGINE
{
	class CManagement;
	class CGameObject;
}

class CFldObj_MiniGame_Shooting :	public CGameObject
{
public:
	explicit CFldObj_MiniGame_Shooting(LPDIRECT3DDEVICE9 pDevice);
	explicit CFldObj_MiniGame_Shooting(const CFldObj_MiniGame_Shooting& rhs);
	virtual ~CFldObj_MiniGame_Shooting() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	_matrix			m_matWorld;
	_double			m_dwTimer = 0.0;
	_int			m_iCnt = 0;
	_double			m_dwRandom = 0.0;

private:
	CManagement*	m_pManagement = nullptr;

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();



};

