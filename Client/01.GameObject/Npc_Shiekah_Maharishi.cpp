#include "stdafx.h"
#include "Npc_Shiekah_Maharishi.h"
#include "KeyMgr.h"
#include "DirectInput.h"
#include "KeyMgr.h"
#include "FontMgr.h"


CNpc_Shiekah_Maharishi::CNpc_Shiekah_Maharishi(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice), m_pManagement(GET_INSTANCE(CManagement)),
	m_pDebug(GET_INSTANCE(CDebug_Console)),
	m_pPhysx(GET_INSTANCE(CPhysXMgr))
{

	Safe_AddRef(m_pDebug);

}

CNpc_Shiekah_Maharishi::CNpc_Shiekah_Maharishi(const CNpc_Shiekah_Maharishi & rhs)
	: CDgnObj(rhs), m_pManagement(rhs.m_pManagement),
	m_pDebug(rhs.m_pDebug),
	m_pPhysx(rhs.m_pPhysx)
{

	Safe_AddRef(m_pDebug);

}

HRESULT CNpc_Shiekah_Maharishi::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CNpc_Shiekah_Maharishi::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	m_pMesh->SetUp_AnimationSet(1);

	_matrix matWorld = *(_matrix*)pArg;

	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(matWorld.m);

	m_pTransform->Get_Actor()->setGlobalPose(pMat);

	NxMat34 mattemp = m_pTransform->Get_Actor()->getGlobalPose();


	//m_pTransform->Set_Scale(&_vec3(0.01f, 0.01f, 0.01f));
	//m_pTransform->Rotation(CNxTransform::ROT_X, D3DXToRadian(180.f));
	//m_pTransform->Rotation(CNxTransform::ROT_Y, D3DXToRadian(180.f));
	return S_OK;
}

_int CNpc_Shiekah_Maharishi::Update_GameObject(const _double & TimeDelta)
{
	CDgnObj::Update_GameObject(TimeDelta);

	m_pMesh->Play_Animation(TimeDelta);
	return NO_EVENT;
}

_int CNpc_Shiekah_Maharishi::LateUpdate_GameObject(const _double & TimeDelta)
{
	CDgnObj::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	//if (nullptr != m_pDebug && m_pDebug->Get_bPlayer() == FALSE)
	//	m_pDebug->Set_PlayerPos(*m_pTransform->Get_Info(CNxTransform::INFO_POS));



	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CNpc_Shiekah_Maharishi::Render_GameObject()
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

	for (_uint i = 0; i < m_pMesh->Get_NumMeshContainer(); ++i)
	{
		if (FAILED(m_pMesh->Update_SkinnedMesh(i)))
			break;

		for (_uint j = 0; j < m_pMesh->Get_NumSubSet(i); ++j)
		{
			if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, j, "g_DiffuseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, j, "g_SpecularTexture", MESHTEXTURE::TYPE_SPECULAR)))
				return E_FAIL;

			if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, j, "g_NormalTexture", MESHTEXTURE::TYPE_NORMAL)))
				return E_FAIL;

			if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, j, "g_EmessiveTexture", MESHTEXTURE::TYPE_EMESSIVE)))
				return E_FAIL;

			if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, j, "g_MaskTexture", MESHTEXTURE::TYPE_MASK)))
				return E_FAIL;

			if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, j, "g_FXTexture", MESHTEXTURE::TYPE_FX)))
				return E_FAIL;

			pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.f, 0.f, 0.f, 1.f));
			pEffect->SetVector("g_vecEmessiveColor", &_vec4(0.1f, 0.5f, 0.7f, 1.f));

			pEffect->SetBool("g_bShade", true);

			pEffect->CommitChanges();

			pEffect->BeginPass(5);

			m_pMesh->Render_Mesh(i, j);

			pEffect->EndPass();
		}
	}

	pEffect->End();

	Safe_Release(pEffect);

	////////////////////

	////////////////////	Render_Fonts
	

	return NO_EVENT;
}

VOID CNpc_Shiekah_Maharishi::Set_Animation(_uint iAniState)
{
	m_iCurAniState = iAniState;

	if (m_iCurAniState != m_iPreAniState)
	{
		m_pMesh->SetUp_AnimationSet(m_iCurAniState);
		m_iPreAniState = m_iCurAniState;
	}
}


HRESULT CNpc_Shiekah_Maharishi::Add_Component()
{
	//For NxTransform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));
	tState.cName = "Npc_Shiekah_Maharishi";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_STATIC;
	tState.vDimensions = { 1.5f, 1.5f, 1.5f };

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

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
		L"Component_Mesh_Npc_Shiekah_Maharishi", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpc_Shiekah_Maharishi::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect || nullptr == m_pMesh || nullptr == m_pTransform)
		return E_FAIL;

	Safe_AddRef(pEffect);

	//if (FAILED(m_pTransform->SetUp_OnShader(pEffect, "g_matWorld")))
	//	return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matWorld", m_pTransform->Get_WorldMatrixPointer())))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matView", &Get_Transform(D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matProj", &Get_Transform(D3DTS_PROJECTION))))
		return E_FAIL;

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CNpc_Shiekah_Maharishi::Create_Capsule_Coll(NxVec3 nxPos)
{
	return E_NOTIMPL;
}

CDgnObj * CNpc_Shiekah_Maharishi::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CNpc_Shiekah_Maharishi(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CNpc_Shiekah_Maharishi::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CNpc_Shiekah_Maharishi(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNpc_Shiekah_Maharishi::Free()
{

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDebug);
	Safe_Release(m_pMesh);


	CDgnObj::Free();
}
