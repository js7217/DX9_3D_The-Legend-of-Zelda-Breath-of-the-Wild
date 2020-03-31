#include "ComponentManager.h"
#include "Component.h"

IMPLEMENT_SINGLETON(CComponentManager)

CComponentManager::CComponentManager()
	: m_pPrototype(nullptr)
{
}

HRESULT CComponentManager::Reserve_Component_Manager(_uint iNumScene)
{
	if (nullptr != m_pPrototype)
		return E_FAIL;

	m_pPrototype = new PROTOTYPES[iNumScene];

	m_iNumScene = iNumScene;

	return S_OK;
}

HRESULT CComponentManager::Add_Component_Prototype(_uint iSceneID, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pPrototype || nullptr == pPrototype || m_iNumScene <= iSceneID)
		return E_FAIL;

	CComponent* pComponent = Find_Component(iSceneID, pPrototypeTag);

	if (nullptr != pComponent)
		return E_FAIL;

	m_pPrototype[iSceneID].insert(PROTOTYPES::value_type(pPrototypeTag, pPrototype));

	return S_OK;
}

CComponent * CComponentManager::Clone_Component(_uint iSceneID, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pPrototype || m_iNumScene <= iSceneID)
		return nullptr;

	CComponent* pComponent = Find_Component(iSceneID, pPrototypeTag);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent->Clone_Component(pArg);
}

HRESULT CComponentManager::Clear_Prototype(_uint iSceneID)
{
	if (nullptr == m_pPrototype || m_iNumScene <= iSceneID)
		return E_FAIL;

	for (auto& Pair : m_pPrototype[iSceneID])
		Safe_Release(Pair.second);

	m_pPrototype[iSceneID].clear();

	return S_OK;
}

CComponent * CComponentManager::Find_Component(_uint iSceneID, const _tchar * pPrototypeTag)
{
	auto iter = find_if(m_pPrototype[iSceneID].begin(), m_pPrototype[iSceneID].end(), CFinder_Tag(pPrototypeTag));

	if (iter == m_pPrototype[iSceneID].end())
		return nullptr;

	return iter->second;
}

void CComponentManager::Free()
{
	for (size_t i = 0; i < m_iNumScene; i++)
	{
		for (auto& Pair : m_pPrototype[i])
			Safe_Release(Pair.second);

		m_pPrototype[i].clear();
	}

	Safe_Delete_Array(m_pPrototype);
}
