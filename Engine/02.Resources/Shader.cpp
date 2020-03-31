#include "Shader.h"

CShader::CShader(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{
}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs),
	m_pEffect(rhs.m_pEffect)
{
	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Ready_Shader(const _tchar * pFileName)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pDevice, pFileName, nullptr, nullptr, 0, nullptr, &m_pEffect, nullptr)))
		return E_FAIL;

	return S_OK;
}

CShader * CShader::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar * pFileName)
{
	CShader* pInstance = new CShader(pDevice);

	if (FAILED(pInstance->Ready_Shader(pFileName)))
	{
		MSG_BOX("CShader Create Failed!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CShader::Clone_Component(void * pArg)
{
	return new CShader(*this);
}

void CShader::Free()
{
	Safe_Release(m_pEffect);

	CComponent::Free();
}

