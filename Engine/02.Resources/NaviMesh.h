#ifndef NaviMesh_h__
#define NaviMesh_h__

#include "Component.h"
#include "Cell.h"

BEGIN(ENGINE)

class ENGINE_DLL CNaviMesh : public CComponent
{
private:
	explicit CNaviMesh(LPDIRECT3DDEVICE9 pDevice);
	explicit CNaviMesh(const CNaviMesh& rhs);
	virtual ~CNaviMesh() = default;

public:
	void Set_CurrentIdx(_ulong dwIndex) { m_dwCurrentIdx = dwIndex; }

public:
	HRESULT Ready_Component_Prototype(void* pArg = nullptr);
	HRESULT Ready_NaviMesh(wstring strPath = L"", wstring strName = L"");
	void Render_NaviMesh();
	_bool Check_OnNaviMesh(const _vec3* pTargetPos, const _vec3* pTargetDir);
	_vec3 MoveOn_NaviMesh(const _vec3* pTargetPos, const _vec3* pTargetDir);
	_vec3 MoveOn_NaviMesh_Jump(const _vec3* pTargetPos, const _vec3* pTargetDir, const _float* fJump);
	_vec3 MoveOn_NaviMesh_Dir(const _vec3* pTargetPos, const _vec3* pTargetDir, _float* fY);
	_float MoveOn_Terrain(const _vec3* pTargetPos, const _vec3* pTargetDir);

private:
	HRESULT Load_NaviMesh(_uint iMax);
	HRESULT LoadforLineDat(wstring strPath, wstring strName);
	HRESULT Link_Cell();
	_float Compute_OnTerrain(const _vec3* pPos, _ulong * pCellIndex);
		
private:
	vector<CCell*> m_vecCell;
	_ulong m_dwCurrentIdx;

public:
	static CNaviMesh* Create(LPDIRECT3DDEVICE9 pDevice, wstring strPath = L"", wstring strName = L"");
	virtual CComponent* Clone_Component(void* pArg = nullptr);
	virtual void Free();

};

END

#endif // NaviMesh_h__
