#pragma once

#include "Engine_Defines.h"

// ���۷��� ī���͸� �����Ѵ�. ���۷���ī��Ʈ : ��������
// ī������ ��, ���� �����Ѵ�.
// ��ü ������ �����Ѵ�.

BEGIN(ENGINE)

class ENGINE_DLL CBase
{
protected:
	explicit CBase();
	virtual ~CBase() = default;
public:
	_ulong		AddRef(); // ���۷��� ī��Ʈ�� ������Ų��.
	_ulong		Release(); // ���۷��� ī��Ʈ�� ���ҽ�Ų��. + ���۷���ī��Ʈ�� 0�ΰ�� �����Ѵ�.
private:
	_ulong			m_dwRefCnt = 0;
public:	
	virtual void Free() = 0;
};

END