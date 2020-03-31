#include "LightManager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLightManager)

CLightManager::CLightManager()
{
}

void CLightManager::Set_Activate(_uint iIndex, _bool bAct)
{
	Get_Light(iIndex)->Set_Activate(bAct);
}

_bool CLightManager::Get_Activate(_uint iIndex)
{
	return Get_Light(iIndex)->Get_Activate();
}

HRESULT CLightManager::Add_Light(LPDIRECT3DDEVICE9 pDevice, const D3DLIGHT9 & LightInfo)
{
	CLight* pLight = CLight::Create(pDevice, LightInfo);

	if (nullptr == pLight)
		return E_FAIL;

	m_LightList.emplace_back(pLight);

	return S_OK;
}

HRESULT CLightManager::Render_Light_Manager(LPD3DXEFFECT pEffect)
{
	for (auto& pLight : m_LightList)
	{
		if (nullptr != pLight)
			pLight->Render_Light(pEffect);
	}

	return NOERROR;
}

const D3DLIGHT9 * CLightManager::Get_LightInfo(const _uint & iIndex)
{
	if (m_LightList.empty())
		return nullptr;

	auto iter = m_LightList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;


	return (*iter)->Get_LightInfo();
}

CLight * CLightManager::Get_Light(const _uint & iIndex)
{
	auto iter = m_LightList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;


	return (*iter);
}

void CLightManager::Free()
{
	for (auto& pLight : m_LightList)
		Safe_Release(pLight);

	m_LightList.clear();
}
