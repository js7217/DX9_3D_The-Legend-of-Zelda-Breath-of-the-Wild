#include "Renderer.h"
#include "GameObject.h"
#include "Shader.h"
#include "Target_Manager.h"
#include "LightManager.h"
#include "FrameMgr.h"


CRenderer::CRenderer(LPDIRECT3DDEVICE9 pDevice)
	:CComponent(pDevice),
	m_pTarget_Manager(GET_INSTANCE(CTarget_Manager))
{
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::Ready_Component()
{
	for (int i = 0; i < RENDER_END; ++i)
	{
		m_RenderList[i].clear();
	}

	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	D3DVIEWPORT9		ViewPort;

	m_pDevice->GetViewport(&ViewPort);

	// For.Target_Diffuse
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Diffuse", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For.Target_Diffuse
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_DiffusePriority", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For.Target_Shadow_View
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Shadow_View", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For.Target_Shadow_Blur
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Shadow_Blur", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For.Target_Shadow_Blur2
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Shadow_Blur2", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For.Target_Normal
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Normal", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_RimLightSpecular
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_RimLightSpecular", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0, 0, 0, 0.f))))
		return E_FAIL;

	// For.Target_Emessive
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Emessive", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0, 0, 0, 0.f))))
		return E_FAIL;

	///
	// For.Target_Water
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Water", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(2.f, 6.f, 8.f, 0.f))))
		return E_FAIL;

	// For.Target_WaterDepth
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_WaterDepth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For.Target_WaterNormal
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_WaterNormal", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_WaterEmessive
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_WaterEmessive", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0, 0, 0, 0.f))))
		return E_FAIL;

	// For.Target_DepthInWater
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_DepthInWater", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	///
	// For.Target_Depth
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Depth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Shade
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Shade", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Fog
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Fog", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For.Target_Specular
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Specular", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For.Target_Cartoon
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Cartoon", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	///

	// For.Target_HDR
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_HDR", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_HDR_X
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_HDR_X", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_HDR_Y
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_HDR_Y", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	///
	// For.Target_DiffuseBlur
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_DiffuseBlur", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	// For.Target_EmessiveBlur
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_EmessiveBlur", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// For.Target_BeforHDR
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_BeforHDR", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	///

	// For.Target_Final
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Final", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	///

	// For.Target_Befor_DOF
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Befor_DOF", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.2f, 0.4f, 0.7f, 0.f))))
		return E_FAIL;

	// For.Target_DOF_Blur
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_DOF_BlurX", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.2f, 0.4f, 0.7f, 0.f))))
		return E_FAIL;

	// For.Target_DOF_Blur
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_DOF_BlurXY", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.2f, 0.4f, 0.7f, 0.f))))
		return E_FAIL;

	///
	
	// For_Target_Blur_X
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Blur_X", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	// For_Target_Blur_Y
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Blur_Y", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	_int iSize = 4096;

	// For.Target_Shadow
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Shadow", iSize, iSize, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilSurface(iSize, iSize, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pShadowStencil, NULL)))
		return E_FAIL;

	// For.Target_Shadow2
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Shadow2", iSize, iSize, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilSurface(iSize, iSize, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pShadowStencil2, NULL)))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.Target_Priority
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Priority", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.Target_Distortion
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Distortion", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.Target_Ray
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Ray", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_RayBlur
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_RayBlur", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.Target_Effect
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Effect", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Effect_Effection
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Effect_Effection", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Effect_Distortion
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Effect_Distortion", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// For.Target_Effect_Final (Blur, Glow)
	if (FAILED(m_pTarget_Manager->Add_Target(m_pDevice, L"Target_Effect_Final", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	//for.MRT_EmessiveBlur
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_EmessiveBlur", L"Target_EmessiveBlur")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	//for.MRT_Shadow
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Shadow", L"Target_Shadow_View")))
		return E_FAIL;
	
	//////////////////////////////////////////////////////////////////////////

	// for.MRT_Effect
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Effect", L"Target_Effect")))
		return E_FAIL;

	// for.MRT_Effect
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Effect", L"Target_Effect_Effection")))
		return E_FAIL;

	// for.MRT_Effect_Distortion
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Effect_Distortion", L"Target_Effect_Distortion")))
		return E_FAIL;

	// for.MRT_Effect_Final
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Effect_Final", L"Target_Effect_Final")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	//for.MRT_RimLightSpecular
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_RimLightSpecular", L"Target_RimLightSpecular")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	//for.MRT_ShadowDepth
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_ShadowDepth", L"Target_Shadow")))
		return E_FAIL;

	//for.MRT_ShadowDepth2
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_ShadowDepth2", L"Target_Shadow2")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.MRT_Deferred
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Depth")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Emessive")))
		return E_FAIL;

	// For.MRT_BlendPriority
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_BlendPriority", L"Target_DiffusePriority")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.MRT_WaterDeferred
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_WaterDeferred", L"Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_WaterDeferred", L"Target_WaterNormal")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_WaterDeferred", L"Target_WaterDepth")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_WaterDeferred", L"Target_WaterEmessive")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.MRT_LightAcc
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Specular")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Cartoon")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_DepthInWater")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.MRT_Ray
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Ray", L"Target_Ray")))
		return E_FAIL;

	// For.MRT_RayBlur
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_RayBlur", L"Target_RayBlur")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.MRT_Water
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Water", L"Target_Water")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.MRT_Effection
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Effection", L"Target_DiffuseBlur")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Effection", L"Target_Fog")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.MRT_BeforHDR
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_BeforHDR", L"Target_BeforHDR")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.MRT_Final
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Final", L"Target_Final")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	//For.MRT_Blur_X
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Blur_X", L"Target_Blur_X")))
		return E_FAIL;
	//For.MRT_Blur_Y
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Blur_Y", L"Target_Blur_Y")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For_MRT_HDR
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_HDR", L"Target_HDR")))
		return E_FAIL;

	// For.MRT_HDR_X
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_HDR_X", L"Target_HDR_X")))
		return E_FAIL;

	// For.MRT_HDR_Y
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_HDR_Y", L"Target_HDR_Y")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For_MRT_Priority
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Priority", L"Target_Priority")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.MRT_Befor_DOF
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Befor_DOF", L"Target_Befor_DOF")))
		return E_FAIL;

	// For.MRT_DOF_BlurX
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_DOF_BlurX", L"Target_DOF_BlurX")))
		return E_FAIL;

	// For.MRT_DOF_BlurXY
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_DOF_BlurXY", L"Target_DOF_BlurXY")))
		return E_FAIL;

	// For.MRT_DOF
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_DepthWater", L"Target_DepthInWater")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.MRT_Distortion_DOF
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Distortion_DOF", L"Target_Distortion")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	// For.Shader_Stencil_Water
	m_pShader_Water = CShader::Create(m_pDevice, L"../ShaderFiles/Shader_Stencil_Water.fx");
	if (nullptr == m_pShader_Water)
		return E_FAIL;

	// For.Shader_LightAcc
	m_pShader_LightAcc = CShader::Create(m_pDevice, L"../ShaderFiles/Shader_LightAcc.fx");
	if (nullptr == m_pShader_LightAcc)
		return E_FAIL;

	// For.Shader_Blend
	m_pShader_Blend = CShader::Create(m_pDevice, L"../ShaderFiles/Shader_Blend.fx");
	if (nullptr == m_pShader_Blend)
		return E_FAIL;

	// For.Shader_Effection
	m_pShader_Effection = CShader::Create(m_pDevice, L"../ShaderFiles/Shader_Effection.fx");
	if (nullptr == m_pShader_Effection)
		return E_FAIL;

	// For.Shader_Shadow
	m_pShader_Shadow = CShader::Create(m_pDevice, L"../ShaderFiles/Shader_Shadow.fx");
	if (nullptr == m_pShader_Shadow)
		return E_FAIL;

	// For.Shader_Blur
	m_pShader_Blur = CShader::Create(m_pDevice, L"../ShaderFiles/Shader_Blur.fx");
	if (nullptr == m_pShader_Blur)
		return E_FAIL;

	// For.Shader_HDR
	m_pShader_HDR = CShader::Create(m_pDevice, L"../ShaderFiles/Shader_HDR.fx");
	if (nullptr == m_pShader_HDR)
		return E_FAIL;

	// For.Shader_DOF
	m_pShader_DOF = CShader::Create(m_pDevice, L"../ShaderFiles/Shader_DOF.fx");
	if (nullptr == m_pShader_DOF)
		return E_FAIL;

	// For.Shader_Ray
	m_pShader_Ray = CShader::Create(m_pDevice, L"../ShaderFiles/Shader_Ray.fx");
	if (nullptr == m_pShader_Ray)
		return E_FAIL;

	// For.Shader_Effect
	m_pShader_Effect = CShader::Create(m_pDevice, L"../ShaderFiles/Shader_Effect.fx");
	if (nullptr == m_pShader_Effect)
		return E_FAIL;

	D3DXCreateTextureFromFileA(m_pDevice, "..\\..\\Resources\\waterfoam.png", &m_pFoamTarget);

	D3DXCreateTextureFromFileA(m_pDevice, "..\\..\\Resources\\Texture\\FIELD.png", &m_pFieldTarget);

	D3DXCreateTextureFromFileA(m_pDevice, "..\\..\\Resources\\BlackPoint.png", &m_pDistortionTarget);
	D3DXCreateTextureFromFileA(m_pDevice, "..\\..\\Resources\\Distortion2.png", &m_pDstrNormalTarget);

	D3DXCreateTextureFromFileA(m_pDevice, "..\\..\\Resources\\Flow.png", &m_pFlowTarget);

	D3DXCreateTextureFromFileA(m_pDevice, "..\\..\\Resources\\flownoisemap.png", &m_pNoiseTarget);

	if (FAILED(m_pDevice->CreateVertexBuffer(sizeof(VTXSCREEN) * 4, 0, FVF_SCREEN, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXSCREEN*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _vec4(0.f - 0.5f, 0.f - 0.5f, 0.0f, 1.f);
	pVertices[0].vTexUV = _vec2(0.0f, 0.f);

	pVertices[1].vPosition = _vec4(ViewPort.Width - 0.5f, 0.0f - 0.5f, 0.0f, 1.f);
	pVertices[1].vTexUV = _vec2(1.0f, 0.f);

	pVertices[2].vPosition = _vec4(ViewPort.Width - 0.5f, ViewPort.Height - 0.5f, 0.0f, 1.f);
	pVertices[2].vTexUV = _vec2(1.0f, 1.f);

	pVertices[3].vPosition = _vec4(0.0f - 0.5f, ViewPort.Height - 0.5f, 0.0f, 1.f);
	pVertices[3].vTexUV = _vec2(0.0f, 1.f);

	m_pVB->Unlock();

	if (FAILED(m_pDevice->CreateIndexBuffer(sizeof(POLYGON16) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	POLYGON16*		pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();

	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Diffuse", 0.f, 150.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Normal", 0.f, 225.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Depth", 0.f, 300.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Cartoon", 0.f, 375.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_RimLightSpecular", 0.f, 450.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shadow_Blur", 0.f, 525.f, 75.f, 75.f);
	//
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shade", 75.f, 150.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Specular", 75.f, 225.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shadow", 75.f, 300.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shadow2", 75.f, 375.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shadow_View", 75.f, 450.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shadow_Blur2", 75.f, 525.f, 75.f, 75.f);
	//
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Water", 150.f, 150.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_WaterNormal", 150.f, 225.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_WaterDepth", 150.f, 300.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_WaterEmessive", 150.f, 375.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_DepthInWater", 150.f, 450.f, 75.f, 75.f);
	//
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Distortion", 225.f, 150.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Ray", 225.f, 225.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_RayBlur", 225.f, 300.f, 75.f, 75.f);
	//
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Effect", 300.f, 150.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Effect_Effection", 300.f, 225.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Effect_Final", 300.f, 300.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Effect_Distortion", 300.f, 375.f, 75.f, 75.f);
	//
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Fog", 1130.f, 75.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_EmessiveBlur", 1130.f, 150.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_DiffusePriority", 1130.f, 225.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_BeforHDR", 1130.f, 300.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Blur_X", 1130.f, 375.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Blur_Y", 1130.f, 450.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_DOF_BlurX", 1130.f, 525.f, 75.f, 75.f);
	//
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_DiffuseBlur", 1205.f, 75.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Emessive", 1205.f, 150.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Final", 1205.f, 225.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_HDR_Y", 1205.f, 300.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Priority", 1205.f, 375.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Befor_DOF", 1205.f, 450.f, 75.f, 75.f);
	m_pTarget_Manager->Ready_Debug_Buffer(L"Target_DOF_BlurXY", 1205.f, 525.f, 75.f, 75.f);
	//




	return NOERROR;
}

HRESULT CRenderer::Add_RenderList(RENDERGROUP eGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject || RENDER_END <= eGroup)
		return E_FAIL;

	m_RenderList[eGroup].emplace_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Render_Renderer()
{
	m_fUVTime += (_float)m_dTime * m_fReverse;
	m_fDistortionTime += (_float)m_dTime * 0.1f;

	if (m_fDistortionTime >= 1)
	{
		m_fDistortionTime = 0;
	}

	if (m_fUVTime >= 1)
	{
		m_fUVTime = 1;
		m_fReverse = -0.05f;
	}
	if (m_fUVTime <= 0.5)
	{
		m_fUVTime = 0.5;
		m_fReverse = 0.05f;
	}

	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_ShadowDepth()))
		return E_FAIL;

	if (FAILED(Render_ShadowDepth2()))
		return E_FAIL;

	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (FAILED(Render_Water()))
		return E_FAIL;

	if (FAILED(Render_DiffusePriority()))
		return E_FAIL;

	if (FAILED(Render_RimLight()))
		return E_FAIL;

	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	if (FAILED(Render_Shadow()))
		return E_FAIL;

	if (FAILED(Render_Alpha()))
		return E_FAIL;

	if (FAILED(Render_Effection()))
		return E_FAIL;
	
	if (FAILED(Render_Blur()))
		return E_FAIL;

	if (FAILED(Render_Ray()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_HDR()))
		return E_FAIL;
	//
	if (FAILED(Render_Final()))
		return E_FAIL;
	//
	if (FAILED(Render_StencilWater()))
		return E_FAIL;
	//
	if (FAILED(Render_DOF()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

	if (FAILED(Render_Loading()))
		return E_FAIL;

	//if (GetAsyncKeyState(VK_F7) & 0x0001)
	//{
	//	m_bRenderDebugBuffer = m_bRenderDebugBuffer ? FALSE : TRUE;
	//}

	//if (m_bRenderDebugBuffer)
	//{
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_ShadowDepth");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_ShadowDepth2");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Deferred");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_RimLightSpecular");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_LightAcc");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Effection");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_EmessiveBlur");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Final");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Shadow");
	//	//m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Shadow_Blur");
	//	//m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Shadow_Blur2");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Blur_X");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Blur_Y");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_BlendPriority");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_HDR_Y");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_BeforHDR");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Priority");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Befor_DOF");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_DOF_BlurX");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_DOF_BlurXY");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_DepthWater");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_WaterDeferred");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Distortion_DOF");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Water");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Ray");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_RayBlur");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Effect");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Effect_Final");
	//	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Effect_Distortion");
	//}
	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Priority")))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_PRIORITY].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Priority")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_NonAlpha()
{
	for (auto& pGameObject : m_RenderList[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_NONALPHA].clear();

	return NOERROR;
}

HRESULT CRenderer::Render_DiffuseAlpha()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_DIFFUSEALPHA])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_DIFFUSEALPHA].clear();

	return NOERROR;
}

HRESULT CRenderer::Render_Alpha()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Effect")))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_ALPHA].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Effect")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	LPD3DXEFFECT pEffect = m_pShader_Effect->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Effect_Final")))
		return E_FAIL;
	
	Safe_AddRef(pEffect);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_EffectTexture", L"Target_Effect")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_EffectEffectionTexture", L"Target_Effect_Effection")))
		return E_FAIL;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Effect_Final")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_DiffusePriority()
{
	// Diffuse * Shade 를 해서 백버퍼에 그리겠다.
	LPD3DXEFFECT pEffect = m_pShader_Blend->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_BlendPriority")))
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DiffuseTexture", L"Target_Diffuse")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_PriorityTexture", L"Target_Priority")))
		return E_FAIL;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(1);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_BlendPriority")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderList[RENDER_UI])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_UI].clear();

	return NOERROR;
}

HRESULT CRenderer::Render_Loading()
{
	for (auto& pGameObject : m_RenderList[RENDER_LOADING])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_LOADING].clear();

	return NOERROR;
}

//HRESULT CRenderer::Render_Effect_Glow()
//{
//	return E_NOTIMPL;
//}
//
//HRESULT CRenderer::Render_Effect_Blur()
//{
//	return E_NOTIMPL;
//}
//
//HRESULT CRenderer::Render_Effect_Distortion()
//{
//	return E_NOTIMPL;
//}

HRESULT CRenderer::Render_Shadow()
{
	// Diffuse * Shade 를 해서 백버퍼에 그리겠다.
	LPD3DXEFFECT pEffect = m_pShader_Shadow->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Shadow")))
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DepthTexture", L"Target_Depth")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_ShadowDepthTexture", L"Target_Shadow")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_NormalTexture", L"Target_Normal")))
		return E_FAIL;

	pEffect->SetMatrix("g_matLightView", &m_matLightView);
	pEffect->SetMatrix("g_matLightProj", &m_matLightProj);

	_matrix		matViewInv, matProjInv;

	m_pDevice->GetTransform(D3DTS_VIEW, &matViewInv);
	D3DXMatrixInverse(&matViewInv, nullptr, &matViewInv);

	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProjInv);
	D3DXMatrixInverse(&matProjInv, nullptr, &matProjInv);

	pEffect->SetMatrix("g_matViewInv", &matViewInv);
	pEffect->SetMatrix("g_matProjInv", &matProjInv);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Shadow")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_Water()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	_int ClearPassThisTarget = 0;

	// 필요한 타겟들을 장치에 셋한다.
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_WaterDeferred", &ClearPassThisTarget)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_WATER])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_GameObject();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_WATER].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_WaterDeferred")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_StencilWater()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	LPD3DXEFFECT pEffect = m_pShader_Water->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	// 필요한 타겟들을 장치에 셋한다.
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Water")))
		return E_FAIL;

	Safe_AddRef(pEffect);

	// g_DepthTexture
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DepthTexture", L"Target_Depth")))
		return E_FAIL;

	// g_DepthOfWater
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DepthOfWater", L"Target_DepthInWater")))
		return E_FAIL;

	// g_ReverseTarget
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_ReverseTarget", L"Target_Befor_DOF")))
		return E_FAIL;

	// g_WaterNormal
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_WaterNormal", L"Target_WaterNormal")))
		return E_FAIL;

	// g_WaterDepth
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_WaterDepth", L"Target_WaterDepth")))
		return E_FAIL;

	if (FAILED(pEffect->SetTexture("g_FoamTexture", m_pFoamTarget)))
		return E_FAIL;

	if (FAILED(pEffect->SetFloat("g_fTime", (_float)m_fUVTime)))
		return E_FAIL;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Water")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_RimLight()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	// 필요한 타겟들을 장치에 셋한다.
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_RimLightSpecular")))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_RIMLIGHT])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_RimLight();
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_RIMLIGHT].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_RimLightSpecular")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_ShadowDepth()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	LPDIRECT3DSURFACE9 pOrigin = nullptr;

	m_pDevice->GetDepthStencilSurface(&pOrigin);

	// 필요한 타겟들을 장치에 셋한다.
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_ShadowDepth")))
		return E_FAIL;

	m_pDevice->SetDepthStencilSurface(m_pShadowStencil);
	m_pDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DXCOLOR(0, 0, 1, 1), 1, 0);

	for (auto& pGameObject : m_RenderList[RENDER_SHADOWDEPTH])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_Shadow();
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_SHADOWDEPTH].clear();

	// 원상 복구한다
	m_pDevice->SetDepthStencilSurface(pOrigin);
	Safe_Release(pOrigin);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_ShadowDepth")))
		return E_FAIL;


	return NOERROR;
}

HRESULT CRenderer::Render_ShadowDepth2()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	LPDIRECT3DSURFACE9 pOrigin = nullptr;

	m_pDevice->GetDepthStencilSurface(&pOrigin);

	// 필요한 타겟들을 장치에 셋한다.
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_ShadowDepth2")))
		return E_FAIL;

	m_pDevice->SetDepthStencilSurface(m_pShadowStencil2);
	m_pDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DXCOLOR(0, 0, 1, 1), 1, 0);

	for (auto& pGameObject : m_RenderList[RENDER_SHADOWDEPTH2])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render_Shadow(2);
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_SHADOWDEPTH2].clear();

	// 원상 복구한다
	m_pDevice->SetDepthStencilSurface(pOrigin);
	Safe_Release(pOrigin);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_ShadowDepth2")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_Deferred()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	// 필요한 타겟들을 장치에 셋한다.
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Deferred")))
		return E_FAIL;

	if (FAILED(Render_NonAlpha()))
		return E_FAIL;

	if (FAILED(Render_DiffuseAlpha()))
		return E_FAIL;

	// 원상 복구한다
	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Deferred")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (nullptr == m_pShader_LightAcc)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_LightAcc")))
		return E_FAIL;

	LPD3DXEFFECT pEffect = m_pShader_LightAcc->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_NormalTexture", L"Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DepthTexture", L"Target_Depth")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_WaterNormalTexture", L"Target_WaterNormal")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_WaterDepthTexture", L"Target_WaterDepth")))
		return E_FAIL;

	_matrix		matViewInv, matProjInv;

	m_pDevice->GetTransform(D3DTS_VIEW, &matViewInv);
	D3DXMatrixInverse(&matViewInv, nullptr, &matViewInv);

	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProjInv);
	D3DXMatrixInverse(&matProjInv, nullptr, &matProjInv);

	pEffect->SetMatrix("g_matViewInv", &matViewInv);
	pEffect->SetMatrix("g_matProjInv", &matProjInv);

	pEffect->SetVector("g_vCamPosition", (_vec4*)&matViewInv.m[3][0]);

	// 셰이더비긴
	pEffect->Begin(nullptr, 0);

	GET_INSTANCE(CLightManager)->Render_Light_Manager(pEffect);

	// 셰이더ㅇ렌드
	pEffect->End();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_LightAcc")))
		return E_FAIL;

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CRenderer::Render_Effection()
{
	if (nullptr == m_pShader_Effection)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Effection")))
		return E_FAIL;

	LPD3DXEFFECT pEffect = m_pShader_Effection->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	Safe_AddRef(pEffect);

	// DiffuseMap 블러처리.
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "Sampler0", L"Target_DiffusePriority")))
		return E_FAIL;

	// 가시거리 표현하는 안개 효과
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "Sampler1", L"Target_Depth")))
		return E_FAIL;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Effection")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////////////////////

	if (nullptr == m_pShader_Effection)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_EmessiveBlur")))
		return E_FAIL;

	LPD3DXEFFECT pEffect2 = m_pShader_Effection->Get_EffectHandle();
	if (nullptr == pEffect2)
		return E_FAIL;

	Safe_AddRef(pEffect2);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect2, "Sampler0", L"Target_Emessive")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect2, "DepthTexture", L"Target_Depth")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect2, "NormalTexture", L"Target_Normal")))
		return E_FAIL;

	//pEffect->SetTexture("NoiseTexture", m_pNoiseTarget);
	//pEffect->SetTexture("g_FlowTexture", m_pFlowTarget);
	//pEffect->SetBool("g_bField", m_bField);
	//pEffect->SetBool("g_bField", TRUE);
	//pEffect->SetVector("g_vFieldColor", &m_vFieldColor);
	//pEffect->SetVector("g_vFieldColor", &_vec4(1, 0, 0, 1));
	//m_fFlowTime += (_float)m_dTime;
	//pEffect->SetFloat("g_fTime", m_fFlowTime);

	pEffect2->Begin(nullptr, 0);
	pEffect2->BeginPass(1);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect2->EndPass();
	pEffect2->End();

	Safe_Release(pEffect2);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_EmessiveBlur")))
		return E_FAIL;

	////////////////////////////////////////////////////////////////////////////////////////////

	if (nullptr == m_pShader_Effection)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_BeforHDR")))
		return E_FAIL;

	pEffect2 = m_pShader_Effection->Get_EffectHandle();
	if (nullptr == pEffect2)
		return E_FAIL;

	Safe_AddRef(pEffect2);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect2, "Sampler0", L"Target_EmessiveBlur")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect2, "Sampler1", L"Target_DiffuseBlur")))
		return E_FAIL;

	pEffect2->Begin(nullptr, 0);
	pEffect2->BeginPass(6);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect2->EndPass();
	pEffect2->End();

	Safe_Release(pEffect2);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_BeforHDR")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_Blend()
{
	// Diffuse * Shade 를 해서 백버퍼에 그리겠다.
	LPD3DXEFFECT pEffect = m_pShader_Blend->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Final")))
		return E_FAIL;

	Safe_AddRef(pEffect);

	//if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_PriorityTexture", L"Target_Priority")))
	//	return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DiffuseTexture", L"Target_DiffusePriority")))
		return E_FAIL;

	// DiffuseMap 의 물의 깊이를 추가하기위한 타겟
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DepthInWater", L"Target_DepthInWater")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_ShadeTexture", L"Target_Cartoon")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_SpecularTexture", L"Target_Specular")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_EmessiveTexture", L"Target_EmessiveBlur")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DepthTexture", L"Target_Depth")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_ShadowTexture", L"Target_Shadow_View")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_FogTexture", L"Target_Fog")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_GodRay", L"Target_RayBlur")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	pEffect->SetTexture("g_FieldTexture", m_pFieldTarget);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_NormalTexture", L"Target_Normal")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	pEffect->SetMatrix("g_matLightView", &m_matLightView);
	pEffect->SetMatrix("g_matLightProj", &m_matLightProj);

	if (GetAsyncKeyState(VK_F8) & 0x0001)
	{
		m_bRayRender = m_bRayRender ? FALSE : TRUE;
	}
	if (m_bRayRender)
		pEffect->SetBool("g_bDungeon", m_bRay);
	else
		pEffect->SetBool("g_bDungeon", m_bRayRender);
	_matrix		matViewInv, matProjInv;

	m_pDevice->GetTransform(D3DTS_VIEW, &matViewInv);
	D3DXMatrixInverse(&matViewInv, nullptr, &matViewInv);

	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProjInv);
	D3DXMatrixInverse(&matProjInv, nullptr, &matProjInv);

	pEffect->SetMatrix("g_matViewInv", &matViewInv);
	pEffect->SetMatrix("g_matProjInv", &matProjInv);

	//////////////////////////////////////////////////////////////////////////

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_RimLightTexture", L"Target_RimLightSpecular")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_ShadowDepthTexture2", L"Target_Shadow2")))
		return E_FAIL;

	pEffect->SetMatrix("g_matLightView2", &m_matLightView2);
	pEffect->SetMatrix("g_matLightProj2", &m_matLightProj2);

	//////////////////////////////////////////////////////////////////////////

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Final")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_HDR()
{
	if (nullptr == m_pShader_HDR ||
		nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_HDR")))
		return E_FAIL;


	LPD3DXEFFECT pEffect = m_pShader_HDR->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_TextureBase", L"Target_BeforHDR")))
		return E_FAIL;

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_HDR")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	if (nullptr == m_pShader_Blur ||
		nullptr == m_pTarget_Manager)
		return E_FAIL;

	D3DVIEWPORT9		ViewPort;

	m_pDevice->GetViewport(&ViewPort);

	_int nSampleCount = 31;
	_vec2	vSampleOffsets[31];
	_float		fSampleWeights[31];

	GetGaussianOffsets(true, D3DXVECTOR2(1.0f / (_float)ViewPort.Width, 1.0f / (_float)ViewPort.Height), vSampleOffsets, fSampleWeights, nSampleCount);

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_HDR_X")))
		return E_FAIL;

	pEffect = m_pShader_Blur->Get_EffectHandle();

	if (nullptr == pEffect)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_texOcclusionMap", L"Target_HDR")))
		return E_FAIL;

	pEffect->SetValue("g_vSampleOffsets2", vSampleOffsets, nSampleCount * sizeof(_vec2));
	pEffect->SetValue("g_fSampleWeights2", fSampleWeights, nSampleCount * sizeof(_float));
	pEffect->SetInt("g_Count", nSampleCount);


	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(1);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_HDR_X")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////////////

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_HDR_Y")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_texOcclusionMap", L"Target_HDR_X")))
		return E_FAIL;

	GetGaussianOffsets(false, D3DXVECTOR2(1.0f / (_float)ViewPort.Width, 1.0f / (_float)ViewPort.Height), vSampleOffsets, fSampleWeights, nSampleCount);

	pEffect->SetValue("g_vSampleOffsets2", vSampleOffsets, nSampleCount * sizeof(_vec2));
	pEffect->SetValue("g_fSampleWeights2", fSampleWeights, nSampleCount * sizeof(_float));
	pEffect->SetInt("g_Count", nSampleCount);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(1);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_HDR_Y")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_HDR_X")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_texOcclusionMap", L"Target_HDR_Y")))
		return E_FAIL;

	GetGaussianOffsets(true, D3DXVECTOR2(1.0f / (_float)ViewPort.Width, 1.0f / (_float)ViewPort.Height), vSampleOffsets, fSampleWeights, nSampleCount);

	pEffect->SetValue("g_vSampleOffsets2", vSampleOffsets, nSampleCount * sizeof(_vec2));
	pEffect->SetValue("g_fSampleWeights2", fSampleWeights, nSampleCount * sizeof(_float));
	pEffect->SetInt("g_Count", nSampleCount);


	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(1);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_HDR_X")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_HDR_Y")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_texOcclusionMap", L"Target_HDR_X")))
		return E_FAIL;

	GetGaussianOffsets(false, D3DXVECTOR2(1.0f / (_float)ViewPort.Width, 1.0f / (_float)ViewPort.Height), vSampleOffsets, fSampleWeights, nSampleCount);

	pEffect->SetValue("g_vSampleOffsets2", vSampleOffsets, nSampleCount * sizeof(_vec2));
	pEffect->SetValue("g_fSampleWeights2", fSampleWeights, nSampleCount * sizeof(_float));
	pEffect->SetInt("g_Count", nSampleCount);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(1);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_HDR_Y")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CRenderer::Render_Final()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Distortion_DOF")))
		return E_FAIL;

	LPD3DXEFFECT pEffect2 = m_pShader_Effection->Get_EffectHandle();
	if (nullptr == pEffect2)
		return E_FAIL;

	Safe_AddRef(pEffect2);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect2, "Sampler0", L"Target_Final")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect2, "Sampler1", L"Target_WaterNormal")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect2, "Sampler2", L"Target_DepthInWater")))
		return E_FAIL;

	pEffect2->Begin(nullptr, 0);
	pEffect2->BeginPass(7);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect2->EndPass();
	pEffect2->End();

	Safe_Release(pEffect2);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Distortion_DOF")))
		return E_FAIL;

	///////////////////////////////////////////////////////////////////////////

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Effect_Distortion")))
		return E_FAIL;

	LPD3DXEFFECT pEffect3 = m_pShader_Effect->Get_EffectHandle();
	if (nullptr == pEffect3)
		return E_FAIL;

	Safe_AddRef(pEffect3);

	pEffect3->SetTexture("g_DistortionTexture", m_pDstrNormalTarget);
	pEffect3->SetFloat("g_fTime", m_fDistortionTime);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect3, "g_BaseTexture", L"Target_Distortion")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect3, "g_EffectEffectionTexture", L"Target_Effect_Effection")))
		return E_FAIL;

	pEffect3->Begin(nullptr, 0);
	pEffect3->BeginPass(1);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect3->EndPass();
	pEffect3->End();

	Safe_Release(pEffect3);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Effect_Distortion")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Befor_DOF")))
		return E_FAIL;

	LPD3DXEFFECT pEffect = m_pShader_Effection->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "Sampler0", L"Target_Effect_Distortion")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "HDR", L"Target_HDR_Y")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "Effect", L"Target_Effect_Final")))
		return E_FAIL;

	pEffect->SetBool("bTest", TRUE);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(4);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Befor_DOF")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////


	return NOERROR;
}

HRESULT CRenderer::Render_DOF()
{
	if (nullptr == m_pShader_Blur ||
		nullptr == m_pTarget_Manager)
		return E_FAIL;

	D3DVIEWPORT9		ViewPort;

	m_pDevice->GetViewport(&ViewPort);

	_int nSampleCount = 15;
	_vec2	vSampleOffsets[15];
	_float		fSampleWeights[15];

	GetGaussianOffsets(true, D3DXVECTOR2(1.0f / (_float)ViewPort.Width, 1.0f / (_float)ViewPort.Height), vSampleOffsets, fSampleWeights, nSampleCount);

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_DOF_BlurX")))
		return E_FAIL;

	LPD3DXEFFECT pEffect = m_pShader_Blur->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_texOcclusionMap", L"Target_Water")))
		return E_FAIL;

	pEffect->SetValue("g_vSampleOffsets", vSampleOffsets, nSampleCount * sizeof(_vec2));
	pEffect->SetValue("g_fSampleWeights", fSampleWeights, nSampleCount * sizeof(_float));
	pEffect->SetInt("g_Count", nSampleCount);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_DOF_BlurX")))
		return E_FAIL;
	//////////////////////////////////////////////////////////////////////////////////

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_DOF_BlurXY")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_texOcclusionMap", L"Target_DOF_BlurX")))
		return E_FAIL;

	GetGaussianOffsets(false, D3DXVECTOR2(1.0f / (_float)ViewPort.Width, 1.0f / (_float)ViewPort.Height), vSampleOffsets, fSampleWeights, nSampleCount);

	pEffect->SetValue("g_vSampleOffsets", vSampleOffsets, nSampleCount * sizeof(_vec2));
	pEffect->SetValue("g_fSampleWeights", fSampleWeights, nSampleCount * sizeof(_float));
	pEffect->SetInt("g_Count", nSampleCount);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_DOF_BlurXY")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	LPD3DXEFFECT pEffect2 = m_pShader_DOF->Get_EffectHandle();
	if (nullptr == pEffect2)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect2, "g_Depth", L"Target_Depth")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect2, "g_Blur", L"Target_DOF_BlurXY")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect2, "g_TextureBase", L"Target_Water")))
		return E_FAIL;


	pEffect2->SetBool("g_bRadial", m_bRadial);
	pEffect2->SetFloat("RadialStrength", m_fRadialStrength);

	pEffect2->Begin(nullptr, 0);
	pEffect2->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect2->EndPass();
	pEffect2->End();

	return NOERROR;
}

HRESULT CRenderer::Render_Blur()
{
	if (nullptr == m_pShader_Blur ||
		nullptr == m_pTarget_Manager)
		return E_FAIL;

	D3DVIEWPORT9		ViewPort;

	m_pDevice->GetViewport(&ViewPort);

	_int nSampleCount = 15;
	_vec2	vSampleOffsets[15];
	_float		fSampleWeights[15];

	GetGaussianOffsets(true, D3DXVECTOR2(1.0f / (_float)ViewPort.Width, 1.0f / (_float)ViewPort.Height), vSampleOffsets, fSampleWeights, nSampleCount);

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Blur_X")))
		return E_FAIL;


	LPD3DXEFFECT pEffect = m_pShader_Blur->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_texOcclusionMap", L"Target_Shadow_View")))
		return E_FAIL;

	pEffect->SetValue("g_vSampleOffsets", vSampleOffsets, nSampleCount * sizeof(_vec2));
	pEffect->SetValue("g_fSampleWeights", fSampleWeights, nSampleCount * sizeof(_float));
	pEffect->SetInt("g_Count", nSampleCount);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Blur_X")))
		return E_FAIL;
	//////////////////////////////////////////////////////////////////////////////////

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Blur_Y")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_texOcclusionMap", L"Target_Blur_X")))
		return E_FAIL;

	GetGaussianOffsets(false, D3DXVECTOR2(1.0f / (_float)ViewPort.Width, 1.0f / (_float)ViewPort.Height), vSampleOffsets, fSampleWeights, nSampleCount);

	pEffect->SetValue("g_vSampleOffsets", vSampleOffsets, nSampleCount * sizeof(_vec2));
	pEffect->SetValue("g_fSampleWeights", fSampleWeights, nSampleCount * sizeof(_float));
	pEffect->SetInt("g_Count", nSampleCount);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Blur_Y")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_Ray()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Ray")))
		return E_FAIL;

	LPD3DXEFFECT pEffect = m_pShader_Ray->Get_EffectHandle();
	if (nullptr == pEffect)
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_DepthTexture", L"Target_Depth")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_ShadowDepthTexture2", L"Target_Shadow2")))
		return E_FAIL;

	pEffect->SetMatrix("g_matLightView2", &m_matLightView2);
	pEffect->SetMatrix("g_matLightProj2", &m_matLightProj2);

	_matrix		matViewInv, matProjInv, matLightInv;

	D3DXMatrixInverse(&matLightInv, nullptr, &m_matLightView2);
	pEffect->SetMatrix("g_matLightViewInv", &matLightInv);

	m_pDevice->GetTransform(D3DTS_VIEW, &matViewInv);
	D3DXMatrixInverse(&matViewInv, nullptr, &matViewInv);

	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProjInv);
	D3DXMatrixInverse(&matProjInv, nullptr, &matProjInv);

	pEffect->SetMatrix("g_matViewInv", &matViewInv);
	pEffect->SetMatrix("g_matProjInv", &matProjInv);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Ray")))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_RayBlur")))
		return E_FAIL;

	Safe_AddRef(pEffect);

	if (FAILED(m_pTarget_Manager->SetUp_OnShader(pEffect, "g_LightTexture", L"Target_Ray")))
		return E_FAIL;

	_matrix matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	pEffect->SetMatrix("g_matView", &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("RadialStrength", 20.f);
	pEffect->SetFloat("RadialPosX", -0.7f);
	pEffect->SetFloat("RadialPosY", -1.f);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(1);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	m_pDevice->SetFVF(FVF_SCREEN);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_RayBlur")))
		return E_FAIL;

	return NOERROR;
}

CRenderer * CRenderer::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CRenderer* pIntance = new CRenderer(pDevice);

	if (FAILED(pIntance->Ready_Component()))
		Safe_Release(pIntance);

	return pIntance;
}

CComponent * CRenderer::Clone_Component(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::GetGaussianOffsets(_bool bHorizontal, const _vec2 & vViewportTexelSie, _vec2 vSampleOffsets[], _float fSampleWeights[], _int nCount)
{
	_float fValue = 10.f;

	vSampleOffsets[0] = _vec2(0.0f, 0.0f);
	fSampleWeights[0] = 1.0f * GetGaussianDistribution(vSampleOffsets[0].x, vSampleOffsets[0].y, 2.f);

	if (bHorizontal)
	{
		for (_int i = 1; i < nCount; i += 2)
		{
			vSampleOffsets[i + 0] = _vec2(i * vViewportTexelSie.x, 0.0f);
			vSampleOffsets[i + 1] = _vec2(-i * vViewportTexelSie.x, 0.0f);

			fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution(_float(i + 0), 0.0f, fValue);
			fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution(_float(i + 1), 0.0f, fValue);
		}
	}

	else
	{
		for (_int i = 1; i < nCount; i += 2)
		{
			vSampleOffsets[i + 0] = _vec2(0.0, i * vViewportTexelSie.x);
			vSampleOffsets[i + 1] = _vec2(0.0, -i * vViewportTexelSie.x);

			fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution(0.0f, _float(i + 0), fValue);
			fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution(0.0f, _float(i + 1), fValue);
		}
	}
}

_float CRenderer::GetGaussianDistribution(_float x, _float y, _float rho)
{
	_float g = 1.0f / sqrt(2.0f * D3DX_PI * rho * rho);

	return g * exp(-(x * x + y * y) / (2.0f * rho * rho));
}

void CRenderer::Free()
{
	Safe_Release(m_pShader_LightAcc);
	Safe_Release(m_pShader_Blend);
	Safe_Release(m_pShader_Effection);
	Safe_Release(m_pShader_Shadow);
	Safe_Release(m_pShader_Blur);
	Safe_Release(m_pShader_Water);
	Safe_Release(m_pShader_HDR);
	Safe_Release(m_pShader_DOF);
	Safe_Release(m_pShader_Ray);
	Safe_Release(m_pShader_Effect);

	Safe_Release(m_pTarget_Manager);

	Safe_Release(m_pFlowTarget);
	Safe_Release(m_pFieldTarget);
	Safe_Release(m_pNoiseTarget);

	Safe_Release(m_pFoamTarget);

	Safe_Release(m_pDistortionTarget);
	Safe_Release(m_pDstrNormalTarget);

	for (size_t i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderList[i])
		{
			Safe_Release(pGameObject);
		}
		m_RenderList[i].clear();
	}

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
	Safe_Release(m_pShadowStencil);
	Safe_Release(m_pShadowStencil2);

	CComponent::Free();
}

