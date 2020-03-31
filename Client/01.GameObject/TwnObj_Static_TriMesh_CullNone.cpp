#include "stdafx.h"
#include "TwnObj_Static_TriMesh_CullNone.h"

#include "KeyMgr.h"
#include "NxPlayer.h"

CTwnObj_Static_TriMesh_CullNone::CTwnObj_Static_TriMesh_CullNone(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice)
{

}

CTwnObj_Static_TriMesh_CullNone::CTwnObj_Static_TriMesh_CullNone(const CTwnObj_Static_TriMesh_CullNone & rhs)
	: CDgnObj(rhs)
{

}

HRESULT CTwnObj_Static_TriMesh_CullNone::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CTwnObj_Static_TriMesh_CullNone::Ready_GameObject(void * pArg)
{	
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CTwnObj_Static_TriMesh_CullNone::Update_GameObject(const _double & TimeDelta)
{
	CDgnObj::Update_GameObject(TimeDelta);
	_vec3 vPos;
	m_pTransform->Get_Info(CNxTransform::INFO_POS, &vPos);


	m_bDraw = m_pFrustumCom->WorldPt_InFrustum(&vPos, m_pTransform, 10.f);

	///////
	_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();
	CNxPlayer* pNxPlayer = (CNxPlayer*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Player", 0);
	if (nullptr == pNxPlayer)
		return NOERROR;
	
	CNxTransform* pTransCom = (CNxTransform*)pNxPlayer->Get_Component(L"Com_NxTransform");
	if (nullptr == pTransCom)
		return NOERROR;

	_vec3 vDir, vPlayerPos;
	pTransCom->Get_Info(CNxTransform::INFO_POS, &vPlayerPos);

	vDir = vPlayerPos - m_pTransform->m_vInfo[CNxTransform::INFO_POS];
	float fLength = D3DXVec3Length(&vDir);

	if (fLength >= 55.f)
	{
		m_bIsFarMeshRender = true;
	}
	else
	{
		m_bIsFarMeshRender = false;
	}

	//if (!m_bDraw)
	//	m_bIsFarMeshRender = true;
	//
	return NO_EVENT;
}

_int CTwnObj_Static_TriMesh_CullNone::LateUpdate_GameObject(const _double & TimeDelta)
{
	CDgnObj::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;
	if (false == m_bDraw)
		return NO_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CTwnObj_Static_TriMesh_CullNone::Render_GameObject()
{
	if (nullptr == m_pShader || nullptr == m_pMesh)
		return E_FAIL;

	//if (false == m_bDraw)
	//	return NOERROR;
	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	if (false == m_bIsFarMeshRender)
	{
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
			pEffect->SetVector("g_vecEmessiveColor", &_vec4(1.f, 0.623f, 0.251f, 1.f));
			pEffect->SetBool("g_bShade", true);

			pEffect->CommitChanges();

			pEffect->BeginPass(7);

			m_pMesh->Render_Mesh(i);

			pEffect->EndPass();
		}

		pEffect->End();


		//////////////////////////////////////////////////////
	}
	else
	{
		if (m_pMesh_Far == nullptr)
			return NOERROR;

		pEffect = m_pShader->Get_EffectHandle();

		if (nullptr == pEffect)
			return E_FAIL;

		Safe_AddRef(pEffect);

		if (FAILED(SetUp_ConstantTable(pEffect)))
			return E_FAIL;

		pEffect->Begin(nullptr, 0);

		for (_uint i = 0; i < m_pMesh_Far->Get_NumMaterials(); ++i)
		{
			if (FAILED(m_pMesh_Far->SetTexture_OnShader(pEffect, i, "g_DiffuseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pMesh_Far->SetTexture_OnShader(pEffect, i, "g_SpecularTexture", MESHTEXTURE::TYPE_SPECULAR)))
				return E_FAIL;

			if (FAILED(m_pMesh_Far->SetTexture_OnShader(pEffect, i, "g_NormalTexture", MESHTEXTURE::TYPE_NORMAL)))
				return E_FAIL;

			if (FAILED(m_pMesh_Far->SetTexture_OnShader(pEffect, i, "g_EmessiveTexture", MESHTEXTURE::TYPE_EMESSIVE)))
				return E_FAIL;

			if (FAILED(m_pMesh_Far->SetTexture_OnShader(pEffect, i, "g_MaskTexture", MESHTEXTURE::TYPE_MASK)))
				return E_FAIL;

			if (FAILED(m_pMesh_Far->SetTexture_OnShader(pEffect, i, "g_FXTexture", MESHTEXTURE::TYPE_FX)))
				return E_FAIL;

			pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.f, 0.f, 0.f, 1.f));
			pEffect->SetVector("g_vecEmessiveColor", &_vec4(1.f, 0.623f, 0.251f, 1.f));
			pEffect->SetBool("g_bShade", true);

			pEffect->CommitChanges();

			pEffect->BeginPass(5);

			m_pMesh_Far->Render_Mesh(i);

			pEffect->EndPass();
		}

		pEffect->End();
	}

	Safe_Release(pEffect);

	return NO_EVENT;
}

HRESULT CTwnObj_Static_TriMesh_CullNone::Set_FarMeshLoad(const _tchar* pName)
{
	//For.Com_Mesh
	if (FAILED(CDgnObj::Add_Component(SCENE_MESH_STATIC,
		pName, L"Com_Mesh_Far", (CComponent**)&m_pMesh_Far)))
	{
		m_pMesh_Far = nullptr;
	};

	return NOERROR;
}

HRESULT CTwnObj_Static_TriMesh_CullNone::Set_NxTransform(const _matrix& matWorld)
{
	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	tState.cName = "TwnObj_Static_TriMesh_CullNone";
	tState.eCollider = COL_TRIMESH;
	tState.eCollOpt = OPT_STATIC;
	tState.vLocalPos = { 0.f, 0.f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
	tState.iMaterial = 0;
	tState.pMesh = m_pMesh->Get_Mesh();
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	m_pTransform->Get_Actor()->getShapes()[0]->setName(tState.cName);

	NxMat34 pMat;
	pMat.isIdentity();


	pMat.setColumnMajor44(matWorld.m);
	//for (int i = 0; i < 3; ++i)
	//{
	//	NxVec3 vPos;
	//	memcpy(&vPos, matWorld.m[i], sizeof(_vec3));
	//	(NxVec3)m_pTransform->m_vInfo[i] = vPos;

	//	pMat.M.setColumn(i, vPos);
	//}

	//memcpy(&pMat.t, matWorld.m[3], sizeof(_vec3));

	m_pTransform->Get_Actor()->setGlobalPose(pMat);

	return S_OK;
}

HRESULT CTwnObj_Static_TriMesh_CullNone::Add_Component(void * pArg)
{
	const _tchar* pName = (_tchar*)pArg;

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
		pName, L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTwnObj_Static_TriMesh_CullNone::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CDgnObj * CTwnObj_Static_TriMesh_CullNone::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CTwnObj_Static_TriMesh_CullNone(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CTwnObj_Static_TriMesh_CullNone::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CTwnObj_Static_TriMesh_CullNone(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTwnObj_Static_TriMesh_CullNone::Free()
{

	Safe_Release(m_pTransform);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);
	Safe_Release(m_pMesh_Far);

	CDgnObj::Free();
}

