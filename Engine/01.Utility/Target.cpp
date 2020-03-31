#include "Target.h"

CTarget::CTarget(LPDIRECT3DDEVICE9 pDevice)
	: m_pDevice(pDevice)
{
	Safe_AddRef(m_pDevice);
}

HRESULT CTarget::Ready_Target(_uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor)
{
	if (FAILED(D3DXCreateTexture(m_pDevice, Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, &m_pTarget_Texture)))
		return E_FAIL;

	if (FAILED(m_pTarget_Texture->GetSurfaceLevel(0, &m_pTarget_Surface)))
		return E_FAIL;

	m_ClearColor = ClearColor;

	return NOERROR;
}

HRESULT CTarget::Clear_Target(_bool bClear)
{
	SetUp_OnGraphicDev(0);

	if (!bClear)
	{
		if (FAILED(m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET, m_ClearColor, 1.f, 0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET, m_ClearColor, 1.f, 0)))
			return E_FAIL;
	}

	Release_OnGraphicDev(0);

	return NOERROR;
}

HRESULT CTarget::SetUp_OnGraphicDev(_ulong dwIndex)
{
	if (nullptr == m_pTarget_Surface ||
		nullptr == m_pDevice)
		return E_FAIL;


	m_pDevice->GetRenderTarget(dwIndex, &m_pOld_Surface);

	if (FAILED(m_pDevice->SetRenderTarget(dwIndex, m_pTarget_Surface)))
		return E_FAIL;	

	return NOERROR;
}

HRESULT CTarget::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName)
{
	return pEffect->SetTexture(pConstantName, m_pTarget_Texture);	
}

HRESULT CTarget::Release_OnGraphicDev(_ulong dwIndex)
{
	if (nullptr == m_pTarget_Surface ||
		nullptr == m_pDevice)
		return E_FAIL;
		
	m_pDevice->SetRenderTarget(dwIndex, m_pOld_Surface);

	Safe_Release(m_pOld_Surface);

	//if (GetKeyState(VK_RETURN) & 0x8000)
	//{
	//	if (0 == dwIndex)
	//		D3DXSaveTextureToFile(L"../Bin/Diffuse.jpg", D3DXIFF_JPG, m_pTarget_Texture, nullptr);
	//	if (1 == dwIndex)
	//		D3DXSaveTextureToFile(L"../Bin/Normal.jpg", D3DXIFF_JPG, m_pTarget_Texture, nullptr);

	//}

	return NOERROR;
}

HRESULT CTarget::Ready_Debug_Buffer(_float fStartX, _float fStartY, _float fSizeX, _float fSizeY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;	

	if (FAILED(m_pDevice->CreateVertexBuffer(sizeof(VTXSCREEN) * 4, 0, FVF_SCREEN, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXSCREEN*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _vec4(fStartX, fStartY, 0.0f, 1.f);
	pVertices[0].vTexUV = _vec2(0.0f, 0.f);

	pVertices[1].vPosition = _vec4(fStartX + fSizeX, fStartY, 0.0f, 1.f);
	pVertices[1].vTexUV = _vec2(1.0f, 0.f);

	pVertices[2].vPosition = _vec4(fStartX + fSizeX, fStartY + fSizeY, 0.0f, 1.f);
	pVertices[2].vTexUV = _vec2(1.0f, 1.f);

	pVertices[3].vPosition = _vec4(fStartX, fStartY + fSizeY, 0.0f, 1.f);
	pVertices[3].vTexUV = _vec2(0.0f, 1.f);
	
	m_pVB->Unlock();

	if (FAILED(m_pDevice->CreateIndexBuffer(sizeof(POLYGON16) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	POLYGON16*		pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();

	return NOERROR;
}

HRESULT CTarget::Render_Debug_Buffer()
{
	if (nullptr == m_pDevice ||
		nullptr == m_pVB ||
		nullptr == m_pIB)
		return E_FAIL;

	m_pDevice->SetTexture(0, m_pTarget_Texture);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	return NOERROR;
}


CTarget * CTarget::Create(LPDIRECT3DDEVICE9 pDevice, _uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor)
{
	CTarget*	pInstance = new CTarget(pDevice);

	if (FAILED(pInstance->Ready_Target(Width, Height, Format, ClearColor)))
	{
		MSG_BOX("Failed to Create CTarget Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTarget::Free()
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	Safe_Release(m_pDevice);
	Safe_Release(m_pTarget_Surface);
	Safe_Release(m_pTarget_Texture);

}
