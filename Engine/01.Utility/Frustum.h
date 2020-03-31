#pragma once

#include "Component.h"
#include "NxTransform.h"

BEGIN(ENGINE)

class CTransform;


class ENGINE_DLL CFrustum final : public CComponent
{
private:
	explicit CFrustum(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CFrustum(const CFrustum& rhs);
	virtual ~CFrustum() = default;
public:
	HRESULT Ready_Frustum();

	// ���念���� �����ϴ� ��ü�� ��ġ�������� �ø�.(���念��)
	_bool WorldPt_InFrustum(const _vec3* pWorldPoint, const CNxTransform* pTransform, _float fRadius = 0.f);

	// �������ۻ� �����ϴ� ������ ��ġ�������� �ø�.(���ÿ���)
	_bool LocalPt_InFrustum(const _vec3* pLocal, const CNxTransform* pTransform, _float fRadius = 0.f);
	CFrustum* Make_LocalPlane(const CNxTransform* pTransform);
private:
	_vec3			m_vPoint[8];
	D3DXPLANE		m_Plane[6];
private:
	HRESULT Transform_ToLocal(const CNxTransform* pTransform, _vec3* pPointArray);
	HRESULT Make_Plane(const _vec3* pPoints, D3DXPLANE* pPlane);
public:
	static CFrustum* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg = nullptr);
	virtual void Free();
};

END