#pragma once

#include "Engine_Defines.h"

// 레퍼런스 카운터를 보관한다. 레퍼런스카운트 : 참조갯수
// 카운터의 증, 감을 수행한다.
// 객체 삭제를 관리한다.

BEGIN(ENGINE)

class ENGINE_DLL CBase
{
protected:
	explicit CBase();
	virtual ~CBase() = default;
public:
	_ulong		AddRef(); // 레퍼런스 카운트를 증가시킨다.
	_ulong		Release(); // 레퍼런스 카운트를 감소시킨다. + 레퍼런스카운트가 0인경우 삭제한다.
private:
	_ulong			m_dwRefCnt = 0;
public:	
	virtual void Free() = 0;
};

END