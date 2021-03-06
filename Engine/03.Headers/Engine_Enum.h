#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	// ID_DYNAMIC : 실시간 업데이트가 필요한 컴포넌트
	enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

	enum BUFFERID { BUFFER_RCCOL, BUFFER_RCTEX, BUFFER_TRICOL, BUFFER_TERRAINTEX, BUFFER_CUBETEX };
	enum INFO	  { INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };
	enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };
	enum TEXTURETYPE { TEX_NORMAL, TEX_CUBE, TEX_END };
	enum RENDERID { RENDER_PRIORITY, RENDER_SHADOW, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };
	enum COLLTYPE { COL_FALSE, COL_TRUE, COL_END };
	enum MESHTYPE { TYPE_STATIC, TYPE_DYNAMIC, TYPE_NAVI };

}



#endif // Engine_Enum_h__
