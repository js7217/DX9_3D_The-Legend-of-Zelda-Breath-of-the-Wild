#ifndef MyMacro_h__
#define MyMacro_h__

#define  WINCX 1280
#define  WINCY 720

#define RESIZEX rc.right - rc.left
#define RESIZEY rc.bottom - rc.top

#define TERRAIN_VTX_X 129
#define TERRAIN_VTX_Z 129
#define TERRAIN_INTRV 1

#define NO_EVENT 0
#define END_EVENT -1

/////////////////////////////////////////////////////////

#define MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)

#define NULL_CHECK(_ptr)	\
{if(_ptr == nullptr) { __asm {int 3}; return; }}

#define NULL_CHECK_RETURN(_ptr, _return)	\
{if(_ptr == nullptr){__asm{int 3};return _return;}}

#define NULL_CHECK_MSG(_ptr, _message)	\
{if(_ptr == nullptr){MessageBox(NULL, _message, L"System Message", MB_OK);__asm {int 3};}}

#define NULL_CHECK_RETURN_MSG(_ptr, _return, _message)	\
{if(_ptr == nullptr){MessageBox(NULL, _message, L"System Message", MB_OK);__asm{int 3}; return _return;}}

#define  FAILED_CHECK(_hr)if(((HRESULT)(_hr)) != S_OK)	\
{MessageBoxW(NULL, L"Failed!", L"System Error!", MB_OK); __asm {int 3}; return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return) if((HRESULT)(_hr) != S_OK)	\
{MessageBoxW(NULL, L"Failed!", L"System Error!", MB_OK); __asm {int 3}; return _return;}

#define  FAILED_CHECK_MSG(_hr, _message)if(((HRESULT)(_hr)) != S_OK)	\
{MessageBoxW(NULL, _message, L"System Error!", MB_OK); __asm {int 3}; return E_FAIL;}

#define FAILED_CHECK_RETURN_MSG(_hr, _return, _message) if((HRESULT)(_hr) != S_OK)	\
{MessageBoxW(NULL, _message, L"System Error!", MB_OK); __asm {int 3}; return _return;}

/////////////////////////////////////////////////////

#define NO_COPY(ClassName)												\
private:																							\
	ClassName(const ClassName& obj);								\
	ClassName& operator=(const ClassName& obj);

#define DECLARE_SINGLETON(ClassName)							\
		NO_COPY(ClassName)														\
public:																							\
	static ClassName* Get_Instance(void)							\
	{																									\
		if(nullptr == m_pInstance)												\
			m_pInstance = new ClassName;									\
																										\
		return m_pInstance;															\
	}																									\
																										\
	void Destroy_Instance(void)												\
	{																									\
		if(nullptr != m_pInstance)												\
		{																								\
			delete m_pInstance;														\
			m_pInstance = nullptr;													\
		}																								\
	}																									\
																										\
private:																							\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)						\
ClassName* ClassName::m_pInstance = nullptr;

#endif // MyMacro_h__
