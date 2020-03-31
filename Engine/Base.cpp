#include "Base.h"

CBase::CBase()
{
}

_ulong CBase::AddRef()
{
	return _ulong(++m_dwRefCnt);
}

unsigned long CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	
	return _ulong(m_dwRefCnt--);
}



