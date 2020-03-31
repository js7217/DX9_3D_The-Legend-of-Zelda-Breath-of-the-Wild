#include "stdafx.h"
#include "SkyBox.h"


CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
	
}

CSkyBox::CSkyBox(const CSkyBox & rhs)
	: CGameObject(rhs)
{
}


// 원형객체의 초기화.
HRESULT CSkyBox::Ready_GameObject_Prototype()
{
	
	return NOERROR;
}

// 복제객체의 초기화.
HRESULT CSkyBox::Ready_GameObject(void* pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransform->Set_Scale(&_vec3(5.f, 5.f, 5.f));

	return NOERROR;
}

_int CSkyBox::Update_GameObject(const _double & TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_pTransform->Rotation_Y(D3DXToRadian(1.f), (_float)TimeDelta);

	return _int();
}

_int CSkyBox::LateUpdate_GameObject(const _double & TimeDelta)
{
	CGameObject::LateUpdate_GameObject(TimeDelta);
	if (nullptr == m_pRenderer)
		return -1;

	Get_CameraPos();

	if (FAILED(m_pRenderer->Add_RenderList(CRenderer::RENDER_PRIORITY, this)))
		return END_EVENT;

	return _int();
}

HRESULT CSkyBox::Render_GameObject()
{
	if (nullptr == m_pShader || 		nullptr == m_pBuffer)
		return E_FAIL;


	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	if (nullptr == pEffect)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(SetUp_ConstantTable(pEffect)))
		return E_FAIL;

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	m_pBuffer->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CSkyBox::Add_Component()
{	
	// For.Com_Transform
	CTransform::STATUS			Status;
	Status.MoveSpeed = 1.f;
	Status.RotationSpeed = D3DXToRadian(90.0f);	

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransform, &Status)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;	

	// For.Com_Buffer
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Buffer_CubeTex", L"Com_Buffer", (CComponent**)&m_pBuffer)))
		return E_FAIL;
	
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_COMPONENT, L"Component_Shader_SkyBox", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;
	
	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_TEXTURE, L"Component_Texture_SkyBox", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;	

	return NOERROR;
}

HRESULT CSkyBox::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect ||
		nullptr == m_pTexture ||
		nullptr == m_pTransform)
		return E_FAIL;

	Safe_AddRef(pEffect);	

	if (FAILED(m_pTransform->SetUp_OnShader(pEffect, "g_matWorld")))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matView", &Get_Transform(D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix("g_matProj", &Get_Transform(D3DTS_PROJECTION))))
		return E_FAIL;
	
	if (FAILED(m_pTexture->SetUp_OnShader(pEffect, "g_BaseTexture")))
		return E_FAIL;	
	
	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CSkyBox::Get_CameraPos()
{
	_matrix matView = CGameObject::Get_Transform(D3DTS_VIEW);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	_vec3 vCamPos;
	memcpy(vCamPos, matView.m[3], sizeof(_vec3));

	m_pTransform->Set_State(CTransform::STATE_POSITION, &vCamPos);

	return S_OK;
}

CGameObject * CSkyBox::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGameObject*	pInstance = new CSkyBox(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed to Create CGameObject Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkyBox::Clone_GameObject(void * pArg)
{
	CGameObject*	pInstance = new CSkyBox(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed to Create CGameObject Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyBox::Free()
{
	Safe_Release(m_pFilterTexture);

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pBuffer);

	CGameObject::Free();
}
