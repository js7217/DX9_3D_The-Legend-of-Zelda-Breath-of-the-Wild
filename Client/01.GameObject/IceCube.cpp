#include "stdafx.h"
#include "IceCube.h"
#include "KeyMgr.h"
#include "NxPlayer.h"

CIceCube::CIceCube(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{

}

CIceCube::CIceCube(CIceCube & rhs)
	: CGameObject(rhs)
{

}

void CIceCube::Set_bDone(_bool bCheck)
{
	m_bDone = bCheck;
	NxVec3 nxPos = m_pTransform->Get_Actor()->getGlobalPosition();
	nxPos.y -= 4.05f;
	m_fHeight = nxPos.y + m_fLocal - 0.35f;
	m_pTransform->Get_Actor()->setGlobalPosition(nxPos);
	m_pTransform->Get_Actor()->clearActorFlag(NX_AF_DISABLE_COLLISION);
}

HRESULT CIceCube::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CIceCube::Ready_GameObject(void * pArg)
{
	_vec3 vWorldPos;
	memcpy(&vWorldPos, pArg, sizeof(_vec3));

	if (FAILED(Add_Component(vWorldPos)))
		return E_FAIL;

	Set_TargetTransform(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player");

	m_pTransform->NxRotation_Y(m_pTarget);

	D3DXCreateTextureFromFileA(m_pDevice, "..\\..\\Resources\\Mesh_Static\\IceMaker\\Ready.png", &m_pReadyCube);

	return S_OK;
}

_int CIceCube::Update_GameObject(const _double & TimeDelta)
{
	if (!m_bPreCreate)
	{
		m_LifeTime += TimeDelta;
		m_TwinkleTime += TimeDelta;
	}

	m_fTime += (_float)TimeDelta;

	if (m_bPreCreate)
	{
		if (m_bDone && m_pTransform->Get_Actor()->getGlobalPosition().x > 999.f)
			return END_EVENT;
		else if (m_bDone)
		{
			m_pTarget->Get_Actor()->wakeUp(10);
			GET_INSTANCE(CSoundMgr)->Play_Sound(L"Sys_Item_IceMaker_Making.wav", CSoundMgr::LINK_ICEMAKE);
		}
	}

	if (m_bPreCreate && !m_bDone)
		Resize_IceMaker(TimeDelta);
	else
		Move_IceMaker(TimeDelta);

	CGameObject::Update_GameObject(TimeDelta);

	if (m_LifeTime > 30.0)
	{
		GET_INSTANCE(CEffectManager)->Get_EffectSet(L"IceShard", m_pTransform->Get_WorldMatrixPointer());
		GET_INSTANCE(CSoundMgr)->Play_3D_Sound(L"IceMakerBlock_Crush.wav", CSoundMgr::LINK_ICEBREAK, m_pTransform->Get_GlobalPos());
		return END_EVENT;
	}

	if (m_TwinkleTime > 2.0)
	{
		GET_INSTANCE(CEffectManager)->Get_EffectSet(L"IceTwinkle", m_pTransform->Get_WorldMatrixPointer());
		m_TwinkleTime = 0.0;
	}

	_int iExit = CGameObject::End_GameObject();

	if (iExit == -1)
	{
		GET_INSTANCE(CEffectManager)->Get_EffectSet(L"IceShard", m_pTransform->Get_WorldMatrixPointer());
		return END_EVENT;
	}

	return NO_EVENT;
}

_int CIceCube::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);

	if (nullptr == m_pRenderer)
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_DIFFUSEALPHA, this)))
		return END_EVENT;

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_RIMLIGHT, this)))
		return END_EVENT;

	if (m_bDone)
	{
		if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_SHADOWDEPTH, this)))
			return END_EVENT;
	}

	return NO_EVENT;
}

HRESULT CIceCube::Render_GameObject()
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

		pEffect->SetTexture("g_IceMaker", m_pReadyCube);

		if (FAILED(m_pTexture->SetUp_OnShader(pEffect, "g_FlowTexture")))
			return E_FAIL;

		if (FAILED(m_pTextureNoise->SetUp_OnShader(pEffect, "g_NoiseTexture")))
			return E_FAIL;

		if (FAILED(pEffect->SetFloat("g_fTime", m_fTime)))
			return E_FAIL;

		pEffect->SetBool("g_bMaskMap", false);

		pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.1f, 0.6f, 0.9f, 1.f));
		pEffect->SetVector("g_vecEmessiveColor", &_vec4(0.2f, 0.7f, 1.0f, 1.f));

		pEffect->SetBool("g_bShade", TRUE);

		pEffect->CommitChanges();

		if (m_bDone)
		{
			pEffect->BeginPass(8);
		}
		else
		{
			pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.1f, 0.6f, 0.9f, 0.5f));

			pEffect->BeginPass(9);
		}
		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);


	return NO_EVENT;
}

void CIceCube::Render_Shadow(_int iPass)
{
	if (nullptr == m_pShader || nullptr == m_pMesh)
		return;


	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	Safe_AddRef(pEffect);

	if (FAILED(SetUp_ConstantTable(pEffect)))
		return;

	pEffect->Begin(nullptr, 0);

	for (_uint i = 0; i < m_pMesh->Get_NumMaterials(); ++i)
	{
		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_DiffuseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_SpecularTexture", MESHTEXTURE::TYPE_SPECULAR)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_NormalTexture", MESHTEXTURE::TYPE_NORMAL)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_EmessiveTexture", MESHTEXTURE::TYPE_EMESSIVE)))
			return;

		pEffect->CommitChanges();

		pEffect->BeginPass(iPass);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);
}

void CIceCube::Render_RimLight()
{
	if (nullptr == m_pShader || nullptr == m_pMesh)
		return;


	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	Safe_AddRef(pEffect);

	if (FAILED(SetUp_ConstantTable(pEffect)))
		return;

	pEffect->Begin(nullptr, 0);

	for (_uint i = 0; i < m_pMesh->Get_NumMaterials(); ++i)
	{
		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_DiffuseTexture", MESHTEXTURE::TYPE_DIFFUSE)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_SpecularTexture", MESHTEXTURE::TYPE_SPECULAR)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_NormalTexture", MESHTEXTURE::TYPE_NORMAL)))
			return;

		if (FAILED(m_pMesh->SetTexture_OnShader(pEffect, i, "g_EmessiveTexture", MESHTEXTURE::TYPE_EMESSIVE)))
			return;

		pEffect->SetVector("g_vecDiffuseColor", &_vec4(0.2f, 0.7f, 1.0f, 1.f));
		pEffect->SetBool("g_bSpecular", TRUE);
		pEffect->SetBool("g_bRimRight", FALSE);

		pEffect->CommitChanges();

		pEffect->BeginPass(3);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);
}

void CIceCube::Resize_IceMaker(const _double & TimeDelta)
{
	NxVec3 vGlobalPos = m_pTransform->Get_Actor()->getGlobalPosition();

	_float fSize = m_pTransform->m_vScale.y;

	fSize += _float(0.001f + TimeDelta);

	if (fSize > 1.f)
		fSize = 0.01f;

	m_pTransform->Set_Scale(&_vec3(1.f, fSize, 1.f));

	if (m_bPreCreate && !m_bDone)
	{
		m_pTransform->NxRotation_Y(m_pTarget);
		vGlobalPos.x = m_pTarget->m_vIceCubePos.x;
		vGlobalPos.y = m_pTarget->m_vIceCubePos.y - 0.1f;
		vGlobalPos.z = m_pTarget->m_vIceCubePos.z;
	}

	m_pTransform->Get_Actor()->setGlobalPosition(vGlobalPos);
}

HRESULT CIceCube::Move_IceMaker(const _double & TimeDelta)
{
	m_pTransform->Set_Scale(&_vec3(1.f, 1.f, 1.f));
	NxVec3 vGlobalPos = m_pTransform->Get_Actor()->getGlobalPosition();

	if (vGlobalPos.y < m_fHeight)
	{
		vGlobalPos.y += _float(TimeDelta * m_fMakeAcc);
		//m_fTotalHeigh += NxReal(TimeDelta * m_fMakeAcc);

		if (m_fMakeAcc < 10.f)
			m_fMakeAcc += 0.08f * m_fMakeAcc;

		if (vGlobalPos.y >= m_fHeight)
		{
			if (m_bPreCreate)
			{
				m_fMakeAcc = 0.4f;
				m_fTotalHeigh = 0.f;

				if (m_bDone)
					m_bPreCreate = FALSE;
			}
		}

		//if (m_bPreCreate && !m_bDone)
		//{
		//	m_pTransform->NxRotation_Y(m_pTarget);
		//	vGlobalPos.x = m_pTarget->m_vIceCubePos.x;
		//	vGlobalPos.y -= m_fHeight;
		//	vGlobalPos.z = m_pTarget->m_vIceCubePos.z;
		//}

		m_pTransform->Get_Actor()->setGlobalPosition(vGlobalPos);
	}

	return S_OK;
}

HRESULT CIceCube::Create_IceCube_Ladder(COLL_STATE tState)
{
	NxBoxShapeDesc LadderDesc;
	LadderDesc.setToDefault();

	LadderDesc.dimensions = NxVec3(tState.vDimensions);
	LadderDesc.skinWidth = 0.01f;
	LadderDesc.userData = this;
	LadderDesc.localPose.t = NxVec3(tState.vLocalPos);
	LadderDesc.name = "IceCube_Ladder";

	LadderDesc.shapeFlags |= NX_TRIGGER_ENABLE;
	LadderDesc.group = 1;

	m_pTransform->Get_Actor()->createShape(LadderDesc);
	return S_OK;
}

HRESULT CIceCube::Add_Component(_vec3 vWorldPos)
{
	// For.Com_Transform
	COLL_STATE tState;
	ZeroMemory(&tState, sizeof(COLL_STATE));

	//For.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_MESH_STATIC,
		L"Component_Mesh_IceMaker", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	BYTE* pVertices;
	_vec3 vMin, vMax;
	m_pMesh->Get_Mesh()->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&pVertices);
	D3DXComputeBoundingBox((D3DXVECTOR3*)pVertices, m_pMesh->Get_Mesh()->GetNumVertices(),
		m_pMesh->Get_Mesh()->GetNumBytesPerVertex(), &vMin, &vMax);
	m_pMesh->Get_Mesh()->UnlockVertexBuffer();

	vWorldPos.y -= (vMax.y - vMin.y) + 0.75f;
	m_fHeight = 1.9f;//vWorldPos.y + (vMax.y - vMin.y); 
	m_fLocal = (vMax.y - vMin.y); //Worldpos.y + m_fLocal;

	tState.cName = "IceMaker";
	tState.eCollider = COL_BOX;
	tState.eCollOpt = OPT_KINEMATIC;
	//tState.NxFlag |= NX_TRIGGER_ENABLE;
	tState.vLocalPos = { 0.f, (vMax.y - vMin.y) * 0.5f, 0.f };
	tState.vStartPos = vWorldPos;
	tState.vAngle = { 0.f, 0.f, 0.f };
	tState.vDimensions = { (vMax.x - vMin.x) * 0.5f, (vMax.y - vMin.y) * 0.5f, (vMax.z - vMin.z) * 0.5f };
	tState.iMaterial = 0;
	if (tState.eCollider == COL_TRIMESH)
		tState.pMesh = m_pMesh->Get_Mesh();

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT,
		L"Component_NxTransform", L"Com_NxTransform", (CComponent**)&m_pTransform, &tState)))
		return E_FAIL;

	//For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_Flow", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	//For.Com_TextureNoise
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE,
		L"Component_Texture_FlowNoise", L"Com_TextureNoise", (CComponent**)&m_pTextureNoise)))
		return E_FAIL;

	Create_IceCube_Ladder(tState);
	//	m_pTransform->Get_Actor()->getShapes()[0]->setGroup(1);
	m_pTransform->Get_Actor()->raiseActorFlag(NX_AF_DISABLE_COLLISION);
	m_pTransform->Get_Actor()->getShapes()[0]->setName("IceMaker");
	return S_OK;
}

HRESULT CIceCube::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

HRESULT CIceCube::Set_TargetTransform(_uint pSceneTag, _tchar * pLayerTag)
{
	CNxTransform* pTrans = (CNxTransform*)GET_INSTANCE(CObjectManager)->Get_Component(pSceneTag,
		pLayerTag, L"Com_NxTransform", 0);

	m_pTarget = pTrans;

	if (m_pTarget == nullptr)
		return E_FAIL;

	CNxPlayer* pPlayer = dynamic_cast<CNxPlayer*>(GET_INSTANCE(CObjectManager)->
		Get_GameObject(GET_INSTANCE(CManagement)->Get_CurrentSceneNum(), L"Layer_Player", 0));

	pPlayer->Set_IceCubeSwitch(this);

	Safe_AddRef(m_pTarget);

	return S_OK;
}

CGameObject * CIceCube::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGameObject* pInstance = new CIceCube(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;;
}

CGameObject * CIceCube::Clone_GameObject(void * pArg)
{
	CGameObject* pInstance = new CIceCube(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CIceCube::Free()
{
	if (GET_INSTANCE(CPhysXMgr) != nullptr)
		GET_INSTANCE(CPhysXMgr)->Set_Ladder(FALSE);

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTarget);
	Safe_Release(m_pMesh);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTextureNoise);
	Safe_Release(m_pReadyCube);

	CGameObject::Free();
}
