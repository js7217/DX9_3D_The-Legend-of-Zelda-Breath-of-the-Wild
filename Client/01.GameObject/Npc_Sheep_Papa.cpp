#include "stdafx.h"
#include "Npc_Sheep_Papa.h"

CNpc_Sheep_Papa::CNpc_Sheep_Papa(LPDIRECT3DDEVICE9 pDevice)
	: CNpc(pDevice)
{

}

CNpc_Sheep_Papa::CNpc_Sheep_Papa(const CNpc_Sheep_Papa & rhs)
	: CNpc(rhs)
{

}

HRESULT CNpc_Sheep_Papa::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CNpc_Sheep_Papa::Ready_GameObject(void * pArg)
{	
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_eActionState = STATE_READY;

	for (int i = 0; i < STATE_END; ++i)
		m_bIsAction[i] = false;

	m_pMesh->SetUp_AnimationSet(SHEEPCARE2);

	return S_OK;
}

_int CNpc_Sheep_Papa::Update_GameObject(const _double & TimeDelta)
{
	CNpc::Update_GameObject(TimeDelta);

	m_dwTimeDelta = TimeDelta;

	_vec3 vPos;
	m_pTransform->Get_Info(CNxTransform::INFO_POS, &vPos);
	m_bDraw = m_pFrustumCom->WorldPt_InFrustum(&vPos, m_pTransform, 10.f);

	_matrix matView = Get_Transform(D3DTS_VIEW);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3 vCamPos = { matView._41, matView._42, matView._43 };

	if (50.f <= D3DXVec3Length(&(vCamPos - vPos)))
		m_bDraw = false;

	ActionSequence(m_dwTimeDelta);

	return NO_EVENT;
}

_int CNpc_Sheep_Papa::LateUpdate_GameObject(const _double & TimeDelta)
{
	CNpc::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (false == m_bDraw)
		return NOERROR;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CNpc_Sheep_Papa::Render_GameObject()
{
	if (nullptr == m_pShader || nullptr == m_pMesh)
		return E_FAIL;
	m_pMesh->Play_Animation(m_dwTimeDelta);

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

			pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.f, 0.f, 0.f, 1.f));
			pEffect->SetVector("g_vecEmessiveColor", &_vec4(0.1f, 0.5f, 0.9f, 1.f));
			pEffect->SetBool("g_bShade", true);

			pEffect->CommitChanges();

			pEffect->BeginPass(4);

			m_pMesh->Render_Mesh(i, j);

			pEffect->EndPass();
		}
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

void CNpc_Sheep_Papa::Set_ActionState()
{
	if (STATE_READY == m_eActionState)
	{
		m_eActionState = STATE_SHEEPCARE;
	}
}

void CNpc_Sheep_Papa::ActionSequence(const _double & TimeDelta)
{
	Set_ActionState();

	if (STATE_READY == m_eActionState)
		return;

	switch (m_eActionState)
	{
	case CNpc_Sheep_Papa::STATE_SHEEPCARE:
		SheepCare();
		break;
	}
}

void CNpc_Sheep_Papa::SheepCare()
{
	if (false == m_bIsAction[STATE_SHEEPCARE])
	{
		m_bIsAction[STATE_SHEEPCARE] = true;
		m_pMesh->SetUp_AnimationSet(SHEEPCARE);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.2f))
	{
		m_bIsAction[STATE_SHEEPCARE] = false;
		m_eActionState = STATE_READY;
	}
}

HRESULT CNpc_Sheep_Papa::Add_Component(void * pArg)
{
	_matrix matWorld = *(_matrix*)pArg;

	// For.Com_Renderer
	if (FAILED(CNpc::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CNpc::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	//For.Com_Mesh
	if (FAILED(CNpc::Add_Component(SCENE_MESH_DYNAMIC,
		L"Component_Mesh_Npc_Sheep_Papa", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	tState.cName = "Npc_Sheep_Papa";
	tState.eCollider = COL_SPHERE;
	tState.eCollOpt = OPT_STATIC;
	tState.NxFlag = NX_SF_DISABLE_RAYCASTING;
	tState.vLocalPos = { 0.f, 1.f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
	tState.fRadius = 1.f;
	tState.iMaterial = 0;
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CNpc::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	NxMat34 pMat;
	pMat.isIdentity();

	for (int i = 0; i < 3; ++i)
	{
		NxVec3 vPos;
		memcpy(&vPos, matWorld.m[i], sizeof(_vec3));
		(NxVec3)m_pTransform->m_vInfo[i] = vPos;

		pMat.M.setColumn(i, vPos);
	}

	memcpy(&pMat.t, matWorld.m[3], sizeof(_vec3));

	m_pTransform->Get_Actor()->setGlobalPose(pMat);
	NxSphereShapeDesc TriggerDesc;
	TriggerDesc.setToDefault();

	TriggerDesc.radius = 1.5f;
	TriggerDesc.name = tState.cName;
	TriggerDesc.shapeFlags |= NX_TRIGGER_ENABLE | NX_SF_DISABLE_RAYCASTING;
	TriggerDesc.localPose.t = NxVec3(0.f, 1.f, 0.f);

	m_pTransform->Get_Actor()->createShape(TriggerDesc);

	return S_OK;
}

HRESULT CNpc_Sheep_Papa::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CNpc * CNpc_Sheep_Papa::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CNpc* pInstance = new CNpc_Sheep_Papa(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CNpc * CNpc_Sheep_Papa::Clone_GameObject(void * pArg)
{
	CNpc* pInstance = new CNpc_Sheep_Papa(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNpc_Sheep_Papa::Free()
{

	Safe_Release(m_pTransform);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CNpc::Free();
}

