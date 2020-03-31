#include "Component.h"

CComponent::CComponent(LPDIRECT3DDEVICE9 pDevice)
	: m_pDevice(pDevice),
	m_isClone(FALSE)
{
	Safe_AddRef(m_pDevice);
}

CComponent::CComponent(const CComponent & rhs)
	: m_pDevice(rhs.m_pDevice),
	m_isClone(TRUE)
{
	Safe_AddRef(m_pDevice);
}

HRESULT CComponent::Ready_Component()
{

	return S_OK;
}

void CComponent::Free()
{
	Safe_Release(m_pDevice);
}
