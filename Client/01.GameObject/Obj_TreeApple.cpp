#include "stdafx.h"
#include "Obj_TreeApple.h"
#include "NxPlayer.h"

#include "Obj_Trunk_TreeApple.h"

#include "EffectManager.h"

#include "KeyMgr.h"
#include "ObjectManager.h"
CObj_TreeApple::CObj_TreeApple(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{

}

CObj_TreeApple::CObj_TreeApple(const CObj_TreeApple & rhs)
	: CGameObject(rhs)
{

}

HRESULT CObj_TreeApple::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CObj_TreeApple::Ready_GameObject(void * pArg)
{
	_matrix matWorld = *(_matrix*)pArg;

	if (FAILED(Add_Component(pArg)))
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
	memcpy(&m_vPosition, matWorld.m[3], sizeof(_vec3));

	m_pTransform->Get_Actor()->setGlobalPose(pMat);

	_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();
	
	//Create Stump, Trunk
	m_pStump = GET_INSTANCE(CObjectManager)->Add_GameObject(iSceneNum, L"GameObject_Obj_Stump_TreeApple", iSceneNum, &matWorld);
	m_pTrunk = GET_INSTANCE(CObjectManager)->Add_GameObject(iSceneNum, L"GameObject_Obj_Trunk_TreeApple", iSceneNum, &matWorld);
	
	return S_OK;
}

_int CObj_TreeApple::Update_GameObject(const _double & TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if(m_pTransform->Get_Actor()->isSleeping())
		m_pTransform->Get_Actor()->wakeUp(9999);

	if (m_pFrustumCom == nullptr)
		return _int();

	_vec3 vPos;
	m_pTransform->Get_Info(CNxTransform::INFO_POS, &vPos);
	m_bDraw = m_pFrustumCom->WorldPt_InFrustum(&vPos, m_pTransform, 1.f);



	if (strcmp("Dead", m_pTransform->Get_Actor()->getName()) == 0)
	{
		m_bIsAttack = true;
		_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();
		// Add_GameObject
		GET_INSTANCE(CObjectManager)->Add_GameObject(m_pStump, L"Layer_TreeApple_Stump", iSceneNum);
		
		if (nullptr == m_pTarget_Grab) //때려서 없애버릴때.
			dynamic_cast<CObj_Trunk_TreeApple*>(m_pTrunk)->Create_NxTransform(m_vVelocity);
		else // 잡힐때.
		{
			dynamic_cast<CObj_Trunk_TreeApple*>(m_pTrunk)->Create_NxTransform(NxVec3(0.f, 0.f, 0.f));
			dynamic_cast<CObj_Trunk_TreeApple*>(m_pTrunk)->Set_Target_Grab(m_pTarget_Grab);
		}
		
		GET_INSTANCE(CSoundMgr)->Play_3D_Sound(L"Tree_Fall_WithLeaf1.wav", CSoundMgr::FIELD_OBJECT1, m_pTransform->Get_GlobalPos());
		GET_INSTANCE(CSoundMgr)->Play_3D_Sound(L"Tree_Medium_FallDown_Default_WithLeaf1.wav", CSoundMgr::FIELD_OBJECT2, m_pTransform->Get_GlobalPos());
		GET_INSTANCE(CObjectManager)->Add_GameObject(m_pTrunk, L"Layer_TreeApple_Trunk", iSceneNum);
		
		return END_EVENT;
	}

	return NO_EVENT;
}

_int CObj_TreeApple::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;
	if (!m_bDraw)
		return NO_EVENT;
	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CObj_TreeApple::Render_GameObject()
{
	if (nullptr == m_pShader || nullptr == m_pMesh_Tree)
		return E_FAIL;

	
	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	if (nullptr == pEffect)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(SetUp_ConstantTable(pEffect)))
		return E_FAIL;

	pEffect->Begin(nullptr, 0);

	for (_uint i = 0; i < m_pMesh_Tree->Get_NumMaterials(); ++i)
	{
		if (FAILED(m_pMesh_Tree->SetTexture_OnShader(pEffect, i, "g_DiffuseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pMesh_Tree->SetTexture_OnShader(pEffect, i, "g_SpecularTexture", MESHTEXTURE::TYPE_SPECULAR)))
			return E_FAIL;

		if (FAILED(m_pMesh_Tree->SetTexture_OnShader(pEffect, i, "g_NormalTexture", MESHTEXTURE::TYPE_NORMAL)))
			return E_FAIL;
		
		if (FAILED(m_pMesh_Tree->SetTexture_OnShader(pEffect, i, "g_EmessiveTexture", MESHTEXTURE::TYPE_EMESSIVE)))
			return E_FAIL;

		if (FAILED(m_pMesh_Tree->SetTexture_OnShader(pEffect, i, "g_MaskTexture", MESHTEXTURE::TYPE_MASK)))
			return E_FAIL;

		pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.f, 0.f, 0.f, 1.f));
		pEffect->SetVector("g_vecEmessiveColor", &_vec4(1.f, 0.623f, 0.251f, 1.f));
		pEffect->SetBool("g_bShade", true);

		pEffect->CommitChanges();

		pEffect->BeginPass(5);

		m_pMesh_Tree->Render_Mesh(i);

		pEffect->EndPass();
	}

	for (_uint i = 0; i < m_pMesh_Leafs->Get_NumMaterials(); ++i)
	{
		if (FAILED(m_pMesh_Leafs->SetTexture_OnShader(pEffect, i, "g_DiffuseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pMesh_Leafs->SetTexture_OnShader(pEffect, i, "g_MaskTexture", MESHTEXTURE::TYPE_MASK)))
			return E_FAIL;

		if (FAILED(m_pMesh_Leafs->SetTexture_OnShader(pEffect, i, "g_SpecularTexture", MESHTEXTURE::TYPE_SPECULAR)))
			return E_FAIL;

		if (FAILED(m_pMesh_Leafs->SetTexture_OnShader(pEffect, i, "g_NormalTexture", MESHTEXTURE::TYPE_NORMAL)))
			return E_FAIL;

		if (FAILED(m_pMesh_Leafs->SetTexture_OnShader(pEffect, i, "g_EmessiveTexture", MESHTEXTURE::TYPE_EMESSIVE)))
			return E_FAIL;

		pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.f, 0.f, 0.f, 1.f));
		pEffect->SetVector("g_vecEmessiveColor", &_vec4(1.f, 0.623f, 0.251f, 1.f));
		pEffect->SetBool("g_bShade", true);

		pEffect->CommitChanges();

		pEffect->BeginPass(5);

		m_pMesh_Leafs->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}


HRESULT CObj_TreeApple::Add_Component(void * pArg)
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
		L"Component_Mesh_Obj_TreeApple_Tree", L"Com_Mesh1", (CComponent**)&m_pMesh_Tree)))
		return E_FAIL;

	//For.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_MESH_STATIC,
		L"Component_Mesh_Obj_TreeApple_Leaf", L"Com_Mesh2", (CComponent**)&m_pMesh_Leafs)))
		return E_FAIL;

	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	tState.cName = "Obj_TreeApple";
	tState.eCollider = COL_TRIMESH;
	tState.eCollOpt = OPT_DYNAMIC;
	tState.vLocalPos = { 0.f, 0.f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
	tState.iMaterial = 0;
	tState.pMesh = m_pMesh_Tree->Get_Mesh();
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	m_pTransform->Get_Actor()->raiseBodyFlag(NX_BF_FROZEN);

	NxCapsuleShapeDesc CapsuleDesc;
	CapsuleDesc.setToDefault();

	CapsuleDesc.height = 4.f;
	CapsuleDesc.radius = 1.f;
	CapsuleDesc.localPose.t = NxVec3(0.f, 2.f, 0.f);
	CapsuleDesc.name = "Obj_Trigger_TreeApple";
	CapsuleDesc.shapeFlags |= NX_TRIGGER_ENABLE | NX_SF_DISABLE_RAYCASTING;
	CapsuleDesc.skinWidth = 0.0f;

	m_pTransform->Get_Actor()->createShape(CapsuleDesc);

	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObj_TreeApple::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect || nullptr == m_pMesh_Tree || nullptr == m_pTransform)
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

CGameObject * CObj_TreeApple::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CObj_TreeApple(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CObj_TreeApple::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CObj_TreeApple(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CObj_TreeApple::Free()
{
	if (!m_bIsAttack)
	{
		Safe_Release(m_pTrunk);
		Safe_Release(m_pStump);
	}

	Safe_Release(m_pTransform);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh_Tree);
	Safe_Release(m_pMesh_Leafs);

	CGameObject::Free();
}

