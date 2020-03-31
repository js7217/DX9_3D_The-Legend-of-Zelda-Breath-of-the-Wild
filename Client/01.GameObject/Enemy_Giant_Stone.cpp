#include "stdafx.h"
#include "Enemy_Giant_Stone.h"
#include "KeyMgr.h"

#include "Enemy_Giant.h"
CEnemy_Giant_Stone::CEnemy_Giant_Stone(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{

}

CEnemy_Giant_Stone::CEnemy_Giant_Stone(const CEnemy_Giant_Stone & rhs)
	: CGameObject(rhs)
{

}

HRESULT CEnemy_Giant_Stone::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CEnemy_Giant_Stone::Ready_GameObject(void * pArg)
{
	_matrix matWorld = *(_matrix*)pArg;
	
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(matWorld.m);
//	pMat.t.x -= 0.2f;

	m_pTransform->Get_Actor()->setGlobalPose(pMat);

//	m_pTransform->Get_Actor()->getShapes()[0]->setFlag(NX_SF_DISABLE_RAYCASTING, TRUE);
//	m_pTransform->Set_LimitVelocity(13.f);
	m_pTransform->Set_LimitVelocity(100.f);

	return S_OK;
}

_int CEnemy_Giant_Stone::Update_GameObject(const _double & TimeDelta)
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

		if (!dynamic_cast<CEnemy_Giant*>(m_pGrabObject)->Get_CatchStone())
		{
			m_bIsCheckDead = true;

			NxVec3 vPlayerPos = dynamic_cast<CEnemy_Giant*>(m_pGrabObject)->Get_PlayerPos();
			NxVec3 nxhand = m_vRHandPos;
			m_vTargetPos = vPlayerPos - nxhand;
			m_fDist = m_vTargetPos.magnitude();

			m_vTargetPos.y = 0.f;
			m_vTargetPos.normalize();

			m_pTransform->Get_Actor()->clearBodyFlag(NX_BF_KINEMATIC);
			m_pTransform->Get_Actor()->wakeUp();
			m_pTransform->Get_Actor()->setLinearVelocity(m_vTargetPos * m_fDist * 10.f);
			m_pGrabObject = nullptr;
		}
		else if (0 >= dynamic_cast<CEnemy_Giant*>(m_pGrabObject)->Get_Hp())
			m_pGrabObject = nullptr;

	}
	else
	{
	//	m_pTransform->Get_Actor()->addForce(m_vTargetPos * m_fDist * 100.f);

		CGameObject::Update_GameObject(TimeDelta);

		if (m_bTimeStop && m_TimeStop == 0)
		{
			//m_bTimeStop = TRUE;
			m_NxVelocity = NxVec3(0.f, 0.f, 0.f);
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
			GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"Bitalock_End_07.wav", CSoundMgr::DGN_OBJECT1);

			GET_INSTANCE(CEffectManager)->Get_EffectSet(L"TimeStop", m_pTransform->Get_WorldMatrixPointer(), 0, CEffect::GLOW);
			m_fBlinkTime = 0.f;
		}

		if (m_TimeStop > 0 && m_TimeStop < 9.0)
		{
			_matrix pMat;
			D3DXMatrixIdentity(&pMat);
			memcpy(pMat.m[3], m_pTransform->Get_WorldMatrixPointer()->m[3], sizeof(_vec3));
			GET_INSTANCE(CEffectManager)->Get_EffectSet(L"TimeStop2", &pMat, 0, CEffect::BLUR);
		}

		if (m_bTimeStop)
		{
			m_TimeStop += TimeDelta;
			TimeStop_Sound(m_fBlinkTime);
		}
	}

	if (m_pTransform->Get_Actor()->isSleeping())
		m_pTransform->Get_Actor()->wakeUp(100);

	return NO_EVENT;
}

_int CEnemy_Giant_Stone::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (m_pTransform->Get_Actor()->isSleeping())
		m_pTransform->Get_Actor()->wakeUp(10);

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CEnemy_Giant_Stone::Render_GameObject()
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

		pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.2f, 0.7f, 1.0f, 1.f));
		pEffect->SetBool("g_bShade", TRUE);

		pEffect->CommitChanges();

		if (m_bTimeStop)
			pEffect->BeginPass(12);
		else
			pEffect->BeginPass(4);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

void CEnemy_Giant_Stone::TimeStop_Sound(_double CurTime)
{
	if (CurTime >= 1)
	{
		GET_INSTANCE(CSoundMgr)->Play_Instance_Sound(L"BitaLock_Timer03.wav", CSoundMgr::DGN_OBJECT1);
	}
}

void CEnemy_Giant_Stone::Move_Target()
{
	const D3DXFRAME_DERIVED* pRHand = m_pEnmey_Giant_Mesh->Get_FrameByName("Weapon_R");
	_matrix matWorld = pRHand->CombinedTransformMatrix;
	_matrix matParent = dynamic_cast<CEnemy_Giant*>(m_pGrabObject)->Get_WorldMat();

	matWorld *= matParent;

	NxMat34 nxMatWorld;
	nxMatWorld.setColumnMajor44(matWorld.m);

	m_pTransform->Get_Actor()->setGlobalPose(nxMatWorld);
	m_pTransform->m_matWorld = matWorld;

	m_vRHandPos = NxVec3(matWorld._41, matWorld._42, matWorld._43);

}

_bool CEnemy_Giant_Stone::Check_Dead()
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

HRESULT CEnemy_Giant_Stone::Add_Component(void * pArg)
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
		L"Component_Mesh_Enemy_Giant_Stone", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	BYTE* pVertices;
	_vec3 vMin, vMax;
	m_pMesh->Get_Mesh()->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&pVertices);
	D3DXComputeBoundingBox((D3DXVECTOR3*)pVertices, m_pMesh->Get_Mesh()->GetNumVertices(),
		m_pMesh->Get_Mesh()->GetNumBytesPerVertex(), &vMin, &vMax);
	m_pMesh->Get_Mesh()->UnlockVertexBuffer();

	tState.cName = "Enemy_Giant_Stone";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_KINEMATIC;
	tState.vLocalPos = { 0.f, (vMax.y - vMin.y) * 0.5f, 0.f };
	tState.vDimensions = { (vMax.x - vMin.x) * 0.5f, (vMax.y - vMin.y) * 0.5f, (vMax.z - vMin.z) * 0.5f };
	tState.fDensity = 100.f;
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	m_pTransform->Get_Actor()->getShapes()[0]->setName("TimeStop");

	NxMaterialDesc material;
	material.restitution = 0.f;
	material.dynamicFriction = 0.1f;
	material.staticFriction = 0.1f;

	NxMaterial* pMaterial = GET_INSTANCE(CPhysXMgr)->Get_NxScene()->createMaterial(material);
	m_pTransform->Get_Actor()->getShapes()[0]->setMaterial(pMaterial->getMaterialIndex());

	return S_OK;
}

HRESULT CEnemy_Giant_Stone::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

HRESULT CEnemy_Giant_Stone::Set_TargetTransform(_uint pSceneTag, _tchar * pLayerTag)
{
	CNxTransform* pTrans = (CNxTransform*)GET_INSTANCE(CObjectManager)->Get_Component(pSceneTag,
		pLayerTag, L"Com_NxTransform", 0);

	m_pTarget = pTrans;

	if (m_pTarget == nullptr)
		return E_FAIL;

	m_pPlayer = dynamic_cast<CNxPlayer*>(GET_INSTANCE(CObjectManager)->Get_GameObject(pSceneTag, pLayerTag, 0));

	Safe_AddRef(m_pTarget);

	return S_OK;
}

CGameObject * CEnemy_Giant_Stone::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CEnemy_Giant_Stone(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CEnemy_Giant_Stone::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CEnemy_Giant_Stone(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CEnemy_Giant_Stone::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTarget);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CGameObject::Free();
}

