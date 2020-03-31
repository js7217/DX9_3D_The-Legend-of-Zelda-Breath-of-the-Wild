#include "stdafx.h"
#include "Bullet_ArrowNormal.h"
#include "NxPlayer.h"

#include "EffectManager.h"

#include "KeyMgr.h"
#include "ObjectManager.h"

#include "Trail.h"


CBullet_ArrowNormal::CBullet_ArrowNormal(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice), m_pEffect(GET_INSTANCE(CEffectManager))
{

}

CBullet_ArrowNormal::CBullet_ArrowNormal(const CBullet_ArrowNormal & rhs)
	: CDgnObj(rhs), m_pEffect(rhs.m_pEffect)
{

}

HRESULT CBullet_ArrowNormal::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CBullet_ArrowNormal::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	NxMat34 matWorld = *(NxMat34*)pArg;
	_matrix matView = CGameObject::Get_Transform(D3DTS_VIEW);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	NxVec3 vLook = matView.m[2];
	matWorld.M.setColumn(2, vLook);

	m_pTransform->Get_Actor()->setGlobalPose(matWorld);
	
	vLook.normalize();
	m_pTransform->Get_Actor()->setLinearVelocity(vLook * 80.f);

	NxVec3 vRight = matView.m[0];
	vRight.normalize();

	NxVec3 vPos = matWorld.t + vLook * 1.25f;
	vPos.y += 0.75f;
	m_pTransform->Get_Actor()->setGlobalPosition(vPos);
	m_pTransform->Get_Actor()->setAngularVelocity(vRight * 1.f);
	m_pTransform->Set_LimitVelocity(999.f);

	matWorld.getColumnMajor44(m_matWorld.m);

	//////////////////////////////////////////////////////////////////////////

	CTrail::TRAIL tTrail;

	tTrail.iTextureNumber = 13;
	tTrail.iBufferSize = 40;
	tTrail.pSwordMatrix = &m_matWorld;
	tTrail.vUnderPivot = { -0.1f, 0, 0 };
	tTrail.vUpperPivot = { 0.1f, 0, 0};

	if (FAILED(GET_INSTANCE(CManagement)->Add_GameObject(SCENE_GAMEOBJECT,
		L"GameObject_Trail", SCENE_GAMEOBJECT, L"Layer_Trail_Arrow", &tTrail)))
		return E_FAIL;

	list<CGameObject*>* pList = GET_INSTANCE(CObjectManager)->Get_ObjectList(SCENE_GAMEOBJECT, L"Layer_Trail_Arrow");

	_uint iNum;

	if (pList == nullptr)
		iNum = 0;
	else
		iNum = (_uint)pList->size();

	m_pTrail = dynamic_cast<CTrail*>(GET_INSTANCE(CObjectManager)->Get_GameObject(SCENE_GAMEOBJECT,
		L"Layer_Trail_Arrow", iNum - 1));

	if (m_pTrail == nullptr)
		return E_FAIL;

	m_pTrail->Set_TrailRender(TRUE);

	//////////////////////////////////////////////////////////////////////////

	return S_OK;
}

_int CBullet_ArrowNormal::Update_GameObject(const _double & TimeDelta)
{
	if (END_EVENT == CGameObject::End_GameObject())
	{
		m_pTrail->Set_Dead();
		Create_ArrowHit();
		return END_EVENT;
	}

	CDgnObj::Update_GameObject(TimeDelta);

	NxVec3 vLook = m_pTransform->Get_Actor()->getGlobalPose().M.getColumn(2);
	vLook.normalize();

	_bool bIsDown = vLook.equals(NxVec3(vLook.x, -1.f, vLook.z), NxReal(0.02));

	if (bIsDown)
		m_pTransform->Get_Actor()->setAngularVelocity(NxVec3(0.f, 0.f, 0.f));
	
	m_pTransform->Get_WorldMatrix(&m_matWorld);
	m_pTrail->Set_TrailRender(TRUE);

	return NO_EVENT;
}

_int CBullet_ArrowNormal::LateUpdate_GameObject(const _double & TimeDelta)
{
	CDgnObj::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CBullet_ArrowNormal::Render_GameObject()
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

void CBullet_ArrowNormal::Create_ArrowHit()
{
	m_pEffect->Get_EffectSet(L"Arrow_Hit", m_pTransform->Get_WorldMatrixPointer());
}


HRESULT CBullet_ArrowNormal::Add_Component(void * pArg)
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
		L"Component_Mesh_Weapon_ArrowNormal", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	tState.cName = "Bullet_Arrow";
	tState.eCollider = COL_TRIMESH;
	tState.eCollOpt = OPT_DYNAMIC;
	tState.pMesh = m_pMesh->Get_Mesh();
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	m_pTransform->Get_Actor()->getShapes()[0]->setFlag(NX_TRIGGER_ENABLE, FALSE);

	NxSphereShapeDesc TriggerShape;
	TriggerShape.setToDefault();
	TriggerShape.name =  "ArrowColl";
	TriggerShape.radius = 0.25f;
	TriggerShape.localPose.t = NxVec3(0.f, 0.f, 0.8f);
	TriggerShape.shapeFlags |= NX_TRIGGER_ENABLE | NX_SF_DISABLE_RAYCASTING;
	TriggerShape.name = tState.cName;
	TriggerShape.skinWidth = NxReal(0.0);

	m_pTriggerShape = m_pTransform->Get_Actor()->createShape(TriggerShape);
	
	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_ArrowNormal::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

	Safe_Release(pEffect);

	return S_OK;
}

CDgnObj * CBullet_ArrowNormal::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CBullet_ArrowNormal(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CBullet_ArrowNormal::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CBullet_ArrowNormal(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBullet_ArrowNormal::Free()
{
	if (nullptr != m_pTriggerShape)
		m_pTransform->Get_Actor()->releaseShape(*m_pTriggerShape);

	Safe_Release(m_pTransform);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CDgnObj::Free();
}

