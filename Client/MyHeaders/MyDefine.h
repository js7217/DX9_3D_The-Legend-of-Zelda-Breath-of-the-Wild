#ifndef MyDefine_h__
#define MyDefine_h__

//Extern
extern HWND g_hWnd;
extern HINSTANCE g_hInst;

//Macro
#define  WINCX 1280
#define  WINCY 720

#define RESIZEX rc.right - rc.left
#define RESIZEY rc.bottom - rc.top

#define NO_EVENT 0
#define END_EVENT -1

//Enum
enum SCENEID
{
	SCENE_MESH_STATIC, SCENE_MESH_DYNAMIC, SCENE_COMPONENT, SCENE_TEXTURE, SCENE_GAMEOBJECT,
	SCENE_LOGO, SCENE_TESTSTAGE, SCENE_STAGE, SCENE_FIELD,
	SCENE_DUNGEON01, SCENE_DUNGEON02, SCENE_DUNGEON03, SCENE_DUNGEON04, SCENE_DUNGEON05,
	SCENE_END
};

enum SKILL_TYPE { BOMB_SPHERE, BOMB_SQUARE, ICECUBE, TIMESTOP, MAGNETIC, SKILL_END };


#endif // MyDefine_h__
