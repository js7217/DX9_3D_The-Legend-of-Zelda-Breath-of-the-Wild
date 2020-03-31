#include "Font.h"

ENGINE::CFont::CFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pSprite(nullptr)
	, m_pFont(nullptr)
{
	m_pGraphicDev->AddRef();
}

ENGINE::CFont::~CFont(void)
{

}

HRESULT ENGINE::CFont::Ready_Font(const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	D3DXFONT_DESC	Desc;
	ZeroMemory(&Desc, sizeof(D3DXFONT_DESC));

	Desc.CharSet = HANGUL_CHARSET;
	lstrcpy(Desc.FaceName, pFontType);
	Desc.Weight = iWeight;
	Desc.Height = iHeight;
	Desc.Width = iWidth;

	if (FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &Desc, &m_pFont)))
	{
		MSG_BOX("Font Com Create Failed");
		return E_FAIL;
	}

	if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite)))
	{
		MSG_BOX("Sprite Create Failed");
		return E_FAIL;
	}

	return S_OK;
}

void ENGINE::CFont::Render_Font(const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color)
{
	RECT	rc{ _long(pPos->x), _long(pPos->y) };

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pFont->DrawTextW(m_pSprite, pString, lstrlen(pString), &rc, DT_NOCLIP, Color);

	m_pSprite->End();
}

void ENGINE::CFont::Free(void)
{
	Safe_Release(m_pFont);
	Safe_Release(m_pSprite);
	Safe_Release(m_pGraphicDev);
}

CFont* ENGINE::CFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	CFont*		pInstance = new CFont(pGraphicDev);

	if (FAILED(pInstance->Ready_Font(pFontType, iWidth, iHeight, iWeight)))
	{
		MSG_BOX("Font Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

