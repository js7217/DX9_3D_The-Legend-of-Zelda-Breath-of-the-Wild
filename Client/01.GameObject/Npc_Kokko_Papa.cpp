#include "stdafx.h"
#include "Npc_Kokko_Papa.h"

void CNpc_Kokko_Papa::Set_AnimationState(ACTIONSTATE eActionState)
{
	for (int i = 0; i < STATE_END; ++i)
		m_bIsAction[i] = false;

	m_eActionState = eActionState;
}

void CNpc_Kokko_Papa::Set_Disable_Collision(const _bool & bIsColl)
{
	if (nullptr == m_pTransform)
		return;

	m_pTransform->Get_Actor()->getShapes()[1]->setFlag(NX_SF_DISABLE_COLLISION, bIsColl);
}

CNpc_Kokko_Papa::CNpc_Kokko_Papa(LPDIRECT3DDEVICE9 pDevice)
	: CNpc(pDevice)
{

}

CNpc_Kokko_Papa::CNpc_Kokko_Papa(const CNpc_Kokko_Papa & rhs)
	: CNpc(rhs)
{

}

HRESULT CNpc_Kokko_Papa::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CNpc_Kokko_Papa::Ready_GameObject(void * pArg)
{	
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_eActionState = STATE_READY;

	for (int i = 0; i < STATE_END; ++i)
		m_bIsAction[i] = false;

	m_pMesh->SetUp_AnimationSet(STATE_JOY_TALK);

	m_eNpcType = NPC_KOKO;
	CNpc::Ready_GameObject(pArg);

	// 만약 퀘스트가 완료된 상태라면 씬 전환에 상관없이 애초에 말을 못걸게 막을 것이다.
	if (TRUE == GET_INSTANCE(CManagement)->Get_bKokkoQuest())
		Set_Disable_Collision(TRUE);

	return S_OK;
}

_int CNpc_Kokko_Papa::Update_GameObject(const _double & TimeDelta)
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
	m_bDraw = m_pFrustumCom->WorldPt_InFrustum(&vPos, m_pTransform, 10.f);

	ActionSequence(m_dwTimeDelta);

	m_pMesh->Play_Animation(TimeDelta);

	return NO_EVENT;
}

_int CNpc_Kokko_Papa::LateUpdate_GameObject(const _double & TimeDelta)
{
	CNpc::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (false == m_bDraw)
		return NOERROR;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	//m_bIsStartTalk = false;
	//m_bTalkEnd = true;

	return NO_EVENT;
}

HRESULT CNpc_Kokko_Papa::Render_GameObject()
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

void CNpc_Kokko_Papa::Set_ActionState()
{
	if (STATE_READY == m_eActionState)
	{
		m_eActionState = STATE_SAD_TALK_WAIT;
	}
}

void CNpc_Kokko_Papa::ActionSequence(const _double & TimeDelta)
{
	Set_ActionState();

	if (STATE_READY == m_eActionState)
		return;

	switch (m_eActionState)
	{
	case CNpc_Kokko_Papa::STATE_NORMAL_TALK:
		Normal_Talk();
		break;
	case CNpc_Kokko_Papa::STATE_SAD_TALK:
		Sad_Talk();
		break;
	case CNpc_Kokko_Papa::STATE_SAD_TALK_WAIT:
		Sad_Talk_Wait();
		break;
	case CNpc_Kokko_Papa::STATE_SAD_TALK_L_ST:
		Sad_Talk_L_st();
		break;
	case CNpc_Kokko_Papa::STATE_SAD_TALK_L:
		Sad_Talk_L();
		break;
	case CNpc_Kokko_Papa::STATE_SURPRISE_TALK:
		Surprise_Talk();
		break;
	case CNpc_Kokko_Papa::STATE_SURPRISE_WAIT:
		Surprise_Wait();
		break;
	case CNpc_Kokko_Papa::STATE_JOY_TALK:
		Joy_Talk();
		break;
	case CNpc_Kokko_Papa::STATE_JOY_WAIT:
		Joy_Wait();
		break;
	}
}

void CNpc_Kokko_Papa::Normal_Talk()
{
	if (false == m_bIsAction[STATE_NORMAL_TALK])
	{
		m_bIsAction[STATE_NORMAL_TALK] = true;
		m_pMesh->SetUp_AnimationSet(NORMAL_TALK);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_NORMAL_TALK] = false;
		m_eActionState = STATE_SAD_TALK_WAIT;
	}
}

void CNpc_Kokko_Papa::Sad_Talk()
{
	if (false == m_bIsAction[STATE_SAD_TALK])
	{
		m_bIsAction[STATE_SAD_TALK] = true;
		m_pMesh->SetUp_AnimationSet(SAD_TALK);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_SAD_TALK] = false;
		m_eActionState = STATE_SAD_TALK_WAIT;
	}
}

void CNpc_Kokko_Papa::Sad_Talk_Wait()
{
	if (false == m_bIsAction[STATE_SAD_TALK_WAIT])
	{
		m_bIsAction[STATE_SAD_TALK_WAIT] = true;
		m_pMesh->SetUp_AnimationSet(SAD_TALK_WAIT);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_SAD_TALK_WAIT] = false;
		m_eActionState = STATE_READY;
	}
}

void CNpc_Kokko_Papa::Sad_Talk_L_st()
{
	if (false == m_bIsAction[STATE_SAD_TALK_L_ST])
	{
		m_bIsAction[STATE_SAD_TALK_L_ST] = true;
		m_pMesh->SetUp_AnimationSet(SAD_TALK_L_ST);

		// Link act surprised while Kokko is not returning phase
		CGameObject* pObject = GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0);
		dynamic_cast<CNxPlayer*>(pObject)->Set_SurpriseAction();
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_SAD_TALK_L_ST] = false;
		m_eActionState = STATE_SAD_TALK_L;
	}
}

void CNpc_Kokko_Papa::Sad_Talk_L()
{
	if (false == m_bIsAction[STATE_SAD_TALK_L])
	{
		m_bIsAction[STATE_SAD_TALK_L] = true;
		m_pMesh->SetUp_AnimationSet(SAD_TALK_L);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_SAD_TALK_L] = false;
		m_eActionState = STATE_SAD_TALK_L;
	}
}

void CNpc_Kokko_Papa::Surprise_Talk()
{
	if (false == m_bIsAction[STATE_SURPRISE_TALK])
	{
		m_bIsAction[STATE_SURPRISE_TALK] = true;
		m_pMesh->SetUp_AnimationSet(SURPRISE_TALK);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_SURPRISE_TALK] = false;
		m_eActionState = STATE_SURPRISE_WAIT;
	}
}

void CNpc_Kokko_Papa::Surprise_Wait()
{
	if (false == m_bIsAction[STATE_SURPRISE_WAIT])
	{
		m_bIsAction[STATE_SURPRISE_WAIT] = true;
		m_pMesh->SetUp_AnimationSet(SURPRISE_WAIT);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_SURPRISE_WAIT] = false;
		m_eActionState = STATE_SURPRISE_WAIT;
	}
}

void CNpc_Kokko_Papa::Joy_Talk()
{
	if (false == m_bIsAction[STATE_JOY_TALK])
	{
		m_bIsAction[STATE_JOY_TALK] = true;
		m_pMesh->SetUp_AnimationSet(JOY_TALK);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_JOY_TALK] = false;
		m_eActionState = STATE_JOY_WAIT;
	}
}

void CNpc_Kokko_Papa::Joy_Wait()
{
	if (false == m_bIsAction[STATE_JOY_WAIT])
	{
		m_bIsAction[STATE_JOY_WAIT] = true;
		m_pMesh->SetUp_AnimationSet(JOY_WAIT);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_JOY_WAIT] = false;
		m_eActionState = STATE_JOY_WAIT;
	}
}

void CNpc_Kokko_Papa::Actual_Rotation(_float fAngle, _float fRotAngle)
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

_bool CNpc_Kokko_Papa::Decide_Rotation(const _vec3 & vTargetPos, _float fRotAngle)
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

HRESULT CNpc_Kokko_Papa::Add_Component(void * pArg)
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
		L"Component_Mesh_Npc_Kokko_Papa", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	tState.cName = "Npc";
	tState.eCollider = COL_SPHERE;
	tState.eCollOpt = OPT_STATIC;
	tState.NxFlag = NX_SF_DISABLE_RAYCASTING;
	tState.vLocalPos = { 0.f, 0.5f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
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
	NxSphereShapeDesc TriggerDesc;
	TriggerDesc.setToDefault();

	TriggerDesc.radius = 1.f;
	TriggerDesc.name = tState.cName;
	TriggerDesc.shapeFlags |= NX_TRIGGER_ENABLE | NX_SF_DISABLE_RAYCASTING;
	TriggerDesc.localPose.t = NxVec3(0.f, 1.f, 0.f);

	m_pTransform->Get_Actor()->createShape(TriggerDesc);

	return S_OK;
}

HRESULT CNpc_Kokko_Papa::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CNpc * CNpc_Kokko_Papa::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CNpc* pInstance = new CNpc_Kokko_Papa(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CNpc * CNpc_Kokko_Papa::Clone_GameObject(void * pArg)
{
	CNpc* pInstance = new CNpc_Kokko_Papa(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNpc_Kokko_Papa::Free()
{

	Safe_Release(m_pTransform);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CNpc::Free();
}

