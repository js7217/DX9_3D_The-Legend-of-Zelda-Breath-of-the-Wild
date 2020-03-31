#ifndef MyTypeDef_h__
#define MyTypeDef_h__

typedef bool		_bool;

typedef signed char			_char;
typedef signed char			_byte;
typedef unsigned char		_ubyte;

typedef wchar_t					_tchar;

typedef signed short			_short;
typedef unsigned short		_ushort;

typedef signed int				_int;
typedef unsigned int			_uint;

typedef signed long			_long;
typedef unsigned long		_ulong;

typedef float						_float;
typedef double						_double;

typedef	D3DXVECTOR2		_vec2;
//typedef	D3DXVECTOR3		_vec3;
typedef	D3DXVECTOR4		_vec4;

typedef D3DXMATRIX			_matrix;

typedef struct tagMyVector3
	: public D3DXVECTOR3
{
public:
	tagMyVector3() {};
	tagMyVector3(const _float fX, const _float fY, const _float fZ)
		: D3DXVECTOR3(fX, fY, fZ) {}
	tagMyVector3(const D3DXVECTOR3& vTemp)
		: D3DXVECTOR3(vTemp) {}

	D3DXVECTOR3 NewDir(const D3DXVECTOR3* vDir, const D3DXVECTOR3* vUp)
	{
		D3DXVECTOR3 vNewDir = *D3DXVec3Cross(&vNewDir, vDir, vUp);
		return *D3DXVec3Normalize(&vNewDir, &vNewDir);
	}
	D3DXVECTOR3 Reverse(const D3DXVECTOR3* vDir)
	{
		D3DXVECTOR3 vNewDir = *vDir * -1.f;
		return *D3DXVec3Normalize(&vNewDir, &vNewDir);
	}


}_vec3;

#endif // MyTypeDef_h__
