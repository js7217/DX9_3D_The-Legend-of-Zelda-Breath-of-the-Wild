#pragma once

#include "DgnObj.h"
#include "NxTransform.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CMesh_Static;
}

class CBoxShutter : public CDgnObj
{
public:
	explicit CBoxShutter(LPDIRECT3DDEVICE9 pDevice);
	explicit CBoxShutter(const CBoxShutter& rhs);
	virtual ~CBoxShutter() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT			Set_MoveInit();
	HRESULT			Move_Shutter(const _double & TimeDelta);
private:
	CRenderer*				m_pRenderer = nullptr;
	CShader*					m_pShader = nullptr;
	CNxTransform*			m_pTransform = nullptr;
	CMesh_Static*			m_pMesh = nullptr;

	// √÷º“∞° 4.8
private:
	_double					m_dwStopTimer = 3.0;
	_double					m_dwPower = 30.0;

	NxVec3					m_vMoveLook;
	NxVec3					m_vMinPos;
	NxVec3					m_vMaxPos;
	_float						m_fMoveDist = 5.f;
	_bool						m_bIsUp = false;
private:
	HRESULT Add_Component(void * pArg = nullptr);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CDgnObj* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CDgnObj* Clone_GameObject(void* pArg);
	virtual void Free();



};

