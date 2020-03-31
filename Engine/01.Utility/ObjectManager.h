#pragma once

#include "Base.h"
#include "Component.h"

BEGIN(ENGINE)

class CLayer;
class CGameObject;
class ENGINE_DLL CObjectManager :	public CBase
{
	DECLARE_SINGLETON(CObjectManager)

private:
	explicit CObjectManager();
	virtual ~CObjectManager() = default;

public:
	CComponent* Get_Component(const _uint& iSceneIdx, const _tchar* pLayerTag, const _tchar* pComponentTag, const _uint& iIndex = 0);
	CGameObject*	Get_GameObject(const _uint & iSceneIdx, const _tchar * pLayerTag, const _uint & iIndex);
	list<CGameObject*>* Get_ObjectList(const _uint & iSceneIdx, const _tchar * pLayerTag);

public:
	HRESULT Reserve_Object_Manager(_uint iNumScene);
	HRESULT Add_GameObject_Prototype(_uint iSceneID, const _tchar* pProtoTag, CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iProtoSceneID, const _tchar* pProtoTag, _uint iSceneID, const _tchar* pLayerTag, void* pArg = nullptr);
	CGameObject* Add_GameObject(_uint iProtoSceneID, const _tchar* pProtoTag, _uint iSceneID, void* pArg = nullptr);
	HRESULT Add_GameObject(CGameObject* pGameObject, const _tchar* pLayerTag, _uint iSceneID);

	_int Update_Object_Manager(const _double & TimeDelta);
	_int LateUpdate_Object_Manager(const _double & TimeDelta);

public:
	HRESULT Clear_Prototype(_uint iSceneID);
	_bool Check_IceCube(const _uint & iSceneIdx, const _tchar * pLayerTag);
	_vec3 Find_ClosestWeapon(const _uint & iSceneIdx, const _tchar * pLayerTag, _vec3& vMonsterPos);
	_vec3 Find_Tree(const _uint & iSceneIdx, _vec3& vMonsterPos, _bool* pbFind);
private:
	//원본 객체들을 모아놓은 컨테이너
	map<const _tchar*, CGameObject*>*	m_pProtoType;
	typedef map<const _tchar*, CGameObject*> PROTOTYPES;

private:
	//CLayer 객체들을 모아놓는 컨테이너(CLayer는 Clone 객체 보관)
	map<const _tchar*, CLayer*>*	m_pMapLayer;
	typedef map<const _tchar*, CLayer*> LAYERS;
	
private:
	map<_float, _vec3> m_mapDist;

private:
	_uint m_iNumScene;

private:
	CGameObject*	Find_ProtoType(_uint iSceneID, const _tchar* pProtoTag);
	CLayer* Find_Layer(_uint iSceneID, const _tchar* pLayerTag);

public:
	virtual void Free();

};

END
