#include "Mesh_Static_Instancing.h"

USING(ENGINE)

ENGINE::CMesh_Static_Instacing::CMesh_Static_Instacing(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{

}

ENGINE::CMesh_Static_Instacing::CMesh_Static_Instacing(const CMesh_Static_Instacing& rhs)
	: CComponent(rhs)
	, m_pMesh(rhs.m_pMesh)
	, m_pMaterials(rhs.m_pMaterials)
	, m_pAdjacency(rhs.m_pAdjacency)
	, m_dwNumMaterials(rhs.m_dwNumMaterials)
	, m_pSubsetMaterial(rhs.m_pSubsetMaterial)
	, m_pMeshTexture(rhs.m_pMeshTexture)
{
	//m_ppTextures = new LPDIRECT3DTEXTURE9[rhs.m_dwSubsetCnt];

	//for (_ulong i = 0; i < rhs.m_dwSubsetCnt; ++i)
	//{
	//	m_ppTextures[i] = rhs.m_ppTextures[i];
	//	m_ppTextures[i]->AddRef();
	//}
	//m_pAdjacency->AddRef();
	//m_pSubsetBuffer->AddRef();
	//m_pOriMesh->AddRef();
	m_pMesh->AddRef();
	m_pMaterials->AddRef();
	m_pAdjacency->AddRef();

	for (size_t i = 0; i < m_dwNumMaterials; ++i)
	{
		Safe_AddRef(m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_DIFFUSE]);
		Safe_AddRef(m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_NORMAL]);
		Safe_AddRef(m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_SPECULAR]);
		Safe_AddRef(m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_EMESSIVE]);
		Safe_AddRef(m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_MASK]);
	}
	m_Instancing.iMax = 0;
}


LPD3DXMESH CMesh_Static_Instacing::Get_Mesh()
{
	return m_pMesh;
}

HRESULT CMesh_Static_Instacing::Load_Instancing_Matrix(INSTANCING * pArg)
{
	Safe_Release(m_pVBTransform);
	Safe_Release(m_pDeclaration);

	m_Instancing = *pArg;

	if (m_Instancing.iMax <= 0)
		return E_FAIL;

	m_pDevice->CreateVertexBuffer(sizeof(VTXMATRIX) *m_Instancing.iMax, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_MANAGED, &m_pVBTransform, NULL);

	VTXMATRIX* pData = nullptr;

	if (m_pVBTransform == nullptr)
		return E_FAIL;

	m_pVBTransform->Lock(0, 0, (void**)&pData, 0);

	int a = 0;

	_int i = 0;

	//_matrix matRotZ;
	//D3DXMatrixRotationX(&matRotZ, D3DXToRadian(-90.f));

	for (auto& pmat : m_Instancing.Instance_Lst)
	{
		//_matrix* matWorld;
		//matWorld = &_matrix(matRotZ * *pmat);

		pData[i].vRight = &*_vec4(pmat->_11, pmat->_12, pmat->_13, pmat->_14);
		pData[i].vUp = &*_vec4(pmat->_21, pmat->_22, pmat->_23, pmat->_24);
		pData[i].vLook = &*_vec4(pmat->_31, pmat->_32, pmat->_33, pmat->_34);
		pData[i].vPosition = &*_vec4(pmat->_41, pmat->_42, pmat->_43, pmat->_44);
		++i;
	}



	m_pVBTransform->Unlock();
	//m_iNumVertices = m_pMesh->GetNumVertices();
	//m_iNumIndexies = m_pMesh->GetNumFaces();


	D3DVERTEXELEMENT9 Elements[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,  0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,    0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },

		{ 1, 0,  D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1 },
		{ 1, 16, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  2 },
		{ 1, 32, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  3 },
		{ 1, 48, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  4 },
		D3DDECL_END()
	};

	m_iVtxSize = D3DXGetDeclVertexSize(Elements, 0);
	m_iVtxSizeInst = D3DXGetDeclVertexSize(Elements, 1);
	m_pDevice->CreateVertexDeclaration(Elements, &m_pDeclaration);

	//Safe_Delete_Array(matWorld);


	return NOERROR;
}

HRESULT CMesh_Static_Instacing::Insert_Instancing_Matrix(INSTANCING * pArg)
{
	Safe_Release(m_pVBTransform);
	Safe_Release(m_pDeclaration);
	//Safe_Release(m_pVB);
	//Safe_Release(m_pIB);

	m_Instancing = *pArg;

	//m_pMesh->GetVertexBuffer(&m_pVB);
	//m_pMesh->GetIndexBuffer(&m_pIB);

	if (m_Instancing.iMax <= 0)
		return E_FAIL;
	//_matrix* matWorld = new _matrix[m_Instancing.Instance_Lst.size()];

	m_pDevice->CreateVertexBuffer(sizeof(VTXMATRIX) *m_Instancing.iMax, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_MANAGED, &m_pVBTransform, NULL);

	VTXMATRIX* pData = nullptr;

	if (m_pVBTransform == nullptr)
		return E_FAIL;

	m_pVBTransform->Lock(0, 0, (void**)&pData, 0);

	int a = 0;

	//for (auto& pMat : m_Instancing.Instance_Lst)
	//{
	//	//D3DXMatrixIdentity(&matWorld[a]);
	//	matWorld[a++] = *pMat;
	//	//D3DXMatrixTranslation(&matWorld[a++], pMat->_41, pMat->_42, pMat->_43);
	//}
	//for (int i = 0; i <m_Instancing.Instance_Lst.size(); ++i)
	//{
	//	pData[i].vRight = *(_vec4*)&matWorld[i].m[0][0];
	//	pData[i].vUp = *(_vec4*)&matWorld[i].m[1][0];
	//	pData[i].vLook = *(_vec4*)&matWorld[i].m[2][0];
	//	pData[i].vPosition = *(_vec4*)&matWorld[i].m[3][0];
	//}
	_int i = 0;

	//_matrix matRotZ;
	//D3DXMatrixRotationX(&matRotZ, D3DXToRadian(-90.f));

	for (auto& pmat : m_Instancing.Instance_Lst)
	{
		_vec4 vRight, vUp, vLook;
		//_matrix* matWorld;
		//matWorld = &_matrix(matRotZ * *pmat);
		
		pData[i].vRight = &*_vec4(pmat->_11, pmat->_12, pmat->_13, pmat->_14);
		pData[i].vUp = &*_vec4(pmat->_21, pmat->_22, pmat->_23, pmat->_24);
		pData[i].vLook = &*_vec4(pmat->_31, pmat->_32, pmat->_33, pmat->_34);
		pData[i].vPosition = &*_vec4(pmat->_41, pmat->_42, pmat->_43, pmat->_44);

		vRight = { pData[i].vRight.x, pData[i].vRight.y,pData[i].vRight.z,pData[i].vRight.w };
		vUp = { pData[i].vUp.x, pData[i].vUp.y,pData[i].vUp.z,pData[i].vUp.w };
		vLook = { pData[i].vLook.x, pData[i].vLook.y,pData[i].vLook.z,pData[i].vLook.w };

		memcpy(&pmat->m[0][0], (_vec4)vRight, sizeof(_vec4));
		memcpy(&pmat->m[1][0], (_vec4)vUp, sizeof(_vec4));
		memcpy(&pmat->m[2][0], (_vec4)vLook, sizeof(_vec4));
		++i;
	}
	


	//m_iNumVertices = m_pMesh->GetNumVertices();
	//m_iNumIndexies = m_pMesh->GetNumFaces();


	D3DVERTEXELEMENT9 Elements[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,  0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,    0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },

		{ 1, 0,  D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1 },
		{ 1, 16, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  2 },
		{ 1, 32, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  3 },
		{ 1, 48, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  4 },
		D3DDECL_END()
	};

	m_iVtxSize = D3DXGetDeclVertexSize(Elements, 0);
	m_iVtxSizeInst = D3DXGetDeclVertexSize(Elements, 1);
	m_pDevice->CreateVertexDeclaration(Elements, &m_pDeclaration);

	//Safe_Delete_Array(matWorld);

	m_pVBTransform->Unlock();

	return NOERROR;
}

HRESULT CMesh_Static_Instacing::Modify_Instancing_Pos(_matrix * pMat)
{
	VTXMATRIX* pData = nullptr;
	m_pVBTransform->Lock(0, 0, (void**)&pData, 0);
	int i = 0;
	int a = 0;
	for (auto& pmat : m_Instancing.Instance_Lst)
	{
		if (pMat == pmat)
		{

			pmat->_41 = pMat->_41;
			pmat->_42 = pMat->_42;
			pmat->_43 = pMat->_43;
		}


		pData[i].vPosition = &*_vec4(pmat->_41, pmat->_42, pmat->_43, pmat->_44);

		++i;
	}

	m_pVBTransform->Unlock();
	return NOERROR;
}

HRESULT CMesh_Static_Instacing::Modify_Instancing_Scale(_matrix * pMat, _float fX, _float fY, _float fZ)
{
	VTXMATRIX* pData = nullptr;
	m_pVBTransform->Lock(0, 0, (void**)&pData, 0);
	int i = 0;
	int a = 0;
	for (auto& pmat : m_Instancing.Instance_Lst)
	{
		if (pMat == pmat)
		{

			//D3DXMatrixTranslation(pmat, pMat->_41, pMat->_42, pMat->_43);
			_vec4	vRight, vUp, vLook;
			if (fX <= 0.f || fY <= 0.f || fZ <= 0.f)
				return E_FAIL;
			vRight = *D3DXVec4Normalize(&vRight, &_vec4(pmat->_11, pmat->_12, pmat->_13, pmat->_14)) * fX;
			vUp = *D3DXVec4Normalize(&vUp, &_vec4(pmat->_21, pmat->_22, pmat->_23, pmat->_24)) * fY;
			vLook = *D3DXVec4Normalize(&vLook, &_vec4(pmat->_31, pmat->_32, pmat->_33, pmat->_34)) * fZ;
	

			//_vec4	vRight, vUp, vLook;
			if (fX <= 0 || fY <= 0 || fZ <= 0)
				return E_FAIL;


			pData[i].vRight = (_vec4)vRight;
			pData[i].vUp = (_vec4)vUp;
			pData[i].vLook = (_vec4)vLook;
			

			memcpy(&pmat->m[0][0], (_vec4)vRight, sizeof(_vec4));
			memcpy(&pmat->m[1][0], (_vec4)vUp, sizeof(_vec4));
			memcpy(&pmat->m[2][0], (_vec4)vLook, sizeof(_vec4));

		}


		++i;
	}

	m_pVBTransform->Unlock();
	return NOERROR;
}

HRESULT CMesh_Static_Instacing::Modify_Instancing_Rot(_matrix * pMat, _vec3 vAngle)
{


	VTXMATRIX* pData = nullptr;
	m_pVBTransform->Lock(0, 0, (void**)&pData, 0);
	int i = 0;
	int a = 0;
	for (auto& pmat : m_Instancing.Instance_Lst)
	{
		if (pMat == pmat)
		{

			//D3DXMatrixTranslation(pmat, pMat->_41, pMat->_42, pMat->_43);
	

			_vec3	vRight3, vUp3, vLook3;
			vRight3 = { 1.f,0.f,0.f };
			vUp3 = { 0.f,1.f,0.f };
			vLook3 = { 0.f,0.f,1.f };

		
			_matrix      matRotX, matRotY, matRotZ;

			D3DXMatrixRotationX(&matRotX, D3DXToRadian(vAngle.x));
			D3DXMatrixRotationY(&matRotY, D3DXToRadian(vAngle.y));
			D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(vAngle.z));

			//D3DXVec3TransformNormal(&vRight, &Get_State(CTransform::STATE_RIGHT), &matRot);
			vRight3 *= D3DXVec3Length(&_vec3(pmat->_11, pmat->_12, pmat->_13));
			D3DXVec3TransformNormal(&vRight3, &vRight3, &matRotX);
			D3DXVec3TransformNormal(&vRight3, &vRight3, &matRotY);
			D3DXVec3TransformNormal(&vRight3, &vRight3, &matRotZ);

			vUp3 *= D3DXVec3Length(&_vec3(pmat->_21, pmat->_22, pmat->_23));
			D3DXVec3TransformNormal(&vUp3, &vUp3, &matRotX);
			D3DXVec3TransformNormal(&vUp3, &vUp3, &matRotY);
			D3DXVec3TransformNormal(&vUp3, &vUp3, &matRotZ);

			vLook3 *= D3DXVec3Length(&_vec3(pmat->_31, pmat->_32, pmat->_33));
			D3DXVec3TransformNormal(&vLook3, &vLook3, &matRotX);
			D3DXVec3TransformNormal(&vLook3, &vLook3, &matRotY);
			D3DXVec3TransformNormal(&vLook3, &vLook3, &matRotZ);

			_vec4	vRight, vUp, vLook;
			vRight = { vRight3.x,vRight3.y,vRight3.z,0.f };
			vUp = { vUp3.x,vUp3.y,vUp3.z,0.f };
			vLook = { vLook3.x,vLook3.y,vLook3.z,0.f };


			pData[i].vRight = (_vec4)vRight;
			pData[i].vUp = (_vec4)vUp;
			pData[i].vLook = (_vec4)vLook;


			memcpy(&pmat->m[0][0], (_vec4)vRight, sizeof(_vec4));
			memcpy(&pmat->m[1][0], (_vec4)vUp, sizeof(_vec4));
			memcpy(&pmat->m[2][0], (_vec4)vLook, sizeof(_vec4));

		}




		++i;
	}

	m_pVBTransform->Unlock();
	return NOERROR;
}

HRESULT ENGINE::CMesh_Static_Instacing::Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName)
{
	_tchar		szFullPath[MAX_PATH] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	LPD3DXMESH			pMesh = nullptr;

	if (FAILED(D3DXLoadMeshFromX(szFullPath, D3DXMESH_MANAGED, m_pDevice, &m_pAdjacency, &m_pMaterials, nullptr, &m_dwNumMaterials, &pMesh)))
		return E_FAIL;

	m_pMeshTexture = new MESHTEXTURE[m_dwNumMaterials];
	ZeroMemory(m_pMeshTexture, sizeof(MESHTEXTURE) * m_dwNumMaterials);

	m_pSubsetMaterial = new D3DXMATERIAL[m_dwNumMaterials];
	memcpy(m_pSubsetMaterial, m_pMaterials->GetBufferPointer(), sizeof(D3DXMATERIAL) * m_dwNumMaterials);


	if (false == (D3DFVF_NORMAL & pMesh->GetFVF()))
	{
		// 메시를 복제한다.
		if (FAILED(pMesh->CloneMeshFVF(pMesh->GetOptions(), pMesh->GetFVF() | D3DFVF_NORMAL, m_pDevice, &m_pMesh)))
			return E_FAIL;

		if (FAILED(D3DXComputeNormals(m_pMesh, (_ulong*)m_pAdjacency->GetBufferPointer())))
			return E_FAIL;
	}
	else
	{
		// 메시를 구성하는 정점구조체의 멤버변수의 정보를 담기위한 구조체배열
		D3DVERTEXELEMENT9			Element[MAX_FVF_DECL_SIZE] = {
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			D3DDECL_END()
		};

		if (FAILED(pMesh->CloneMesh(pMesh->GetOptions(), Element, m_pDevice, &m_pMesh)))
			return E_FAIL;
	}

	Safe_Release(pMesh);


	for (size_t i = 0; i < m_dwNumMaterials; ++i)
	{
		lstrcpy(szFullPath, pFilePath);

		_tchar		szFileName[MAX_PATH] = L"";

		MultiByteToWideChar(CP_ACP, 0, m_pSubsetMaterial[i].pTextureFilename, (_int)strlen(m_pSubsetMaterial[i].pTextureFilename)
			, szFileName, MAX_PATH);

		lstrcat(szFullPath, szFileName);

		// For.DiffuseMap
		if (FAILED(D3DXCreateTextureFromFile(m_pDevice, szFullPath, &m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_DIFFUSE])))
			return E_FAIL;

		// For.NormalMap
		Change_TextureFileName(szFullPath, L'N');
		D3DXCreateTextureFromFile(m_pDevice, szFullPath, &m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_NORMAL]);

		// For.SpecularMap
		Change_TextureFileName(szFullPath, L'S');
		D3DXCreateTextureFromFile(m_pDevice, szFullPath, &m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_SPECULAR]);

		// For.EmessiveMap
		Change_TextureFileName(szFullPath, L'E');
		D3DXCreateTextureFromFile(m_pDevice, szFullPath, &m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_EMESSIVE]);

		// For.MaskMap
		Change_TextureFileName(szFullPath, L'M');
		D3DXCreateTextureFromFile(m_pDevice, szFullPath, &m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_MASK]);
	}

	return NOERROR;
}

HRESULT CMesh_Static_Instacing::Change_TextureFileName(_tchar* pFileName, _tchar Text)
{
	_uint iLength = lstrlen(pFileName);

	for (size_t i = iLength; i >= 0; --i)
	{
		if (pFileName[i] == L'.')
		{
			pFileName[i - 1] = Text;
			break;
		}
	}

	return NOERROR;
}

HRESULT CMesh_Static_Instacing::Ready_Instancing_Meshes(void* pArg)
{

	m_Instancing = *(INSTANCING*)pArg;  

	m_pMesh->GetVertexBuffer(&m_pVB);
	m_pMesh->GetIndexBuffer(&m_pIB);


	_matrix* matWorld = new _matrix[m_Instancing.iMax];
	
	m_pDevice->CreateVertexBuffer(sizeof(VTXMATRIX) * m_Instancing.iMax, D3DUSAGE_WRITEONLY, NULL, D3DPOOL_MANAGED, &m_pVBTransform, NULL);
	
	VTXMATRIX* pData = nullptr;
	m_pVBTransform->Lock(0, 0, (void**)&pData, 0);
	
	int a = 0;
	
	for (auto& pMat : m_Instancing.Instance_Lst)
	{
		D3DXMatrixIdentity(&matWorld[a]);
		D3DXMatrixTranslation(&matWorld[a++], pMat->_41, pMat->_42, pMat->_43);
	}


	_int i = 0;

	_matrix matRotZ;
	D3DXMatrixRotationX(&matRotZ, D3DXToRadian(-90.f));

	for (auto& pmat : m_Instancing.Instance_Lst)
	{
		_matrix* matWorld;
		matWorld = &_matrix(matRotZ * *pmat);

		pData[i].vRight = &*_vec4(matWorld->_11, matWorld->_12, matWorld->_13, matWorld->_14);
		pData[i].vUp = &*_vec4(matWorld->_21, matWorld->_22, matWorld->_23, matWorld->_24);
		pData[i].vLook = &*_vec4(matWorld->_31, matWorld->_32, matWorld->_33, matWorld->_34);
		pData[i].vPosition = &*_vec4(matWorld->_41, matWorld->_42, matWorld->_43, matWorld->_44);
		++i;
	}


	m_pVBTransform->Unlock();
	
	m_iNumVertices = m_pMesh->GetNumVertices();
	m_iNumIndexies = m_pMesh->GetNumFaces();

	D3DVERTEXELEMENT9 Elements[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,  0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,    0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },

		{ 1, 0,  D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1 },
		{ 1, 16, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  2 },
		{ 1, 32, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  3 },
		{ 1, 48, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  4 },
		D3DDECL_END()
	};
	
	m_iVtxSize = D3DXGetDeclVertexSize(Elements, 0);
	m_iVtxSizeInst = D3DXGetDeclVertexSize(Elements, 1);
	m_pDevice->CreateVertexDeclaration(Elements, &m_pDeclaration);

	m_pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL);



	DWORD AttributeSize = 0;
	HRESULT hr = m_pMesh->GetAttributeTable(nullptr, &AttributeSize);
	m_pAttributeTable = new D3DXATTRIBUTERANGE[AttributeSize];
	m_AttributeSize = AttributeSize;

	m_pMesh->GetAttributeTable(m_pAttributeTable, &AttributeSize);

	//Safe_Delete_Array(pAdjBuffer);
	Safe_Delete_Array(matWorld);



	return S_OK;
}



void ENGINE::CMesh_Static_Instacing::Render_Meshes(LPD3DXEFFECT pEffect)
{
	

}

HRESULT CMesh_Static_Instacing::Render_Mesh(LPD3DXEFFECT pEffect, _uint iSubSetID)
{
	if (nullptr == m_pMesh)
		return E_FAIL;

	if (m_pDeclaration == nullptr)
		return E_FAIL;

	m_pDevice->SetStreamSourceFreq(0, (D3DSTREAMSOURCE_INDEXEDDATA | m_Instancing.iMax));
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_iVtxSize);

	// 객체당 몇개의 행렬정보를 사용할 것인지.
	m_pDevice->SetStreamSourceFreq(1, (D3DSTREAMSOURCE_INSTANCEDATA | 1));
	m_pDevice->SetStreamSource(1, m_pVBTransform, 0, sizeof(VTXMATRIX));

	m_pDevice->SetVertexDeclaration(m_pDeclaration);

	m_pDevice->SetIndices(m_pIB);

	// 객체하나를 그리기위해 필요한 정보 
	//m_dwNumMaterials

	//여기서 셋 텍스처 해야 된다.
	for (_uint i = 0; i < m_dwNumMaterials; ++i)
	{
		SetTexture_OnShader(pEffect, i, "g_DiffuseTexture", MESHTEXTURE::TYPE_DIFFUSE);
		SetTexture_OnShader(pEffect, i, "g_EmessiveTexture", MESHTEXTURE::TYPE_EMESSIVE);
		SetTexture_OnShader(pEffect, i, "g_SpecularTexture", MESHTEXTURE::TYPE_SPECULAR);
		SetTexture_OnShader(pEffect, i, "g_NormalTexture", MESHTEXTURE::TYPE_NORMAL);
		SetTexture_OnShader(pEffect, i, "g_MaskTexture", MESHTEXTURE::TYPE_MASK);

		pEffect->CommitChanges();

		pEffect->BeginPass(6);
		//m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumIndexies);


		m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_pAttributeTable[i].VertexStart, m_pAttributeTable[i].VertexCount, m_pAttributeTable[i].FaceStart * 3, m_pAttributeTable[i].FaceCount);
		pEffect->EndPass();
	}
	m_pDevice->SetStreamSourceFreq(0, 1);
	m_pDevice->SetStreamSourceFreq(1, 1);

	return NOERROR;
}

HRESULT CMesh_Static_Instacing::SetTexture_OnShader(LPD3DXEFFECT pEffect, _uint iSubsetID, const char * pConstantName, MESHTEXTURE::TYPE eType)
{
	if (nullptr == pEffect ||
		nullptr == m_pMeshTexture)
		return E_FAIL;

	if (m_pMeshTexture[iSubsetID].pTextures[eType] == nullptr)
	{
		switch (eType)
		{
		case ENGINE::tagMeshTexture::TYPE_DIFFUSE:
			pEffect->SetBool("g_bDiffuseMap", FALSE);
			break;
		case ENGINE::tagMeshTexture::TYPE_NORMAL:
			pEffect->SetBool("g_bNormalMap", FALSE);
			break;
		case ENGINE::tagMeshTexture::TYPE_SPECULAR:
			pEffect->SetBool("g_bSpecularMap", FALSE);
			break;
		case ENGINE::tagMeshTexture::TYPE_EMESSIVE:
			pEffect->SetBool("g_bEmessiveMap", FALSE);
			break;
		case ENGINE::tagMeshTexture::TYPE_MASK:
			pEffect->SetBool("g_bMaskMap", FALSE);
			break;
		case ENGINE::tagMeshTexture::TYPE_FX:
			pEffect->SetBool("g_bFXMap", FALSE);
			break;
		}
	}
	else
	{
		switch (eType)
		{
		case ENGINE::tagMeshTexture::TYPE_DIFFUSE:
			pEffect->SetBool("g_bDiffuseMap", TRUE);
			break;
		case ENGINE::tagMeshTexture::TYPE_NORMAL:
			pEffect->SetBool("g_bNormalMap", TRUE);
			break;
		case ENGINE::tagMeshTexture::TYPE_SPECULAR:
			pEffect->SetBool("g_bSpecularMap", TRUE);
			break;
		case ENGINE::tagMeshTexture::TYPE_EMESSIVE:
			pEffect->SetBool("g_bEmessiveMap", TRUE);
			break;
		case ENGINE::tagMeshTexture::TYPE_MASK:
			pEffect->SetBool("g_bMaskMap", TRUE);
			break;
		case ENGINE::tagMeshTexture::TYPE_FX:
			pEffect->SetBool("g_bFXMap", TRUE);
			break;
		}
	}

	pEffect->SetTexture(pConstantName, m_pMeshTexture[iSubsetID].pTextures[eType]);

	return NOERROR;
}

CMesh_Static_Instacing* ENGINE::CMesh_Static_Instacing::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName)
{
	CMesh_Static_Instacing*	pInstance = new CMesh_Static_Instacing(pGraphicDev);

	if (FAILED(pInstance->Ready_Meshes(pFilePath, pFileName)))
	{
		MSG_BOX("CMesh_Static_Instacing Created Failed");
		ENGINE::Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* ENGINE::CMesh_Static_Instacing::Clone_Component(void* pArg)
{
	CMesh_Static_Instacing*	pInstance = new CMesh_Static_Instacing(*this);

	if (FAILED(pInstance->Ready_Instancing_Meshes(pArg)))
	{
		MSG_BOX("CMesh_Static_Instacing Cloned Failed");
		ENGINE::Safe_Release(pInstance);
	}

	return pInstance;
	//return new CMesh_Static_Instacing(*this);
}

void ENGINE::CMesh_Static_Instacing::Free(void)
{
	if (true == m_isClone)
	{
		Safe_Release(m_pVBTransform);
		Safe_Release(m_pDeclaration);
		Safe_Release(m_pVB);
		Safe_Release(m_pIB);
		Safe_Delete_Array(m_pAttributeTable);
	}
	for (size_t i = 0; i < m_dwNumMaterials; ++i)
	{
		Safe_Release(m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_DIFFUSE]);
		Safe_Release(m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_NORMAL]);
		Safe_Release(m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_SPECULAR]);
		Safe_Release(m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_EMESSIVE]);
		Safe_Release(m_pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_MASK]);
	}

	if (false == m_isClone)
	{
		Safe_Delete_Array(m_pSubsetMaterial);
		Safe_Delete_Array(m_pMeshTexture);


	}

	Safe_Release(m_pMaterials);
	Safe_Release(m_pAdjacency);
	_ulong dwRefCnt = Safe_Release(m_pMesh);

	CComponent::Free();



}

_bool ENGINE::CMesh_Static_Instacing::Find_Alpha(const char* pFileName)
{
	_uint		iLength = (_uint)strlen(pFileName);

	for (_uint i = 0; i < iLength + 1; ++i)
	{
		if (pFileName[i] == '.')
		{
			if (pFileName[i - 1] == 'A')
				return true;				
		}
	}

	return false;
}


