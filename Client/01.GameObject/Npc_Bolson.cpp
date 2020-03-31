#include "stdafx.h"
#include "Npc_Bolson.h"

#include "KeyMgr.h"
#include "NxPlayer.h"

CNpc_Bolson::CNpc_Bolson(LPDIRECT3DDEVICE9 pDevice)
	: CNpc(pDevice)
{

}

CNpc_Bolson::CNpc_Bolson(const CNpc_Bolson & rhs)
	: CNpc(rhs)
{

}

HRESULT CNpc_Bolson::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CNpc_Bolson::Ready_GameObject(void * pArg)
{	
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_eActionState = STATE_IDLE;
	m_eAnimationSeq = SEQ_START;

	for (int i = 0; i < STATE_END; ++i)
		m_bIsAction[i] = false;

	m_pMesh->SetUp_AnimationSet(STAND_ST);
	m_eNpcType = NPC_BOLSON;
	CNpc::Ready_GameObject(pArg);

	return S_OK;
}

_int CNpc_Bolson::Update_GameObject(const _double & TimeDelta)
{
	if (m_bIsLook)
	{		
		NxVec3 vPlayerPos = m_pPlayer->Get_NxTransform()->Get_GlobalPos();
		
		if (Decide_Rotation(_vec3(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z)))
			Actual_Rotation(m_fRotationAngle);
		else
			m_bIsLook = FALSE;
	}

	CNpc::Update_GameObject(TimeDelta);

	m_dwTimeDelta = TimeDelta;

	_vec3 vPos;
	m_pTransform->Get_Info(CNxTransform::INFO_POS, &vPos);
	m_bDraw = m_pFrustumCom->WorldPt_InFrustum(&vPos, m_pTransform, 5.f);

	ActionSequence(m_dwTimeDelta);

	if(m_bIsAnimation)
		m_pMesh->Play_Animation(TimeDelta);

	return NO_EVENT;
}

_int CNpc_Bolson::LateUpdate_GameObject(const _double & TimeDelta)
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

HRESULT CNpc_Bolson::Render_GameObject()
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

void CNpc_Bolson::Set_ActionState()
{
	if (STATE_READY == m_eActionState)
	{
		if (m_bIsStartTalk)
			m_eActionState = STATE_STAND_END;
		else
			m_eActionState = STATE_IDLE;
	}
}

void CNpc_Bolson::ActionSequence(const _double & TimeDelta)
{
	Set_ActionState();

	if (STATE_READY == m_eActionState)
		return;

	switch (m_eActionState)
	{

	case CNpc_Bolson::STATE_IDLE:
		Idle();
		break;
	case CNpc_Bolson::STATE_STAND_END:
		Stand_End();
		break;
	case CNpc_Bolson::STATE_STAND_ST:
		Stand_Start();
		break;
	case CNpc_Bolson::STATE_DANCE:
		Dance();
		break;
	}
}

void CNpc_Bolson::Idle()
{
	if (false == m_bIsAction[STATE_IDLE])
	{
		m_bIsAction[STATE_IDLE] = true;
		m_pMesh->SetUp_AnimationSet(IDLE);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.2f))
	{
		m_bIsAction[STATE_IDLE] = false;
		m_eActionState = STATE_READY;
	}
}

void CNpc_Bolson::Stand_End()
{
	if (false == m_bIsAction[STATE_STAND_END])
	{
		m_bIsAction[STATE_STAND_END] = true;
		m_pMesh->SetUp_AnimationSet(STAND_END);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_STAND_END] = false;
		m_eActionState = STATE_DANCE;
	}
}

void CNpc_Bolson::Stand_Start()
{
	if (false == m_bIsAction[STATE_STAND_ST])
	{
		m_bIsAction[STATE_STAND_ST] = true;
		m_pMesh->SetUp_AnimationSet(STAND_ST);
	}

	if (!m_bIsAnimation)
	{
		if (m_dwTimer >= 3.0)
		{
			m_bIsAnimation = true;
			m_dwTimer = 0.0;
		}
		else
			m_dwTimer += m_dwTimeDelta;
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_STAND_ST] = false;
		m_eActionState = STATE_READY;
		m_bIsStartTalk = false;
		m_bTalkEnd = true;
	}
}

void CNpc_Bolson::Dance()
{
	if (false == m_bIsAction[STATE_DANCE])
	{
		m_bIsAction[STATE_DANCE] = true;
		m_pMesh->SetUp_AnimationSet(DANCE);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.2f))
	{
		m_bIsAction[STATE_DANCE] = false;
		m_eActionState = STATE_STAND_ST;
		m_bIsAnimation = false;

		// Link act surprised while Kokko is not returning phase
		CGameObject* pObject = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
		dynamic_cast<CNxPlayer*>(pObject)->Set_SurpriseAction();
	}
}

void CNpc_Bolson::Actual_Rotation(_float fAngle, _float fRotAngle)
{
	if (fAngle > D3DXToRadian(fRotAngle))
	{
		m_pTransform->Rotation(CNxTransform::ROT_Y, -0.1f);
	}
	else if (fAngle < D3DXToRadian(-fRotAngle))
	{
		m_pTransform->Rotation(CNxTransform::ROT_Y, 0.1f);
	}
	else
		return;
}

_bool CNpc_Bolson::Decide_Rotation(const _vec3 & vTargetPos, _float fRotAngle)
{
	_vec3 vLook = *m_pTransform->Get_Info(CNxTransform::INFO_LOOK) * -1.f;
	_vec3 vRight = {};
	D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vLook);
	NxVec3 vNxPos = m_pTransform->Get_Actor()->getGlobalPosition();
	_vec3 vTarget = vTargetPos - _vec3(vNxPos.x, vNxPos.y, vNxPos.z);

	D3DXVec3Normalize(&vLook, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vTarget, &vTarget);


	_float fDotResult = D3DXVec3Dot(&vTarget, &vLook);
	m_fRotationAngle = acosf(fDotResult);
	_float fDotCheck = D3DXVec3Dot(&vRight, &vTarget);

	if (fDotCheck > 0.f)
		m_fRotationAngle *= -1.f;

	if (m_fRotationAngle > D3DXToRadian(fRotAngle))
	{
		return true;
	}
	else if (m_fRotationAngle < D3DXToRadian(-fRotAngle))
	{
		return true;
	}
	else
	{
		return false;
	}
}

HRESULT CNpc_Bolson::Add_Component(void * pArg)
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
		L"Component_Mesh_Npc_Hylia_Carpenter_000", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	tState.cName = "Npc";
	tState.eCollider = COL_CAPSULE;
	tState.eCollOpt = OPT_STATIC;
	tState.NxFlag = NX_SF_DISABLE_RAYCASTING;
	tState.vLocalPos = { 0.f, 1.f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
	tState.fHeight = 1.f;
	tState.fRadius = 0.5f;
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

	m_pTransform->Get_Actor()->setGlobalPosition(NxVec3(12.f, 11.f, 93.f));

	NxSphereShapeDesc TriggerDesc;
	TriggerDesc.setToDefault();

	TriggerDesc.radius = 1.5f;
	TriggerDesc.name = tState.cName;
	TriggerDesc.shapeFlags |= NX_TRIGGER_ENABLE | NX_SF_DISABLE_RAYCASTING;
	TriggerDesc.localPose.t = NxVec3(0.f, 1.5f, 0.f);

	m_pTransform->Get_Actor()->createShape(TriggerDesc);

	return S_OK;
}

HRESULT CNpc_Bolson::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CNpc * CNpc_Bolson::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CNpc* pInstance = new CNpc_Bolson(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CNpc * CNpc_Bolson::Clone_GameObject(void * pArg)
{
	CNpc* pInstance = new CNpc_Bolson(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNpc_Bolson::Free()
{

	Safe_Release(m_pTransform);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CNpc::Free();
}

