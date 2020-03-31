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

		LPD3DXMESH					pOriMesh; // ������ ���¸� ������ �ִ� �޽�(���� ���ϸ� �ȵǴ� ���� �޽� ����)

		_ulong						dwNumBones;
		
		_matrix*					pFrameOffeSetMatrix; // ���� �迭�� �Ҵ��Ͽ� ����� �ּҰ� ������
														 // �ִϸ��̼��� ������ ���·� �������� �� �� ������ ������� ������ �ִ� ���

		_matrix**					ppFrameCombinedMatrix; 
		// ������ ������ �ִ� CombinedTransformationMatrix�� ��� ���ŵǱ� ������
		// �ƿ� �� CombinedTransformationMatrix����� �ּҸ� �����ؼ� �Ź� ���ŵ� ����� ������ ���� ����									

		_matrix*					pRenderingMatrix;


	}D3DXMESHCONTAINER_DERIVED;

}

#endif // Engine_Struct_h__
