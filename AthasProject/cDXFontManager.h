#pragma once
#include "cSingletonBase.h"

class cDXFontManager : public cSingletonBase<cDXFontManager>
{
private:
	LPD3DXFONT			pDefultFont;		//디폴트 폰트
	LPD3DXSPRITE		pFontSprite;


private:
	cDXFontManager();
public:
	friend cDXFontManager* cSingletonBase::GetInstance();
	~cDXFontManager();

	HRESULT Init();
	void Release();

	void PrintText(std::string str, int x, int y, DWORD color);
	void PrintTextShadow(std::string str, int x, int y, DWORD color, DWORD shadow = 0xff000000);
	void PrintTextOutline(std::string str, int x, int y, DWORD color, DWORD outLine = 0xff000000);


	void SetSprite(LPD3DXSPRITE pSprite){
		this->pFontSprite = pSprite;
	}
};

#define DXFONT_MGR cDXFontManager::GetInstance()