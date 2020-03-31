#ifndef FontMgr_h__
#define FontMgr_h__

#include "Font.h"

BEGIN(ENGINE)

class ENGINE_DLL CFontMgr : public CBase
{
	DECLARE_SINGLETON(CFontMgr)

private:
	explicit CFontMgr(void);
	virtual ~CFontMgr(void);

public:
	HRESULT	Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev, 
						const _tchar* pFontTag,
						const _tchar* pFontType, 
						const _uint& iWidth,
						const _uint& iHeight, 
						const _uint& iWeight);

	void	Render_Font(const _tchar* pFontTag,
						const _tchar* pString,
						const _vec2* pPos,
						D3DXCOLOR Color);

private:
	CFont*		Find_Font(const _tchar* pFontTag);

private:
	map<const _tchar*, CFont*>			m_mapFont;
	
public:
	virtual void	Free(void);

};

END
#endif // FontMgr_h__
