#pragma once

#include "Base.h"

#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "Transform.h"
#include "UTransform.h"
#include "NxTransform.h"
#include "Mesh_Static.h"
#include "Mesh_Static_Instancing.h"
#include "Mesh_Dynamic.h"
#include "Buffer_RcTex.h"
#include "Buffer_CubeTex.h"
#include "Buffer_PtTex.h"
#include "Buffer_Terrain.h"
#include "Buffer_Trail.h"
#include "Frustum.h"

BEGIN(ENGINE)

class CComponent;
class ENGINE_DLL CComponentManager :	public CBase
{
	DECLARE_SINGLETON(CComponentManager)
private:
	explicit CComponentManager();
	virtual ~CComponentManager() = default;
public:
	HRESULT Reserve_Component_Manager(_uint iNumScene);
	HRESULT Add_Component_Prototype(_uint iSceneID, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iSceneID, const _tchar* pPrototypeTag, void* pArg);
public:
	HRESULT Clear_Prototype(_uint iSceneID);
private:
	map<const _tchar*, CComponent*>*			m_pPrototype;
	typedef map<const _tchar*, CComponent*>		PROTOTYPES;
private:
	_uint		m_iNumScene;
private:
	CComponent* Find_Component(_uint iSceneID, const _tchar* pPrototypeTag);
public:
	virtual void Free();
};

END