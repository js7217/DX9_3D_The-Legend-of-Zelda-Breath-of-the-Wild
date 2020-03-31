#include "Layer.h"
#include "GameObject.h"


CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const _tchar * pComponentTag, const _uint & iIndex)
{
	if (m_ObjectList.size() <= iIndex)
		return nullptr;

	auto	iter = m_ObjectList.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_Component(pComponentTag);
}

CGameObject * CLayer::Get_GameObject(const _uint & iIndex)
{
	if (m_ObjectList.size() <= iIndex)
		return nullptr;

	auto	iter = m_ObjectList.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return *iter;
}

HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.emplace_back(pGameObject);

	return S_OK;
}

_int CLayer::Update_Layer(const _double & TimeDelta)
{
	_int iExit = 0;
	OBJECTLIST::iterator iter_End = m_ObjectList.end();
	OBJECTLIST::iterator iter = m_ObjectList.begin();

	for (; iter != iter_End;)
	{
		iExit = (*iter)->Update_GameObject(TimeDelta);

		if (iExit & 0x80000000)	//iExit == END_EVENT(-1);
		{
			Safe_Release(*iter);
			iter = m_ObjectList.erase(iter);
		}
		else
			++iter;
	}


	return iExit;
}

_int CLayer::LateUpdate_Layer(const _double & TimeDelta)
{
	_int iExit = 0;
	OBJECTLIST::iterator iter_End = m_ObjectList.end();
	OBJECTLIST::iterator iter = m_ObjectList.begin();

	for (; iter != iter_End;)
	{
		iExit = (*iter)->LateUpdate_GameObject(TimeDelta);

		if (iExit & 0x80000000)	//iExit == END_EVENT(-1);
		{
			Safe_Release(*iter);
			iter = m_ObjectList.erase(iter);
		}
		else
			++iter;
	}

	return iExit;
}

CLayer * CLayer::Create()
{
	CLayer* pInstance = new CLayer();

	if (FAILED(pInstance->Ready_Layer()))
		Safe_Release(pInstance);

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_ObjectList)
		Safe_Release(pGameObject);

	m_ObjectList.clear();
}
