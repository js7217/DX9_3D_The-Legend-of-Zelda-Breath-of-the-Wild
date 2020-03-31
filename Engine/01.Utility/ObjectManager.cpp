#include "ObjectManager.h"
#include "Layer.h"
#include "GameObject.h"
#include "Weapon_Melee.h"
#include "Obj_TreeApple.h"

IMPLEMENT_SINGLETON(CObjectManager)

CObjectManager::CObjectManager()
	: m_pProtoType(nullptr), m_pMapLayer(nullptr),
	m_iNumScene(0)
{

}

CComponent * CObjectManager::Get_Component(const _uint & iSceneIdx, const _tchar * pLayerTag, const _tchar * pComponentTag, const _uint & iIndex)
{
	if (m_iNumScene <= iSceneIdx ||		nullptr == m_pMapLayer)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iSceneIdx, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(pComponentTag, iIndex);
}

CGameObject * CObjectManager::Get_GameObject(const _uint & iSceneIdx, const _tchar * pLayerTag, const _uint & iIndex)
{
	if (m_iNumScene <= iSceneIdx || nullptr == m_pMapLayer)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iSceneIdx, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_GameObject(iIndex);
}

list<CGameObject*>* CObjectManager::Get_ObjectList(const _uint & iSceneIdx, const _tchar * pLayerTag)
{
	if (m_iNumScene <= iSceneIdx || nullptr == m_pMapLayer)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iSceneIdx, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return &pLayer->Get_Objlist();
}

HRESULT CObjectManager::Reserve_Object_Manager(_uint iNumScene)
{
	if (nullptr != m_pProtoType || nullptr != m_pMapLayer)
		return E_FAIL;

	m_pProtoType = new PROTOTYPES[iNumScene];
	m_pMapLayer = new LAYERS[iNumScene];

	m_iNumScene = iNumScene;

	return S_OK;
}

HRESULT CObjectManager::Add_GameObject_Prototype(_uint iSceneID, const _tchar * pProtoTag, CGameObject * pPrototype)
{
	if (nullptr == m_pProtoType || nullptr == pPrototype || m_iNumScene <= iSceneID)
		return E_FAIL;

	CGameObject* pObject = Find_ProtoType(iSceneID, pProtoTag);
	if (nullptr != pObject)
		return E_FAIL;

	m_pProtoType[iSceneID].insert(PROTOTYPES::value_type(pProtoTag, pPrototype));

	return S_OK;
}

HRESULT CObjectManager::Add_GameObject(_uint iProtoSceneID, const _tchar * pProtoTag, _uint iSceneID, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pMapLayer || m_iNumScene <= iProtoSceneID || m_iNumScene <= iSceneID)
		return E_FAIL;

	CGameObject* pProtoType = Find_ProtoType(iProtoSceneID, pProtoTag);

	if (nullptr == pProtoType)
		return E_FAIL;

	CGameObject* pGameObject = pProtoType->Clone_GameObject(pArg);

	if (nullptr == pGameObject)
		return E_FAIL;
	

	CLayer* pLayer = Find_Layer(iSceneID, pLayerTag);

	if (nullptr == pLayer)
	{
		if (nullptr == (pLayer = CLayer::Create()))
		{
			Safe_Release(pGameObject);
			return E_FAIL;
		}
	}

	pLayer->Add_GameObject(pGameObject);

	m_pMapLayer[iSceneID].insert(LAYERS::value_type(pLayerTag, pLayer));

	return S_OK;
}

CGameObject * CObjectManager::Add_GameObject(_uint iProtoSceneID, const _tchar * pProtoTag, _uint iSceneID, void * pArg)
{
	if (nullptr == m_pMapLayer || m_iNumScene <= iProtoSceneID || m_iNumScene <= iSceneID)
		return nullptr;

	CGameObject* pProtoType = Find_ProtoType(iProtoSceneID, pProtoTag);

	if (nullptr == pProtoType)
		return nullptr;

	CGameObject* pGameObject = pProtoType->Clone_GameObject(pArg);

	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

HRESULT CObjectManager::Add_GameObject(CGameObject* pGameObject, const _tchar* pLayerTag, _uint iSceneID)
{
	CLayer* pLayer = Find_Layer(iSceneID, pLayerTag);

	if (nullptr == pLayer)
	{
		if (nullptr == (pLayer = CLayer::Create()))
		{
			Safe_Release(pGameObject);
			return E_FAIL;
		}
	}

	pLayer->Add_GameObject(pGameObject);

	m_pMapLayer[iSceneID].insert(LAYERS::value_type(pLayerTag, pLayer));

	return S_OK;
}

_int CObjectManager::Update_Object_Manager(const _double & TimeDelta)
{
	_int iExit = 0;

	for (size_t i = 0; i < m_iNumScene; ++i)
	{
		for (auto& Pair : m_pMapLayer[i])
		{
			iExit = Pair.second->Update_Layer(TimeDelta);

			if (iExit & 0x80000000)
				return iExit;
		}
	}

	return iExit;
}

_int CObjectManager::LateUpdate_Object_Manager(const _double & TimeDelta)
{
	_int iExit = 0;

	for (size_t i = 0; i < m_iNumScene; ++i)
	{
		for (auto& Pair : m_pMapLayer[i])
		{
			iExit = Pair.second->LateUpdate_Layer(TimeDelta);

			if (iExit & 0x80000000)
				return iExit;
		}
	}

	return iExit;
}

HRESULT CObjectManager::Clear_Prototype(_uint iSceneID)
{
	if (nullptr == m_pProtoType || m_iNumScene <= iSceneID)
		return E_FAIL;

	for (auto& Pair : m_pMapLayer[iSceneID])
		Safe_Release(Pair.second);

	m_pMapLayer[iSceneID].clear();

	for (auto& Pair : m_pProtoType[iSceneID])
		Safe_Release(Pair.second);
	
	m_pProtoType[iSceneID].clear();
	
	return S_OK;
}

_bool CObjectManager::Check_IceCube(const _uint & iSceneIdx, const _tchar * pLayerTag)
{
	if (m_iNumScene <= iSceneIdx || nullptr == m_pMapLayer)
		return FALSE;

	CLayer*		pLayer = Find_Layer(iSceneIdx, pLayerTag);

	if (nullptr == pLayer)
		return FALSE;

	list<CGameObject*>& pList = pLayer->Get_Objlist();

	while (!pList.empty() && pList.front() == nullptr)
		pList.pop_front();

	if (pList.size() > 3)
	{
		pList.front()->Set_GameObject(TRUE);
		return TRUE;
	}

	return FALSE;
}

_vec3 CObjectManager::Find_ClosestWeapon(const _uint & iSceneIdx, const _tchar * pLayerTag, _vec3& vMonsterPos)
{
	_vec3 vFailReturn = { 0.f, -100.f, 0.f };

	if (m_iNumScene <= iSceneIdx || nullptr == m_pMapLayer)
		return vFailReturn;

	CLayer*		pLayer = Find_Layer(iSceneIdx, pLayerTag);

	if (nullptr == pLayer)
		return vFailReturn;

	list<CGameObject*> pList = pLayer->Get_Objlist();

	if (TRUE == pList.empty())
		return vFailReturn;

	vector<CWeapon_Melee*> vecWeaponMeleeContainer;

	for (auto& pGameObject : pList)
	{
		CWeapon_Melee* pWeapon = dynamic_cast<CWeapon_Melee*>(pGameObject);

		if (pWeapon == nullptr)
			return vFailReturn;

		if (pWeapon->Get_ItemInfo().eItemStatus == ITEM_FIELD)
		{
			_float fDist = D3DXVec3Length(&(pWeapon->Get_ItemPosition() - vMonsterPos));

			if (fDist <= 10.f)
			{
				vecWeaponMeleeContainer.push_back(pWeapon);
			}
		}	
	}

	_int iClosestWeaponIndex = 0;

	if (vecWeaponMeleeContainer.empty())
		return vFailReturn;
	else
	{
		for (_int i = 0; i < vecWeaponMeleeContainer.size(); ++i)
		{
			if (i == 0)
				iClosestWeaponIndex = i;
			else
			{
				if (D3DXVec3Length(&(vecWeaponMeleeContainer[i]->Get_ItemPosition() - vMonsterPos)) <
					D3DXVec3Length(&(vecWeaponMeleeContainer[iClosestWeaponIndex]->Get_ItemPosition() - vMonsterPos)))
				{
					iClosestWeaponIndex = i;
				}
			}
		}

		return vecWeaponMeleeContainer[iClosestWeaponIndex]->Get_ItemPosition();
	}

	return vFailReturn;
}

_vec3 CObjectManager::Find_Tree(const _uint & iSceneIdx, _vec3 & vMonsterPos, _bool* pbFind)
{
	_vec3 vFailReturn = { 0.f, 0.f, 0.f };

	if (m_iNumScene <= iSceneIdx || nullptr == m_pMapLayer)
		return vFailReturn;

	CLayer*		pLayer = Find_Layer(iSceneIdx, L"Layer_Tree");

	if (nullptr == pLayer)
	{
		*pbFind = false;
		return vFailReturn;
	}

	list<CGameObject*> pList = pLayer->Get_Objlist();

	if (TRUE == pList.empty())
	{
		*pbFind = false;
		return vFailReturn;
	}

	for (auto& pGameObject : pList)
	{
		_vec3 vTreePos = pGameObject->Get_vPosition();

		_float fDist = D3DXVec3Length(&(vTreePos - vMonsterPos));
		
		m_mapDist.insert({ fDist, vTreePos });
	}

	// 가장 가까운 것 찾아가기.
	_vec3 vTreePos = m_mapDist.begin()->second;
	m_mapDist.clear();
	*pbFind = true;

	return vTreePos;
}

CGameObject * CObjectManager::Find_ProtoType(_uint iSceneID, const _tchar * pProtoTag)
{
	auto iter = find_if(m_pProtoType[iSceneID].begin(), m_pProtoType[iSceneID].end(), CFinder_Tag(pProtoTag));

	if (iter == m_pProtoType[iSceneID].end())
		return nullptr;

	return iter->second;
}

CLayer * CObjectManager::Find_Layer(_uint iSceneID, const _tchar * pLayerTag)
{
	auto iter = find_if(m_pMapLayer[iSceneID].begin(), m_pMapLayer[iSceneID].end(), CFinder_Tag(pLayerTag));

	if (iter == m_pMapLayer[iSceneID].end())
		return nullptr;

	return iter->second;
}

void CObjectManager::Free()
{
	for (size_t i = 0; i < m_iNumScene; i++)
	{
		for (auto & Pair : m_pMapLayer[i])
			Safe_Release(Pair.second);
		m_pMapLayer[i].clear();

		for (auto & Pair : m_pProtoType[i])
			Safe_Release(Pair.second);
		m_pProtoType[i].clear();
	}

	Safe_Delete_Array(m_pMapLayer);
	Safe_Delete_Array(m_pProtoType);

}
