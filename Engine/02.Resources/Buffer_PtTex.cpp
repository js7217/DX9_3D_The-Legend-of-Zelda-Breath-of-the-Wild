#include "Buffer_PtTex.h"
#include "Transform.h"

CBuffer_PtTex::CBuffer_PtTex(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{

}

CBuffer_PtTex::CBuffer_PtTex(const CBuffer_PtTex & rhs)
	: CVIBuffer(rhs)
	, m_pVBTransform(rhs.m_pVBTransform)
	, m_pDeclaration(rhs.m_pDeclaration)
{
	Safe_AddRef(m_pVBTransform);
	Safe_AddRef(m_pDeclaration);
}

HRESULT CBuffer_PtTex::Ready_VIBuffer_Clone(void* Points)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (Points == nullptr)
		return E_FAIL;

	particle = ((PARTICLES*)Points);

	m_PtCnt = particle->iParticleSize;

	particlePointer.pDir = new _vec3[m_PtCnt];
	particlePointer.pAccel = new _vec3[m_PtCnt];
	particlePointer.pSpeed = new _float[m_PtCnt];
	particlePointer.pSize = new _float[m_PtCnt];
	particlePointer.pAddAccel = new _vec3[m_PtCnt];
	particlePointer.pFadeColor = new _vec4[m_PtCnt];
	particlePointer.pStartTime = new _float[m_PtCnt];
	particlePointer.pFadeSpeed = new _float[m_PtCnt];
	particlePointer.pMove = new _bool[m_PtCnt];
	particlePointer.pAngleSpeed = new _float[m_PtCnt];

	for (_uint i = 0; i < m_PtCnt; ++i)
	{
		particlePointer.pMove[i] = FALSE;

		particlePointer.pAngleSpeed[i] = D3DXToRadian(GetRandom(particle->fMinAngleSpeed, particle->fMaxAngleSpeed));

		particlePointer.pFadeSpeed[i] = GetRandom(particle->fMinFadeSpeed, particle->fMaxFadeSpeed);

		particlePointer.pStartTime[i] = GetRandom(particle->fMinStartTime, particle->fMaxStartTime);

		particlePointer.pSpeed[i] = GetRandom(particle->fMinSpeed, particle->fMaxSpeed);

		particlePointer.pAccel[i] =
			_vec3(GetRandom(particle->vMinAccel.x, particle->vMaxAccel.x),
				GetRandom(particle->vMinAccel.y, particle->vMaxAccel.y),
				GetRandom(particle->vMinAccel.z, particle->vMaxAccel.z));

		particlePointer.pAddAccel[i] = particlePointer.pAccel[i];

		particlePointer.pDir[i] =
			_vec3(GetRandom(particle->vMinDir.x, particle->vMaxDir.x),
				GetRandom(particle->vMinDir.y, particle->vMaxDir.y),
				GetRandom(particle->vMinDir.z, particle->vMaxDir.z));

		particlePointer.pFadeColor[i] =
			_vec4(GetRandom(particle->vMinColorFade.x, particle->vMaxColorFade.x),
				GetRandom(particle->vMinColorFade.y, particle->vMaxColorFade.y),
				GetRandom(particle->vMinColorFade.z, particle->vMaxColorFade.z),
				GetRandom(particle->vMinColorFade.w, particle->vMaxColorFade.w));

		particlePointer.pSize[i] = GetRandom(particle->vMinSize.x, particle->vMaxSize.x);

		D3DXVec3Normalize(&particlePointer.pDir[i], &particlePointer.pDir[i]);
	}

	m_iStride = sizeof(VTXPTTEX);
	m_iNumVertices = m_PtCnt;

	m_dwFVF = 0;
	m_iNumPolygons = m_PtCnt;

	m_iPolygonSize = sizeof(INDEX16);
	m_eFormat = D3DFMT_INDEX16;

	// 정점버퍼를 할당한다 + 인덱스버퍼를 할당한다.
	if (FAILED(CVIBuffer::Ready_VIBuffer()))
		return E_FAIL;

	VTXPTTEX*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	for (_uint i = 0; i < m_PtCnt; ++i)
	{
		pVertices[i].vPosition = _vec3(0, 0, 0);
		m_pPosition[i] = pVertices[i].vPosition;

		pVertices[i].fPSize = GetRandom(particle->vMinInitSize.x, particle->vMaxInitSize.x);
		pVertices[i].fPSize1 = D3DXToRadian(GetRandom(particle->fMinAngle, particle->fMaxAngle));

		if (pVertices[i].fPSize < particlePointer.pSize[i])
			particle->bSizePlus = TRUE;
		else
			particle->bSizePlus = FALSE;

		pVertices[i].vInitColor =
			D3DXCOLOR(GetRandom(particle->vInitColorMin.x, particle->vInitColorMax.x),
				GetRandom(particle->vInitColorMin.y, particle->vInitColorMax.y),
				GetRandom(particle->vInitColorMin.z, particle->vInitColorMax.z),
				GetRandom(particle->vInitColorMin.w, particle->vInitColorMax.w));

		pVertices[i].vTexUV = D3DXVECTOR2(0, 0);
	}

	m_pVB->Unlock();

	if (FAILED(m_pDevice->CreateVertexBuffer(sizeof(VTXMATRIX) * m_PtCnt, 0, 0, D3DPOOL_MANAGED, &m_pVBTransform, nullptr)))
		return E_FAIL;

	D3DXMATRIX		matPivot;
	D3DXMatrixIdentity(&matPivot);

	VTXMATRIX*		pVertices_Matrix = nullptr;

	m_pVBTransform->Lock(0, 0, (void**)&pVertices_Matrix, 0);

	for (_uint i = 0; i < m_PtCnt; ++i)
	{
		D3DXMatrixTranslation(&matPivot, GetRandom(particle->vMinPos.x, particle->vMaxPos.x), GetRandom(particle->vMinPos.y, particle->vMaxPos.y), GetRandom(particle->vMinPos.z, particle->vMaxPos.z));
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

	m_pIB->Unlock();

	D3DVERTEXELEMENT9      Element[MAX_FVF_DECL_SIZE] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_PSIZE, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, 40, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_PSIZE, 1 },

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

HRESULT CBuffer_PtTex::Update_Buffer(const _float& fTimeDelta)
{
	m_fStartTime += fTimeDelta;

	VTXPTTEX*		pVertices = nullptr;
	VTXMATRIX*		pVertices_Matrix = nullptr;

	m_pVBTransform->Lock(0, 0, (void**)&pVertices_Matrix, 0);
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	for (_uint i = 0; i < m_PtCnt; ++i)
	{
		if (particlePointer.pStartTime[i] >= m_fStartTime)
		{
			pVertices[i].vInitColor.a = 0;
			continue;
		}

		particlePointer.pAddAccel[i] += particlePointer.pAccel[i];

		if (particlePointer.pMove[i] == FALSE)
		{
			if (particle->bActStop)
			{
				if ((particlePointer.pSpeed[i] + particlePointer.pAddAccel[i].x) > 1 || particlePointer.pSpeed[i] + particlePointer.pAddAccel[i].x < -1)
					pVertices_Matrix[i].vPosition += (_vec4(particlePointer.pDir[i].x, particlePointer.pDir[i].y, particlePointer.pDir[i].z, 0)
						* fTimeDelta * (particlePointer.pSpeed[i] + particlePointer.pAddAccel[i].x));
				else
				{
					particlePointer.pMove[i] = TRUE;
				}
			}
			else
				pVertices_Matrix[i].vPosition += (_vec4(particlePointer.pDir[i].x, particlePointer.pDir[i].y, particlePointer.pDir[i].z, 0)
					* fTimeDelta * (particlePointer.pSpeed[i] + particlePointer.pAddAccel[i].x));
		}
		if (particle->bSizePlus)
		{
			if (pVertices[i].fPSize <= particlePointer.pSize[i])
				pVertices[i].fPSize += particle->vMinSize.y;
		}
		else
		{
			if (pVertices[i].fPSize >= particlePointer.pSize[i])
				pVertices[i].fPSize -= particle->vMinSize.y;
		}
		//////////////////////////////////////////////////////////////////////////

		pVertices[i].fPSize1 += particlePointer.pAngleSpeed[i] * fTimeDelta;

		//////////////////////////////////////////////////////////////////////////

		if (pVertices[i].vInitColor.r > particlePointer.pFadeColor[i].x)
			pVertices[i].vInitColor.r -= particlePointer.pFadeSpeed[i];
		else if (pVertices[i].vInitColor.r < particlePointer.pFadeColor[i].x)
			pVertices[i].vInitColor.r += particlePointer.pFadeSpeed[i];

		if (pVertices[i].vInitColor.g > particlePointer.pFadeColor[i].y)
			pVertices[i].vInitColor.g -= particlePointer.pFadeSpeed[i];
		else if (pVertices[i].vInitColor.g < particlePointer.pFadeColor[i].y)
			pVertices[i].vInitColor.g += particlePointer.pFadeSpeed[i];

		if (pVertices[i].vInitColor.b > particlePointer.pFadeColor[i].z)
			pVertices[i].vInitColor.b -= particlePointer.pFadeSpeed[i];
		else if (pVertices[i].vInitColor.b < particlePointer.pFadeColor[i].z)
			pVertices[i].vInitColor.b += particlePointer.pFadeSpeed[i];

		if (pVertices[i].vInitColor.a > particlePointer.pFadeColor[i].w)
			pVertices[i].vInitColor.a -= particlePointer.pFadeSpeed[i];
		else if (pVertices[i].vInitColor.a < particlePointer.pFadeColor[i].w)
			pVertices[i].vInitColor.a += particlePointer.pFadeSpeed[i];

		if (pVertices[i].vInitColor.r <= 0.f)
			pVertices[i].vInitColor.r = 0.f;
		if (pVertices[i].vInitColor.g <= 0.f)
			pVertices[i].vInitColor.g = 0.f;
		if (pVertices[i].vInitColor.b <= 0.f)
			pVertices[i].vInitColor.b = 0.f;
		if (pVertices[i].vInitColor.a <= 0.f)
			pVertices[i].vInitColor.a = 0.f;

		//////////////////////////////////////////////////////////////////////////
		if (particle->bEnvironment)
		{
			_vec3 vPos = { 0,0,0 };
			if (m_pMat != nullptr)
				vPos = { m_pMat->_41, m_pMat->_42, m_pMat->_43 };

			if (pVertices_Matrix[i].vPosition.y <= particle->vMinPos.y + vPos.y || pVertices_Matrix[i].vPosition.y >= particle->vMaxPos.y + vPos.y)
			{
				pVertices_Matrix[i].vPosition =
					_vec4(GetRandom(particle->vMinPos.x, particle->vMaxPos.x) + vPos.x,
						GetRandom(particle->vMinPos.y, particle->vMaxPos.y) + vPos.y,
						GetRandom(particle->vMinPos.z, particle->vMaxPos.z) + vPos.z, 1);

				pVertices[i].fPSize = GetRandom(particle->vMinInitSize.x, particle->vMaxInitSize.x);

				particlePointer.pStartTime[i] = GetRandom(particle->fMinStartTime, particle->fMaxStartTime);

				particlePointer.pAddAccel[i] = _vec3(0, 0, 0);
			}
		}
	}

	m_pVBTransform->Unlock();
	m_pVB->Unlock();

	m_bOnce = FALSE;

	return NOERROR;
}

HRESULT CBuffer_PtTex::ResetPos()
{
	if (!m_bOnce)
	{
		VTXPTTEX*		pVertices = nullptr;
		VTXMATRIX*		pVertices_Matrix = nullptr;

		m_pVBTransform->Lock(0, 0, (void**)&pVertices_Matrix, 0);
		m_pVB->Lock(0, 0, (void**)&pVertices, 0);

		for (_uint i = 0; i < m_PtCnt; ++i)
		{
			particlePointer.pStartTime[i] = GetRandom(particle->fMinStartTime, particle->fMaxStartTime);

			particlePointer.pAddAccel[i] = _vec3(0, 0, 0);
			pVertices_Matrix[i].vPosition = _vec4(GetRandom(particle->vMinPos.x, particle->vMaxPos.x), GetRandom(particle->vMinPos.y, particle->vMaxPos.y), GetRandom(particle->vMinPos.z, particle->vMaxPos.z), 1);

			pVertices[i].fPSize = GetRandom(particle->vMinInitSize.x, particle->vMaxInitSize.x);

			pVertices[i].fPSize1 = D3DXToRadian(GetRandom(particle->fMinAngle, particle->fMaxAngle));
			particlePointer.pAngleSpeed[i] = D3DXToRadian(GetRandom(particle->fMinAngleSpeed, particle->fMaxAngleSpeed));

			pVertices[i].vInitColor =
				D3DXCOLOR(GetRandom(particle->vInitColorMin.x, particle->vInitColorMax.x),
					GetRandom(particle->vInitColorMin.y, particle->vInitColorMax.y),
					GetRandom(particle->vInitColorMin.z, particle->vInitColorMax.z),
					GetRandom(particle->vInitColorMin.w, particle->vInitColorMax.w));

			particlePointer.pFadeColor[i] =
				_vec4(GetRandom(particle->vMinColorFade.x, particle->vMaxColorFade.x),
					GetRandom(particle->vMinColorFade.y, particle->vMaxColorFade.y),
					GetRandom(particle->vMinColorFade.z, particle->vMaxColorFade.z),
					GetRandom(particle->vMinColorFade.w, particle->vMaxColorFade.w));

			particlePointer.pAddAccel[i] = _vec3(0, 0, 0);

			particlePointer.pMove[i] = FALSE;

			particlePointer.pFadeSpeed[i] = GetRandom(particle->fMinFadeSpeed, particle->fMaxFadeSpeed);

			particlePointer.pSize[i] = GetRandom(particle->vMinSize.x, particle->vMaxSize.x);

			m_fStartTime = 0.f;
		}

		m_pVBTransform->Unlock();
		m_pVB->Unlock();

		m_bOnce = TRUE;
	}

	return NOERROR;
}

HRESULT CBuffer_PtTex::ResetInfo()
{
	VTXPTTEX*		pVertices = nullptr;
	VTXMATRIX*		pVertices_Matrix = nullptr;

	m_pVBTransform->Lock(0, 0, (void**)&pVertices_Matrix, 0);
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	for (_uint i = 0; i < m_PtCnt; ++i)
	{
		particlePointer.pStartTime[i] = GetRandom(particle->fMinStartTime, particle->fMaxStartTime);

		pVertices_Matrix[i].vPosition =
			_vec4(GetRandom(particle->vMinPos.x, particle->vMaxPos.x),
				GetRandom(particle->vMinPos.y, particle->vMaxPos.y),
				GetRandom(particle->vMinPos.z, particle->vMaxPos.z), 1);

		particlePointer.pSpeed[i] = GetRandom(particle->fMinSpeed, particle->fMaxSpeed);

		particlePointer.pMove[i] = FALSE;

		particlePointer.pAccel[i] =
			_vec3(GetRandom(particle->vMinAccel.x, particle->vMaxAccel.x),
				GetRandom(particle->vMinAccel.y, particle->vMaxAccel.y),
				GetRandom(particle->vMinAccel.z, particle->vMaxAccel.z));

		particlePointer.pAddAccel[i] = { 0,0,0 };

		particlePointer.pDir[i] =
			_vec3(GetRandom(particle->vMinDir.x, particle->vMaxDir.x),
				GetRandom(particle->vMinDir.y, particle->vMaxDir.y),
				GetRandom(particle->vMinDir.z, particle->vMaxDir.z));

		pVertices[i].vInitColor =
			D3DXCOLOR(GetRandom(particle->vInitColorMin.x, particle->vInitColorMax.x),
				GetRandom(particle->vInitColorMin.y, particle->vInitColorMax.y),
				GetRandom(particle->vInitColorMin.z, particle->vInitColorMax.z),
				GetRandom(particle->vInitColorMin.w, particle->vInitColorMax.w));

		particlePointer.pFadeColor[i] =
			_vec4(GetRandom(particle->vMinColorFade.x, particle->vMaxColorFade.x),
				GetRandom(particle->vMinColorFade.y, particle->vMaxColorFade.y),
				GetRandom(particle->vMinColorFade.z, particle->vMaxColorFade.z),
				GetRandom(particle->vMinColorFade.w, particle->vMaxColorFade.w));

		pVertices[i].fPSize = GetRandom(particle->vMinInitSize.x, particle->vMaxInitSize.x);

		D3DXVec3Normalize(&particlePointer.pDir[i], &particlePointer.pDir[i]);

		particlePointer.pFadeSpeed[i] = GetRandom(particle->fMinFadeSpeed, particle->fMaxFadeSpeed);

		pVertices[i].fPSize1 = D3DXToRadian(GetRandom(particle->fMinAngle, particle->fMaxAngle));
		particlePointer.pAngleSpeed[i] = D3DXToRadian(GetRandom(particle->fMinAngleSpeed, particle->fMaxAngleSpeed));

		particlePointer.pSize[i] = GetRandom(particle->vMinSize.x, particle->vMaxSize.x);

		m_fStartTime = 0.f;
	}

	m_pVBTransform->Unlock();
	m_pVB->Unlock();

	return NOERROR;
}

void CBuffer_PtTex::Render_Buffer(const CTransform* pTransform)
{
	if (nullptr == m_pDevice)
		return;

	m_pDevice->SetVertexDeclaration(m_pDeclaration);

	m_pDevice->SetStreamSourceFreq(0, 1);
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_iStride);

	m_pDevice->SetStreamSourceFreq(1, 1);
	m_pDevice->SetStreamSource(1, m_pVBTransform, 0, sizeof(VTXMATRIX));

	m_pDevice->DrawPrimitive(D3DPT_POINTLIST, 0, m_iNumVertices);

	m_pDevice->SetStreamSourceFreq(0, 1);
	m_pDevice->SetStreamSourceFreq(1, 1);
}

CBuffer_PtTex * CBuffer_PtTex::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return new CBuffer_PtTex(pGraphic_Device);
}

CComponent * CBuffer_PtTex::Clone_Component(void* pArg)
{
	CBuffer_PtTex* pInstance = new CBuffer_PtTex(*this);

	if (FAILED(pInstance->Ready_VIBuffer_Clone(pArg)))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuffer_PtTex::Free()
{
	Safe_Delete_Array(m_pPosition);
	Safe_Delete_Array(m_pIndices);

	Safe_Release(m_pVBTransform);
	Safe_Release(m_pDeclaration);

	CVIBuffer::Free();
}
