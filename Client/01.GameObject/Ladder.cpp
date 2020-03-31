#include "stdafx.h"
#include "Ladder.h"

CLadder::CLadder(LPDIRECT3DDEVICE9 pDevice, const _tchar* pMeshName)
	: CGameObject(pDevice)
	, m_pMeshName(pMeshName)
{

}

CLadder::CLadder(CLadder & rhs)
	: CGameObject(rhs)
	, m_pMeshName(rhs.m_pMeshName)
{

}

HRESULT CLadder::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CLadder::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	
	m_pTransform->Get_Actor()->getShapes()[0]->setSkinWidth(NxReal(0.01));
	m_pTransform->Get_Actor()->getShapes()[0]->setGroup(1);

	if(nullptr != pArg)
		m_vecLadder = *(vector<_vec4>*)pArg;

	return S_OK;
}

_int CLadder::Update_GameObject(const _double & TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);


	return NO_EVENT;
}

_int CLadder::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_NONALPHA, this)))
		return END_EVENT;

	return NO_EVENT;
}

HRESULT CLadder::Render_GameObject()
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

	return S_OK;
}

const _vec4 & CLadder::Ladder_Sort(const NxVec3 & vPos)
{
	map<_float, _vec4> mapDist;
	// TODO: 여기에 반환 구문을 삽입합니다.
	for (auto& vLadder : m_vecLadder)
	{
		NxVec3 vLadderPos;
		memcpy(&vLadderPos, &vLadder, sizeof(NxVec3));

		_float fDist = vPos.distance(vLadderPos);

		mapDist.insert({ fDist, vLadder });

	}
	return mapDist.begin()->second;
}

HRESULT CLadder::Add_Component()
{
	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	//For.Com_Mesh
	if (FAILED(CGameObject::Add_Component(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(),
		m_pMeshName, L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	BYTE* pVertices;
	_vec3 vMin, vMax;
	m_pMesh->Get_Mesh()->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&pVertices);
	D3DXComputeBoundingBox((D3DXVECTOR3*)pVertices, m_pMesh->Get_Mesh()->GetNumVertices(),
		D3DXGetFVFVertexSize(m_pMesh->Get_Mesh()->GetFVF()), &vMin, &vMax);
	m_pMesh->Get_Mesh()->UnlockVertexBuffer();

	tState.cName = "Ladder";
	tState.NxFlag |= NX_SF_VISUALIZATION | NX_TRIGGER_ENABLE;
	tState.eCollider = COL_TRIMESH;
	tState.eCollOpt = OPT_STATIC;
	tState.vLocalPos = { 0.f, 0.f, 0.f };
	tState.vAngle = { 0.f, 0.f, 0.f };
	tState.fHeight = 1.f;
	tState.iMaterial = 0;
	tState.pMesh = m_pMesh->Get_Mesh();
	tState.pGameObject = this;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLadder::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

CGameObject * CLadder::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pMeshName)
{
	CGameObject* pInstance = new CLadder(pDevice, pMeshName);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CLadder::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CLadder(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLadder::Free()
{
	m_vecLadder.clear();

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMesh);

	CGameObject::Free();
}
