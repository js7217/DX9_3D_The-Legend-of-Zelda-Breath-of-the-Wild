#pragma once

#include "MyDefine.h"
#include "PhysXMgr.h"
#include "GameObject.h"
#include "Management.h"

namespace ENGINE
{
	class CRenderer;
	class CShader;
	class CTexture;
	class CTransform;
	class CBuffer_Terrain;
	class CFrustum;
}

class CTerrain :	public CGameObject
{
public:
	explicit CTerrain(LPDIRECT3DDEVICE9 pDevice);
	explicit CTerrain(const CTerrain & rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg); 
	virtual _int Update_GameObject(const _double & TimeDelta);
	virtual _int LateUpdate_GameObject(const _double & TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Load_SplattingMap();
	HRESULT Load_SplattingIdx();
	//HRESULT Create_PhysxPlane();

private:
	CRenderer*					m_pRenderer;
	CShader*					m_pShader;
	CTexture*					m_pTexture;
	CTexture*					m_pSplattingTexCom = nullptr; //스플래팅 텍스처를 툴에서 선택해서 원하는 색깔에 그릴수 있게 한다
	//CTexture*					m_pNormalTexCom = nullptr;
	CTransform*					m_pTransform;
	CBuffer_Terrain*			m_pBuffer;
	CFrustum*					m_pFrustumCom = nullptr;

	ENGINE::INDEX32*			m_pIndex = nullptr;
	_ulong						m_dwTriCnt = 0;

private:
	CPhysXMgr*					m_pPhysx;
	NxActor*					m_pActorPlane = nullptr;
	

private:
	LPDIRECT3DTEXTURE9	m_pFilterTexture;
	_uint m_iSplattingTexIdxR = 0;
	_uint m_iSplattingTexIdxG = 0;
	_uint m_iSplattingTexIdxB = 0;
private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT Ready_Splatting();

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

