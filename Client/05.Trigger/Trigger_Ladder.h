#pragma once

#include "GameObject.h"
#include "NxTransform.h"

class CTriger_Ladder :	public CGameObject
{
public:
	explicit CTriger_Ladder(LPDIRECT3DDEVICE9 pDevice);
	explicit CTriger_Ladder(CTriger_Ladder& rhs);
	virtual ~CTriger_Ladder() = default;


public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
private:
	CNxTransform*			m_pTransform = nullptr;

private:
	HRESULT Add_Component(void * pArg = nullptr);

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

