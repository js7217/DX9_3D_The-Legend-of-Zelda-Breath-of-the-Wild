#include "stdafx.h"
#include "Terrain.h"


CTerrain::CTerrain(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice	), m_pPhysx(GET_INSTANCE(CPhysXMgr)),
	m_pRenderer(nullptr),	m_pShader(nullptr),	m_pTexture(nullptr), m_pTransform(nullptr), m_pBuffer(nullptr),
	m_pFilterTexture(nullptr)
{
	Safe_AddRef(m_pPhysx);
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs), m_pPhysx(rhs.m_pPhysx),
	m_pRenderer(nullptr), m_pShader(nullptr), m_pTexture(nullptr), m_pTransform(nullptr), m_pBuffer(nullptr),
	m_pFilterTexture(nullptr)
{
	Safe_AddRef(m_pPhysx);
}

HRESULT CTerrain::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CTerrain::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	//if (FAILED(Ready_Splatting()))
	//	return E_FAIL;
	m_pIndex = new ENGINE::INDEX32[m_pBuffer->Get_TriCnt()];

	if (FAILED(Load_SplattingMap()))
		return E_FAIL;

	if (FAILED(Load_SplattingIdx()))
		return E_FAIL;

	m_pPhysx->Create_PhysxPlane();

	return S_OK;
}

_int CTerrain::Update_GameObject(const _double & TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_pRenderer->Set_Ray(FALSE);

	return NO_EVENT;
}

_int CTerrain::LateUpdate_GameObject(const _double & TimeDelta)
{

	//m_pBuffer->Culling(m_pTransform, m_pFrustumCom);


	CGameObject::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CTerrain::Render_GameObject()
{


	if (nullptr == m_pShader || nullptr == m_pBuffer)
		return END_EVENT;

	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	if (nullptr == pEffect)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(SetUp_ConstantTable(pEffect)))
		return E_FAIL;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	
	m_pBuffer->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
	
	return S_OK;
}

HRESULT CTerrain::Load_SplattingMap()
{
	_uint		iNumVerticesX, iNumVerticesZ;

	HANDLE		hFile = 0;
	_ulong		dwByte = 0;

	hFile = CreateFile(L"../../Resources/Filter.bmp", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;
	//불러올때 
	BITMAPFILEHEADER	m_fh;
	BITMAPINFOHEADER	m_ih;
	// 파일정보
	ReadFile(hFile, &m_fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);

	// 이미지정보
	ReadFile(hFile, &m_ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	iNumVerticesX = m_ih.biWidth;
	iNumVerticesZ = m_ih.biHeight;

	_ulong*	m_pPixel = new _ulong[iNumVerticesX * iNumVerticesZ];

	// 픽셀정보
	ReadFile(hFile, m_pPixel, sizeof(_ulong) * (iNumVerticesX * iNumVerticesZ), &dwByte, nullptr);

	Safe_Release(m_pFilterTexture);

	if (FAILED(D3DXCreateTexture(m_pDevice, iNumVerticesX, iNumVerticesZ, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pFilterTexture)))
		return E_FAIL;

	D3DLOCKED_RECT		LockedRect;

	m_pFilterTexture->LockRect(0, &LockedRect, nullptr, 0);
	_uint		iHeightIdx = (iNumVerticesX) * (iNumVerticesZ);
	for (_uint i = 0; i < iNumVerticesZ; ++i)
	{
		iHeightIdx = (iNumVerticesX) * (iNumVerticesZ);
		iHeightIdx -= (iNumVerticesX * (i + 1)) + 1;

		for (_uint j = 0; j < iNumVerticesX; ++j)
		{
			iHeightIdx++;
			_uint iIndex = i * iNumVerticesX + j;
			((_ulong*)LockedRect.pBits)[iIndex] = m_pPixel[iHeightIdx];
		}
	}

	m_pFilterTexture->UnlockRect(0);

	CloseHandle(hFile);
	//Update_Splatting();
	Safe_Delete_Array(m_pPixel);

	return NOERROR;
}

HRESULT CTerrain::Load_SplattingIdx()
{
	HANDLE hFile = CreateFile(__T("../../Resources/Data/SplaattingTexIdx.dat"),
		GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	// OPEN_EXISTING - 이미 존재하는 파일을 연다 

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"안된다", L"Read", MB_OK);
		return E_FAIL;
	}

	DWORD dwBytes = 0;
	_uint iIdx = 0;

	ReadFile(hFile, &(m_iSplattingTexIdxR), sizeof(_uint), &dwBytes, nullptr);
	ReadFile(hFile, &(m_iSplattingTexIdxG), sizeof(_uint), &dwBytes, nullptr);
	ReadFile(hFile, &(m_iSplattingTexIdxB), sizeof(_uint), &dwBytes, nullptr);

	CloseHandle(hFile);

	return NOERROR;
}

HRESULT CTerrain::Add_Component()
{
	// For.Com_Transform
	CTransform::STATUS			Status;
	Status.MoveSpeed = 1.f;
	Status.RotationSpeed = D3DXToRadian(90.0f);

	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransform, &Status)))
		return E_FAIL;
		
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Terrain", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(),
		L"Component_Buffer_Terrain", L"Com_Buffer", (CComponent**)&m_pBuffer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_Terrain", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;
	// For.Com_Slatting_Tex
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_Terrain_Splatting", L"Com_Splatting", (CComponent**)&m_pSplattingTexCom)))
		return E_FAIL;
	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;
	//

	//if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
	//	L"Component_Texture_TerrainNormal", L"Com_NormalTex", (CComponent**)&m_pNormalTexCom)))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CTerrain::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect ||
		nullptr == m_pTexture ||
		nullptr == m_pTransform)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(m_pTransform->SetUp_OnShader(pEffect, "g_matWorld")))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matView", &Get_Transform(D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matProj", &Get_Transform(D3DTS_PROJECTION))))
		return E_FAIL;

	pEffect->SetVector("g_vLightDir", &_vec4(1.f, -1.f, 1.f, 0.f));
	pEffect->SetVector("g_vLightDiffuse", &_vec4(1.f, 1.f, 1.f, 1.f));

	if (FAILED(m_pTexture->SetUp_OnShader(pEffect, "g_SourTexture", 0)))
		return E_FAIL;
	//if (FAILED(m_pTexture->SetUp_OnShader(pEffect, "g_DestTexture", 1)))
	//	return E_FAIL;

	//r 
	if (FAILED(m_pSplattingTexCom->SetUp_OnShader(pEffect, "g_DestTexture", m_iSplattingTexIdxR)))
		return E_FAIL;

	//if (FAILED(m_pNormalTexCom->SetUp_OnShader(pEffect, "g_DestNormalTexture", m_iSplattingTexIdxR)))
	//	return E_FAIL;

	//g
	if (FAILED(m_pSplattingTexCom->SetUp_OnShader(pEffect, "g_DestTexture1", m_iSplattingTexIdxG)))
		return E_FAIL;
	//b
	if (FAILED(m_pSplattingTexCom->SetUp_OnShader(pEffect, "g_DestTexture2", m_iSplattingTexIdxB)))
		return E_FAIL;

	if (FAILED(pEffect->SetTexture("g_FilterTexture", m_pFilterTexture)))
		return E_FAIL;

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CTerrain::Ready_Splatting()
{
	if (FAILED(D3DXCreateTexture(m_pDevice, 129, 129, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pFilterTexture)))
		return E_FAIL;

	D3DLOCKED_RECT		LockedRect;

	m_pFilterTexture->LockRect(0, &LockedRect, nullptr, 0);

	for (size_t i = 0; i < 129; ++i)
	{
		for (size_t j = 0; j < 129; ++j)
		{
			size_t iIndex = i * 129 + j;

			if (j < 64)
				((_ulong*)LockedRect.pBits)[iIndex] = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			else
				((_ulong*)LockedRect.pBits)[iIndex] = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);

		}
	}

	m_pFilterTexture->UnlockRect(0);

	if (FAILED(D3DXSaveTextureToFile(L"../../Resources/Filter.bmp", D3DXIFF_BMP, m_pFilterTexture, nullptr)))
		return E_FAIL;

	return S_OK;
}

CGameObject * CTerrain::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CTerrain(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CTerrain::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);
	
	return pInstance;
}

void CTerrain::Free()
{
	Safe_Delete_Array(m_pIndex);

	Safe_Release(m_pFrustumCom);

	Safe_Release(m_pFilterTexture);
	Safe_Release(m_pPhysx);
	//Safe_Release(m_pNormalTexCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pSplattingTexCom);
	Safe_Release(m_pTransform);
	Safe_Release(m_pBuffer);

	CGameObject::Free();
}
