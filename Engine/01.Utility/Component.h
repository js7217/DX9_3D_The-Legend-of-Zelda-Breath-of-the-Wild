#pragma once

#include "Base.h"

BEGIN(ENGINE)

class ENGINE_DLL CComponent :	public CBase
{
protected:
	explicit CComponent(LPDIRECT3DDEVICE9 pDevice);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	HRESULT Ready_Component();

public:
	virtual _int Update_Component(const _double & TimeDelta) { return 0; }
	virtual _int LateUpdate_Component(const _double & TimeDelta) { return 0; }

protected:
	LPDIRECT3DDEVICE9 m_pDevice = nullptr;
protected:
	_bool m_isClone = false;

public:
	virtual CComponent* Clone_Component(void* pArg = nullptr) = 0;
	virtual void Free();


};

END