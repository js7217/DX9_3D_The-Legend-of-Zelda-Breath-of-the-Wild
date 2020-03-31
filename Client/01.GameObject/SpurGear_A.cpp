#include "stdafx.h"
#include "SpurGear_A.h"
#include "KeyMgr.h"

#define	ROT_SPEED 90.f

CSpurGear_A::CSpurGear_A(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice)
{

}

CSpurGear_A::CSpurGear_A(const CSpurGear_A & rhs)
	: CDgnObj(rhs)
{

}

HRESULT CSpurGear_A::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CSpurGear_A::Ready_GameObject(void * pArg)
{
	_matrix matWorld = *(_matrix*)pArg;
	
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(matWorld.m);


	m_pTransform->Get_Actor()->setGlobalPose(pMat);

	NxMat34 mattemp =  m_pTransform->Get_Actor()->getGlobalPose();

	//m_pTransform->Set_Scale(&_vec3(1.f, 1.f, 1.f));
	m_pTransform->Get_Actor()->setMass(100.f);
	return S_OK;
}

_int CSpurGear_A::Update_GameObject(const _double & TimeDelta)
{

	CDgnObj::Update_GameObject(TimeDelta);

	m_fTime = (_float)TimeDelta;

	if (m_bTimeStop)
	{
		if (m_bBlinkTime)
			m_fBlinkTime += (_float)(TimeDelta * m_TimeStop);
		else
			m_fBlinkTime -= (_float)(TimeDelta * m_TimeStop);

		if (m_fBlinkTime >= 1.f || m_fBlinkTime <= 0.f)
			m_bBlinkTime = !m_bBlinkTime;
	}

	if (!m_bTimeStop && m_TimeStop == 0)
	{
		Move_SpurGear(TimeDelta);
	}

if (m_bTimeStop && m_TimeStop == 0)
	{
		//m_bTimeStop = TRUE;
		m_NxVelocity = NxVec3(0.f, 0.f, 0.f);
		m_NxVelocity = m_pTransform->Get_Actor()->getLinearVelocity();
		m_pTransform->Get_Actor()->raiseBodyFlag(NX_BF_FROZEN);
		m_pTransform->Get_Actor()->putToSleep();

		GET_INSTANCE(CEffectManager)->Get_EffectSet(L"TimeStop", m_pTransform->Get_WorldMatrixPointer(), 0, CEffect::GLOW);
	}

	if (m_bTimeStop && m_TimeStop > 10.0)
	{
		m_TimeStop = 0;
		m_bTimeStop = FALSE;
		m_NxVelocity *= m_fPower;
		m_pTransform->Get_Actor()->setLinearVelocity(m_NxVelocity);
		m_pTransform->Get_Actor()->clearBodyFlag(NX_BF_FROZEN);
		_matrix pMat = *m_pTransform->Get_WorldMatrixPointer();
		//GET_INSTANCE(CEffectManager)->Get_EffectSet(L"TimeStopPat", &pMat);
		GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"Bitalock_End_07.wav", CSoundMgr::DGN_OBJECT1);

		GET_INSTANCE(CEffectManager)->Get_EffectSet(L"TimeStop", m_pTransform->Get_WorldMatrixPointer(), 0, CEffect::GLOW);
		m_fBlinkTime = 0.f;
	}

	if (m_bTimeStop)
	{
		m_TimeStop += TimeDelta;
		CDgnObj::TimeStop_Sound(m_fBlinkTime);
	}

	if (m_TimeStop > 0 && m_TimeStop < 9.0)
	{
		_matrix pMat;
		D3DXMatrixIdentity(&pMat);
		pMat._41 = m_pTransform->Get_WorldMatrixPointer()->_41;
		pMat._42 = m_pTransform->Get_WorldMatrixPointer()->_42;
		pMat._43 = m_pTransform->Get_WorldMatrixPointer()->_43;
		GET_INSTANCE(CEffectManager)->Get_EffectSet(L"TimeStop2", &pMat, 0, CEffect::BLUR);
	}

	if(m_pTransform->Get_Actor()->isSleeping())
		m_pTransform->Get_Actor()->wakeUp(100);

	return NO_EVENT;
}

_int CSpurGear_A::LateUpdate_GameObject(const _double & TimeDelta)
{
	CDgnObj::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CSpurGear_A::Render_GameObject()
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

		if (m_bTimeStop)
			pEffect->BeginPass(13);
		else
			pEffect->BeginPass(5);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

void CSpurGear_A::Move_SpurGear(const _double & TimeDelta)
{
	m_RotAngle += TimeDelta * ROT_SPEED;
	if (m_RotAngle >= 360.0)
		m_RotAngle = 0.0;


	NxMat33 matRotZ, matRotY, matWorld;

	matRotY.rotY(D3DXToRadian(-90.f));
	matRotZ.rotZ(NxReal(D3DXToRadian(m_RotAngle)));

	matWorld = matRotY * matRotZ;

	m_pTransform->Get_Actor()->moveGlobalOrientation(matWorld);
	GET_INSTANCE(CSoundMgr)->Play_3D_Sound(L"SwitchGear_Roll_Near.wav", CSoundMgr::DGN_OBJECT0, m_pTransform->Get_GlobalPos());
}

HRESULT CSpurGear_A::Add_Component(void * pArg)
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
		L"Component_Mesh_DgnObj_SpurGear_A", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;
	//

	BYTE* pVertices;
	_vec3 vMin, vMax;
	m_pMesh->Get_Mesh()->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&pVertices);
	D3DXComputeBoundingBox((D3DXVECTOR3*)pVertices, m_pMesh->Get_Mesh()->GetNumVertices(),
		m_pMesh->Get_Mesh()->GetNumBytesPerVertex(), &vMin, &vMax);
	m_pMesh->Get_Mesh()->UnlockVertexBuffer();

	//memcpy(&tState, pArg, sizeof(COLL_STATE));

	tState.cName = "SpurGear_A";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_KINEMATIC;
	//tState.vStartPos = _vec3(rand() % 10, 1.f, rand() % 10);
	//	tState.vStartPos = { -24.f, 10.f, -40.f };
	tState.vLocalPos = { 0.f, 0.f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
	//tState.vDimensions = { 5.5f, 5.5f, 10.5f };
	tState.vDimensions = { (vMax.x - vMin.x) * 0.5f, (vMax.y - vMin.y) * 0.5f, (vMax.z - vMin.z) * 0.5f };
	tState.vVelocity = { 1.f, 1.f, 1.f };
	tState.fHeight = 0.f;
	tState.fRadius = (vMax.z - vMin.z) * 0.5f;
	tState.iMaterial = 0;
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	m_pTransform->Get_Actor()->getShapes()[0]->setName("TimeStop");

	return S_OK;
}

HRESULT CSpurGear_A::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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
	_matrix matCamView;
	D3DXMatrixInverse(&matCamView, nullptr, &Get_Transform(D3DTS_VIEW));

	_vec4 ViewPos;
	memcpy(&ViewPos, matCamView.m[3], sizeof(_vec4));

	_vec4 ViewDir(*m_pTransform->Get_Info(CNxTransform::INFO_POS), 0);

	ViewPos = (ViewDir - ViewPos);

	if (FAILED(pEffect->SetVector("g_ViewPos", D3DXVec4Normalize(&D3DXVECTOR4(ViewPos), &D3DXVECTOR4(ViewPos)))))
		return E_FAIL;

	m_fFlowTime += m_fTime;
	pEffect->SetFloat("g_fTime", m_fFlowTime);
	pEffect->SetTexture("g_FlowTexture", m_pRenderer->Get_FlowTexture());
	pEffect->SetTexture("g_NoiseTexture", m_pRenderer->Get_NoiseTexture());
	pEffect->SetFloat("g_fBlinkTime", m_fBlinkTime);
	pEffect->SetBool("g_bTimeStop", TRUE);

	Safe_Release(pEffect);

	return S_OK;
}

CDgnObj * CSpurGear_A::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CSpurGear_A(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CSpurGear_A::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CSpurGear_A(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CSpurGear_A::Free()
{

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CDgnObj::Free();
}

