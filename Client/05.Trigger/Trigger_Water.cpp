#include "stdafx.h"
#include "Trigger_Water.h"


CTriger_Water::CTriger_Water(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{

}

CTriger_Water::CTriger_Water(CTriger_Water & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTriger_Water::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CTriger_Water::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	

	return S_OK;
}

_int CTriger_Water::Update_GameObject(const _double & TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_fTime += (_float)TimeDelta;

	return NO_EVENT;
}

_int CTriger_Water::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_WATER, this)))
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_RIMLIGHT, this)))
		return END_EVENT;

	if (m_pTransform->Get_Actor()->isSleeping())
		m_pTransform->Get_Actor()->wakeUp(1000);
	
	return NO_EVENT;
}

HRESULT CTriger_Water::Render_GameObject()
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

		if (FAILED(m_pTexture->SetUp_OnShader(pEffect, "g_FlowTexture")))
			return E_FAIL;

		if (FAILED(m_pTextureNoise->SetUp_OnShader(pEffect, "g_NoiseTexture")))
			return E_FAIL;

		if (FAILED(pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.5f, 0.7f, 0.9f, 0.64f))))
			return E_FAIL;

		pEffect->SetBool("g_bShade", true);
		pEffect->SetBool("g_bSpecular", true);

		pEffect->CommitChanges();

		pEffect->BeginPass(0);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

void CTriger_Water::Render_RimLight()
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

		pEffect->SetBool("g_bSpecular", true);

		pEffect->CommitChanges();

		pEffect->BeginPass(1);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	//////////////////////////////////////////////////////////////////////////

	//if (FAILED(SetUp_ConstantTable(pEffect)))
	//	return;

	//pEffect->Begin(nullptr, 0);

	//for (_uint i = 0; i < m_pMesh->Get_NumMaterials(); ++i)
	//{
	//	if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_DiffuseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
	//		return;

	//	if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_SpecularTexture", MESHTEXTURE::TYPE_SPECULAR)))
	//		return;

	//	if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_NormalTexture", MESHTEXTURE::TYPE_NORMAL)))
	//		return;

	//	if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_EmessiveTexture", MESHTEXTURE::TYPE_EMESSIVE)))
	//		return;

	//	//if (FAILED(pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.8f, 0.8f, 0.7f, 0.6f))))
	//	//	return;

	//	pEffect->CommitChanges();

	//	pEffect->BeginPass(2);

	//	m_pMesh->Render_Mesh(i);

	//	pEffect->EndPass();
	//}

	//pEffect->End();

	Safe_Release(pEffect);

	return;
}

HRESULT CTriger_Water::Add_Component(void * pArg)
{
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));
	
	if (pArg != nullptr)
		memcpy(&tState, pArg, sizeof(COLL_STATE));

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Water", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	//For.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_MESH_STATIC,
		L"Component_Mesh_DungeonWater", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	//For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_Flow", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	//For.Com_TextureNoise
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_FlowNoise", L"Com_TextureNoise", (CComponent**)&m_pTextureNoise)))
		return E_FAIL;

	m_pTransform->Set_Scale(&tState.vVelocity);
	m_pTransform->Get_Actor()->getShapes()[0]->setSkinWidth(NxReal(0.01));
	m_pTransform->Get_Actor()->getShapes()[0]->setName(tState.cName);
	
	return S_OK;
}

HRESULT CTriger_Water::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect || nullptr == m_pMesh || nullptr == m_pTransform)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (GET_INSTANCE(CManagement)->Get_CurrentSceneNum() == SCENE_FIELD)
	{
		pEffect->SetFloat("g_matTilling", 3.5f);
	}
	else
	{
		pEffect->SetFloat("g_matTilling", 1.5f);
	}

	if (FAILED(pEffect->SetMatrix("g_matWorld", m_pTransform->Get_WorldMatrixPointer())))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matView", &Get_Transform(D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matProj", &Get_Transform(D3DTS_PROJECTION))))
		return E_FAIL;

	if (FAILED(pEffect->SetFloat("g_fTime", m_fTime)))
		return E_FAIL;

	//±¤¿øÀÇ ºäÇà·Ä, Åõ¿µÇà·Ä
	pEffect->SetMatrix("g_matLightView", &m_pRenderer->Get_LightViewMatrix());
	pEffect->SetMatrix("g_matLightProj", &m_pRenderer->Get_LightProjMatrix());

	Safe_Release(pEffect);

	return S_OK;
}

CGameObject * CTriger_Water::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CTriger_Water(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CTriger_Water::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CTriger_Water(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTriger_Water::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTextureNoise);

	CGameObject::Free();
}
