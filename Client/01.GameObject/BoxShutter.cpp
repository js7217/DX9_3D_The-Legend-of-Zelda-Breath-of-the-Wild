#include "stdafx.h"
#include "BoxShutter.h"

#include "KeyMgr.h"

CBoxShutter::CBoxShutter(LPDIRECT3DDEVICE9 pDevice)
	: CDgnObj(pDevice)
{

}

CBoxShutter::CBoxShutter(const CBoxShutter & rhs)
	: CDgnObj(rhs)
{

}

HRESULT CBoxShutter::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CBoxShutter::Ready_GameObject(void * pArg)
{
	_matrix matWorld = *(_matrix*)pArg;
	
	float fRotY = matWorld._11;
	if (1 == fRotY)
		m_bIsUp = true;
	else
		m_bIsUp = false;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;
	
	NxMat34 pMat;
	pMat.isIdentity();
	pMat.setColumnMajor44(matWorld.m);
	
	m_pTransform->Get_Actor()->setGlobalPose(pMat);
	m_pTransform->Get_Actor()->raiseBodyFlag(NX_BF_KINEMATIC);

	Set_MoveInit();

	return S_OK;
}

_int CBoxShutter::Update_GameObject(const _double & TimeDelta)
{
	if (TimeDelta == 0.0)
		return NO_EVENT;
	CDgnObj::Update_GameObject(TimeDelta);

	// 움직이는 로직
	Move_Shutter(TimeDelta);
	return NO_EVENT;
}

_int CBoxShutter::LateUpdate_GameObject(const _double & TimeDelta)
{
	CDgnObj::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CBoxShutter::Render_GameObject()
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

		pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.f, 0.f, 0.f, 1.f));
		pEffect->SetVector("g_vecEmessiveColor", &_vec4(0.1f, 0.5f, 0.7f, 1.f));
	
		pEffect->SetBool("g_bShade", true);

		pEffect->CommitChanges();

		pEffect->BeginPass(5);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

HRESULT CBoxShutter::Set_MoveInit()
{
	if (nullptr == m_pTransform->Get_Actor())
		return E_FAIL;
	
	NxMat34 matWorld = m_pTransform->Get_Actor()->getGlobalPose();
	m_vMoveLook = matWorld.M.getColumn(2);
	m_vMoveLook.normalize();

	m_vMinPos = matWorld.t;
	m_vMaxPos = m_vMinPos + (m_vMoveLook * m_fMoveDist);

	return S_OK;
}

HRESULT CBoxShutter::Move_Shutter(const _double & TimeDelta)
{
	NxVec3 vGlobalPos = m_pTransform->Get_Actor()->getGlobalPosition();
	m_dwStopTimer -= TimeDelta;

	if (m_dwStopTimer > 0.0 || TimeDelta == 0.0)
		return S_OK;
	else
	{
		if (m_bIsUp)
		{
			_float fDist = vGlobalPos.distance(m_vMaxPos);	// 높은곳과의 거리를 구한다.

			if (fDist <= 1.f)
			{

				m_pTransform->Get_Actor()->moveGlobalPosition(m_vMaxPos);	// 거리가 가까워지면 때려넣기.
				m_dwStopTimer = 3.0;
				m_vMoveLook = m_pTransform->Get_Actor()->getGlobalPose().M.getColumn(2);
				m_vMoveLook.normalize();
				m_vMoveLook *= -1.f;

				if (m_bIsUp)
				{
					GET_INSTANCE(CSoundMgr)->Play_3D_Sound(L"CapShutter_Close01.wav", CSoundMgr::DGN_DEFALULT, m_pTransform->Get_GlobalPos());
				}
				m_bIsUp = false;
				return S_OK;

			}
		}
		else
		{
			_float fDist = vGlobalPos.distance(m_vMinPos);	// 낮은곳과의 거리를 구한다.

			if (fDist <= 1.f)
			{
				m_pTransform->Get_Actor()->moveGlobalPosition(m_vMinPos);	// 거리가 가까워지면 때려넣기.
				m_dwStopTimer = 3.0;
				m_vMoveLook = m_pTransform->Get_Actor()->getGlobalPose().M.getColumn(2);
				m_vMoveLook.normalize();
				m_bIsUp = true;
				return S_OK;
			}
		}

		vGlobalPos += m_vMoveLook * NxReal((1.f / 60.f) * m_dwPower);
		m_pTransform->Get_Actor()->moveGlobalPosition(vGlobalPos);
	}

	return S_OK;
}

HRESULT CBoxShutter::Add_Component(void * pArg)
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
		L"Component_Mesh_BoxShutter", L"Com_Mesh", (CComponent**)&m_pMesh)))
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

	tState.cName = "BoxShutter";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_DYNAMIC;
	tState.vLocalPos = { 0.f, 0.f, -vMax.z };
	tState.vAngle = { -90.f, 0.f, 0.f };
	tState.vDimensions = { (vMax.x - vMin.x) * 0.5f, (vMax.y - vMin.y) * 0.5f, (vMax.z - vMin.z) * 0.5f };
	tState.vVelocity = { 1.f, 1.f, 1.f };
	tState.fRadius = 1.f;

	// For.Com_Transform
	if (FAILED(CDgnObj::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoxShutter::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CDgnObj * CBoxShutter::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDgnObj* pInstance = new CBoxShutter(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDgnObj * CBoxShutter::Clone_GameObject(void * pArg)
{
	CDgnObj* pInstance = new CBoxShutter(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBoxShutter::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CDgnObj::Free();
}

