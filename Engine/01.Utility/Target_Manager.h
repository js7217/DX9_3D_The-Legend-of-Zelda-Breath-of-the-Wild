#pragma once

#include "Base.h"

BEGIN(ENGINE)

class CTarget;
class ENGINE_DLL CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	explicit CTarget_Manager();
	virtual ~CTarget_Manager() = default;
public:
	HRESULT Add_Target(LPDIRECT3DDEVICE9 pDevice, const _tchar* pTargetTag, _uint Width, _uint Height, D3DFORMAT Format, D3DXCOLOR ClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
	HRESULT Begin_MRT(const _tchar* pMRTTag, _int iIdx[] = nullptr, _bool bClear = FALSE);
	HRESULT End_MRT(const _tchar* pMRTTag);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName, const _tchar* pTargetTag);
	HRESULT Ready_Debug_Buffer(const _tchar* pTargetTag, _float fStartX, _float fStartY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_Buffer(const _tchar* pMRTTag);

private:
	map<const _tchar*, CTarget*>				m_Targets;
	typedef map<const _tchar*, CTarget*>		TARGETS;
private:
	map<const _tchar*, list<CTarget*>>			m_MRT;
	typedef map<const _tchar*, list<CTarget*>>	MRT;

private:
	CTarget*	Find_Target(const _tchar* pTargetTag);
	list<CTarget*>*	Find_MRT(const _tchar* pMRTTag);
public:
	virtual void Free();	
};

END