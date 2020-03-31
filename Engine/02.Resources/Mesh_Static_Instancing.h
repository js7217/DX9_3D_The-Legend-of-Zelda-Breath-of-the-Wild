#ifndef CMesh_Static_Instacing_h__
#define CMesh_Static_Instacing_h__

#include "Component.h"

BEGIN(ENGINE)

class ENGINE_DLL CMesh_Static_Instacing final : public CComponent
{
private:
	explicit CMesh_Static_Instacing(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMesh_Static_Instacing(const CMesh_Static_Instacing& rhs);
	virtual ~CMesh_Static_Instacing(void) = default;

public:
	_uint Get_NumMaterials() const {
		return m_dwNumMaterials;
	}
	//_ulong		Get_NumVtx(void) const { return m_dwNumVtx; }
	//_ulong		Get_Stride(void) const { return m_dwStride; }
	//const _vec3*	Get_VtxMeshPos(void) const { return m_pVtxPos; }
	LPD3DXMESH Get_Mesh();
	HRESULT Load_Instancing_Matrix(INSTANCING* pArg);
	HRESULT Insert_Instancing_Matrix(INSTANCING* pArg);
	HRESULT Modify_Instancing_Pos(_matrix* pMat);
	HRESULT Modify_Instancing_Scale(_matrix* pMat, _float fX=1, _float fY = 1, _float fZ = 1);
	HRESULT Modify_Instancing_Rot(_matrix* pMat, _vec3 vAngle);
public:
	HRESULT	Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName);
	HRESULT Change_TextureFileName(_tchar * pFileName, _tchar Text);
	HRESULT Ready_Instancing_Meshes(void* pArg);
	HRESULT SetTexture_OnShader(LPD3DXEFFECT pEffect, _uint iSubsetID, const char * pConstantName, MESHTEXTURE::TYPE eType);
	void	Render_Meshes(LPD3DXEFFECT pEffect);
	HRESULT Render_Mesh(LPD3DXEFFECT pEffect, _uint iSubSetID);
	_bool	Find_Alpha(const char* pFileName);

private:
	// 메시를 대표하는 객체.
	LPD3DXMESH				m_pMesh = nullptr;

	// 메시의 재질정보가 담긴다.
	LPD3DXBUFFER			m_pMaterials = nullptr;
	LPD3DXBUFFER			m_pAdjacency = nullptr;
	_ulong					m_dwNumMaterials = 0;
private: // 동적배열. 배열의 갯수 == 서브셋의 갯수.
	D3DXMATERIAL*			m_pSubsetMaterial = nullptr;
	MESHTEXTURE*			m_pMeshTexture = nullptr;
	INSTANCING				m_Instancing;
	//여기서부터 수정함 
	LPDIRECT3DVERTEXBUFFER9			m_pVBTransform;
	LPDIRECT3DVERTEXDECLARATION9	m_pDeclaration;

	LPDIRECT3DVERTEXBUFFER9			m_pVB = nullptr;
	_uint							m_iVtxSize = 0;
	_uint							m_iVtxSizeInst = 0;
	_uint							m_iNumVertices = 0;
	_ulong							m_dwVtxFVF = 0;
	_uint							m_iNumPolygons = 0;
	LPD3DXMESH						m_pOriMesh = nullptr;
	LPDIRECT3DINDEXBUFFER9			m_pIB = nullptr;
	_uint							m_iIndexSize = 0;
	_uint							m_iNumIndexies = 0;
	D3DXATTRIBUTERANGE*				m_pAttributeTable = nullptr;
	DWORD							m_AttributeSize = 0;
	DWORD*							m_pAdjBuffer = nullptr;
public:
	static CMesh_Static_Instacing*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void		Free(void);

};

END
#endif // StaticMesh_h__
