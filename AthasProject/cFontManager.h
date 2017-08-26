#pragma once

// 8 �ؽ�Ʈ�� ����� ���� ����ž���.

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
