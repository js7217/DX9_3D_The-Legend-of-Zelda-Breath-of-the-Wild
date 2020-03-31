#include "Texture.h"

CTexture::CTexture(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_vecTexture(rhs.m_vecTexture)	
{
	for (auto& pTexture : m_vecTexture)
		pTexture->AddRef();

	lstrcpy(m_szFileName, rhs.m_szFileName);
}

HRESULT CTexture::Ready_Texture(TEXTURETYPE eType, const _tchar * pFileName, const _uint & iCnt)
{
	lstrcpy(m_szFileName, pFileName);
	
	m_vecTexture.reserve(iCnt);

	for (size_t i = 0; i < iCnt; ++i)
	{
		_tchar			szFullPath[MAX_PATH] = L"";

		wsprintf(szFullPath, pFileName, i);

		IDirect3DBaseTexture9*			pTexture = nullptr;

		HRESULT			hr = 0;

		switch (eType)
		{
		case TYPE_GENERAL:
			hr = D3DXCreateTextureFromFile(m_pDevice, szFullPath, (LPDIRECT3DTEXTURE9*)&pTexture);
			break;
		case TYPE_CUBE:
			hr = D3DXCreateCubeTextureFromFile(m_pDevice, szFullPath, (LPDIRECT3DCUBETEXTURE9*)&pTexture);
			break;
		}

		if (FAILED(hr))
			return E_FAIL;

		m_vecTexture.push_back(pTexture);
	}

	return NOERROR;
}

HRESULT CTexture::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName, const _uint & iIndex)
{
	if (nullptr == pEffect)
		return E_FAIL;
	if (m_vecTexture.size() <= iIndex)
		return E_FAIL;

	pEffect->SetTexture(pConstantName, m_vecTexture[iIndex]);

	return NOERROR;
}

CTexture * CTexture::Create(LPDIRECT3DDEVICE9 pDevice, TEXTURETYPE eType, const _tchar * pFileName, const _uint & iCnt)
{
	CTexture*		pInstance = new CTexture(pDevice);

	if (FAILED(pInstance->Ready_Texture(eType, pFileName, iCnt)))
	{
		MSG_BOX("CTexture Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTexture::Clone_Component(void* pArg)
{
	return new CTexture(*this);
}

void CTexture::Free()
{
	for (auto& pTexture : m_vecTexture)
	{
		Safe_Release(pTexture);
	}

	m_vecTexture.clear();

	CComponent::Free();
}
