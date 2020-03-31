#include "Frustum.h"
#include "Transform.h"


CFrustum::CFrustum(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{

}

CFrustum::CFrustum(const CFrustum & rhs)
	: CComponent(rhs)
{
	memcpy(m_vPoint, rhs.m_vPoint, sizeof(_vec3) * 8);
	
}

HRESULT CFrustum::Ready_Frustum()
{
	// In.Projection Space
	m_vPoint[0] = _vec3(-10.f, 1.f, 0.f);
	m_vPoint[1] = _vec3(10.f, 1.f, 0.f);
	m_vPoint[2] = _vec3(10.f, -1.f, 0.f);
	m_vPoint[3] = _vec3(-10.f, -1.f, 0.f);

	m_vPoint[4] = _vec3(-10.f, 1.f, 1.f);
	m_vPoint[5] = _vec3(10.f, 1.f, 1.f);
	m_vPoint[6] = _vec3(10.f, -1.f, 1.f);
	m_vPoint[7] = _vec3(-10.f, -1.f, 1.f);

	//// 평면구성.(투영)
	//D3DXPlaneTransform()

	return NOERROR;
}

_bool CFrustum::WorldPt_InFrustum(const _vec3 * pWorldPoint, const CNxTransform * pTransform, _float fRadius)
{
	_vec3			vPoint[8];
	_vec3			vLocalPoint;

	_matrix WorldMatrix = *pTransform->Get_WorldMatrixPointer();

	D3DXVec3Normalize((_vec3*)&WorldMatrix.m[0][0], (_vec3*)&WorldMatrix.m[0][0]);
	D3DXVec3Normalize((_vec3*)&WorldMatrix.m[1][0], (_vec3*)&WorldMatrix.m[1][0]);
	D3DXVec3Normalize((_vec3*)&WorldMatrix.m[2][0], (_vec3*)&WorldMatrix.m[2][0]);
	

	D3DXMatrixInverse(&WorldMatrix, nullptr, &WorldMatrix);
	D3DXVec3TransformCoord(&vLocalPoint, pWorldPoint, &WorldMatrix);


	if (FAILED(Transform_ToLocal(pTransform, vPoint)))
		return false;

	D3DXPLANE		Plane[6];

	if (FAILED(Make_Plane(vPoint, Plane)))
		return false;

	for (size_t i = 0; i < 6; ++i)
	{
		// if(ax + by + cz + d > 0)
		//		return false;

		_float fDistance = D3DXPlaneDotCoord(&Plane[i], &vLocalPoint);

		if (fDistance  > fRadius)
			return false;		
	}

	return _bool(true);
}

_bool CFrustum::LocalPt_InFrustum(const _vec3 * pLocal, const CNxTransform * pTransform, _float fRadius)
{
	for (size_t i = 0; i < 6; ++i)
	{
		// if(ax + by + cz + d > 0)
		//		return false;

		_float fDistance = D3DXPlaneDotCoord(&m_Plane[i], pLocal);

		if (fDistance > fRadius)
			return false;
	}

	return _bool(true);
}

CFrustum* CFrustum::Make_LocalPlane(const CNxTransform * pTransform)
{
	_vec3			vPoint[8];

	// 투영스페이스 상에 있는 절두체를 구성하기위한 여덟개의 점을 로컬스페이스로 옮긴다.
	if (FAILED(Transform_ToLocal(pTransform, vPoint)))
		return nullptr;	

	// 로컬영역상의 평면을 구성한다.
	if (FAILED(Make_Plane(vPoint, m_Plane)))
		return nullptr;

	return this;
}

HRESULT CFrustum::Transform_ToLocal(const CNxTransform * pTransform, _vec3* pPointArray)
{
	if (nullptr == m_pDevice ||
		nullptr == pTransform)
		return E_FAIL;

	_matrix			matProj, matView;
	

	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, nullptr, &matProj);

	// 뷰스페이스로 변환했다.
	for (size_t i = 0; i < 8; ++i)	
		D3DXVec3TransformCoord(&pPointArray[i], &m_vPoint[i], &matProj);

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	// 월드스페이스로 변환했따.
	for (size_t i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&pPointArray[i], &pPointArray[i], &matView);



	_matrix WorldMatrix = *pTransform->Get_WorldMatrixPointer();

	D3DXVec3Normalize((_vec3*)&WorldMatrix.m[0][0], (_vec3*)&WorldMatrix.m[0][0]);
	D3DXVec3Normalize((_vec3*)&WorldMatrix.m[1][0], (_vec3*)&WorldMatrix.m[1][0]);
	D3DXVec3Normalize((_vec3*)&WorldMatrix.m[2][0], (_vec3*)&WorldMatrix.m[2][0]);


	D3DXMatrixInverse(&WorldMatrix, nullptr, &WorldMatrix);

	// 로컬스페이스로 변환했따.
	for (size_t i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&pPointArray[i], &pPointArray[i], &WorldMatrix);

	return NOERROR;
}

HRESULT CFrustum::Make_Plane(const _vec3 * pPoints, D3DXPLANE * pPlane)
{
	// +x
	D3DXPlaneFromPoints(&pPlane[0], &pPoints[1], &pPoints[5], &pPoints[6]);
	// -x
	D3DXPlaneFromPoints(&pPlane[1], &pPoints[4], &pPoints[0], &pPoints[3]);

	// +y
	D3DXPlaneFromPoints(&pPlane[2], &pPoints[4], &pPoints[5], &pPoints[1]);
	// -y
	D3DXPlaneFromPoints(&pPlane[3], &pPoints[3], &pPoints[2], &pPoints[6]);

	// +z
	D3DXPlaneFromPoints(&pPlane[4], &pPoints[5], &pPoints[4], &pPoints[7]);
	// -z
	D3DXPlaneFromPoints(&pPlane[5], &pPoints[0], &pPoints[1], &pPoints[2]);

	return NOERROR;

}

CFrustum * CFrustum::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFrustum* pInstance = new CFrustum(pGraphic_Device);

	if (FAILED(pInstance->Ready_Frustum()))
	{
		MSG_BOX("CFrustum Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CFrustum::Clone_Component(void * pArg)
{
	return new CFrustum(*this);
}

void CFrustum::Free()
{
	CComponent::Free();
}
