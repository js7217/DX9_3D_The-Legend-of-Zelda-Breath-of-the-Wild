#pragma once

#include "Base.h"
#include "Component.h"

BEGIN(ENGINE)

class CGameObject;
class CLayer final :	public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer() = default;

public:
	CComponent* Get_Component(const _tchar* pComponentTag, const _uint& iIndex);
	CGameObject* Get_GameObject(const _uint& iIndex);
	list<CGameObject*>&Get_Objlist() { return m_ObjectList; }

public:
	HRESULT Ready_Layer();
	HRESULT Add_GameObject(CGameObject* pGameObject);
	_int Update_Layer(const _double & TimeDelta);
	_int LateUpdate_Layer(const _double & TimeDelta);

private:
	list<CGameObject*> m_ObjectList;
	typedef list<CGameObject*> OBJECTLIST;

public:
	static CLayer*	Create();
	virtual void Free();
};

END