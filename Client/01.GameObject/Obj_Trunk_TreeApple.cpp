#include "stdafx.h"
#include "Obj_Trunk_TreeApple.h"
#include "NxPlayer.h"

#include "Obj_Log_TreeApple.h"
#include "Obj_Leaf_TreeApple.h"

#include "EffectManager.h"

#include "KeyMgr.h"
#include "ObjectManager.h"
CObj_Trunk_TreeApple::CObj_Trunk_TreeApple(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
	, m_pPhysx(GET_INSTANCE(CPhysXMgr))
{

}

CObj_Trunk_TreeApple::CObj_Trunk_TreeApple(const CObj_Trunk_TreeApple & rhs)
	: CGameObject(rhs)
	, m_pPhysx(rhs.m_pPhysx)
{

}

HRESULT CObj_Trunk_TreeApple::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CObj_Trunk_TreeApple::Ready_GameObject(void * pArg)
{

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_matWorld = *(_matrix*)pArg;

	_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();

	m_pLog = GET_INSTANCE(CObjectManager)->Add_GameObject(iSceneNum, L"GameObject_Obj_Log_TreeApple", iSceneNum);

	// GameObject_Obj_Leaf_TreeApple
	for (_int i = 0; i < 15; ++i)
	{
		CGameObject* pLeaf = GET_INSTANCE(CObjectManager)->Add_GameObject(iSceneNum, L"GameObject_Obj_Leaf_TreeApple", iSceneNum);
		m_LeafList.emplace_back(pLeaf);
	}

	return S_OK;
}

_int CObj_Trunk_TreeApple::Update_GameObject(const _double & TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bIsCreate_Log || nullptr != m_pTarget_Grab)
	{
		_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();
		// Add_GameObject
		_matrix matWorld = *m_pTransform->Get_WorldMatrixPointer();
		dynamic_cast<CObj_Log_TreeApple*>(m_pLog)->Create_NxTransform(matWorld);
		dynamic_cast<CObj_Log_TreeApple*>(m_pLog)->Set_GrabObject(m_pTarget_Grab);

		GET_INSTANCE(CObjectManager)->Add_GameObject(m_pLog, L"Layer_TreeApple_Log", iSceneNum);

		if (m_bIsCreate_Log)
			matWorld._42 += 2.f;
		else
			matWorld._42 += 5.f;

		for (auto& pLeaf : m_LeafList)
		{
			dynamic_cast<CObj_Leaf_TreeApple*>(pLeaf)->Create_NxTransform(matWorld);
			GET_INSTANCE(CObjectManager)->Add_GameObject(pLeaf, L"Layer_TreeApple_Leaf", iSceneNum);
		}
		m_LeafList.clear();
		return END_EVENT;
	}

	Check_CreateLog();

	return NO_EVENT;
}

_int CObj_Trunk_TreeApple::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CObj_Trunk_TreeApple::Render_GameObject()
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

HRESULT CObj_Trunk_TreeApple::Create_NxTransform(NxVec3 vVelocity)
{
	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	tState.cName = "Obj_Trunk_TreeApple";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_DYNAMIC;
	tState.vLocalPos = { 0.f, 3.f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
	tState.vDimensions = { 0.4f, 2.f, 0.4f };
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
		memcpy(&vPos, m_matWorld.m[i], sizeof(_vec3));
		(NxVec3)m_pTransform->m_vInfo[i] = vPos;

		pMat.M.setColumn(i, vPos);
	}

	memcpy(&pMat.t, m_matWorld.m[3], sizeof(_vec3));

	m_pTransform->Get_Actor()->setGlobalPose(pMat);
	
	vVelocity.normalize();
	vVelocity.y = 0.f;
	vVelocity *= 5.f;

	m_pTransform->Get_Actor()->setLinearVelocity(vVelocity);
	return S_OK;
}

void CObj_Trunk_TreeApple::Check_CreateLog()
{
	_int iCnt = m_pPhysx->Get_NxScene()->getNbActors();

	NxBoxShape* pBoxShape = m_pTransform->Get_Shape()->isBox();
	if (nullptr == pBoxShape)
		return;

	NxBox Box;
	pBoxShape->getWorldOBB(Box);

	for (_int i = 0; i < iCnt; ++i)
	{
		NxActor* pActor = m_pPhysx->Get_NxScene()->getActors()[i];

		const char* pName = pActor->getName();
		const char* pShapeName = pActor->getShapes()[0]->getName();

		if (nullptr != pShapeName && strcmp(pShapeName, "Terrain") == 0)
		{
			m_bIsCreate_Log = m_pPhysx->Get_NxScene()->getActors()[i]->getShapes()[0]->checkOverlapOBB(Box);
			if (m_bIsCreate_Log)
				return;
		}
		else
			continue;
	}
}


HRESULT CObj_Trunk_TreeApple::Add_Component(void * pArg)
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
		L"Component_Mesh_Obj_Trunk_TreeApple", L"Com_Mesh1", (CComponent**)&m_pMesh_Tree)))
		return E_FAIL;

	//For.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_MESH_STATIC,
		L"Component_Mesh_Obj_TreeApple_Leaf", L"Com_Mesh2", (CComponent**)&m_pMesh_Leafs)))
		return E_FAIL;

	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObj_Trunk_TreeApple::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CGameObject * CObj_Trunk_TreeApple::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CObj_Trunk_TreeApple(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CObj_Trunk_TreeApple::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CObj_Trunk_TreeApple(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CObj_Trunk_TreeApple::Free()
{
	//
	for (auto& pObj : m_LeafList)
		Safe_Release(pObj);

	if(!m_bIsCreate_Log && nullptr == m_pTarget_Grab)
		Safe_Release(m_pLog);

	Safe_Release(m_pTransform);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh_Tree);
	Safe_Release(m_pMesh_Leafs);

	CGameObject::Free();
}

