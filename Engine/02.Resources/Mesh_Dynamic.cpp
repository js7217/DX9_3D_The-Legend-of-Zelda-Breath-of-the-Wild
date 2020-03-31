#include "Mesh_Dynamic.h"
#include "HierarchyLoader.h"
#include "AnimationCtrl.h"

CMesh_Dynamic::CMesh_Dynamic(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
	
}

CMesh_Dynamic::CMesh_Dynamic(const CMesh_Dynamic & rhs)	
	: CComponent(rhs)
	, m_pLoader(rhs.m_pLoader)
	, m_pRootFrame(rhs.m_pRootFrame)
	, m_pRootFrame2(rhs.m_pRootFrame2)
	, m_MeshContainers(rhs.m_MeshContainers)
	, m_matPivot(rhs.m_matPivot)
	, m_bIsTangent(rhs.m_bIsTangent)
	, m_pAniCtrl(CAnimationCtrl::Create(*rhs.m_pAniCtrl))
	, m_pAniCtrl2(CAnimationCtrl::Create(*rhs.m_pAniCtrl))
{	
	Safe_AddRef(m_pLoader);	
}

HRESULT CMesh_Dynamic::Ready_Mesh_Dynamic(const _tchar * pFilePath, const _tchar * pFileName, _bool bTangent)
{
	_tchar		szFullPath[MAX_PATH] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);
	m_bIsTangent = bTangent;
	m_pLoader = CHierarchyLoader::Create(m_pDevice, pFilePath, bTangent);
	if (nullptr == m_pLoader)
		return E_FAIL;

	LPD3DXANIMATIONCONTROLLER			pAniCtrl = nullptr;
		
	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath, D3DXMESH_MANAGED, m_pDevice, m_pLoader, nullptr, &m_pRootFrame, &pAniCtrl)))
		return E_FAIL;

	m_pAniCtrl = CAnimationCtrl::Create(pAniCtrl);
	if (nullptr == m_pAniCtrl)
		return E_FAIL;

	Safe_Release(pAniCtrl);

	if (!lstrcmp(pFileName, L"TestLink.X") || !lstrcmp(pFileName, L"Bokoblin.X") || !lstrcmp(pFileName, L"Enemy_Giant.X") || !lstrcmp(pFileName, L"WizzRobe.X")
		|| !lstrcmp(pFileName, L"Dog.X") || !lstrcmp(pFileName, L"Npc_King_Vagrant.X"))
		D3DXMatrixRotationY(&m_matPivot, D3DXToRadian(180.0f));
	else
		D3DXMatrixRotationY(&m_matPivot, D3DXToRadian(0.f));

	if (FAILED(Update_CombinedTransformationMatrix(m_pRootFrame, &m_matPivot)))
		return E_FAIL;

	if (FAILED(SetUp_CombinedMatrixPointer(m_pRootFrame)))
		return E_FAIL;	

	return NOERROR;
}

HRESULT CMesh_Dynamic::Render_Mesh(_uint iMeshContainerID, _uint iSubSetID)
{
	m_MeshContainers[iMeshContainerID]->MeshData.pMesh->DrawSubset(iSubSetID);

	return NOERROR;
}

HRESULT CMesh_Dynamic::Update_SkinnedMesh(_uint iMeshContainerID)
{
	if (m_MeshContainers.size() <= iMeshContainerID)
		return E_FAIL;

	D3DXMESHCONTAINER_DERIVED* pMeshContainer = m_MeshContainers[iMeshContainerID];

	for (size_t i = 0; i < pMeshContainer->dwNumBones; ++i)
		pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffeSetMatrix[i] * *pMeshContainer->ppFrameCombinedMatrix[i];

	void*	pSour, *pDest;

	pMeshContainer->pOriMesh->LockVertexBuffer(0, &pSour);
	pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDest);

	pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrix, nullptr, pSour, pDest);

	if (m_bIsTangent)
	{
		D3DVERTEXELEMENT9			Element[MAX_FVF_DECL_SIZE] = {
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
			//{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },

			//D3DXComputeTangent()
			D3DDECL_END()
		};

		pMeshContainer->pSkinInfo->SetDeclaration(Element);
	}

	pMeshContainer->pOriMesh->UnlockVertexBuffer();
	pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();

	return NOERROR;
}

HRESULT CMesh_Dynamic::SetTexture_OnShader(LPD3DXEFFECT pEffect, _uint iMeshContainerID, _uint iSubsetID, const char * pConstantName, MESHTEXTURE::TYPE eType)
{
	if (m_MeshContainers[iMeshContainerID]->pMeshTexture[iSubsetID].pTextures[eType] == nullptr)
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

	pEffect->SetTexture(pConstantName, m_MeshContainers[iMeshContainerID]->pMeshTexture[iSubsetID].pTextures[eType]);

	return NOERROR;
}

HRESULT CMesh_Dynamic::SetUp_AnimationSet(_uint iIndex)
{
	if (nullptr == m_pAniCtrl)
		return E_FAIL;

	m_pAniCtrl->SetUp_AnimationSet(iIndex);

	return NOERROR;
}

HRESULT CMesh_Dynamic::SetUp_AnimationSet_2(_uint iIndex)
{
	if (nullptr == m_pAniCtrl2)
		return E_FAIL;

	m_pAniCtrl2->SetUp_AnimationSet(iIndex);

	return NOERROR;
}

//HRESULT CMesh_Dynamic::SetUp_AnimationSet(const char * pName)
//{
//	if (nullptr == m_pAniCtrl)
//		return E_FAIL;
//
//	m_pAniCtrl->SetUp_AnimationSet(pName);
//
//	return NOERROR;
//}

HRESULT CMesh_Dynamic::Play_Animation(const _double& TimeDelta, _float fAdjSpeed)
{
	if (nullptr == m_pAniCtrl)
		return E_FAIL;

	m_pAniCtrl->Play_Animation(TimeDelta, fAdjSpeed);

	Update_CombinedTransformationMatrix(m_pRootFrame, &m_matPivot);

	return NOERROR;
}

HRESULT CMesh_Dynamic::Play_Animation_2(const _double & TimeDelta, _float fAdjSpeed)
{
	if (nullptr == m_pAniCtrl2)
		return E_FAIL;

	m_pAniCtrl2->Play_Animation(TimeDelta, fAdjSpeed);

	const D3DXFRAME_DERIVED* pFrame = Get_FrameByName("Skl_Root");

	_matrix matFrame = pFrame->CombinedTransformMatrix;

	Update_CombinedTransformationMatrix(m_pRootFrame2, &matFrame);

	return NOERROR;
}

_bool CMesh_Dynamic::IsAnimationSetEnd(_float fInterpolatedValue)
{
	return m_pAniCtrl->IsAnimationSetEnd(fInterpolatedValue);
}

_bool CMesh_Dynamic::IsAnimationSetEnd_2(_float fInterpolatedValue)
{
	return m_pAniCtrl2->IsAnimationSetEnd(fInterpolatedValue);
}

const D3DXFRAME_DERIVED * CMesh_Dynamic::Get_FrameByName(const char * pFrameName)
{
	return (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pFrameName);
}

const D3DXFRAME_DERIVED * CMesh_Dynamic::Get_FrameByName_Root2(const char * pFrameName)
{
	return (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame2, pFrameName);
}

void CMesh_Dynamic::AnimationBoneSeparation(D3DXFRAME_DERIVED * frame, const char * bodyName)
{
	if (!strcmp(frame->Name, bodyName))
	{
		m_pRootFrame2 = frame->pFrameSibling;
		frame->pFrameSibling = NULL;
	}

	if (frame->pFrameSibling)
		AnimationBoneSeparation((D3DXFRAME_DERIVED*)frame->pFrameSibling, bodyName);

	if (frame->pFrameFirstChild)
		AnimationBoneSeparation((D3DXFRAME_DERIVED*)frame->pFrameFirstChild, bodyName);
}

HRESULT CMesh_Dynamic::Update_CombinedTransformationMatrix(D3DXFRAME * pFrame, const D3DXMATRIX* pParentMatrix)
{
	D3DXFRAME_DERIVED*		pFrame_Derived = (D3DXFRAME_DERIVED*)pFrame;

	pFrame_Derived->CombinedTransformMatrix = pFrame_Derived->TransformationMatrix * *pParentMatrix;

	if (nullptr != pFrame_Derived->pFrameFirstChild)
	{
		Update_CombinedTransformationMatrix(pFrame_Derived->pFrameFirstChild, &pFrame_Derived->CombinedTransformMatrix);
	}

	if (nullptr != pFrame_Derived->pFrameSibling)
	{
		Update_CombinedTransformationMatrix(pFrame_Derived->pFrameSibling, pParentMatrix);
	}

	return NOERROR;
}

HRESULT CMesh_Dynamic::SetUp_CombinedMatrixPointer(D3DXFRAME* pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		m_MeshContainers.push_back(pMeshContainer);

		for (_uint i = 0; i < pMeshContainer->dwNumBones; ++i)
		{
			D3DXFRAME_DERIVED* pFindFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pMeshContainer->pSkinInfo->GetBoneName(i));

			pMeshContainer->ppFrameCombinedMatrix[i] = &(pFindFrame->CombinedTransformMatrix);
		}		
	}

	if (nullptr != pFrame->pFrameSibling)
		SetUp_CombinedMatrixPointer(pFrame->pFrameSibling);

	if (nullptr != pFrame->pFrameFirstChild)
		SetUp_CombinedMatrixPointer(pFrame->pFrameFirstChild);

	return NOERROR;
}

CMesh_Dynamic * CMesh_Dynamic::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath, const _tchar * pFileName, _bool bTangent)
{
	CMesh_Dynamic* pInstance = new CMesh_Dynamic(pGraphic_Device);

	if (FAILED(pInstance->Ready_Mesh_Dynamic(pFilePath, pFileName, bTangent)))
	{
		MSG_BOX("CMesh_Dynamic Created Failed");
		Safe_Release(pInstance);
	}

	if (!lstrcmp(pFileName, L"TestLink.X"))
		pInstance->AnimationBoneSeparation((D3DXFRAME_DERIVED*)pInstance->Get_RootFrame(), "Spine_1");

	return pInstance;
}

CComponent * CMesh_Dynamic::Clone_Component(void* pArg)
{
	return new CMesh_Dynamic(*this);
}

void CMesh_Dynamic::Free()
{
	if (false == m_isClone)
	{
		m_pLoader->DestroyFrame(m_pRootFrame);

		if (nullptr != m_pRootFrame2)
			m_pLoader->DestroyFrame(m_pRootFrame2);
	}	

	Safe_Release(m_pLoader);
	Safe_Release(m_pAniCtrl);
	Safe_Release(m_pAniCtrl2);

	m_MeshContainers.clear();

	CComponent::Free();
}
