#include "AdvanceCamera.h"

#define NO_EVENT 0


CAdvanceCamera::CAdvanceCamera(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{
}

CAdvanceCamera::CAdvanceCamera(CAdvanceCamera & rhs)
	: CComponent(rhs), 
	m_bActivce(TRUE)
{
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matWorld);	

	ZeroMemory(&m_CamDesc, sizeof(CAMERA_DESC));

	m_CamDesc.fCamDist = 10.f;
	m_CamDesc.vUp = {0.f, 1.f, 0.f};
}

void CAdvanceCamera::Set_ActiveCamera(_bool bActive)
{
	m_bActivce = bActive;
}

void CAdvanceCamera::Set_Status(STATUS tStat)
{
	m_Status = tStat;
}

HRESULT CAdvanceCamera::Make_Camera()
{
	D3DXMatrixLookAtLH(&m_matView, &m_CamDesc.vEye, &m_CamDesc.vAt, &m_CamDesc.vUp);
	m_pDevice->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_CamDesc.fFovy, m_CamDesc.fAspect, m_CamDesc.fNear, m_CamDesc.fFar);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

void CAdvanceCamera::Make_ViewMatrix(const D3DXVECTOR3 * pEye, const D3DXVECTOR3 * pAt, const D3DXVECTOR3 * pUp)
{
	D3DXMatrixLookAtLH(&m_matView, pEye, pAt, pUp);
	m_pDevice->SetTransform(D3DTS_VIEW, &m_matView);
}

_int CAdvanceCamera::Update_Component(const _double & TimeDelta)
{
	if (m_bActivce)
	{
		m_CamDesc.vAt = Get_State(CAdvanceCamera::STATE_POSITION);
		m_CamDesc.vAt.y += 1.f;

		m_CamDesc.vEye = (m_CamDesc.vAt - Get_State(STATE_LOOK) * m_CamDesc.fCamDist);
		m_CamDesc.vEye.y += 3.f;

		Make_ViewMatrix(&m_CamDesc.vEye, &m_CamDesc.vAt, &m_CamDesc.vUp);
	}
	
	return NO_EVENT;
}

_int CAdvanceCamera::LateUpdate_Component(const _double & TimeDelta)
{

	return NO_EVENT;
}

HRESULT CAdvanceCamera::Ready_Component()
{
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matWorld);

	return S_OK;
}

HRESULT CAdvanceCamera::Ready_Status(void * pArg)
{
	m_CamDesc = *(CAMERADESC*)pArg;

	m_CamDesc.vAt = Get_State(STATE_POSITION);
	m_CamDesc.vAt.y += 1.f;

	m_CamDesc.vEye = (m_CamDesc.vAt - Get_State(STATE_LOOK) * m_CamDesc.fCamDist);
	m_CamDesc.vEye.y += 3.f;
	m_CamDesc.vUp = {0.f, 1.f, 0.f};

	if (FAILED(Make_Camera()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAdvanceCamera::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName)
{
	if (nullptr == pEffect)
		return E_FAIL;

	if (FAILED(pEffect->SetMatrix(pConstantName, &m_matWorld)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAdvanceCamera::Set_Scaling(const _vec3 * pScale)
{
	_vec3	vRight, vUp, vLook;

	vRight = *D3DXVec3Normalize(&vRight, &Get_State(STATE_RIGHT)) * pScale->x;
	vUp = *D3DXVec3Normalize(&vUp, &Get_State(STATE_UP)) * pScale->y;
	vLook = *D3DXVec3Normalize(&vLook, &Get_State(STATE_LOOK)) * pScale->z;

	Set_State(STATE_RIGHT, &vRight);
	Set_State(STATE_UP, &vUp);
	Set_State(STATE_LOOK, &vLook);

	return S_OK;
}

HRESULT CAdvanceCamera::Go_Straight(_double TimeDelta)
{
	_vec3	vPosition = Get_State(STATE_POSITION);
	_vec3	vLook = Get_State(STATE_LOOK);

	vPosition += *D3DXVec3Normalize(&vLook, &vLook) * _float(m_Status.MoveSpeed * TimeDelta);

	Set_State(STATE_POSITION, &vPosition);

	return S_OK;
}

HRESULT CAdvanceCamera::Go_BackWard(_double TimeDelta)
{
	_vec3	vPosition = Get_State(STATE_POSITION);
	_vec3	vLook = Get_State(STATE_LOOK);

	vPosition -= *D3DXVec3Normalize(&vLook, &vLook) * _float(m_Status.MoveSpeed * TimeDelta);

	Set_State(STATE_POSITION, &vPosition);

	return S_OK;
}

HRESULT CAdvanceCamera::Go_Left(_double TimeDelta)
{
	_vec3	vPosition = Get_State(STATE_POSITION);
	_vec3	vRight = Get_State(STATE_RIGHT);

	vPosition -= *D3DXVec3Normalize(&vRight, &vRight) *  _float(m_Status.MoveSpeed * TimeDelta);

	Set_State(STATE_POSITION, &vPosition);

	return S_OK;
}

HRESULT CAdvanceCamera::Go_Right(_double TimeDelta)
{
	_vec3	vPosition = Get_State(STATE_POSITION);
	_vec3	vRight = Get_State(STATE_RIGHT);

	vPosition += *D3DXVec3Normalize(&vRight, &vRight) * _float(m_Status.MoveSpeed * TimeDelta);

	Set_State(STATE_POSITION, &vPosition);

	return S_OK;
}

HRESULT CAdvanceCamera::Rotation_Axis(const _vec3 * pAxis, _double TimeDelta)
{
	_matrix		matRot;
	_vec3		vRight, vUp, vLook;

	D3DXMatrixRotationAxis(&matRot, pAxis, _float(m_Status.RotationSpeed * TimeDelta));

	D3DXVec3TransformNormal(&vRight, &Get_State(STATE_RIGHT), &matRot);
	D3DXVec3TransformNormal(&vUp, &Get_State(STATE_UP), &matRot);
	D3DXVec3TransformNormal(&vLook, &Get_State(STATE_LOOK), &matRot);

	Set_State(STATE_RIGHT, &vRight);
	Set_State(STATE_UP, &vUp);
	Set_State(STATE_LOOK, &vLook);

	return S_OK;
}


HRESULT CAdvanceCamera::Chase_Target(const CTransform * pTarget, _double TimeDelta)
{
	_vec3	vPosition = Get_State(STATE_POSITION);

	_vec3	vLook, vRight;
	D3DXVec3Normalize(&vLook, &(pTarget->Get_State(CTransform::STATE_POSITION) - vPosition));

	vPosition += vLook *  _float(m_Status.MoveSpeed * TimeDelta);

	Set_State(STATE_POSITION, &vPosition);

	vLook *= D3DXVec3Length(&Get_State(STATE_LOOK));

	_vec3		vUp(0.f, 1.f, 0.f);

	vUp *= D3DXVec3Length(&Get_State(STATE_UP));

	D3DXVec3Cross(&vRight, &vUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vLook, &vRight, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);

	vRight *= D3DXVec3Length(&Get_State(STATE_RIGHT));
	vLook *= D3DXVec3Length(&Get_State(STATE_LOOK));

	Set_State(STATE_RIGHT, &vRight);
	Set_State(STATE_UP, &vUp);
	Set_State(STATE_LOOK, &vLook);

	return S_OK;
}

CAdvanceCamera * CAdvanceCamera::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CAdvanceCamera* pInstance = new CAdvanceCamera(pDevice);

	if (FAILED(pInstance->Ready_Component()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CAdvanceCamera::Clone_Component(void * pArg)
{
	CAdvanceCamera* pInstance = new CAdvanceCamera(*this);

	if (FAILED(pInstance->Ready_Status(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CAdvanceCamera::Free()
{
	CComponent::Free();
}
