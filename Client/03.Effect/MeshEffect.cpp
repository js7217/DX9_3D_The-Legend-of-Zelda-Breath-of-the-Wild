#include "stdafx.h"
#include "MeshEffect.h"

CMeshEffect::CMeshEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CEffect(pGraphic_Device)
{
}

CMeshEffect::CMeshEffect(const CMeshEffect & rhs)
	: CEffect(rhs)
{
	if (rhs.m_pFilter != nullptr)
		m_pFilter = (CTexture*)rhs.m_pFilter->Clone_Component(nullptr);
	if (rhs.m_pTexture != nullptr)
		m_pTexture = (CTexture*)rhs.m_pTexture->Clone_Component(nullptr);
	if (rhs.m_pMesh != nullptr)
		m_pMesh = (CMesh_Static*)rhs.m_pMesh->Clone_Component(nullptr);
}

HRESULT CMeshEffect::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CMeshEffect::Ready_GameObject(void * pArg)
{
	if (FAILED(CEffect::Ready_GameObject_Prototype()))
		return E_FAIL;

	m_eType = MESH;

	if (FAILED(CGameObject::Add_Component(SCENE_MESH_STATIC, (_tchar*)pArg, L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMeshEffect::Ready_GameObject(_matrix * pArg)
{
	if (FAILED(CEffect::Ready_GameObject_Prototype()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_matWorld = *pArg;

		if (m_tEffectInfo.bIsTarget)
		{
			m_pMatWorld = pArg;
		}
	}
	m_bOnce = TRUE;

	return NOERROR;
}

_int CMeshEffect::Update_GameObject(const _double & fTimeDelta)
{
	m_fTime += (_float)fTimeDelta;

	if (m_bDead)
		return END_EVENT;

	if (m_bOnce)
	{
		m_tEffectInfo.m_bUVStart = m_tEffectInfo.m_bUV;
		m_tEffectInfo.m_bRotStart = m_tEffectInfo.m_bRot;
		m_tEffectInfo.m_bScalingStart = m_tEffectInfo.m_bScaling;
		m_tEffectInfo.m_bMoveStart = m_tEffectInfo.m_bMove;

		Set_Transform(CEffect::POSITION, &m_vPos);
		Set_Transform(CEffect::ROTATION, &m_vRot);
		Set_Transform(CEffect::SCALE, &m_vScale);

		//_matrix matTest = m_matWorld;
		//matTest.m[3][0] = 0;
		//matTest.m[3][1] = 0;
		//matTest.m[3][2] = 0;

		_matrix matWorld = *m_pTransform->Get_WorldMatrixPointer() * m_matWorld;
		m_pTransform->Set_State(CTransform::STATE_RIGHT, &_vec3(matWorld.m[0][0], matWorld.m[0][1], matWorld.m[0][2]));
		m_pTransform->Set_State(CTransform::STATE_UP, &_vec3(matWorld.m[1][0], matWorld.m[1][1], matWorld.m[1][2]));
		m_pTransform->Set_State(CTransform::STATE_LOOK, &_vec3(matWorld.m[2][0], matWorld.m[2][1], matWorld.m[2][2]));
		m_pTransform->MovePosWithMyPos(&_vec3(m_matWorld.m[3][0], m_matWorld.m[3][1], m_matWorld.m[3][2]));

		m_bOnce = FALSE;
	}

	if (nullptr == m_pTransform)
		return -1;

	if (m_pMatWorld != nullptr)
	{
		Set_Transform(CEffect::POSITION, &m_vPos);
		m_pTransform->MovePosWithMyPos(&_vec3(m_pMatWorld->m[3][0], m_pMatWorld->m[3][1], m_pMatWorld->m[3][2]));
	}
	else
	{
		if (m_tEffectInfo.bIsTarget)
		{
			Set_Transform(CEffect::POSITION, &m_vPos);
			m_pTransform->MovePosWithMyPos(&_vec3(m_matWorld.m[3][0], m_matWorld.m[3][1], m_matWorld.m[3][2]));
		}
	}

	if (m_tEffectInfo.m_bUVStart || m_tEffectInfo.m_bRotStart || m_tEffectInfo.m_bScalingStart || m_tEffectInfo.m_bMoveStart)
	{
		m_tEffectInfo.m_fTime += (_float)fTimeDelta;
	}
	else
		return END_EVENT;

	if (m_tEffectInfo.m_bUV && m_tEffectInfo.m_bUVStart)
	{
		MoveTextureUV((_float)fTimeDelta);
	}
	if (m_tEffectInfo.m_bRot && m_tEffectInfo.m_bRotStart)
	{
		RotationEffect((_float)fTimeDelta);
	}
	if (m_tEffectInfo.m_bMove && m_tEffectInfo.m_bMoveStart)
	{
		MovePosEffect((_float)fTimeDelta);
	}
	if (m_tEffectInfo.m_bScaling && m_tEffectInfo.m_bScalingStart)
	{
		ScalingEffect((_float)fTimeDelta);
	}

	return NO_EVENT;
}

_int CMeshEffect::LateUpdate_GameObject(const _double & fTimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	Compute_ViewZ(m_pTransform);

	m_pRenderer->Add_RenderList(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CMeshEffect::Render_GameObject()
{
	if (!m_bRender[0] && !m_bRender[1] && !m_bRender[2])
		return E_FAIL;

	if (nullptr == m_pShader ||
		nullptr == m_pTransform ||
		nullptr == m_pMesh ||
		(nullptr == m_pTexture && nullptr == m_pFilter))
		return E_FAIL;

	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	if (nullptr == pEffect)
		return E_FAIL;

	pEffect->AddRef();

	pEffect->Begin(nullptr, 0);

	for (_uint i = 0; i < m_pMesh->Get_NumMaterials(); ++i)
	{
		if (FAILED(SetUp_ConstantTable(pEffect)))
			return E_FAIL;

		pEffect->SetBool("g_bMesh", TRUE);

		if (m_eRenderType & 1)
			pEffect->SetBool("g_bDistortion", TRUE);
		else
			pEffect->SetBool("g_bDistortion", FALSE);

		if (m_eRenderType & 2)
			pEffect->SetBool("g_bGlow", TRUE);
		else
			pEffect->SetBool("g_bGlow", FALSE);

		if (m_eRenderType & 4)
			pEffect->SetBool("g_bBlur", TRUE);
		else
			pEffect->SetBool("g_bBlur", FALSE);

		pEffect->CommitChanges();

		pEffect->BeginPass(m_tEffectInfo.m_iPassIdx + m_iPass);

		m_pMesh->Render_Mesh(i);

		pEffect->EndPass();
	}

	pEffect->End();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CMeshEffect::Add_Component()
{
	return NOERROR;
}

HRESULT CMeshEffect::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return E_FAIL;

	pEffect->AddRef();

	_matrix matWorld = *m_pTransform->Get_WorldMatrixPointer();

	pEffect->SetFloat("g_fMaxU", 1);
	pEffect->SetFloat("g_fMaxV", 1);
	pEffect->SetFloat("g_fU", 1);
	pEffect->SetFloat("g_fV", 1);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &CGameObject::Get_Transform(D3DTS_VIEW));
	pEffect->SetMatrix("g_matProj", &CGameObject::Get_Transform(D3DTS_PROJECTION));

	pEffect->SetFloat("g_fTime", m_fTime);

	if (m_pFilter != nullptr)
	{
		m_pFilter->SetUp_OnShader(pEffect, "g_FilterTexture");
		m_tEffectInfo.m_iPassIdx = 1;
	}
	if (m_pTexture != nullptr)
	{
		m_pTexture->SetUp_OnShader(pEffect, "g_BaseTexture1");
		m_tEffectInfo.m_iPassIdx = 2;
	}

	Safe_Release(pEffect);

	return NOERROR;
}

CMeshEffect * CMeshEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device, void * pArg)
{
	CMeshEffect* pInstance = new CMeshEffect(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMeshEffect::Clone_GameObject(void * pArg)
{
	CMeshEffect* pInstance = new CMeshEffect(*this);

	if (FAILED(pInstance->Ready_GameObject((_matrix*)pArg)))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshEffect::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pMesh);
	Safe_Release(m_pTexture);
	Safe_Release(m_pFilter);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);

	CEffect::Free();
}
