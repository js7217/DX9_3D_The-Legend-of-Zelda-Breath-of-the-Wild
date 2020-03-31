#include "stdafx.h"
#include "CapShutter.h"
#include "KeyMgr.h"
#include "ObjectManager.h"
#include "SpurGear_A.h"

#define	ROT_SPEED 90.f

CCapShutter::CCapShutter(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice)
{

}

CCapShutter::CCapShutter(const CCapShutter & rhs)
	: CDgnObj(rhs)
{

}

HRESULT CCapShutter::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CCapShutter::Ready_GameObject(void * pArg)
{
	_matrix matWorld = *(_matrix*)pArg;
	
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(matWorld.m);
	m_pTransform->Get_Actor()->setGlobalPose(pMat);

	m_pTransform->Get_Actor()->setMass(100.f);
	m_pTransform->Get_Actor()->getShapes()[0]->setSkinWidth(0.f);

	return S_OK;
}

_int CCapShutter::Update_GameObject(const _double & TimeDelta)
{
	CDgnObj::Update_GameObject(TimeDelta);

	if(!m_bTimeStop)
		Move_CapShutter(TimeDelta);

	if (m_bTimeStop)
		m_TimeStop += TimeDelta;

	return NO_EVENT;
}

_int CCapShutter::LateUpdate_GameObject(const _double & TimeDelta)
{
	CDgnObj::LateUpdate_GameObject(TimeDelta);

	CSpurGear_A* pGear = dynamic_cast<CSpurGear_A*>(GET_INSTANCE(CObjectManager)->
		Get_GameObject(SCENE_DUNGEON03, L"Layer_Gear", 0));

	if (pGear != nullptr && pGear->m_bTimeStop)
		m_bTimeStop = TRUE;
	else
		m_bTimeStop = FALSE;

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CCapShutter::Render_GameObject()
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

		pEffect->BeginPass(5);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

void CCapShutter::Move_CapShutter(const _double & TimeDelta)
{
	m_RotAngle += TimeDelta * ROT_SPEED;

	NxMat33 matRotX, matRotY, matWorld;

	matRotX.rotX(NxReal(D3DXToRadian(-m_RotAngle)));

	matWorld = matRotX;

	m_pTransform->Get_Actor()->moveGlobalOrientation(matRotX);
}

HRESULT CCapShutter::Add_Component(void * pArg)
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
		L"Component_Mesh_DgnObj_CapShutter", L"Com_Mesh", (CComponent**)&m_pMesh)))
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

	tState.cName = "CapShutter";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_KINEMATIC;
	tState.vLocalPos = { -0.2f, -0.1f, 0.f };
	tState.vDimensions = { (vMax.x - vMin.x) * 0.5f, (vMax.y - vMin.y) * 0.5f + 0.2f, (vMax.z - vMin.z) * 0.5f };
	tState.fRadius = (vMax.z - vMin.z) * 0.5f;	tState.iMaterial = 0;
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	m_pTransform->Set_Scale(&_vec3(1.f, 1.5f, 1.f));

	m_pTransform->Get_Actor()->raiseBodyFlag(NX_BF_FROZEN_POS);

	return S_OK;
}

HRESULT CCapShutter::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CDgnObj * CCapShutter::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CCapShutter(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CCapShutter::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CCapShutter(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCapShutter::Free()
{

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CDgnObj::Free();
}

