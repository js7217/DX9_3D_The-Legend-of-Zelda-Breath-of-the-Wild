#include "GameObject.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pDevice)
	:m_pDevice(pDevice)
{
	Safe_AddRef(m_pDevice);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
{
	m_bGrabState = FALSE;
	Safe_AddRef(m_pDevice);
}

void CGameObject::Erase_Component(const _tchar * pComponentTag)
{
	CComponent* pComponent = Find_Component(pComponentTag);
	Safe_Release(pComponent);
	m_MapComponent.erase(pComponentTag);
}

CComponent * CGameObject::Get_Component(const _tchar * pComponentTag)
{
	CComponent*	pComponent = Find_Component(pComponentTag);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

void CGameObject::Render_Shadow(_int iPass)
{
}

HRESULT CGameObject::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CGameObject::Ready_GameObject(void * pArg)
{

	return S_OK;
}

HRESULT CGameObject::Compute_ViewZ(CTransform * pTransform)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	_matrix			matView;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3		vDir = (pTransform->Get_State(CTransform::STATE_POSITION) - *(_vec3*)&matView.m[3][0]);

	m_fViewZ = D3DXVec3Length(&vDir);

	return NOERROR;
}

_int CGameObject::End_GameObject()
{
	if (m_bEndEvent)
		return -1;

	return 0;
}

_int CGameObject::Update_GameObject(const _double & TimeDelta)
{
	for (auto& Comp : m_MapComponent)
		Comp.second->Update_Component(TimeDelta);

	return _int();
}

_int CGameObject::LateUpdate_GameObject(const _double & TimeDelta)
{
	for (auto& Comp : m_MapComponent)
		Comp.second->LateUpdate_Component(TimeDelta);

	return _int();
}

HRESULT CGameObject::Render_GameObject()
{

	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iSCeneID, const _tchar * pProtoTag, const _tchar * pComponentTag, CComponent ** ppOutComponent, void * pArg)
{
	CComponent* pComponent = Find_Component(pComponentTag);

	if (nullptr != pComponent)
		return E_FAIL;

	CManagement* pManagement = GET_INSTANCE(CManagement);

	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);
	/////////////////////////////////////

	if (FAILED(pManagement->Clone_Component(iSCeneID, pProtoTag, ppOutComponent, pArg)))
	{
		Safe_Release(pManagement);
		return E_FAIL;
	};
	//return E_FAIL;

	m_MapComponent.insert(COMPONENTS::value_type(pComponentTag, *ppOutComponent));

	/////////////////////////////////////
	Safe_AddRef(*ppOutComponent);

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CGameObject::Add_Component_Mag(_uint iSCeneID, const _tchar * pProtoTag, const _tchar * pComponentTag, CComponent ** ppOutComponent, void * pArg)
{
	CComponent* pComponent = nullptr;

	if (nullptr != pComponent)
		return E_FAIL;

	CManagement* pManagement = GET_INSTANCE(CManagement);

	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);
	/////////////////////////////////////

	if (FAILED(pManagement->Clone_Component(iSCeneID, pProtoTag, ppOutComponent, pArg)))
	{
		Safe_Release(pManagement);
		return E_FAIL;
	};
	//return E_FAIL;

	m_MapComponent.insert(COMPONENTS::value_type(pComponentTag, *ppOutComponent));

	/////////////////////////////////////
	Safe_AddRef(*ppOutComponent);

	Safe_Release(pManagement);

	return S_OK;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto iter = find_if(m_MapComponent.begin(), m_MapComponent.end(), CFinder_Tag(pComponentTag));

	if (iter == m_MapComponent.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& Pair : m_MapComponent)
 		Safe_Release(Pair.second);

	m_MapComponent.clear();

	Safe_Release(m_pDevice);
}
