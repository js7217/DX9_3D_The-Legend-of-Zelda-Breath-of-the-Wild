#include "stdafx.h"
#include "Enemy_Giant.h"

#include "NxPlayer.h"
#include "Enemy_Giant_Stone.h"
#include "HinoxHpBar.h"

CEnemy_Giant::CEnemy_Giant(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice), m_pManagement(GET_INSTANCE(CManagement)),
	m_pDebug(GET_INSTANCE(CDebug_Console)),
	m_pPhysx(GET_INSTANCE(CPhysXMgr))
{

	Safe_AddRef(m_pDebug);

}

CEnemy_Giant::CEnemy_Giant(const CEnemy_Giant & rhs)
	: CGameObject(rhs), m_pManagement(rhs.m_pManagement),
	m_pDebug(rhs.m_pDebug),
	m_pPhysx(rhs.m_pPhysx)
{

	Safe_AddRef(m_pDebug);

}

void CEnemy_Giant::Set_Target()
{
	if (nullptr == m_pPlayer)
		m_pPlayer = (CNxPlayer*)GET_INSTANCE(CObjectManager)->Get_GameObject(m_pManagement->Get_CurrentSceneNum(), L"Layer_Player", 0);
	else
	{
		m_vPlayerPos = m_pPlayer->Get_NxTransform()->Get_Actor()->getGlobalPosition();
		NxVec3 vPos = m_pTransform->Get_Actor()->getGlobalPosition();
		m_fDist = vPos.distance(m_vPlayerPos);
	}
}

HRESULT CEnemy_Giant::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CEnemy_Giant::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_eActionState = STATE_SLEEP_LOOP;
	m_eAnimationSeq = SEQ_START; 

	m_pMesh->SetUp_AnimationSet(SLEEP_LOOP);

	m_matWorld = *(_matrix*)pArg;

	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(m_matWorld.m);

	m_NxMatrix = pMat;
	m_pTransform->Get_Actor()->setGlobalPose(pMat);

//	m_bIsAction[STATE_DEAD_STAND] = FALSE;

	for (int i = 0; i < STATE_END; ++i)
		m_bIsAction[i] = false;

	return S_OK;
}

_int CEnemy_Giant::Update_GameObject(const _double & TimeDelta)
{
	m_fTime = (_float)TimeDelta;

	if (CGameObject::End_GameObject() && m_dwDisolveTime >= 1.f)
	{
		_matrix mat = *m_pTransform->Get_WorldMatrixPointer();

		mat._42 += 5.f;
		GET_INSTANCE(CEffectManager)->Get_EffectSet(L"ZeldaSmoke_Giant", &mat);
		mat._42 -= 4.f;
		GET_INSTANCE(CEffectManager)->Get_EffectSet(L"ZeldaSmoke2_Giant", &mat);
		GET_INSTANCE(CSoundMgr)->Play_Sound(L"Chemical_SmokeStart.wav", CSoundMgr::FIELD_OBJECT2);
		GET_INSTANCE(CManagement)->Set_DeadGiant();

		GET_INSTANCE(CSoundMgr)->Stop_SoundChannel(CSoundMgr::BGM);
		GET_INSTANCE(CSoundMgr)->Play_BGM(L"Hateno.mp3");

		return END_EVENT;
	}

	if (m_bIsAction[STATE_DEAD_STAND])
	{
		m_dwDisolveTime += TimeDelta * 0.25;
	}

	m_pTransform->Get_Actor()->setGlobalPose(m_NxMatrix);			// Transform 으로 관리할 NxMatrix으로 변환한 뒤 Update_GameObject를 탄다.
	Set_Target();
	CGameObject::Update_GameObject(TimeDelta);
	m_dwTimeDelta = TimeDelta;

	//1. 애니메이션 동작.
	ActionSequence(TimeDelta);
	//2. 각 파츠 콜라이더 위치 업데이트 하기.
	Update_Shapes();

	 m_pMesh->Play_Animation(m_dwTimeDelta);
	return NO_EVENT;
}

_int CEnemy_Giant::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	if (m_bCryRadial)
	{
		m_fCryRadial -= m_fTime * 4.f;

		m_pRenderer->Set_RadialStrength(m_fCryRadial);

		if (m_fCryRadial <= 0.f)
		{
			m_pRenderer->Set_Radial(FALSE);
			m_fCryRadial = 0;
			m_bCryRadial = FALSE;
		}
	}

	if (nullptr == m_pRenderer)
		return END_EVENT;

	//if (false == m_bDraw)
	//	return NO_EVENT;


	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CEnemy_Giant::Render_GameObject()
{
	if (nullptr == m_pShader || nullptr == m_pMesh)
		return E_FAIL;

	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();
//	m_pMesh->Play_Animation(m_dwTimeDelta);

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

			if (FAILED(m_pDisolve->SetUp_OnShader(pEffect, "g_DisolveTexture")))
				return E_FAIL;


			pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.f, 0.f, 0.f, 1.f));
			pEffect->SetVector("g_vecEmessiveColor", &_vec4(0.1f, 0.5f, 0.9f, 1.f));
			pEffect->SetBool("g_bShade", true);

			pEffect->CommitChanges();

			if (m_bIsAction[STATE_DEAD_STAND])
			{
				pEffect->SetFloat("g_fDisolveTime", (_float)m_dwDisolveTime);
				pEffect->BeginPass(10);
			}
			else
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

void CEnemy_Giant::Create_Shapes()
{
	//1. 몸통 : Center_1
	NxMaterialDesc materialDesc;
	materialDesc.setToDefault();
	materialDesc.restitution = 0.0;
	materialDesc.staticFriction = 1.0;
	materialDesc.dynamicFriction = 1.0;

	NxMaterial* pMaterial = GET_INSTANCE(CPhysXMgr)->Get_NxScene()->createMaterial(materialDesc);


	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.setToDefault();

	capsuleDesc.localPose.t = NxVec3(0.f, 0.f, 0.f);
	capsuleDesc.name = "Enemy_Giant";
	capsuleDesc.radius = NxReal(3.0);
	capsuleDesc.height = NxReal(2.0);
	capsuleDesc.shapeFlags |= NX_SF_DISABLE_RAYCASTING;

	capsuleDesc.materialIndex = pMaterial->getMaterialIndex();

	m_pTransform->Get_Actor()->createShape(capsuleDesc);

	//2. 오른 팔 : Weapon_R - 트리거 용도로 사용할 것.
	NxSphereShapeDesc TriggerDesc;
	TriggerDesc.setToDefault();

	TriggerDesc.localPose.t = NxVec3(0.f, 0.f, 0.f);
	TriggerDesc.name = "Enemy_Giant_RHand";
	TriggerDesc.radius = NxReal(1.5);
	TriggerDesc.shapeFlags |= NX_SF_DISABLE_RAYCASTING | NX_SF_DISABLE_COLLISION;
	TriggerDesc.skinWidth = 0.0;

	m_pTransform->Get_Actor()->createShape(TriggerDesc);

	//3. 눈알 : Eyeball
	NxSphereShapeDesc EyeDesc;
	EyeDesc.setToDefault();

	EyeDesc.localPose.t = NxVec3(0.f, 0.f, 0.f);
	EyeDesc.name = "Enemy_Giant_EyeBall";
	EyeDesc.radius = NxReal(1.0);
	EyeDesc.shapeFlags |= NX_SF_DISABLE_RAYCASTING;
	EyeDesc.skinWidth = NxReal(0.025);

	m_pTransform->Get_Actor()->createShape(EyeDesc);

	//// 2. 머리 : Head
	//NxSphereShapeDesc SphereDesc;
	//SphereDesc.setToDefault();

	//SphereDesc.localPose.t = NxVec3(0.f, 0.f, 0.f);
	//SphereDesc.name = "Enemy_Giant";
	//SphereDesc.radius = NxReal(2.5);

	//SphereDesc.materialIndex = pMaterial->getMaterialIndex();

	//m_pTransform->Get_Actor()->createShape(SphereDesc);
}

void CEnemy_Giant::Update_Shapes()
{
	m_matWorld = *m_pTransform->Get_WorldMatrixPointer();

	// 왼팔
	const D3DXFRAME_DERIVED* pLHand = m_pMesh->Get_FrameByName("Weapon_L");
	_matrix matWorld = pLHand->CombinedTransformMatrix;

	matWorld *= m_matWorld;

	NxMat34 nxmatWorld;
	nxmatWorld.setColumnMajor44(matWorld.m);

	nxmatWorld.t.y -= 0.5f;
	m_pTransform->Get_Actor()->getShapes()[0]->setGlobalPose(nxmatWorld);

	// 몸통
	const D3DXFRAME_DERIVED* pSpine = m_pMesh->Get_FrameByName("Center_1");
	matWorld = pSpine->CombinedTransformMatrix;

	matWorld *= m_matWorld;

	nxmatWorld.setColumnMajor44(matWorld.m);

	nxmatWorld.t.z += 0.5f;
	m_pTransform->Get_Actor()->getShapes()[1]->setGlobalPose(nxmatWorld);

	// 오른팔
	const D3DXFRAME_DERIVED* pRHand = m_pMesh->Get_FrameByName("Weapon_R");
	matWorld = pRHand->CombinedTransformMatrix;

	matWorld *= m_matWorld;

	nxmatWorld.setColumnMajor44(matWorld.m);
	m_pTransform->Get_Actor()->getShapes()[2]->setGlobalPose(nxmatWorld);

	// 눈알 Eyeball
	const D3DXFRAME_DERIVED* pEyeBall = m_pMesh->Get_FrameByName("Eyeball");
	matWorld = pEyeBall->CombinedTransformMatrix;

	matWorld *= m_matWorld;

	nxmatWorld.setColumnMajor44(matWorld.m);
	m_pTransform->Get_Actor()->getShapes()[3]->setGlobalPose(nxmatWorld);


	if(m_pTransform->Get_Actor()->isSleeping())
		m_pTransform->Get_Actor()->wakeUp(9999);
}

void CEnemy_Giant::Set_ActionState()
{
	if (STATE_READY == m_eActionState)
	{
		// 1. 일어나지 않았을때.
		if (!m_bIsWakeUp)
			m_eActionState = STATE_SLEEP_LOOP;
		// 2. 일어나고선 나무를 들지 않았을때.
		else if (!m_bIsCatchTree)
		{
			if (m_bSearchTree)
				m_eActionState = STATE_WALK;
			else
			{
				if (Check_Front_Player())
				{
					if (10.f >= m_fDist && 5.f <= m_fDist)
						m_eActionState = STATE_ATTACK_HANDCLAP;
					else if (20.f <= m_fDist && m_bIsThrowStone)
						m_eActionState = STATE_THROW_STONE_START;
					else
						m_eActionState = STATE_RUN;
				}
				else
				{
					if (3.5f >= m_fDist)
						m_eActionState = STATE_ATTACK_HIPDROP;
					else
						m_eActionState = STATE_RUN;
				}
			}
		}
		// 3.
		else if (m_bIsCatchTree)
		{
			// 일정거리가 안되면 플레이어에게 쫓아온다.
			if (10.f >= m_fDist && Check_Front_Player())
			{
				// 나무집었을때 공격하기.
				m_eActionState = STATE_ATTACK_TREE;

				//if (m_iThrowCnt > 0)
				//	m_eActionState = STATE_ATTACK_TREE;
			}
			else if (15.f <= m_fDist && Check_Front_Player())
			{
				if(m_iThrowCnt <= 0)
					m_eActionState = STATE_ATTACK_THROW;
				else
					m_eActionState = STATE_RUN;
			}
			else
			{
				m_eActionState = STATE_RUN;
			}
		}
		// 완전 아무것도 안할때.
		else
			m_eActionState = STATE_IDLE;
	}
}

void CEnemy_Giant::ActionSequence(const _double & TimeDelta)
{
	Set_ActionState();

	if (STATE_READY == m_eActionState)
		return;

	switch (m_eActionState)
	{
	case CEnemy_Giant::STATE_IDLE:
		Idle();
		break;
	case CEnemy_Giant::STATE_SLEEP_LOOP:
		Sleep();
		break;
	case CEnemy_Giant::STATE_SLEEP_END:
		Sleep_End();
		break;
	case CEnemy_Giant::STATE_SEATCH_TREE:
		Search_Battle();
		break;
	case CEnemy_Giant::STATE_WALK:
		Walk();
		break;
	case CEnemy_Giant::STATE_BREAK_TREE_START:
		Break_Tree_Start();
		break;
	case CEnemy_Giant::STATE_BREAK_TREE_END:
		Break_Tree_End();
		break;
	case CEnemy_Giant::STATE_RUN:
		Run();
		break;
	case CEnemy_Giant::STATE_ATTACK_TREE:
		Attack_Tree();
		break;
	case CEnemy_Giant::STATE_ATTACK_THROW:
		Attack_Throw();
		break;
	case CEnemy_Giant::STATE_CRY:
		Cry();
		break;
	case CEnemy_Giant::STATE_DAMAGE_EYE_START:
		Damage_Eye_Start();
		break;
	case CEnemy_Giant::STATE_DAMAGE_EYE_LOOP:
		Damage_Eye_Loop();
		break;
	case CEnemy_Giant::STATE_DAMAGE_EYE_END:
		Damage_Eye_End();
		break;
	case CEnemy_Giant::STATE_CATCH_TREE_START:
		Catch_Tree_Start();
		break;
	case CEnemy_Giant::STATE_CATCH_TREE_END:
		Catch_Tree_End();
		break;
	case CEnemy_Giant::STATE_ATTACK_HANDCLAP:
		Attack_HandClap();
		break;
	case CEnemy_Giant::STATE_ATTACK_HIPDROP:
		Attack_HipDrop();
		break;
	case CEnemy_Giant::STATE_THROW_STONE_START:
		Throw_Stone_Start();
		break;
	case CEnemy_Giant::STATE_THROW_STONE_END:
		Throw_Stone_End();
		break;
	case CEnemy_Giant::STATE_DEAD_STAND:
		Dead_Stand();
		break;
	}
}

void CEnemy_Giant::Sleep()
{
	if (false == m_bIsAction[STATE_SLEEP_LOOP])
	{
		m_bIsAction[STATE_SLEEP_LOOP] = true;
		m_pMesh->SetUp_AnimationSet(SLEEP_LOOP);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-23.f))				// 손의 콜라이더를 살짝 늘릴것.
	{
		//{ 4.f, 0.2f, 2.5f };COL_BOX
		NxBoxShape* pBoxShape = m_pTransform->Get_Actor()->getShapes()[0]->isBox();
		pBoxShape->setDimensions(NxVec3(8.f, 0.3f, 5.5f));

		m_pPlayer->Get_NxTransform()->Get_Actor()->wakeUp(999);
	}
	if (true == m_pMesh->IsAnimationSetEnd(-20.f))				// 손의 콜라이더를 되돌린다.
	{
		NxBoxShape* pBoxShape = m_pTransform->Get_Actor()->getShapes()[0]->isBox();
		pBoxShape->setDimensions(NxVec3(1.f, 0.2f, 1.f));
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.2f))
	{
		m_bIsAction[STATE_SLEEP_LOOP] = false;
		m_eActionState = STATE_READY;
		m_pMesh->SetUp_AnimationSet(SLEEP_LOOP2);


		//GiantVo_Sleep4.wav
		GET_INSTANCE(CSoundMgr)->Play_3D_Sound(L"GiantVo_Sleep4.wav", CSoundMgr::DGN_OBJECT1, m_pTransform->Get_Actor()->getGlobalPosition());
	}
}

void CEnemy_Giant::Sleep_End()
{
	if (false == m_bIsAction[STATE_SLEEP_END])
	{
		m_bIsAction[STATE_SLEEP_END] = true;
		m_pMesh->SetUp_AnimationSet(SLEEP_END);

		NxBoxShape* pBoxShape = m_pTransform->Get_Actor()->getShapes()[0]->isBox();
		pBoxShape->setDimensions(NxVec3(1.f, 0.2f, 1.f));

	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_SLEEP_END] = false;
		m_eActionState = STATE_CRY;
	}
}

void CEnemy_Giant::Search_Battle()
{
	if (false == m_bIsAction[STATE_SEATCH_TREE])
	{
		m_bIsAction[STATE_SEATCH_TREE] = true;
		m_pMesh->SetUp_AnimationSet(SEARCH_BALLTE);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_SEATCH_TREE] = false;
		m_eActionState = STATE_READY;
		//// 나무를 찾지않은 상태라면.
		//if (!m_bSearchTree)
		//{
		//	_vec3 vPos = *m_pTransform->Get_WorldMatrixPointer();
		//	_vec3 vTreePos = GET_INSTANCE(CObjectManager)->Find_Tree(m_pManagement->Get_CurrentSceneNum(), vPos);
	
		//	if (vTreePos != _vec3(0.f, 0.f, 0.f))
		//	{
		//		m_vTreePos = vTreePos;
		//		m_bSearchTree = true;
		//		m_eActionState = STATE_WALK;
		//	}
		//	else
		//		m_eActionState = STATE_READY;
		//}
	}
}

void CEnemy_Giant::Walk()
{
	if (false == m_bIsAction[STATE_WALK])
	{
		m_bIsAction[STATE_WALK] = true;
		m_pMesh->SetUp_AnimationSet(WALK);
	}

	//_bool bIsBreakStart = false;
	_bool bIsBreakStart = false;

	if (m_bLog)
	{
		m_vTreePos = m_vLogPos;
		if (Check_Front_Tree(m_vTreePos))
			bIsBreakStart = Walk_Distance();
	}
	else
	{
		if (Check_Front_Tree(m_vTreePos, 0.9962f))
			bIsBreakStart = Walk_Distance();
	}

	if (Decide_Rotation(m_vTreePos, 0.f))
		Actual_Rotation(m_fRotationAngle, 0.f);

	//if (!m_bIsCatchTree)
	//{
	//	if(Decide_Rotation(m_vTreePos))
	//		Actual_Rotation(m_fRotationAngle);

	//	bIsBreakStart = Walk_Distance();
	//}

	if (true == m_pMesh->IsAnimationSetEnd(-0.5f) && false == bIsBreakStart)
	{
		m_bIsAction[STATE_WALK] = false;
		m_eActionState = STATE_READY;
	}
}

void CEnemy_Giant::Wake_Up()
{
}

void CEnemy_Giant::Idle()
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

void CEnemy_Giant::Break_Tree_Start()
{
	if (false == m_bIsAction[STATE_BREAK_TREE_START])
	{
		m_bIsAction[STATE_BREAK_TREE_START] = true;
		m_pMesh->SetUp_AnimationSet(BREAK_TREE_START);
		m_pTransform->Get_Actor()->getShapes()[2]->setFlag(NX_SF_DISABLE_COLLISION, false);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.2f))
	{
		m_bIsAction[STATE_BREAK_TREE_START] = false;
		m_eActionState = STATE_BREAK_TREE_END;
	}
}

void CEnemy_Giant::Break_Tree_End()
{
	if (false == m_bIsAction[STATE_BREAK_TREE_END])
	{
		m_bIsAction[STATE_BREAK_TREE_END] = true;
		m_pMesh->SetUp_AnimationSet(BREAK_TREE_END);
		m_bIsCatchTree = true;		// 잡았다.

		GET_INSTANCE(CSoundMgr)->Play_3D_Sound(L"Tree_Fall_WithLeaf1.wav", CSoundMgr::FIELD_OBJECT1, m_pTransform->Get_GlobalPos());
		GET_INSTANCE(CSoundMgr)->Play_3D_Sound(L"Tree_Medium_FallDown_Default_WithLeaf1.wav", CSoundMgr::FIELD_OBJECT2, m_pTransform->Get_GlobalPos());
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_BREAK_TREE_END] = false;
		m_eActionState = STATE_READY;
		m_pTransform->Get_Actor()->getShapes()[2]->setFlag(NX_SF_DISABLE_COLLISION, true);
		m_bLog = true;
	}
}

void CEnemy_Giant::Run()
{
	if (false == m_bIsAction[STATE_RUN])
	{
		m_bIsAction[STATE_RUN] = true;
		m_pMesh->SetUp_AnimationSet(RUN);

		// Giant_Run_L00.wav
		GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"Giant_Run_L00.wav", CSoundMgr::DGN_OBJECT0);
	}

	_vec3 vPlayerPos = { m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z };

	if (Decide_Rotation(vPlayerPos))
		Actual_Rotation(m_fRotationAngle);

	NxVec3 vWalk = NxVec3(m_vTargetDir * 2.f * (_float)m_dwTimeDelta);
	m_NxMatrix.t += NxVec3(vWalk.x, 0.f, vWalk.z);


	if (!m_bSearchTree)
	{
		NxVec3 vMonsterPos = m_pTransform->Get_Actor()->getGlobalPosition();
		_vec3 vPos = _vec3(vMonsterPos.x, vMonsterPos.y, vMonsterPos.z);
		_bool bIsFind = false;
		_vec3 vTreePos = GET_INSTANCE(CObjectManager)->Find_Tree(m_pManagement->Get_CurrentSceneNum(), vPos, &bIsFind);

		if (bIsFind)
		{
			_vec3 vTreeDist = vPos - vTreePos;
			_float fTreeDist = D3DXVec3Length(&vTreeDist);

			if (fTreeDist <= 15.f && fTreeDist <= m_fDist && Check_Front_Tree(vTreePos))
			{
				m_vTreePos = vTreePos;
				m_bSearchTree = true;
				m_eActionState = STATE_READY;
			}
		}
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_RUN] = false;
		m_eActionState = STATE_READY;
//		GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"Giant_Run_R00.wav", CSoundMgr::DGN_OBJECT1);

	}
}

void CEnemy_Giant::Attack_Tree()
{
	if (false == m_bIsAction[STATE_ATTACK_TREE])
	{
		m_bIsAction[STATE_ATTACK_TREE] = true;
		m_pMesh->SetUp_AnimationSet(ATTACK_TREE);
		m_iThrowCnt -= 1;

		// GiantVo_AttackCatch.wav
		GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"GiantVo_AttackCatch.wav", CSoundMgr::DGN_DEFALULT);
	}

	_vec3 vPlayerPos = { m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z };
	Decide_Rotation(vPlayerPos);

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_ATTACK_TREE] = false;
		m_eActionState = STATE_READY;
	}
}

void CEnemy_Giant::Attack_Throw()
{
	if (false == m_bIsAction[STATE_ATTACK_THROW])
	{
		m_bIsAction[STATE_ATTACK_THROW] = true;
		m_pMesh->SetUp_AnimationSet(ATTACK_THROW);
	}

	_vec3 vPlayerPos = { m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z };
	Decide_Rotation(vPlayerPos);

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_ATTACK_THROW] = false;
		m_eActionState = STATE_READY;
		// 초기화.
		m_bSearchTree = false;
		m_bIsCatchTree = false;
		m_bLog = false;
		m_iThrowCnt = 2;
	}
}

void CEnemy_Giant::Cry()
{
	if (false == m_bIsAction[STATE_CRY])
	{
		m_bIsAction[STATE_CRY] = true;
		m_pMesh->SetUp_AnimationSet(CRY);

		GET_INSTANCE(CSoundMgr)->Stop_SoundChannel(CSoundMgr::BGM);
		GET_INSTANCE(CSoundMgr)->Play_BGM(L"Hinox Battle.mp3");

		CHinoxHpBar* pHinoxHpBar = (CHinoxHpBar*)GET_INSTANCE(CObjectManager)->Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_HinoxHpBar", 0);
		if (pHinoxHpBar != nullptr)
			pHinoxHpBar->Set_IsDraw(true, 0);
	}

	m_fCryRadial += m_fTime;

	m_pRenderer->Set_Radial(TRUE);
	m_pRenderer->Set_RadialStrength(m_fCryRadial);

	_vec3 vPlayerPos = { m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z };
	if (Decide_Rotation(vPlayerPos))
		Actual_Rotation(m_fRotationAngle);

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_CRY] = false;
		m_eActionState = STATE_READY;

		m_bCryRadial = TRUE;
	}
}

void CEnemy_Giant::Damage_Eye_Start()
{
	if (false == m_bIsAction[STATE_DAMAGE_EYE_START])
	{
		for (int i = 0; i < STATE_END; ++i)
			m_bIsAction[i] = false;

		m_bIsAction[STATE_DAMAGE_EYE_START] = true;
		m_pMesh->SetUp_AnimationSet(DAMAGE_EYE_START);
		m_bIsCatchTree = false;
		m_pTransform->Get_Actor()->getShapes()[2]->setFlag(NX_SF_DISABLE_COLLISION, true);

		m_pRenderer->Set_Radial(FALSE);
		m_pRenderer->Set_RadialStrength(m_fCryRadial);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_DAMAGE_EYE_START] = false;
		m_eActionState = STATE_DAMAGE_EYE_LOOP;
	}
}

void CEnemy_Giant::Damage_Eye_Loop()
{
	if (false == m_bIsAction[STATE_DAMAGE_EYE_LOOP])
	{
		m_bIsAction[STATE_DAMAGE_EYE_LOOP] = true;
		m_pMesh->SetUp_AnimationSet(DAMAGE_EYE_LOOP);
	}


	if (true == m_pMesh->IsAnimationSetEnd(-0.2f))
	{
		m_bIsAction[STATE_DAMAGE_EYE_LOOP] = false;
		m_eActionState = STATE_DAMAGE_EYE_END;
	}
}

void CEnemy_Giant::Damage_Eye_End()
{
	if (false == m_bIsAction[STATE_DAMAGE_EYE_END])
	{
		m_bIsAction[STATE_DAMAGE_EYE_END] = true;
		m_pMesh->SetUp_AnimationSet(DAMAGE_EYE_END);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_DAMAGE_EYE_END] = false;
		m_eActionState = STATE_READY;
		m_pTransform->Get_Actor()->getShapes()[2]->setFlag(NX_SF_DISABLE_COLLISION, false);
	}
}

void CEnemy_Giant::Catch_Tree_Start()
{
	if (false == m_bIsAction[STATE_CATCH_TREE_START])
	{
		m_bIsAction[STATE_CATCH_TREE_START] = true;
		m_pMesh->SetUp_AnimationSet(CATCH_TREE_START);
		NxSphereShape* pShpere = m_pTransform->Get_Actor()->getShapes()[2]->isSphere();
		pShpere->setRadius(NxReal(2.0));

		m_pTransform->Get_Actor()->getShapes()[2]->setFlag(NX_SF_DISABLE_COLLISION, false);
	}

	if (Decide_Rotation(m_vLogPos, 0.f))
		Actual_Rotation(m_fRotationAngle, 0.f);

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_CATCH_TREE_START] = false;
		m_eActionState = STATE_CATCH_TREE_END;
	}
}

void CEnemy_Giant::Catch_Tree_End()
{
	if (false == m_bIsAction[STATE_CATCH_TREE_END])
	{
		m_bIsAction[STATE_CATCH_TREE_END] = true;
		m_pMesh->SetUp_AnimationSet(CATCH_TREE_END);
		NxSphereShape* pShpere = m_pTransform->Get_Actor()->getShapes()[2]->isSphere();
		pShpere->setRadius(NxReal(5.0));
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_CATCH_TREE_END] = false;
		m_eActionState = STATE_READY;
		m_pTransform->Get_Actor()->getShapes()[2]->setFlag(NX_SF_DISABLE_COLLISION, true);


		NxSphereShape* pShpere = m_pTransform->Get_Actor()->getShapes()[2]->isSphere();
		pShpere->setRadius(NxReal(1.5));
	}
}

void CEnemy_Giant::Attack_HandClap()
{
	if (false == m_bIsAction[STATE_ATTACK_HANDCLAP])
	{
		m_bIsAction[STATE_ATTACK_HANDCLAP] = true;
		m_pMesh->SetUp_AnimationSet(ATTACK_HANDCLAP);
		m_bIsThrowStone = true;
		GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"Giant_AttackHandClap.wav", CSoundMgr::DGN_OBJECT0);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_ATTACK_HANDCLAP] = false;
		m_eActionState = STATE_READY;
	}
}

void CEnemy_Giant::Attack_HipDrop()
{
	if (false == m_bIsAction[STATE_ATTACK_HIPDROP])
	{
		m_bIsAction[STATE_ATTACK_HIPDROP] = true;
		m_pMesh->SetUp_AnimationSet(ATTACK_HIPDROP);
		m_bIsThrowStone = true;
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_ATTACK_HIPDROP] = false;
		m_eActionState = STATE_READY;
	}
}

void CEnemy_Giant::Throw_Stone_Start()
{
	if (false == m_bIsAction[STATE_THROW_STONE_START])
	{
		m_bIsAction[STATE_THROW_STONE_START] = true;
		m_pMesh->SetUp_AnimationSet(THROW_STONE_START);
	}

	if (!m_bIsCatchStone && m_pMesh->IsAnimationSetEnd(-1.2f))
	{
		m_pTransform->Get_Actor()->getShapes()[2]->setFlag(NX_SF_DISABLE_COLLISION, true);
		m_bIsCatchStone = true;
		const D3DXFRAME_DERIVED* pRHand = m_pMesh->Get_FrameByName("Weapon_R");
		_matrix matWorld = pRHand->CombinedTransformMatrix;

		matWorld *= m_matWorld;
		_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();

		CGameObject* pStone = GET_INSTANCE(CObjectManager)->Add_GameObject(iSceneNum, L"GameObject_Enemy_Giant_Stone", iSceneNum, &matWorld);

		dynamic_cast<CEnemy_Giant_Stone*>(pStone)->Set_GrabObject(this);
		dynamic_cast<CEnemy_Giant_Stone*>(pStone)->Set_Enemy_Giant_Mesh(m_pMesh);

		GET_INSTANCE(CObjectManager)->Add_GameObject(pStone, L"Layer_Stone", iSceneNum);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_THROW_STONE_START] = false;
		m_eActionState = STATE_THROW_STONE_END;
	}
}

void CEnemy_Giant::Throw_Stone_End()
{
	if (false == m_bIsAction[STATE_THROW_STONE_END])
	{
		m_bIsAction[STATE_THROW_STONE_END] = true;
		m_pMesh->SetUp_AnimationSet(THROW_STONE_END);
	}

	if (m_bIsCatchStone && m_pMesh->IsAnimationSetEnd(-1.5f))
		m_bIsCatchStone = false;

	_vec3 vPlayerPos = { m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z };
	if (Decide_Rotation(vPlayerPos, 0.5f))
		Actual_Rotation(m_fRotationAngle, 0.5f);

	if (true == m_pMesh->IsAnimationSetEnd(-0.4f))
	{
		m_bIsAction[STATE_THROW_STONE_END] = false;
		m_eActionState = STATE_READY;
		m_bIsThrowStone = FALSE;
	}
}

void CEnemy_Giant::Dead_Stand()
{
	if (false == m_bIsAction[STATE_DEAD_STAND])
	{
		for (int i = 0; i < STATE_END; ++i)
			m_bIsAction[i] = false;

		m_bIsAction[STATE_DEAD_STAND] = true;
		m_pMesh->SetUp_AnimationSet(DEAD_STAND);

		m_pTransform->Get_Actor()->getShapes()[2]->setFlag(NX_SF_DISABLE_COLLISION, true);
		m_bIsCatchTree = false;
		m_bSearchTree = false;

		// GiantVo_Dead0.wav
		GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"GiantVo_Dead0.wav", CSoundMgr::DGN_OBJECT2);
	}

	if (true == m_pMesh->IsAnimationSetEnd(-0.2f))
	{
		m_bIsAction[STATE_DEAD_STAND] = false;
		m_eActionState = STATE_READY;
		Set_GameObject(TRUE);
	}
}

void CEnemy_Giant::Actual_Rotation(_float fAngle, _float fRotAngle)
{
	if (fAngle > D3DXToRadian(fRotAngle))
	{
		m_pTransform->Rotation(CNxTransform::ROT_Y, -m_fRotSpeed);
	}
	else if (fAngle < D3DXToRadian(-fRotAngle))
	{
		m_pTransform->Rotation(CNxTransform::ROT_Y, m_fRotSpeed);
	}
	else
		return;
}

_bool CEnemy_Giant::Decide_Rotation(const _vec3& vTargetPos, _float fRotAngle)
{
	_vec3 vLook = *m_pTransform->Get_Info(CNxTransform::INFO_LOOK);
	_vec3 vRight = {};
	D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vLook);
	NxVec3 vNxPos = m_pTransform->Get_Actor()->getGlobalPosition();
	_vec3 vTarget = vTargetPos - _vec3(vNxPos.x, vNxPos.y, vNxPos.z);

	D3DXVec3Normalize(&vLook, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vTarget, &vTarget);
	m_vTargetDir = vTarget;


	_float fDotResult = D3DXVec3Dot(&vTarget, &vLook);
	m_fRotationAngle = acosf(fDotResult);
	_float fDotCheck = D3DXVec3Dot(&vRight, &vTarget);

	if ((fDotResult >= 0.707f && fDotResult <= 1.f) ||
		(fDotResult <= -0.707f && fDotResult >= -1.f))
	{
		m_fRotSpeed = 0.01f;
		m_bFront = true;
	}
	else
	{
		m_fRotSpeed = 0.1f;
		m_bFront = false;
	}

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

_bool CEnemy_Giant::Walk_Distance()
{
	NxVec3 vTreePos = NxVec3(m_vTreePos);
	NxVec3 vPos = m_pTransform->Get_Actor()->getGlobalPosition();
	_float fDist = 0.f;
	ACTIONSTATE eActionState;
	if (m_bLog)
	{
		fDist = 10.f;
		eActionState = STATE_CATCH_TREE_START;
	}
	else
	{
		fDist = 10.f;
		eActionState = STATE_BREAK_TREE_START;
	}

	if (fDist >= vTreePos.distance(vPos))
	{
		m_eActionState = eActionState;
		m_bIsAction[STATE_WALK] = false;

		return true;
	}
	else
	{
		NxVec3 vWalk = NxVec3(m_vTargetDir * 1.5f * (_float)m_dwTimeDelta);
		_float fY = m_NxMatrix.t.y;
		m_NxMatrix.t += NxVec3(vWalk.x, 0.f, vWalk.z);
		return false;
	}
}

_bool CEnemy_Giant::Check_Front_Player()
{
	_vec3 vLook = *m_pTransform->Get_Info(CNxTransform::INFO_LOOK);
	NxVec3 vNxPos = m_pTransform->Get_Actor()->getGlobalPosition();
	_vec3 vPlayerPos = _vec3(m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z);

	_vec3 vTarget = vPlayerPos - _vec3(vNxPos.x, vNxPos.y, vNxPos.z);

	D3DXVec3Normalize(&vLook, &vLook);
	D3DXVec3Normalize(&vTarget, &vTarget);


	_float fDotResult = D3DXVec3Dot(&vTarget, &vLook);
	m_fRotationAngle = acosf(fDotResult);

	if (fDotResult >= 0.96f && fDotResult <= 1.f)
		return true;
	else
		return false;
}

_bool CEnemy_Giant::Check_Front_Tree(const _vec3& vTreePos, _float fRadian)
{
	_vec3 vLook = *m_pTransform->Get_Info(CNxTransform::INFO_LOOK);
	NxVec3 vNxPos = m_pTransform->Get_Actor()->getGlobalPosition();

	_vec3 vTarget = vTreePos - _vec3(vNxPos.x, vNxPos.y, vNxPos.z);

	D3DXVec3Normalize(&vLook, &vLook);
	D3DXVec3Normalize(&vTarget, &vTarget);


	_float fDotResult = D3DXVec3Dot(&vTarget, &vLook);

	if ((fDotResult >= fRadian && fDotResult <= 1.f) ||
		(fDotResult <= -fRadian && fDotResult >= -1.f))
		return true;
	else
		return false;
}

HRESULT CEnemy_Giant::Add_Component()
{
	//For NxTransform

	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));
	tState.cName = "Enemy_Giant";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_KINEMATIC;
	tState.NxFlag |= NX_SF_DISABLE_RAYCASTING;
	tState.vDimensions = { 1.f, 0.2f, 1.f };
	tState.NxFlag = 0;
	tState.vLocalPos = { 0.f, -1.f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
	tState.pGameObject = this;


	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	m_pTransform->Get_Actor()->getShapes()[0]->setFlag(NX_SF_DISABLE_RAYCASTING, true);

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;
	
	//For.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_MESH_DYNAMIC,
		L"Component_Mesh_Enemy_Giant", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_Disolve", L"Com_Texture", (CComponent**)&m_pDisolve)))
		return E_FAIL;

	Create_Shapes();

	// Shapes
	// 0 : 왼손(올라타는 용도)
	// 1 : 몸통(캡슐)
	// 2 : 오른손(나무 잡는 용도)
	// 3 : Eyeball(눈알)
	return S_OK;
}

HRESULT CEnemy_Giant::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CGameObject * CEnemy_Giant::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CEnemy_Giant(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CEnemy_Giant::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CEnemy_Giant(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CEnemy_Giant::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDebug);
	Safe_Release(m_pMesh);
	Safe_Release(m_pDisolve);

	CGameObject::Free();
}
