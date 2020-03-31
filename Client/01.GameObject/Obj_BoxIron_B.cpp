#include "stdafx.h"
#include "Obj_BoxIron_B.h"
#include "KeyMgr.h"

Obj_BoxIron_B::Obj_BoxIron_B(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice)
{

}

Obj_BoxIron_B::Obj_BoxIron_B(const Obj_BoxIron_B & rhs)
	: CDgnObj(rhs)
{

}

HRESULT Obj_BoxIron_B::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT Obj_BoxIron_B::Ready_GameObject(void * pArg)
{
	_matrix matWorld = *(_matrix*)pArg;
	
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(matWorld.m);

	m_pTransform->Get_Actor()->setGlobalPose(pMat);

	NxMat34 mattemp =  m_pTransform->Get_Actor()->getGlobalPose();
	m_pTransform->Get_Actor()->wakeUp(1000);

	return S_OK;
}

_int Obj_BoxIron_B::Update_GameObject(const _double & TimeDelta)
{
	m_pTransform->Set_NxPos_Direction(&NxVec3(0.f, 0.00000001f, 0.f));

	CDgnObj::Update_GameObject(TimeDelta);

	m_fTime = (_float)TimeDelta;

	if (m_bMagnetCatch)
	{
		if (m_bBlinkTime)
			m_fBlinkTime += (_float)(TimeDelta);
		else
			m_fBlinkTime -= (_float)(TimeDelta);

		if (m_fBlinkTime >= 1.f || m_fBlinkTime <= 0.f)
			m_bBlinkTime = !m_bBlinkTime;
	}
	else
	{
		m_fBlinkTime = 0;
		m_bBlinkTime = FALSE;
	}


	if (m_pTransform->Get_Actor()->getGlobalPosition().y < -99.f)
		return END_EVENT;
	
	return NO_EVENT;
}

_int Obj_BoxIron_B::LateUpdate_GameObject(const _double & TimeDelta)
{
	CDgnObj::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT Obj_BoxIron_B::Render_GameObject()
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

		if (!m_bMagnetCatch)
			pEffect->BeginPass(4);
		else
			pEffect->BeginPass(12);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

HRESULT Obj_BoxIron_B::Add_Component(void * pArg)
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
		L"Component_Mesh_Obj_BoxIron_B", L"Com_Mesh", (CComponent**)&m_pMesh)))
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

	tState.cName = "Iron";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_DYNAMIC;
	tState.vLocalPos = { 0.f,(vMax.z - vMin.z) * 0.5f, 0.f };
	tState.vDimensions = { (vMax.x - vMin.x) * 0.5f, (vMax.y - vMin.y) * 0.5f, (vMax.z - vMin.z) * 0.5f };
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	m_pTransform->Get_Actor()->getShapes()[0]->setName("Magnet");

	return S_OK;
}

HRESULT Obj_BoxIron_B::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

	m_fFlowTime += m_fTime;
	pEffect->SetFloat("g_fTime", m_fFlowTime);
	pEffect->SetTexture("g_FlowTexture", m_pRenderer->Get_FlowTexture());
	pEffect->SetTexture("g_NoiseTexture", m_pRenderer->Get_NoiseTexture());
	pEffect->SetFloat("g_fBlinkTime", m_fBlinkTime);
	pEffect->SetBool("g_bTimeStop", FALSE);

	Safe_Release(pEffect);

	return S_OK;
}

CDgnObj * Obj_BoxIron_B::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new Obj_BoxIron_B(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * Obj_BoxIron_B::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new Obj_BoxIron_B(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void Obj_BoxIron_B::Free()
{

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CDgnObj::Free();
}

