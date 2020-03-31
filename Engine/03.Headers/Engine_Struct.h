#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef	struct tagVertexColor
	{
		_vec3			vPos;
		_ulong			dwColor;

	}VTXCOL;

	const _ulong		FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	typedef	struct tagVertexTexture
	{
		_vec3			vPos;
		_vec3			vNormal;
		_vec2		 	vTexUV;

	}VTXTEX;

	const _ulong		FVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	typedef	struct tagVertexCubeTexture
	{
		_vec3			vPos;
		_vec3		 	vTex;

	}VTXCUBE;

	const _ulong		FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

	typedef struct tagScreenVertex
	{
		_vec4				vPos;
		_vec2				vTexUV;

	}VTXSCREEN;

	const _ulong		FVF_SCREEN = D3DFVF_XYZRHW | D3DFVF_TEX1;

	typedef	struct tagIndex16
	{
		_ushort	_0, _1, _2;

	}INDEX16;

	typedef	struct tagIndex32
	{
		_ulong	_0, _1, _2;

	}INDEX32;


	typedef struct D3DXFRAME_DERIVED : public D3DXFRAME
	{
		_matrix				CombinedTransformMatrix;

	}D3DXFRAME_DERIVED;

	typedef struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER
	{
		LPDIRECT3DTEXTURE9*			ppTexture;

		LPD3DXMESH					pOriMesh; // 최초의 상태를 가지고 있는 메시(절대 변하면 안되는 원본 메쉬 상태)

		_ulong						dwNumBones;
		
		_matrix*					pFrameOffeSetMatrix; // 동적 배열로 할당하여 행렬의 주소값 보관용
														 // 애니메이션을 포함한 형태로 렌더링을 할 때 최초의 뼈대들이 가지고 있는 행렬

		_matrix**					ppFrameCombinedMatrix; 
		// 뼈들이 가지고 있는 CombinedTransformationMatrix가 계속 갱신되기 때문에
		// 아예 그 CombinedTransformationMatrix행렬의 주소를 보관해서 매번 갱신된 행렬의 정보를 얻어올 예정									

		_matrix*					pRenderingMatrix;


	}D3DXMESHCONTAINER_DERIVED;

}

#endif // Engine_Struct_h__
