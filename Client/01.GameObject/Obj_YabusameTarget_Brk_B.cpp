#include "stdafx.h"
#include "Obj_YabusameTarget_Brk_B.h"
#include "KeyMgr.h"

CObj_YabusameTarget_Brk_B::CObj_YabusameTarget_Brk_B(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice)
{

}

CObj_YabusameTarget_Brk_B::CObj_YabusameTarget_Brk_B(const CObj_YabusameTarget_Brk_B & rhs)
	: CDgnObj(rhs)
{

}

HRESULT CObj_YabusameTarget_Brk_B::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CObj_YabusameTarget_Brk_B::Ready_GameObject(void * pArg)
{	
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_fScele = NxMath::rand(0.1f, 1.5f);

	return S_OK;
}

_int CObj_YabusameTarget_Brk_B::Update_GameObject(const _double & TimeDelta)
{
	if (m_pTransform->Get_Actor()->isSleeping())
		return NO_EVENT;

	CDgnObj::Update_GameObject(TimeDelta);

	NxVec3 vPos = m_pTransform->Get_Actor()->getGlobalPosition();

	if (vPos.y <= -20.0)
	{
		m_pTransform->Get_Actor()->raiseActorFlag(NX_AF_DISABLE_COLLISION);
		m_pTransform->Get_Actor()->putToSleep();
		return END_EVENT;
	}
	return NO_EVENT;
}

_int CObj_YabusameTarget_Brk_B::LateUpdate_GameObject(const _double & TimeDelta)
{
	CDgnObj::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (!m_pTransform->Get_Actor()->isSleeping())
	{
		if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
			return END_EVENT;
	}
	return NO_EVENT;
}

HRESULT CObj_YabusameTarget_Brk_B::Render_GameObject()
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
		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_BaseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
			return E_FAIL;

		pEffect->CommitChanges();

		pEffect->BeginPass(0);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

void CObj_YabusameTarget_Brk_B::Create_NxTransform(NxVec3 vPos)
{
	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	BYTE* pVertices;
	_vec3 vMin, vMax;

	m_pMesh->Get_Mesh()->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&pVertices);
	D3DXComputeBoundingBox((D3DXVECTOR3*)pVertices, m_pMesh->Get_Mesh()->GetNumVertices(),
		m_pMesh->Get_Mesh()->GetNumBytesPerVertex(), &vMin, &vMax);
	m_pMesh->Get_Mesh()->UnlockVertexBuffer();

	tState.cName = "Obj_YabusameTarget_Brk_B";
	tState.eCollider = COL_BOX;
	tState.NxFlag = NX_SF_DISABLE_RAYCASTING;
	tState.eCollOpt = OPT_DYNAMIC;
	tState.vDimensions = _vec3(1.f, 1.f, 1.f);
//	tState.vDimensions = _vec3((vMax.x - vMin.x) * 0.5f, (vMax.y - vMin.y) * 0.5f, (vMax.z - vMin.z) * 0.5f);
	tState.fRadius = (vMax.z - vMin.z) * 0.5f;
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return;

	m_pTransform->Set_Scale(&_vec3(m_fScele, m_fScele, m_fScele));

	m_pTransform->Get_Actor()->setGlobalPosition(vPos);
	m_pTransform->Get_Actor()->getShapes()[0]->setGroup(1);

	NxReal fRan = (NxReal)NxMath::rand(0, 100);
	m_pTransform->Get_Actor()->addForce(NxVec3(0.f, 100.f, 0.f));


	m_pTransform->Set_LimitVelocity(5.f);
}

HRESULT CObj_YabusameTarget_Brk_B::Add_Component(void * pArg)
{
	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

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
		L"Component_Mesh_Obj_YabusameTarget_Brk_B", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObj_YabusameTarget_Brk_B::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CDgnObj * CObj_YabusameTarget_Brk_B::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CObj_YabusameTarget_Brk_B(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CObj_YabusameTarget_Brk_B::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CObj_YabusameTarget_Brk_B(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CObj_YabusameTarget_Brk_B::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CDgnObj::Free();
}

