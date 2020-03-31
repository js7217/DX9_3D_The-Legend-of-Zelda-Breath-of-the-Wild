#include "stdafx.h"
#include "Obj_Log_TreeApple.h"

#include "Enemy_Giant.h"

CObj_Log_TreeApple::CObj_Log_TreeApple(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{

}

CObj_Log_TreeApple::CObj_Log_TreeApple(const CObj_Log_TreeApple & rhs)
	: CGameObject(rhs)
{

}

void CObj_Log_TreeApple::Set_GrabObject(CGameObject * pObj)
{
	if (nullptr != pObj)
	{
		m_pGrabObject = pObj;
		m_pGrabMesh = (CMesh_Dynamic*)m_pGrabObject->Get_Component(L"Com_Mesh");
		m_pTransform->Get_Actor()->getShapes()[0]->setFlag(NX_SF_DISABLE_COLLISION, true);

		m_pGiant = m_pGrabObject;
		m_pTransform->Get_Actor()->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);

		m_bCheckHit = TRUE;
	}
}

HRESULT CObj_Log_TreeApple::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CObj_Log_TreeApple::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	D3DXMatrixRotationX(&m_matRotX_M90, D3DXToRadian(-60.f));
	D3DXMatrixTranslation(&m_matTrans, 0.f, -1.f, 1.5f);

	return S_OK;
}

_int CObj_Log_TreeApple::Update_GameObject(const _double & TimeDelta)
{
	//if (m_bIsCheckDead && Check_Dead())
	//	return END_EVENT;
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

	if (nullptr != m_pGrabObject)
	{
		Move_Target();

		// 만약 나무를 잡고있는 상태가 아니라면
		// 나무에게 자유를 주고 m_pGrabObject를 nullptr로 초기화 시켜버린다.
		if (!dynamic_cast<CEnemy_Giant*>(m_pGrabObject)->Get_CatchTree())
		{
			m_pTransform->Get_Actor()->getShapes()[0]->setFlag(NX_SF_DISABLE_COLLISION, false);
			m_pTransform->Get_Actor()->clearBodyFlag(NX_BF_DISABLE_GRAVITY);
			m_bCheckHit = FALSE;

			if (!dynamic_cast<CEnemy_Giant*>(m_pGrabObject)->Get_bLog())
			{
				D3DXVec3Normalize(&m_vRHandLook, &m_vRHandLook);
				m_vRHandLook.y = 0.f;
				m_pTransform->Get_Actor()->setLinearVelocity(NxVec3(m_vRHandLook * -100.f));
				m_bIsCheckDead = true;
			}
			else
				m_pTransform->Get_Actor()->setLinearVelocity(NxVec3(m_vRHandLook));

			m_pTransform->Get_Actor()->setAngularVelocity(NxVec3(0.f, 0.f, 0.f));

			m_pGrabObject = nullptr;
		}
		else
			m_pTransform->Get_Actor()->getShapes()[0]->setFlag(NX_SF_DISABLE_COLLISION, true);
	}
	else
	{
		if (nullptr != m_pGiant && dynamic_cast<CEnemy_Giant*>(m_pGiant)->Get_Dead())
		{
			m_pGiant = nullptr;
			m_pGrabMesh = nullptr;
		}

		CGameObject::Update_GameObject(TimeDelta);

		if (m_bTimeStop && m_TimeStop == 0)
		{
			//m_bTimeStop = TRUE;
			m_NxVelocity = NxVec3(0.f, 0.f, 0.f);
			m_pTransform->Get_Actor()->setLinearVelocity(m_NxVelocity);
			m_pTransform->Get_Actor()->raiseBodyFlag(NX_BF_FROZEN);
			m_pTransform->Get_Actor()->putToSleep();

			GET_INSTANCE(CEffectManager)->Get_EffectSet(L"TimeStop", m_pTransform->Get_WorldMatrixPointer(), 0, CEffect::GLOW);
		}

		if (m_bTimeStop && m_TimeStop > 10.0)
		{
			m_TimeStop = 0;
			m_bTimeStop = FALSE;
			m_NxVelocity *= 1.f;
			m_pTransform->Get_Actor()->setLinearVelocity(m_NxVelocity);
			m_pTransform->Get_Actor()->clearBodyFlag(NX_BF_FROZEN);
			GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"Bitalock_End_07.wav", CSoundMgr::DGN_OBJECT1);

			GET_INSTANCE(CEffectManager)->Get_EffectSet(L"TimeStop", m_pTransform->Get_WorldMatrixPointer(), 0, CEffect::GLOW);
			m_fBlinkTime = 0.f;
		}

		if (m_bTimeStop)
		{
			m_TimeStop += TimeDelta;
			TimeStop_Sound(m_fBlinkTime);
		}

		if (m_TimeStop > 0 && m_TimeStop < 9.0)
		{
			_matrix pMat;
			D3DXMatrixIdentity(&pMat);
			memcpy(pMat.m[3], m_pTransform->Get_WorldMatrixPointer()->m[3], sizeof(_vec3));
			GET_INSTANCE(CEffectManager)->Get_EffectSet(L"TimeStop2", &pMat, 0, CEffect::BLUR);
		}

		if (m_pTransform->Get_Actor()->isSleeping())
			m_pTransform->Get_Actor()->wakeUp(999);
	}

	if (nullptr != m_pGiant)
	{
		NxVec3 vPos = m_pTransform->Get_Actor()->getGlobalPosition();
		dynamic_cast<CEnemy_Giant*>(m_pGiant)->Set_LogPos(_vec3(vPos.x, vPos.y, vPos.z));
	}

	return NO_EVENT;
}

_int CObj_Log_TreeApple::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CObj_Log_TreeApple::Render_GameObject()
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

		pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.f, 0.f, 0.f, 1.f));
		pEffect->SetVector("g_vecEmessiveColor", &_vec4(1.f, 0.623f, 0.251f, 1.f));
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

void CObj_Log_TreeApple::Move_Target()
{
	if (nullptr == m_pGrabMesh)
		return;
	_matrix matLog = m_matRotX_M90 * m_matTrans;

	const D3DXFRAME_DERIVED* pRHand = m_pGrabMesh->Get_FrameByName("Weapon_R");
	_matrix matWorld = pRHand->CombinedTransformMatrix;

	_matrix matParent = dynamic_cast<CEnemy_Giant*>(m_pGrabObject)->Get_WorldMat();

	matLog = matLog * matWorld * matParent;

	NxMat34 nxMatWorld;
	nxMatWorld.setColumnMajor44(matLog.m);
//	nxMatWorld.setRowMajor44(matLog.m);

	m_pTransform->Get_Actor()->setGlobalPose(nxMatWorld);
	m_pTransform->m_matWorld = matLog;

	memcpy(&m_vRHandLook, &matLog.m[2], sizeof(_vec3));
}

_bool CObj_Log_TreeApple::Check_Dead()
{
	if (nullptr == m_pTransform)
		return false;
	_int iCnt = GET_INSTANCE(CPhysXMgr)->Get_NxScene()->getNbActors();

	NxBoxShape* pBoxShape = m_pTransform->Get_Shape()->isBox();
	if (nullptr == pBoxShape)
		return false;

	NxBox Box;
	pBoxShape->getWorldOBB(Box);

	for (_int i = 0; i < iCnt; ++i)
	{
		NxActor* pActor = GET_INSTANCE(CPhysXMgr)->Get_NxScene()->getActors()[i];

		const char* pName = pActor->getName();
		const char* pShapeName = pActor->getShapes()[0]->getName();

		if (nullptr != pShapeName && strcmp(pShapeName, "Terrain") == 0)
		{
			_bool bIsCheck = GET_INSTANCE(CPhysXMgr)->Get_NxScene()->getActors()[i]->getShapes()[0]->checkOverlapOBB(Box);
			if (bIsCheck)
				return true;
		}
		else
			continue;
	}

	return false;
}

void CObj_Log_TreeApple::TimeStop_Sound(_double CurTime)
{
	if (CurTime >= 1)
	{
		GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"BitaLock_Timer03.wav", CSoundMgr::DGN_OBJECT1);
	}
}

HRESULT CObj_Log_TreeApple::Create_NxTransform(const _matrix matWorld)
{
	BYTE* pVertices;
	_vec3 vMin, vMax;
	m_pMesh->Get_Mesh()->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&pVertices);
	D3DXComputeBoundingBox((D3DXVECTOR3*)pVertices, m_pMesh->Get_Mesh()->GetNumVertices(),
		m_pMesh->Get_Mesh()->GetNumBytesPerVertex(), &vMin, &vMax);
	m_pMesh->Get_Mesh()->UnlockVertexBuffer();

	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	tState.cName = "Obj_Log_TreeApple";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_DYNAMIC;
	tState.NxFlag |= NX_SF_DISABLE_RAYCASTING;
	tState.vLocalPos = { 0.f, 3.5f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
	tState.vDimensions = { (vMax.x - vMin.x) * 0.3f, (vMax.y - vMin.y) * 0.5f, (vMax.z - vMin.z) * 0.3f };
	tState.fDensity = 10.f;
	tState.iMaterial = 0;
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	if (FAILED(GET_INSTANCE(CPhysXMgr)->Set_ContactActor("Obj_Log_TreeApple", "Bokoblin")))
		return E_FAIL;

	if (FAILED(GET_INSTANCE(CPhysXMgr)->Set_ContactActor("Obj_Log_TreeApple", "WizzRobe")))
		return E_FAIL;

	if (FAILED(GET_INSTANCE(CPhysXMgr)->Set_ContactActor("Obj_Log_TreeApple", "Enemy_Giant")))
		return E_FAIL;

	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(matWorld.m);

	m_pTransform->Get_Actor()->setGlobalPose(pMat);
	m_pTransform->Get_Actor()->getShapes()[0]->setName("TimeStop");


	NxMaterialDesc materialdesc;
	materialdesc.setToDefault();

	materialdesc.restitution = 0.1f;
	materialdesc.staticFriction = 100.f;
	materialdesc.dynamicFriction = 100.f;

	NxMaterial* pMaterial = GET_INSTANCE(CPhysXMgr)->Get_NxScene()->createMaterial(materialdesc);

	m_pTransform->Get_Actor()->getShapes()[0]->setMaterial(pMaterial->getMaterialIndex());

	// Trigger 만들기.
	NxCapsuleShapeDesc TriggerDesc;
	TriggerDesc.setToDefault();

	TriggerDesc.height = 3.5f;
	TriggerDesc.radius = 0.5f;
	TriggerDesc.localPose.t = NxVec3(0.f, 3.5f, 0.f);
	TriggerDesc.shapeFlags |= NX_TRIGGER_ENABLE | NX_SF_DISABLE_RAYCASTING;
	TriggerDesc.name = tState.cName;

	m_pShape = m_pTransform->Get_Actor()->createShape(TriggerDesc);
	return S_OK;
}

HRESULT CObj_Log_TreeApple::Add_Component(void * pArg)
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	//For.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_MESH_STATIC,
		L"Component_Mesh_Obj_Log_TreeApple", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CObj_Log_TreeApple::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CGameObject * CObj_Log_TreeApple::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CObj_Log_TreeApple(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CObj_Log_TreeApple::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CObj_Log_TreeApple(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CObj_Log_TreeApple::Free()
{
	m_pGiant = nullptr;
	m_pGrabObject = nullptr;
	m_pGrabMesh = nullptr;

	Safe_Release(m_pTransform);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CGameObject::Free();
}

