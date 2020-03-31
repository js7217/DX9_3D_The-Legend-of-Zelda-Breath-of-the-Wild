#include "stdafx.h"
#include "FldObj_EntranceElevator.h"
#include "KeyMgr.h"


CFldObj_EntranceElevator::CFldObj_EntranceElevator(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice), m_pManagement(GET_INSTANCE(CManagement)),
	m_pDebug(GET_INSTANCE(CDebug_Console)),
	m_pPhysx(GET_INSTANCE(CPhysXMgr))
{

	Safe_AddRef(m_pDebug);

}

CFldObj_EntranceElevator::CFldObj_EntranceElevator(const CFldObj_EntranceElevator & rhs)
	: CDgnObj(rhs), m_pManagement(rhs.m_pManagement),
	m_pDebug(rhs.m_pDebug),
	m_pPhysx(rhs.m_pPhysx)
{

	Safe_AddRef(m_pDebug);

}


HRESULT CFldObj_EntranceElevator::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CFldObj_EntranceElevator::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	 
	_matrix matWorld = *(_matrix*)pArg;

	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(matWorld);

	m_pTransform->Get_Actor()->setGlobalPose(pMat);

	return S_OK;
}

_int CFldObj_EntranceElevator::Update_GameObject(const _double & TimeDelta)
{
	CDgnObj::Update_GameObject(TimeDelta);

	_vec3 vPos;
	m_pTransform->Get_Info(CNxTransform::INFO_POS, &vPos);
	m_bDraw = m_pFrustumCom->WorldPt_InFrustum(&vPos, m_pTransform, 5.f);

	if(m_bIsMove)
		Move_Elevator(TimeDelta);
	else
	{
		if (m_bIsPlayAnimation)
		{
			m_pTransform->Get_Actor()->releaseShape(*m_pTriggerShape);
			GET_INSTANCE(CSoundMgr)->Play_3D_Sound(L"Event_EntranceElevator_Move01.wav",
				CSoundMgr::FIELD_OBJECT1, m_pTransform->Get_GlobalPos());
			m_bIsMove = true;
		}
	}

	return NO_EVENT;
}

_int CFldObj_EntranceElevator::LateUpdate_GameObject(const _double & TimeDelta)
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

HRESULT CFldObj_EntranceElevator::Render_GameObject()
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
		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_BaseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
			return E_FAIL;

		pEffect->CommitChanges();

		pEffect->BeginPass(0);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	////////////////////

	////////////////////	Render_Fonts
	

	return NO_EVENT;
}

void CFldObj_EntranceElevator::Move_Elevator(const _double & TimeDelta)
{
	if (m_dwTimer <= 0.0)
	{
		m_bIsPlayAnimation = false;
		m_dwTimer = 10.0;
		return;
	}

	m_dwTimer -= TimeDelta;

	NxMat34 matWorld = m_pTransform->Get_Actor()->getGlobalPose();
	matWorld.t.y -= NxReal(0.18f * TimeDelta);
	m_pTransform->Get_Actor()->moveGlobalPose(matWorld);
}

HRESULT CFldObj_EntranceElevator::Add_Component()
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
		L"Component_Mesh_FldObj_EntranceElevator", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));
	tState.cName = "FldObj_EntranceElevator";
	tState.eCollider = COL_TRIMESH;
	tState.eCollOpt = OPT_KINEMATIC;
	tState.pGameObject = this;
	tState.pMesh = m_pMesh->Get_Mesh();

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	// Trigger ¸¸µé±â.
	NxTriangleMeshShapeDesc TriShape = m_pTransform->Create_NxMeshDesc();
	TriShape.name = tState.cName;
	TriShape.shapeFlags = NX_TRIGGER_ENABLE | NX_SF_VISUALIZATION;

	m_pTriggerShape = m_pTransform->Get_Actor()->createShape(TriShape);

	return S_OK;
}

HRESULT CFldObj_EntranceElevator::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CDgnObj * CFldObj_EntranceElevator::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CFldObj_EntranceElevator(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CFldObj_EntranceElevator::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CFldObj_EntranceElevator(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CFldObj_EntranceElevator::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDebug);
	Safe_Release(m_pMesh);


	CDgnObj::Free();
}
