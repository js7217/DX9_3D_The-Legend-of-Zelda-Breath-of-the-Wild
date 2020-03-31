#ifndef GraphicDev_h__
#define GraphicDev_h__

#include "Base.h"

BEGIN(ENGINE)

class ENGINE_DLL CGraphicDev : public CBase
{
	DECLARE_SINGLETON(CGraphicDev)

public:
	enum	WINMODE { MODE_FULL, MODE_WIN };

private:
	explicit	CGraphicDev(void);
	virtual		~CGraphicDev(void) = default;

public:
	LPDIRECT3DDEVICE9		Get_Device(void) { return m_pGraphicDev;  }
	LPD3DXSPRITE					Get_Sprite(void) { return m_pSprite; }
	LPD3DXLINE						Get_Line(void) { return m_pLine; }


public:
	HRESULT		Ready_GraphicDev(HWND hWnd, 
								WINMODE eMode, 
								const _uint& iSizeX,
								const _uint& iSizeY,
								LPDIRECT3DDEVICE9* ppDevice);

	void		Render_Begin(D3DXCOLOR Color);
	void		Render_End(void);

private:
	LPDIRECT3D9				m_pSDK = nullptr;
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
	LPD3DXSPRITE						m_pSprite = nullptr;
	LPD3DXLINE							m_pLine = nullptr;

public:
	virtual void Free(void);
};

END
#endif // GraphicDev_h__
