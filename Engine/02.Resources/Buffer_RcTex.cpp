#include "Buffer_RcTex.h"


CBuffer_RcTex::CBuffer_RcTex(LPDIRECT3DDEVICE9 pDevice)
	: CVIBuffer(pDevice)
{

}

CBuffer_RcTex::CBuffer_RcTex(const CBuffer_RcTex & rhs)
	: CVIBuffer(rhs)
	, m_pDeclaration(rhs.m_pDeclaration)
	, m_pVBTransform(rhs.m_pVBTransform)
{
	Safe_AddRef(m_pDeclaration);
	Safe_AddRef(m_pVBTransform);
}

HRESULT CBuffer_RcTex::Ready_VIBuffer()
{
	m_iNumVertices = 4;
	m_iStride = sizeof(VTXTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;
	//m_pPosition = new _vec3[4];

	m_iNumPolygons = 2;
	m_iPolygonSize = sizeof(POLYGON16);	
	m_eFormat = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_VIBuffer()))
		return E_FAIL;

	VTXTEX*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _vec3(-1.f, 1.f, 0.f);
	pVertices[0].vTexUV = _vec2(0.0f, 0.f);

	pVertices[1].vPosition = _vec3(1.f, 1.f, 0.f);
	pVertices[1].vTexUV = _vec2(1.f, 0.f);

	pVertices[2].vPosition = _vec3(1.f, -1.f, 0.f);
	pVertices[2].vTexUV = _vec2(1.f, 1.f);

	pVertices[3].vPosition = _vec3(-1.f, -1.f, 0.f);
	pVertices[3].vTexUV = _vec2(0.f, 1.f);

	for (size_t i = 0; i < m_iNumVertices; ++i)
		m_pPosition[i] = pVertices[i].vPosition;	

	//pVertices[0].vPosition = _vec3(0.5f, 0.5f, 0.f);
	//m_pPosition[0] = pVertices[0].vPosition;
	//pVertices[0].vTexUV = _vec2(0.0f, 0.f);

	//pVertices[1].vPosition = _vec3(-0.5f, 0.5f, 0.f);
	//m_pPosition[1] = pVertices[1].vPosition;
	//pVertices[1].vTexUV = _vec2(1.0f, 0.f);

	//pVertices[2].vPosition = _vec3(-0.5f, -0.5f, 0.f);
	//m_pPosition[2] = pVertices[2].vPosition;
	//pVertices[2].vTexUV = _vec2(1.0f, 1.f);

	//pVertices[3].vPosition = _vec3(0.5f, -0.5f, 0.f);
	//m_pPosition[3] = pVertices[3].vPosition;
	//pVertices[3].vTexUV = _vec2(0.0f, 1.f);


	m_pVB->Unlock();


	POLYGON16*		pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();

	memcpy(m_pIndices, pIndices, sizeof(INDEX16) * m_iNumPolygons);

	return NOERROR;
}

HRESULT CBuffer_RcTex::Ready_VIBuffer(_uint iNumInstance)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;

	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
	m_iNumPolygons = 2 * iNumInstance;

	m_iPolygonSize = sizeof(INDEX16);
	m_eFormat = D3DFMT_INDEX16;

	// 정점버퍼를 할당한다 + 인덱스버퍼를 할당한다.
	if (FAILED(CVIBuffer::Ready_VIBuffer()))
		return E_FAIL;

	VTXTEX*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);


	pVertices[0].vPosition = _vec3(0.5f, 0.5f, 0.f);
	m_pPosition[0] = pVertices[0].vPosition;
	pVertices[0].vTexUV = _vec2(0.0f, 0.f);

	pVertices[1].vPosition = _vec3(-0.5f, 0.5f, 0.f);
	m_pPosition[1] = pVertices[1].vPosition;
	pVertices[1].vTexUV = _vec2(1.0f, 0.f);

	pVertices[2].vPosition = _vec3(-0.5f, -0.5f, 0.f);
	m_pPosition[2] = pVertices[2].vPosition;
	pVertices[2].vTexUV = _vec2(1.0f, 1.f);

	pVertices[3].vPosition = _vec3(0.5f, -0.5f, 0.f);
	m_pPosition[3] = pVertices[3].vPosition;
	pVertices[3].vTexUV = _vec2(0.0f, 1.f);


	m_pVB->Unlock();

	if (FAILED(m_pDevice->CreateVertexBuffer(sizeof(VTXMATRIX) * 2, 0, 0, D3DPOOL_MANAGED, &m_pVBTransform, nullptr)))
		return E_FAIL;

	D3DXMATRIX		matPivot;
	D3DXMatrixIdentity(&matPivot);

	VTXMATRIX*		pVertices_Matrix = nullptr;


	m_pVBTransform->Lock(0, 0, (void**)&pVertices_Matrix, 0);

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		D3DXMatrixTranslation(&matPivot, 0, (_float)0, 0.f);

		pVertices_Matrix[i].vRight = *(_vec4*)&matPivot.m[0][0];
		pVertices_Matrix[i].vUp = *(_vec4*)&matPivot.m[1][0];
		pVertices_Matrix[i].vLook = *(_vec4*)&matPivot.m[2][0];
		pVertices_Matrix[i].vPosition = *(_vec4*)&matPivot.m[3][0];
	}

	m_pVBTransform->Unlock();


	INDEX16*	pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;


	pIndices[2]._0 = 0;
	pIndices[2]._1 = 1;
	pIndices[2]._2 = 2;

	pIndices[3]._0 = 0;
	pIndices[3]._1 = 2;
	pIndices[3]._2 = 3;



	memcpy(m_pIndices, pIndices, sizeof(INDEX16) * m_iNumPolygons);

	m_pIB->Unlock();

	D3DVERTEXELEMENT9		Element[MAX_FVF_DECL_SIZE] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

		{ 1, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 1, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		{ 1, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
		{ 1, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
		D3DDECL_END()
	};

	if (FAILED(m_pDevice->CreateVertexDeclaration(Element, &m_pDeclaration)))
		return E_FAIL;


	return NOERROR;
}

void CBuffer_RcTex::Render_Buffer(_uint iNumInstance, const CTransform * pTransform)
{
	// 정점버퍼를 몇번 셰이더에 던지며 렌더링할 것인지?! (즉, 그려야하는 객체의 갯수)
	m_pDevice->SetStreamSourceFreq(0, (D3DSTREAMSOURCE_INDEXEDDATA | iNumInstance));
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_iStride);

	// 객체당 몇개의 행렬정보를 사용할 것인지.
	m_pDevice->SetStreamSourceFreq(1, (D3DSTREAMSOURCE_INSTANCEDATA | 1));
	m_pDevice->SetStreamSource(1, m_pVBTransform, 0, sizeof(VTXMATRIX));

	m_pDevice->SetVertexDeclaration(m_pDeclaration);

	m_pDevice->SetIndices(m_pIB);

	// 객체하나를 그리기위해 필요한 정보 
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, 2);

	m_pDevice->SetStreamSourceFreq(0, 1);
	m_pDevice->SetStreamSourceFreq(1, 1);
}

void CBuffer_RcTex::Render_VIBuffer()
{	
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pDevice->SetFVF(m_dwFVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumPolygons);
	// m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumPolygons);
}


CBuffer_RcTex * CBuffer_RcTex::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CBuffer_RcTex*	pInstance = new CBuffer_RcTex(pDevice);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_RcTex Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;

}

CBuffer_RcTex * CBuffer_RcTex::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iNumInstance)
{
	CBuffer_RcTex* pInstance = new CBuffer_RcTex(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer(iNumInstance)))
	{
		MessageBox(0, L"CBuffer_RcTexInstancing Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CBuffer_RcTex::Clone_Component(void* pArg)
{
	return new CBuffer_RcTex(*this);
}

void CBuffer_RcTex::Free()
{
	Safe_Release(m_pDeclaration);
	Safe_Release(m_pVBTransform);

	CVIBuffer::Free();
}
