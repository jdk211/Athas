#pragma once

// 8 텍스트뷰 만들기 전에 만드셔야함.

#include "cSingletonBase.h"

class cFontManager : public cSingletonBase<cFontManager>
{
public:
	enum eFontType
	{
		E_DEFAULT,
		E_QUEST,
	};

private:
	std::map<eFontType, LPD3DXFONT> m_mapFont;

public:
	LPD3DXFONT GetFont(eFontType e);
	void Destroy();
};

#define g_pFontManager cFontManager::GetInstance()
