#include "stdafx.h"
#include "BoxShutter_Plane.h"

#include "KeyMgr.h"

CBoxShutter_Plane::CBoxShutter_Plane(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice)
{

}

CBoxShutter_Plane::CBoxShutter_Plane(const CBoxShutter_Plane & rhs)
	: CDgnObj(rhs)
{

}

HRESULT CBoxShutter_Plane::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CBoxShutter_Plane::Ready_GameObject(void * pArg)
{
	_matrix matWorld = *(_matrix*)pArg;
	
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(matWorld.m);
	
	m_pTransform->Get_Actor()->setGlobalPose(pMat);

	return S_OK;
}

_int CBoxShutter_Plane::Update_GameObject(const _double & TimeDelta)
{
	CDgnObj::Update_GameObject(TimeDelta);

	return NO_EVENT;
}

_int CBoxShutter_Plane::LateUpdate_GameObject(const _double & TimeDelta)
{
	CDgnObj::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CBoxShutter_Plane::Render_GameObject()
{
	if (nullptr == m_pShader || nullptr == m_pMesh)
		return E_FAIL;


	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	if (nullptr == pEffect)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(SetUp_ConstantTable(pEffect)))
		return E_FAIL;

	pEffect->Begin(nullptr, 0);

	for (_uint i = 0; i < m_pMesh->Get_NumMaterials(); ++i)
	{
		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_DiffuseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_SpecularTexture", MESHTEXTURE::TYPE_SPECULAR)))
			return E_FAIL;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_NormalTexture", MESHTEXTURE::TYPE_NORMAL)))
			return E_FAIL;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_EmessiveTexture", MESHTEXTURE::TYPE_EMESSIVE)))
			return E_FAIL;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_MaskTexture", MESHTEXTURE::TYPE_MASK)))
			return E_FAIL;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_FXTexture", MESHTEXTURE::TYPE_FX)))
			return E_FAIL;

		pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.f, 0.f, 0.f, 1.f));
		pEffect->SetVector("g_vecEmessiveColor", &_vec4(0.1f, 0.5f, 0.7f, 1.f));

		pEffect->SetBool("g_bShade", true);

		pEffect->CommitChanges();

		pEffect->BeginPass(4);


		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

HRESULT CBoxShutter_Plane::Add_Component(void * pArg)
{
	// For.Com_Renderer
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	//For.Com_Mesh
	if (FAILED(CDgnObj::Add_Component(SCENE_MESH_STATIC,
		L"Component_Mesh_BoxShutter_Plane", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	BYTE* pVertices;
	_vec3 vMin, vMax;
	m_pMesh->Get_Mesh()->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&pVertices);
	D3DXComputeBoundingBox((D3DXVECTOR3*)pVertices, m_pMesh->Get_Mesh()->GetNumVertices(),
		m_pMesh->Get_Mesh()->GetNumBytesPerVertex(), &vMin, &vMax);
	m_pMesh->Get_Mesh()->UnlockVertexBuffer();

	tState.cName = "BoxShutter_Plane";
	tState.eCollider = COL_TRIMESH;
	tState.eCollOpt = OPT_END;
	tState.vAngle = { -90.f, 0.f, 0.f };
	tState.pMesh = m_pMesh->Get_Mesh();

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoxShutter_Plane::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect || nullptr == m_pMesh || nullptr == m_pTransform)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(m_pTransform->SetUp_OnShader(pEffect, "g_matWorld")))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matView", &Get_Transform(D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matProj", &Get_Transform(D3DTS_PROJECTION))))
		return E_FAIL;

	//if (FAILED(m_pTexture->SetUp_OnShader(pEffect, "g_BaseTexture")))
	//	return E_FAIL;

	Safe_Release(pEffect);

	return S_OK;
}

CDgnObj * CBoxShutter_Plane::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CBoxShutter_Plane(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CBoxShutter_Plane::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CBoxShutter_Plane(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBoxShutter_Plane::Free()
{

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CDgnObj::Free();
}

