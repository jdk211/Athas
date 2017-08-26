#include "stdafx.h"
#include "cDXFontManager.h"


cDXFontManager::cDXFontManager()
: pFontSprite( NULL )
{
}


cDXFontManager::~cDXFontManager()
{
}


HRESULT cDXFontManager::Init()
{
	//Default Font 생성
	if (FAILED(D3DXCreateFont(
		Device,					//디바이스를 넘긴다
		30.0f,					//폰트의 높이, 
		0.0f,				//폰트의 폭 ( 이값이 0 이면 앞에서 설정한 폰트 높이에 비례하여 원래 폰트가 가지고 있는 종횡비에 따라 가로 폭크기가 자동으로 맞춰진다 ) 	
		FW_BOLD,			//폰트의 두께 설정( FW_BOLD 폰트의 BOLD 스타일 크기를 같는다 )
		1,					//밉맵 ( 그냥 1  )
		FALSE,				//이텔릭 여부
		DEFAULT_CHARSET,	//캐릭터 Set ( 그냥 DEFAULT_CHARSET )
		OUT_DEFAULT_PRECIS,	//외각 처리방식 ( 그냥 OUT_DEFAULT_PRECIS )
		DEFAULT_QUALITY,			//퀄리티 ( 그냥 DEFAULT_QUALITY ),
		DEFAULT_PITCH | FF_DONTCARE,		//글시체 패밀리셋 설정 (그냥 DEFAULT_PITCH | FF_DONTCARE )
		"맑은 고딕",					//사용될 폰트이름 그냥 폰트이름을 스트링으로 쓰면 된다.
		&pDefultFont)))
	{
		return E_FAIL;
	}

	return S_OK;

}
void cDXFontManager::Release()
{
	SAFE_RELEASE(pDefultFont);
}

void cDXFontManager::PrintText(std::string str, int x, int y, DWORD color)
{
	RECT rc = { x, y, 0, 0 };

	pDefultFont->DrawTextA(
		this->pFontSprite,		//스프라이트 인스턴스  ( 일단 NULL )
		str.c_str() ,//문자열 주소
		-1,					//찍을 문자열 갯수 ( -1 이면 전체 )
		&rc,				//영역
		DT_LEFT | DT_NOCLIP,	//DT_NOCLIP 이옵션이 들어가면 사각형 영역에 구에 받지 않는다
		color);
}
void cDXFontManager::PrintTextShadow(std::string str, int x, int y, DWORD color, DWORD shadow)
{
	//쉐도우 1픽셀 우한 밀어서찍어
	PrintText(str, x + 1, y + 1, shadow);

	//본래 Text 를 찍는다.
	PrintText(str, x, y, color);
}
void cDXFontManager::PrintTextOutline(std::string str, int x, int y, DWORD color, DWORD outLine)
{
	//외각 찍는다
	PrintText(str, x + 1, y, outLine);
	PrintText(str, x, y + 1, outLine);
	PrintText(str, x - 1, y, outLine);
	PrintText(str, x, y - 1, outLine);

	//본래 Text 를 찍는다.
	PrintText(str, x, y, color);
}