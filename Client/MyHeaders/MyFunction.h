#ifndef MyFunction_h__
#define MyFunction_h__

template <typename T>
DWORD Safe_AddRef(T& pointer)
{
	DWORD	dwRefCnt = 0;

	if (NULL != pointer)
		dwRefCnt = pointer->AddRef();

	return dwRefCnt;
}

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

class CFinder_Tag
{
public:
	explicit CFinder_Tag(const _tchar* pTag)
		: m_pTargetTag(pTag) {		}
	~CFinder_Tag() {		}
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

#endif // MyFunction_h__
