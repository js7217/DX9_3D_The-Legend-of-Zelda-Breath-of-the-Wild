#pragma once
#include "Component.h"


BEGIN(ENGINE)

class CShader;
class CGameObject;
class CTarget_Manager;
class ENGINE_DLL CRenderer final :	public CComponent
{
public:
	enum RENDERGROUP
	{
		RENDER_PRIORITY, RENDER_SHADOWDEPTH, RENDER_SHADOWDEPTH2, RENDER_WATER, RENDER_RIMLIGHT, RENDER_NONALPHA, RENDER_DIFFUSEALPHA, RENDER_ALPHA, RENDER_UI, RENDER_LOADING, RENDER_END
	};

private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CRenderer() = default;

public:
	LPDIRECT3DTEXTURE9 Get_FlowTexture() { return m_pFlowTarget; }
	LPDIRECT3DTEXTURE9 Get_NoiseTexture() { return m_pNoiseTarget; }
	void Get_Time(const _double& dTime) { m_dTime = dTime; }

public:
	//void Set_Field(_bool bCheck) { m_bField = bCheck; }
	//void Set_FieldColor(_vec4 vColor) { m_vFieldColor = vColor; }
	void Set_Radial(_bool bCheck) { m_bRadial = bCheck; }
	void Set_RadialStrength(_float fStrength) { m_fRadialStrength = fStrength; }
	void Set_Ray(_bool bCheck) { m_bRay = bCheck; }

public:
	void			Set_LightViewMatrix(const _matrix& matrixView) { m_matLightView = matrixView; }
	void			Set_LightProjMatrix(const _matrix& matrixProj) { m_matLightProj = matrixProj; }
	const _matrix&	Get_LightViewMatrix() { return m_matLightView; }
	const _matrix&	Get_LightProjMatrix() { return m_matLightProj; }

	void			Set_LightViewMatrix2(const _matrix& matrixView) { m_matLightView2 = matrixView; }
	void			Set_LightProjMatrix2(const _matrix& matrixProj) { m_matLightProj2 = matrixProj; }
	const _matrix&	Get_LightViewMatrix2() { return m_matLightView2; }
	const _matrix&	Get_LightProjMatrix2() { return m_matLightProj2; }

public:
	HRESULT Ready_Component();
	HRESULT Add_RenderList(RENDERGROUP eGroup, CGameObject* pGameObject);
	HRESULT Render_Renderer();

private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlpha();
	HRESULT Render_DiffuseAlpha();
	HRESULT Render_Alpha();
	HRESULT Render_DiffusePriority();
	HRESULT Render_UI();
	HRESULT Render_Loading();

//private:
//	HRESULT Render_Effect_Glow();
//	HRESULT Render_Effect_Blur();
//	HRESULT Render_Effect_Distortion();

private:
	HRESULT Render_Shadow();
	HRESULT Render_Water();
	HRESULT Render_StencilWater();
	HRESULT Render_RimLight();
	HRESULT Render_ShadowDepth();
	HRESULT Render_ShadowDepth2();
	HRESULT Render_Deferred();
	HRESULT Render_LightAcc();
	HRESULT Render_Effection();
	HRESULT Render_Blend();
	HRESULT Render_HDR();
	HRESULT	Render_Final();
	HRESULT Render_DOF();
	HRESULT Render_Blur();
	HRESULT Render_Ray();

private:
	CTarget_Manager*			m_pTarget_Manager = nullptr;
	CShader*					m_pShader_LightAcc = nullptr;
	CShader*					m_pShader_Blend = nullptr;
	CShader*					m_pShader_Effection = nullptr;
	CShader*					m_pShader_Shadow = nullptr;
	CShader*					m_pShader_Blur = nullptr;
	CShader*					m_pShader_Water = nullptr;
	CShader*					m_pShader_HDR = nullptr;
	CShader*					m_pShader_DOF = nullptr; 
	CShader*					m_pShader_Ray = nullptr;
	CShader*					m_pShader_Effect = nullptr;

private:
	_double						m_dTime = 0;
	_float						m_fUVTime = 0;
	_float						m_fDistortionTime = 0;
	_float						m_fReverse = 1.f;
	_matrix						m_matLightView;
	_matrix						m_matLightProj;
	_matrix						m_matLightView2;
	_matrix						m_matLightProj2;
	_bool						m_bRenderDebugBuffer = false;
	_bool						m_bRadial = false;
	_bool						m_bRay = false;
	_bool						m_bRayRender = false;
	_float						m_fRadialStrength = 0;
	//_bool						m_bField = FALSE;
	//_vec4						m_vFieldColor;
	//_float					m_fFlowTime = 0;

private:
	LPDIRECT3DVERTEXBUFFER9	m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9	m_pIB = nullptr;

	LPDIRECT3DTEXTURE9	m_pNoiseTarget = nullptr;
	LPDIRECT3DTEXTURE9	m_pFieldTarget = nullptr;
	LPDIRECT3DTEXTURE9	m_pTargetSSAO = nullptr;

	LPDIRECT3DTEXTURE9	m_pFlowTarget = nullptr;

	LPDIRECT3DTEXTURE9	m_pFoamTarget = nullptr;

	LPDIRECT3DTEXTURE9	m_pDistortionTarget = nullptr;
	LPDIRECT3DTEXTURE9	m_pDstrNormalTarget = nullptr;

	LPDIRECT3DSURFACE9	m_pShadowStencil = nullptr;
	LPDIRECT3DSURFACE9	m_pShadowStencil2 = nullptr;

private:
	list<CGameObject*>			m_RenderList[RENDER_END];
	typedef list<CGameObject*>	RENDERLIST;

public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent* Clone_Component(void* pArg);

private:
	void GetGaussianOffsets(_bool bHorizontal, const _vec2& vViewportTexelSie,
		_vec2 vSampleOffsets[], _float fSampleWeights[], _int nCount);
	_float GetGaussianDistribution(_float x, _float y, _float rho);

public:
	virtual void Free();

};

END