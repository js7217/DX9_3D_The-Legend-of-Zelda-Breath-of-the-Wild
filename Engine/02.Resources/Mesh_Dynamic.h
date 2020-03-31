#pragma once

#include "Component.h"

BEGIN(ENGINE)

class CAnimationCtrl;
class CHierarchyLoader;
class ENGINE_DLL CMesh_Dynamic final : public CComponent
{
private:
	explicit CMesh_Dynamic(LPDIRECT3DDEVICE9 pDevice);
	explicit CMesh_Dynamic(const CMesh_Dynamic& rhs);
	virtual ~CMesh_Dynamic() = default;
public:
	_uint Get_NumMeshContainer() {
		return _uint(m_MeshContainers.size());} 
	_uint Get_NumSubSet(_uint iMeshContainerID) {
		return m_MeshContainers[iMeshContainerID]->NumMaterials; }
	const D3DXFRAME_DERIVED* Get_FrameDesc(const char* pFrameName) {
		return (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pFrameName); }
	vector<D3DXMESHCONTAINER_DERIVED*>&	Get_MeshContainer() { return m_MeshContainers; }
	D3DXFRAME*	Get_RootFrame() { return m_pRootFrame; }
	CAnimationCtrl* Get_AniCtrl1() { return m_pAniCtrl; }
	CAnimationCtrl* Get_AniCtrl2() {return m_pAniCtrl2;	}

public:
	HRESULT Ready_Mesh_Dynamic(const _tchar* pFilePath, const _tchar* pFileName, _bool bTangent = FALSE);
	HRESULT Render_Mesh(_uint iMeshContainerID, _uint iSubSetID);
	HRESULT Update_SkinnedMesh(_uint iMeshContainerID);
	HRESULT SetTexture_OnShader(LPD3DXEFFECT pEffect, _uint iMeshContainerID, _uint iSubsetID, const char* pConstantName, MESHTEXTURE::TYPE eType);
	HRESULT SetUp_AnimationSet(_uint iIndex);
	HRESULT SetUp_AnimationSet_2(_uint iIndex);
	//HRESULT SetUp_AnimationSet(const char* pName);
	HRESULT Play_Animation(const _double& TimeDelta, _float fAdjSpeed = 1.f);
	HRESULT Play_Animation_2(const _double & TimeDelta, _float fAdjSpeed = 1.f);
	_bool	IsAnimationSetEnd(_float fInterpolatedValue = 0.0f);
	_bool	IsAnimationSetEnd_2(_float fInterpolatedValue = 0.0f);
	const	D3DXFRAME_DERIVED* Get_FrameByName(const char* pFrameName);
	const D3DXFRAME_DERIVED * Get_FrameByName_Root2(const char * pFrameName);
	void AnimationBoneSeparation(D3DXFRAME_DERIVED * frame, const char * bodyName);

private:
	CHierarchyLoader*		m_pLoader = nullptr;
	D3DXFRAME*				m_pRootFrame = nullptr;
	D3DXFRAME*				m_pRootFrame2 = nullptr;
	vector<D3DXMESHCONTAINER_DERIVED*>	m_MeshContainers;
	CAnimationCtrl*			m_pAniCtrl = nullptr;
	CAnimationCtrl*			m_pAniCtrl2 = nullptr;
	_matrix					m_matPivot;
	_bool					m_bIsTangent = FALSE;
private:
	HRESULT Update_CombinedTransformationMatrix(D3DXFRAME* pFrame, const D3DXMATRIX* pParentMatrix);
	HRESULT SetUp_CombinedMatrixPointer(D3DXFRAME* pFrame);

public:
	static CMesh_Dynamic* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pFilePath, const _tchar* pFileName, _bool bTangent = FALSE);
	virtual CComponent* Clone_Component(void* pArg = nullptr);
	virtual void Free();
};

END