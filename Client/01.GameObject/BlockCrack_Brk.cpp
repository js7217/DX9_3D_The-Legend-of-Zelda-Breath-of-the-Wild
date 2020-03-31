#include "stdafx.h"
#include "BlockCrack_Brk.h"
#include "KeyMgr.h"

CBlockCrack_Brk::CBlockCrack_Brk(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice)
{

}

CBlockCrack_Brk::CBlockCrack_Brk(const CBlockCrack_Brk & rhs)
	: CDgnObj(rhs)
{

}

HRESULT CBlockCrack_Brk::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CBlockCrack_Brk::Ready_GameObject(void * pArg)
{	
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_fScele = NxMath::rand(0.1f, 1.5f);

	return S_OK;
}

_int CBlockCrack_Brk::Update_GameObject(const _double & TimeDelta)
{
	if (m_pTransform->Get_Actor()->isSleeping())
		return NO_EVENT;

	CDgnObj::Update_GameObject(TimeDelta);

	NxVec3 vPos = m_pTransform->Get_Actor()->getGlobalPosition();

	if (vPos.y <= -10.0)
	{
		m_pTransform->Get_Actor()->raiseActorFlag(NX_AF_DISABLE_COLLISION);
		m_pTransform->Get_Actor()->putToSleep();
	}
	return NO_EVENT;
}

_int CBlockCrack_Brk::LateUpdate_GameObject(const _double & TimeDelta)
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

HRESULT CBlockCrack_Brk::Render_GameObject()
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

void CBlockCrack_Brk::Create_NxTransform(NxVec3 vPos)
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

	tState.cName = "BlockCrack_Brk";
	tState.eCollider = COL_BOX;
	tState.NxFlag = NX_SF_DISABLE_RAYCASTING;
	tState.eCollOpt = OPT_DYNAMIC;
	tState.vDimensions = _vec3((vMax.x - vMin.x) * 0.5f, (vMax.y - vMin.y) * 0.5f, (vMax.z - vMin.z) * 0.5f);
	tState.fRadius = (vMax.z - vMin.z) * 0.5f;
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return;

	m_pTransform->Set_Scale(&_vec3(m_fScele, m_fScele, m_fScele));
	m_pTransform->Get_Actor()->setGlobalPosition(vPos);

	m_pTransform->Get_Actor()->getShapes()[0]->setGroup(1);
	m_pTransform->Set_LimitVelocity(2.f);
}

HRESULT CBlockCrack_Brk::Add_Component(void * pArg)
{
	// For.Com_Renderer
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	_int iRandom = NxMath::rand(0, 2);

	if (0 == iRandom)
	{
		//For.Com_Mesh
		if (FAILED(CDgnObj::Add_Component(SCENE_MESH_STATIC,
			L"Component_Mesh_DgnObj_BlockCrack_Brk_A", L"Com_Mesh", (CComponent**)&m_pMesh)))
			return E_FAIL;
	}
	else
		//For.Com_Mesh
		if (FAILED(CDgnObj::Add_Component(SCENE_MESH_STATIC,
			L"Component_Mesh_DgnObj_BlockCrack_Brk_B", L"Com_Mesh", (CComponent**)&m_pMesh)))
			return E_FAIL;

	return S_OK;
}

HRESULT CBlockCrack_Brk::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CDgnObj * CBlockCrack_Brk::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CBlockCrack_Brk(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CBlockCrack_Brk::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CBlockCrack_Brk(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBlockCrack_Brk::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CDgnObj::Free();
}

