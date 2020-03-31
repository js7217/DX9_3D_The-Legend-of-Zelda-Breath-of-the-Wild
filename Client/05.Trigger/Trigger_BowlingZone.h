#pragma once

#include "GameObject.h"

class CTrigger_BowlingZone : public CGameObject
{
private:
	explicit CTrigger_BowlingZone(LPDIRECT3DDEVICE9 pDevice);
	explicit CTrigger_BowlingZone(CTrigger_BowlingZone & rhs);
	virtual ~CTrigger_BowlingZone() = default;

public:
	void		Set_AddCnt() { m_iCnt++; }
public:
	const _int& Get_iCnt() { return m_iCnt; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);

private:
	CNxTransform*			m_pTransform = nullptr;
	_int					m_iCnt = 0;
private:
	HRESULT Add_Component(void * pArg = nullptr);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

