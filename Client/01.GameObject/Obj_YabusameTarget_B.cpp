#include "stdafx.h"
#include "Obj_YabusameTarget_B.h"
#include "KeyMgr.h"

#include "Obj_YabusameTarget_Brk_B.h"
#include "Quest_Ballon_UI.h"

CObj_YabusameTarget_B::CObj_YabusameTarget_B(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice), m_pPhysXMgr(GET_INSTANCE(CPhysXMgr))
{


}

CObj_YabusameTarget_B::CObj_YabusameTarget_B(const CObj_YabusameTarget_B & rhs)
	: CDgnObj(rhs), m_pPhysXMgr(rhs.m_pPhysXMgr)
{

}

HRESULT CObj_YabusameTarget_B::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CObj_YabusameTarget_B::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	_matrix matWorld = *(_matrix*)pArg;

	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(matWorld.m);

	m_pTransform->Get_Actor()->setGlobalPose(pMat);
	m_pTransform->Get_Actor()->wakeUp(1000.f);

	_uint iScene = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();

	for (int i = 0; i < 20; ++i)
	{
		CGameObject* pGameObject =
			GET_INSTANCE(CObjectManager)->Add_GameObject(iScene, L"GameObject_Obj_YabusameTarget_Brk_B", iScene, (void*)&pMat.t);
		m_BrkList.emplace_back(pGameObject);
	}

	return S_OK;
}

_int CObj_YabusameTarget_B::Update_GameObject(const _double & TimeDelta)
{
	CDgnObj::Update_GameObject(TimeDelta);

	if (strcmp("Dead", m_pTransform->Get_Actor()->getName()) == 0)
	{
		GET_INSTANCE(CEffectManager)->Get_EffectSet(L"Destroy_Stone", m_pTransform->Get_WorldMatrixPointer());
		NxVec3 vPos = m_pTransform->Get_Actor()->getGlobalPosition();
	//	vPos.y += 5.f;
		for (auto& pObj : m_BrkList)
		{
			dynamic_cast<CObj_YabusameTarget_Brk_B*>(pObj)->Create_NxTransform(vPos);
			GET_INSTANCE(CObjectManager)->Add_GameObject(pObj, L"Layer_FieldStage", SCENE_FIELD);
		}

		m_BrkList.clear();

		// 미니 게임 클래스 생성. // 미니 게임 시작 구문 
		_int iSceneNum = GET_INSTANCE(CManagement)->Get_CurrentSceneNum();
		_matrix matWorld = *m_pTransform->Get_WorldMatrixPointer();

		if (FAILED(GET_INSTANCE(CManagement)->Add_GameObject(iSceneNum, L"GameObject_FldObj_MiniGame_Shooting", iSceneNum, L"Layer_FieldStage", (void*)&matWorld)))
			return E_FAIL;

		GET_INSTANCE(CSoundMgr)->Stop_SoundChannel(CSoundMgr::BGM);
		GET_INSTANCE(CSoundMgr)->Play_BGM(L"MiniGameBGM.mp3");
		CQuest_Ballon_UI* pQuest_UI = (CQuest_Ballon_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_Ballon_UI", 0);
		if (pQuest_UI == nullptr)
			return E_FAIL;
		pQuest_UI->Set_GameStart(true);
		return END_EVENT;
	}

	_vec3 vPos;
	m_pTransform->Get_Info(CNxTransform::INFO_POS, &vPos);
	m_bDraw = m_pFrustumCom->WorldPt_InFrustum(&vPos, m_pTransform, 5.f);
	return NO_EVENT;
}

_int CObj_YabusameTarget_B::LateUpdate_GameObject(const _double & TimeDelta)
{
	CDgnObj::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (false == m_bDraw)
		return NO_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CObj_YabusameTarget_B::Render_GameObject()
{
	if (nullptr == m_pShader || nullptr == m_pMesh)
		return E_FAIL;

	NxMat34 matWOrld = m_pTransform->Get_Actor()->getGlobalPose();

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

		pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.f, 0.f, 0.f, 1.f));
		pEffect->SetVector("g_vecEmessiveColor", &_vec4(0.1f, 0.5f, 0.9f, 1.f));
		pEffect->SetBool("g_bShade", true);

		pEffect->CommitChanges();

		pEffect->BeginPass(5);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	////////////////////

	////////////////////	Render_Fonts
	

	return NO_EVENT;
}

HRESULT CObj_YabusameTarget_B::Add_Component()
{

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
		L"Component_Mesh_Obj_YabusameTarget_B", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	//For NxTransform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	tState.cName = "Obj_YabusameTarget_B";
	tState.eCollider = COL_TRIMESH;
	tState.eCollOpt = OPT_STATIC;
	tState.NxFlag = NX_SF_VISUALIZATION;
	tState.pMesh = m_pMesh->Get_Mesh();
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

//	m_pTransform->Get_Actor()->raiseBodyFlag(NX_BF_FROZEN);
	m_pTransform->Get_Actor()->getShapes()[0]->setSkinWidth(0.0);

	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CObj_YabusameTarget_B::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CDgnObj * CObj_YabusameTarget_B::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CObj_YabusameTarget_B(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CObj_YabusameTarget_B::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CObj_YabusameTarget_B(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CObj_YabusameTarget_B::Free()
{
	for (auto& pObj : m_BrkList)
		Safe_Release(pObj);


	Safe_Release(m_pTransform);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);


	CDgnObj::Free();
}
