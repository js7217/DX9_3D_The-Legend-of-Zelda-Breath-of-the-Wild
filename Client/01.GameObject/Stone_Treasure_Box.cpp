#include "stdafx.h"
#include "Stone_Treasure_Box.h"
#include "KeyMgr.h"
#include "DirectInput.h"
#include "KeyMgr.h"
#include "FontMgr.h"
#include "TreasureBox_UI.h"

#define _CLOSED 0
#define _OPENED 1
#define _OPENING 2

CStone_Treasure_Box::CStone_Treasure_Box(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice), m_pManagement(GET_INSTANCE(CManagement)),
	m_pDebug(GET_INSTANCE(CDebug_Console)),
	m_pPhysx(GET_INSTANCE(CPhysXMgr))
{

	Safe_AddRef(m_pDebug);

}

CStone_Treasure_Box::CStone_Treasure_Box(const CStone_Treasure_Box & rhs)
	: CDgnObj(rhs), m_pManagement(rhs.m_pManagement),
	m_pDebug(rhs.m_pDebug),
	m_pPhysx(rhs.m_pPhysx)
{

	Safe_AddRef(m_pDebug);
	ZeroMemory(&m_tItemInfo, sizeof(ITEM));
}

void CStone_Treasure_Box::Set_Magnet()
{
	m_pTransform->Get_Actor()->getShapes()[0]->setName("Magnet");
}

HRESULT CStone_Treasure_Box::Add_MeshComponent(BOXTYPE eType)
{
	switch (eType)
	{
	case CStone_Treasure_Box::STONE:
		//For.Com_Mesh
		if (FAILED(CDgnObj::Add_Component(SCENE_MESH_DYNAMIC,
			L"Component_Mesh_Obj_TBox_Dungeon_Stone", L"Com_Mesh", (CComponent**)&m_pMesh)))
			return E_FAIL;

		break;
	case CStone_Treasure_Box::IRON:
		//For.Com_Mesh
		if (FAILED(CDgnObj::Add_Component(SCENE_MESH_DYNAMIC,
			L"Component_Mesh_IronBox", L"Com_Mesh", (CComponent**)&m_pMesh)))
			return E_FAIL;

		Set_Magnet();

		break;
	}

	m_pMesh->SetUp_AnimationSet(_CLOSED);
	m_iCurrentAni = _CLOSED;

	return S_OK;
}

HRESULT CStone_Treasure_Box::Ready_GameObject_Prototype()
{


	return S_OK;
}

HRESULT CStone_Treasure_Box::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	//_matrix matWorld = *(_matrix*)pArg;
	DGNOBJECT tObjectInfo = *(DGNOBJECT*)pArg;
	_matrix matWorld = tObjectInfo.matWorld;
	m_iNum = tObjectInfo.iNum;
	m_tItemInfo = tObjectInfo.tItemInfo;

	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(matWorld.m);

	m_pTransform->Get_Actor()->setGlobalPose(pMat);


	return S_OK;
}

_int CStone_Treasure_Box::Update_GameObject(const _double & TimeDelta)
{
	m_dTimeDelta = TimeDelta;
	if (m_pTransform->Get_Actor()->isSleeping())
		m_pTransform->Get_Actor()->wakeUp(999);
	CDgnObj::Update_GameObject(TimeDelta);

	m_fTime = (_float)TimeDelta;

	if (m_bMagnetCatch)
	{
		if (m_bBlinkTime)
			m_fBlinkTime += (_float)(TimeDelta);
		else
			m_fBlinkTime -= (_float)(TimeDelta);

		if (m_fBlinkTime >= 1.f || m_fBlinkTime <= 0.f)
			m_bBlinkTime = !m_bBlinkTime;
	}
	else
	{
		m_fBlinkTime = 0;
		m_bBlinkTime = FALSE;
	}

	//
	if (_OPENING == m_iCurrentAni)
	{
		_uint iSceneNum = CManagement::Get_Instance()->Get_CurrentSceneNum();
		CTreasureBox_UI* pTreasureBox_UI = (CTreasureBox_UI*)GET_INSTANCE(CObjectManager)->Get_GameObject(iSceneNum, L"Layer_TreasureBox_UI", 0);
		if (pTreasureBox_UI == nullptr)
			return NO_EVENT;

		pTreasureBox_UI->Set_IsDraw(true);
		pTreasureBox_UI->Set_IsNum(m_iNum);
	}

	//
	
	return NO_EVENT;
}

_int CStone_Treasure_Box::LateUpdate_GameObject(const _double & TimeDelta)
{
	CDgnObj::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	//if (nullptr != m_pDebug && m_pDebug->Get_bPlayer() == FALSE)
	//	m_pDebug->Set_PlayerPos(*m_pTransform->Get_Info(CNxTransform::INFO_POS));
	
	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CStone_Treasure_Box::Render_GameObject()
{
	if (nullptr == m_pShader || nullptr == m_pMesh)
		return E_FAIL;

	if (_OPENING == m_iCurrentAni)
	{
		if (m_pMesh->IsAnimationSetEnd(-0.2f))
		{
			m_pMesh->SetUp_AnimationSet(_OPENED);
			m_iCurrentAni = _OPENED;
			m_bIsOpened = true;
		}
	}

	m_pMesh->Play_Animation(m_dTimeDelta);

	NxMat34 matWOrld = m_pTransform->Get_Actor()->getGlobalPose();

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
			if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, j, "g_BaseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
				return E_FAIL;

			pEffect->CommitChanges();

			if (!m_bMagnetCatch)
				pEffect->BeginPass(0);
			else
				pEffect->BeginPass(12);

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

HRESULT CStone_Treasure_Box::Add_Component()
{
	//For NxTransform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));
	tState.cName = "StoneBox";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_DYNAMIC;
	tState.vLocalPos = { 0.f, 0.5f, 0.f };
	tState.vDimensions = { 0.5f, 0.5f, 0.5f };
	tState.fDensity = 100.f;
	tState.pGameObject = this;
	
	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStone_Treasure_Box::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect || nullptr == m_pMesh || nullptr == m_pTransform)
		return E_FAIL;

	Safe_AddRef(pEffect);

	//if (FAILED(m_pTransform->SetUp_OnShader(pEffect, "g_matWorld")))
	//	return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matWorld", m_pTransform->Get_WorldMatrixPointer())))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matView", &Get_Transform(D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matProj", &Get_Transform(D3DTS_PROJECTION))))
		return E_FAIL;

	m_fFlowTime += m_fTime;
	pEffect->SetFloat("g_fTime", m_fFlowTime);
	pEffect->SetTexture("g_FlowTexture", m_pRenderer->Get_FlowTexture());
	pEffect->SetTexture("g_NoiseTexture", m_pRenderer->Get_NoiseTexture());
	pEffect->SetFloat("g_fBlinkTime", m_fBlinkTime);
	pEffect->SetBool("g_bTimeStop", FALSE);

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CStone_Treasure_Box::Create_Capsule_Coll(NxVec3 nxPos)
{
	return E_NOTIMPL;
}

CDgnObj * CStone_Treasure_Box::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CStone_Treasure_Box(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CStone_Treasure_Box::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CStone_Treasure_Box(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CStone_Treasure_Box::Free()
{

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDebug);
	Safe_Release(m_pMesh);


	CDgnObj::Free();
}
