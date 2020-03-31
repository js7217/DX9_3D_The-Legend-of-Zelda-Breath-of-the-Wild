#pragma once

#include "MyDefine.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(ENGINE)
class CRenderer;
class CShader;
class CTexture;
class CTransform;
class CBuffer_CubeTex;
END

class CSkyBox final : public CGameObject
{
public:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pDevice);
	explicit CSkyBox(const CSkyBox& rhs);
	virtual ~CSkyBox() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CRenderer*			m_pRenderer = nullptr;
	CShader*			m_pShader = nullptr;
	CTexture*			m_pTexture = nullptr;
	CTransform*			m_pTransform = nullptr;
	CBuffer_CubeTex*	m_pBuffer = nullptr;

private:
	LPDIRECT3DTEXTURE9	m_pFilterTexture = nullptr;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT Get_CameraPos();

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};
