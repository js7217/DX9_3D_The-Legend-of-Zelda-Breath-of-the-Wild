#include "stdafx.h"
#include "Obj_Leaf_TreeApple.h"


CObj_Leaf_TreeApple::CObj_Leaf_TreeApple(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{

}

CObj_Leaf_TreeApple::CObj_Leaf_TreeApple(const CObj_Leaf_TreeApple & rhs)
	: CGameObject(rhs)
{

}

HRESULT CObj_Leaf_TreeApple::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CObj_Leaf_TreeApple::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_fScele = NxMath::rand(0.1f, 1.5f);

	return S_OK;
}

_int CObj_Leaf_TreeApple::Update_GameObject(const _double & TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);	

	NxVec3 vPos = m_pTransform->Get_Actor()->getGlobalPosition();
	_vec3 vPosition;
	memcpy(vPosition, &vPos, sizeof(_vec3));
	m_bDraw = m_pFrustumCom->WorldPt_InFrustum(&vPosition, m_pTransform, 1.f);



	if (vPos.y <= -20.0)
	{
	//	m_pTransform->Get_Actor()->raiseActorFlag(NX_AF_DISABLE_COLLISION);
	//	m_pTransform->Get_Actor()->putToSleep();
		return END_EVENT;
	}

	return NO_EVENT;
}

_int CObj_Leaf_TreeApple::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	if (!m_bDraw)
		return NO_EVENT;

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CObj_Leaf_TreeApple::Render_GameObject()
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

		pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.f, 0.f, 0.f, 1.f));
		pEffect->SetVector("g_vecEmessiveColor", &_vec4(1.f, 0.623f, 0.251f, 1.f));
		pEffect->SetBool("g_bShade", true);

		pEffect->CommitChanges();

		pEffect->BeginPass(5);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

HRESULT CObj_Leaf_TreeApple::Create_NxTransform(const _matrix matWorld)
{
	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	tState.cName = "Obj_Leaf_TreeApple";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_DYNAMIC;
	tState.vLocalPos = { 0.f, 0.f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
	tState.vDimensions = { 0.1f, 0.1f, 0.1f };
	tState.iMaterial = 0;
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	NxMat34 pMat;
	pMat.isIdentity();

	for (int i = 0; i < 3; ++i)
	{
		NxVec3 vPos;
		memcpy(&vPos, matWorld.m[i], sizeof(_vec3));
		(NxVec3)m_pTransform->m_vInfo[i] = vPos;

		pMat.M.setColumn(i, vPos);
	}


	memcpy(&pMat.t, matWorld.m[3], sizeof(_vec3));

	m_pTransform->Get_Actor()->setGlobalPose(pMat);
	m_pTransform->Get_Actor()->getShapes()[0]->setGroup(1);
	m_pTransform->Get_Actor()->addForce(NxVec3(0.f, 100.f, 0.f));
	m_pTransform->Set_LimitVelocity(5.f);
	return S_OK;
}

HRESULT CObj_Leaf_TreeApple::Add_Component(void * pArg)
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	//For.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_MESH_STATIC,
		L"Component_Mesh_Obj_Leaf_TreeApple", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObj_Leaf_TreeApple::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CGameObject * CObj_Leaf_TreeApple::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CObj_Leaf_TreeApple(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CObj_Leaf_TreeApple::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CObj_Leaf_TreeApple(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CObj_Leaf_TreeApple::Free()
{

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CGameObject::Free();
}

