#pragma once

#include "Component.h"
#include "Transform.h"
#include "UTransform.h"

BEGIN(ENGINE)

class CTransform;
class ENGINE_DLL CAdvanceCamera :	public CComponent
{
public:
	typedef struct tagStatus
	{
		_double	MoveSpeed;
		_double	RotationSpeed;
	}STATUS;

	enum STATE
	{
		STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION
	};

private:
	explicit CAdvanceCamera(LPDIRECT3DDEVICE9 pDevice);
	explicit CAdvanceCamera(CAdvanceCamera& rhs);
	virtual ~CAdvanceCamera() = default;

public:
	void Set_State(STATE eState, const _vec3* pState)
	{		memcpy(&m_matWorld.m[eState][0], pState, sizeof(_vec3));	}

	_vec3 Get_State(STATE eState) const
	{		return *((_vec3*)&m_matWorld.m[eState][0]);	}
	_matrix Get_Inverse_WorldMatrix()
	{		_matrix		matWorldInv;
		return *D3DXMatrixInverse(&matWorldInv, nullptr, &m_matWorld);	}

public:
	void Set_ActiveCamera(_bool bActive);
	void Set_Status(STATUS tStat);


public:
	virtual _int Update_Component(const _double& TimeDelta) override;
	virtual _int LateUpdate_Component(const _double & TimeDelta) override;

public:
	HRESULT Ready_Component();
	HRESULT Ready_Status(void* pArg);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName);

public:
	HRESULT Set_Scaling(const _vec3* pScale);
	HRESULT Go_Straight(_double TimeDelta);
	HRESULT Go_BackWard(_double TimeDelta);
	HRESULT Go_Left(_double TimeDelta);
	HRESULT Go_Right(_double TimeDelta);
	HRESULT Rotation_Axis(const _vec3* pAxis, _double TimeDelta);
	HRESULT Chase_Target(const CTransform* pTarget, _double TimeDelta);

private:
	HRESULT Make_Camera();

	void Make_ViewMatrix(
		const D3DXVECTOR3* pEye,
		const D3DXVECTOR3* pAt,
		const D3DXVECTOR3* pUp);
	
private:
	_double m_Time;
	_bool m_bActivce;

	//Transform
private:
	_matrix			m_matWorld;
	STATUS			m_Status;

	//Camera 
private:
	_matrix	 m_matView;
	_matrix	 m_matProj;

	CAMERADESC m_CamDesc;

public:
	static CAdvanceCamera* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();

};

END