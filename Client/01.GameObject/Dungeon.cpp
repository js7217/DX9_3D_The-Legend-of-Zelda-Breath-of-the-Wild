#include "stdafx.h"
#include "Dungeon.h"
#include "KeyMgr.h"

#include "NxPlayer.h"

CDungeon::CDungeon(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CDungeon::CDungeon(CDungeon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDungeon::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CDungeon::Ready_GameObject(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_tState, pArg, sizeof(COLL_STATE));

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CDungeon::Update_GameObject(const _double & TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_fTime = (_float)TimeDelta;
	m_pRenderer->Set_Ray(TRUE);
	
	CNxPlayer* pPlayer = dynamic_cast<CNxPlayer*>(GET_INSTANCE(CObjectManager)->
		Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0));
	if (pPlayer != nullptr)
	{
		CNxTransform* pTransform = dynamic_cast<CNxTransform*>(pPlayer->Get_Component(L"Com_NxTransform"));

		if (pTransform != nullptr)
			m_vPlayerPos = *pTransform->Get_Info(CNxTransform::INFO_POS);

		m_bTimestop = pPlayer->Flag_Check_State(FLAG_TIMESTOP_TARGET);
		m_bMagnet = pPlayer->Flag_Check_State(FLAG_MAGNET_TARGET);
	}

	//CNxTransform* pTrans = (CNxTransform*)GET_INSTANCE(CObjectManager)->Get_Component(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", L"Com_NxTransform", 0);
	//m_vPlayerPos = *pTrans->Get_Info(CNxTransform::INFO_POS);

	return NO_EVENT;
}

_int CDungeon::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;
	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_SHADOWDEPTH2, this)))
		return END_EVENT;
	//if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_RIMLIGHT, this)))
	//	return END_EVENT;

	return NO_EVENT;
}

HRESULT CDungeon::Render_GameObject()
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
		pEffect->SetVector("g_vecEmessiveColor", &_vec4(0.1f, 0.5f, 0.9f, 1.f));
		pEffect->SetBool("g_bShade", true);
		pEffect->SetVector("g_vPlayerPos", &_vec4(m_vPlayerPos, 0));

		pEffect->SetBool("g_bMagnetDG", m_bMagnet);
		pEffect->SetBool("g_bTimeStopDG", m_bTimestop);

		if (m_bMagnet || m_bTimestop)
			m_fDist += m_fTime;
		else
			m_fDist = 0;

		pEffect->SetFloat("g_fDist", m_fDist);

		pEffect->CommitChanges();

		pEffect->BeginPass(14);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

void CDungeon::Render_Shadow(_int iPass)
{
	if (nullptr == m_pShader || nullptr == m_pMesh)
		return;


	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	Safe_AddRef(pEffect);

	if (FAILED(SetUp_ConstantTable(pEffect)))
		return;

	pEffect->Begin(nullptr, 0);

	for (_uint i = 0; i < m_pMesh->Get_NumMaterials(); ++i)
	{
		if (GET_INSTANCE(CManagement)->Get_CurrentSceneNum() == SCENE_DUNGEON01)
		{
			if (i != 35 && i != 9 && i != 11 && i != 10 && i != 4 && i != 23 && i != 8)
				continue;
		}
		else if (GET_INSTANCE(CManagement)->Get_CurrentSceneNum() == SCENE_DUNGEON02)
		{
			if (i != 1 && i != 3 && i != 5 && i != 8 && i != 9 && i != 12 && i != 13 
				&& i != 19 && i != 20 && i != 21 && i != 23 && i != 38 && i != 30 && i != 32)
				continue;
		}
		else if (GET_INSTANCE(CManagement)->Get_CurrentSceneNum() == SCENE_DUNGEON03)
		{
			if (i != 35 && i != 9 && i != 11 && i != 10 && i != 4 && i != 23 && i != 8)
				continue;
		}
		else if (GET_INSTANCE(CManagement)->Get_CurrentSceneNum() == SCENE_DUNGEON04)
		{
			if (i != 35 && i != 9 && i != 11 && i != 10 && i != 4 && i != 23 && i != 8)
				continue;
		}

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_DiffuseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_SpecularTexture", MESHTEXTURE::TYPE_SPECULAR)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_NormalTexture", MESHTEXTURE::TYPE_NORMAL)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_EmessiveTexture", MESHTEXTURE::TYPE_EMESSIVE)))
			return;

		pEffect->CommitChanges();

		pEffect->BeginPass(iPass);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);
}

void CDungeon::Render_RimLight()
{
	if (nullptr == m_pShader || nullptr == m_pMesh)
		return;

	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	Safe_AddRef(pEffect);

	if (FAILED(SetUp_ConstantTable(pEffect)))
		return;

	pEffect->Begin(nullptr, 0);

	for (_uint i = 0; i < m_pMesh->Get_NumMaterials(); ++i)
	{
		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_DiffuseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_SpecularTexture", MESHTEXTURE::TYPE_SPECULAR)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_NormalTexture", MESHTEXTURE::TYPE_NORMAL)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_EmessiveTexture", MESHTEXTURE::TYPE_EMESSIVE)))
			return;

		pEffect->SetBool("g_bSpecular", TRUE);
		pEffect->SetBool("g_bRimRight", FALSE);

		pEffect->CommitChanges();

		pEffect->BeginPass(3);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CDungeon::Add_Component()
{
	_tchar pName[256] = { 0, };
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_tState.cName, (_int)strlen(m_tState.cName), pName, 256);

	//For.Com_Mesh
	if (FAILED(CGameObject::Add_Component(m_tState.iSceneNum,
		pName, L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	if (m_tState.eCollider == COL_TRIMESH)
		m_tState.pMesh = m_pMesh->Get_Mesh();
	if (m_tState.pGameObject == nullptr)
		m_tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &m_tState)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeon::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

	if (FAILED(pEffect->SetFloat("g_fShadow", 0.f)))
		return E_FAIL;

	m_fFlowTime += m_fTime;
	pEffect->SetFloat("g_fTime", m_fFlowTime);
	pEffect->SetTexture("g_FlowTexture", m_pRenderer->Get_FlowTexture());
	pEffect->SetTexture("g_NoiseTexture", m_pRenderer->Get_NoiseTexture());
	pEffect->SetFloat("g_fBlinkTime", 1);
	pEffect->SetBool("g_bTimeStop", FALSE);

	Safe_Release(pEffect);
	return S_OK;
}

CGameObject * CDungeon::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CDungeon(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CDungeon::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CDungeon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CDungeon::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CGameObject::Free();
}

