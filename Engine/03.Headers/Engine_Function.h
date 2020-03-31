#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{
	template <typename T>
	DWORD Safe_Release(T& pointer)
	{
		DWORD	dwRefCnt = 0;
		if (NULL != pointer)
		{
			dwRefCnt = pointer->Release();
			if (dwRefCnt == 0)
				pointer = NULL;
		}
		return dwRefCnt;
	}

	template <typename T>
	void Safe_Single_Destory(T& pointer)
	{
		if (NULL != pointer)
		{
			pointer->DestroyInstance();
			pointer = NULL;
		}
	}

	template <typename T>
	void Safe_Delete(T& pointer)
	{
		if (NULL != pointer)
		{
			delete pointer;
			pointer = NULL;
		}
	}

	template <typename T>
	void Safe_Delete_Array(T& pointer)
	{
		if (NULL != pointer)
		{
			delete[] pointer;
			pointer = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////
	/////////////////////////////////Functor//////////////////////////

	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const _tchar* pTag)
			: m_pTargetTag(pTag) {		}
		~CTag_Finder() {		}
	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
			{
				return true;
			}

			return false;
		}

	private:
		const _tchar*		m_pTargetTag = nullptr;
	};

	class CDeleteObj
	{
	public:
		explicit CDeleteObj(void) {}
		~CDeleteObj(void) {}
	public: // operator
		template <typename T>
		void operator () (T& pInstance)
		{
			_ulong dwRefCnt = 0;

			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}
	};

	// 연관컨테이너 삭제용
	class CDeleteMap
	{
	public:
		explicit CDeleteMap(void) {}
		~CDeleteMap(void) {}
	public: // operator	
		template <typename T>
		void operator () (T& Pair)
		{
			_ulong dwRefCnt = 0;

			dwRefCnt = Pair.second->Release();

			if (0 == dwRefCnt)
				Pair.second = NULL;
		}
	};
}


#endif // Engine_Function_h__
