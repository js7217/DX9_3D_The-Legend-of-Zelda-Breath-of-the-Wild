#include "stdafx.h"
#include "Maracas.h"

CMaracas::CMaracas(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice), m_pManagement(GET_INSTANCE(CManagement)),
	m_pDebug(GET_INSTANCE(CDebug_Console)),
	m_pPhysx(GET_INSTANCE(CPhysXMgr))
{
	Safe_AddRef(m_pDebug);
}

CMaracas::CMaracas(const CMaracas & rhs)
	: CGameObject(rhs), m_pManagement(rhs.m_pManagement),
	m_pDebug(rhs.m_pDebug),
	m_pPhysx(rhs.m_pPhysx)
{
	Safe_AddRef(m_pDebug);
}

HRESULT CMaracas::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CMaracas::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
	{
		ZeroMemory(&m_tWeaponHolsterInfo, sizeof(WEAPONHOLSTER));
		m_tWeaponHolsterInfo = *(WEAPONHOLSTER*)pArg;
	}

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CMaracas::Update_GameObject(const _double & TimeDelta)
{

	CGameObject::Update_GameObject(TimeDelta);

	_matrix matCombinedWorld;
	D3DXMatrixIdentity(&matCombinedWorld);

	return NO_EVENT;
}

_int CMaracas::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);


	//_matrix pMatX;
	//D3DXMatrixRotationX(&pMatX, D3DXToRadian(10.f));

	//_matrix pMatY;
	//D3DXMatrixRotationY(&pMatY, D3DXToRadian(0.f));

	_matrix pMatZ;
	D3DXMatrixRotationZ(&pMatZ, D3DXToRadian(30.f));

	//_matrix pMatTrans;
	//D3DXMatrixTranslation(&pMatTrans, 0.00f, -0.143f, -0.20f);

	_matrix pMat;
	pMat = pMatZ /** pMatY * pMatZ * pMatTrans*/;

	m_pTransform->Set_ParentMatrix(&(pMat * (*m_tWeaponHolsterInfo.pTargetHandMatrix * *m_tWeaponHolsterInfo.pTargetWorldMatrix)));

	//m_pTransform->Set_Scale(&_vec3(1.f, 1.f, 1.f));

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CMaracas::Render_GameObject()
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
		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_BaseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
			return E_FAIL;

		pEffect->CommitChanges();

		pEffect->BeginPass(0);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NO_EVENT;
}

HRESULT CMaracas::Add_Component()
{
	//For NxTransform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));
	tState.cName = "Maracas";
	tState.NxFlag |= NX_TRIGGER_ENABLE | NX_SF_DISABLE_RAYCASTING;
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_STATIC;
	//tState.vStartPos = { 15.f, 4.f, 31.f };
	tState.vStartPos = { 0.f, 0.f, 0.f };
	tState.NxFlag = 0;
	tState.vLocalPos = { 0.f, 0.f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
	tState.vDimensions = { 0.5f, 0.5f, 0.5f };
	tState.iMaterial = 0;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

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
		L"Component_Mesh_Maracas", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	//For.FrumStum
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustum)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMaracas::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect || nullptr == m_pMesh || nullptr == m_pTransform)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(pEffect->SetMatrix("g_matWorld", m_pTransform->Get_WorldMatrixPointer())))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matView", &Get_Transform(D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matProj", &Get_Transform(D3DTS_PROJECTION))))
		return E_FAIL;

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CMaracas::Create_Capsule_Coll(NxVec3 nxPos)
{
	return E_NOTIMPL;
}

CGameObject * CMaracas::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CMaracas(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CMaracas::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CMaracas(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMaracas::Free()
{

	Safe_Release(m_pFrustum);
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pDebug);
	Safe_Release(m_pMesh);

	CGameObject::Free();
}
