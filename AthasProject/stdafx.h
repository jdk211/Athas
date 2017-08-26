// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include <Windows.h>
#include <cmath>
#include <string>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment( lib, "d3d9.lib")
#pragma comment( lib, "d3dx9.lib")

//
// 매크로 상수
//
#define WINSIZE_X	1200		//초기 윈도우 가로 크기
#define WINSIZE_Y	800			//초기 윈도우 세로 크기
#define WINPOS_X	0					//초기 윈도우 시작 위치 X
#define WINPOS_Y	0					//초기 윈도우 시작 위치 Y
#define WIN_CLASS	"Windows Exam"		//윈도우 클레스 이름
#define WIN_TITLE	"Windows Exam"		//윈도우 타이틀 이름
#define WIN_STYLE	WS_OVERLAPPEDWINDOW	//윈도우 형태

#define ONE_RAD 0.017453f				//라디안 1도
#define EPSILON 0.0001f					//이정도 오차는 Float 을 같은걸로 따진다.

//
// 매크로 함수
//
#define SAFE_DELETE(p)		{ if(p){ delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARR(p)	{ if(p){ delete[] (p); (p) = NULL; } }
#define SAFE_RELEASE(p)		{ if(p){ (p)->Release(); (p) = NULL; }}
#define FLOAT_EQUAL(a,b)	(abs( (a) - (b) ) <= EPSILON) 

#define SINGLETONE(class_name) private:\
class_name(void);\
~class_name(void);\
public:\
	static class_name* GetInstance()\
	{\
		static class_name instance;\
		return &instance;\
	}
//전역변수

//
//#define SYNTHESIZE(varType, varName, funName)\
//protected: varType varName;\
//public: inline varType Get##funName(void) const { return varName; }\
//public: inline void Set##funName(varType var){ varName = var; }
//
//#define SYNTHESIZE_PASS_BY_REF(varType, varName, funName)\
//protected: varType varName;\
//public: inline varType& Get##funName(void) { return varName; }\
//public: inline void Set##funName(varType& var){ varName = var; }
//
//#define SYNTHESIZE_ADD_REF(varType, varName, funName)    \
//protected: varType varName; \
//public: virtual varType Get##funName(void) const { return varName; } \
//public: virtual void Set##funName(varType var){\
//	if (varName != var) {\
//	SAFE_ADD_REF(var);\
//	SAFE_RELEASE(varName);\
//	varName = var;\
//	}\
//}
//RECT 만들기
inline RECT RectMake(int x, int y, int width, int height)
{
	RECT rc = { x, y, x + width, y + height };
	return rc;
}
//
// 전역 변수
//
extern HWND			g_hWnd;		//윈도우 핸들 ( 자신의 프로그램에서 돌고있는 윈도우 번호 )
extern HINSTANCE	g_hInst;	//프로그램 인스턴스 핸들 ( OS 가 부여한 프로그램 번호 )

//////////////////////////////////////////////////////////////
typedef struct PNT_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3	n;
	D3DXVECTOR2 t;
	enum { FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 };
}PNTVERTEX, *LPPNTVERTEX;

struct ST_SIZEN
{
	int nWidth;
	int nHeight;
	ST_SIZEN() : nWidth(0), nHeight(0) {}
	ST_SIZEN(int _nWidth, int _nHeight)
		: nWidth(_nWidth), nHeight(_nHeight) {}
};

struct ST_PCT_VERTEX
{
	D3DXVECTOR3 p;
	D3DCOLOR	c;
	D3DXVECTOR2 t;
	enum { FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 };
};

struct ST_PNT_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3	n;
	D3DXVECTOR2 t;
	enum { FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 };
};

struct ST_RHWC_VERTEX
{
	D3DXVECTOR4 p;
	D3DCOLOR	c;
	enum { FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE };
};
extern int Gold;
//////////////////////////////////////////////////////////////

#include <vector>
#include <map>
#include <assert.h>
extern POINT _ptMouse;
using namespace std;

#include "MyUtil.h"
using namespace MYUTIL;

#include "cTransform.h"

#include "cObject.h"
#include "cUIObject.h"
#include "cGameObject.h"

//
// 매니져
//
#include "cTimeMgr.h"
#include "cKeyMgr.h"
#include "cLogMgr.h"
#include "cDXFontManager.h"
#include "cGizmoManager.h"
#include "cSpriteManager.h"
#include "cPhysicsManager.h"
#include "cGraphManager.h"
#include "soundManager.h"
#include "cSceneManager.h"
#include "cTextureManager.h"
#include "cResourceMgr_Texture.h"
#include "cResourceMgr_Effect.h"
#include "cResourceMgr_XMeshStatic.h"
#include "cResourceMgr_XMeshSkinned.h"
//
// 씬 
//
#include "cScene.h"

//UI
#include "cFontManager.h"
#include "cObjectManager.h"


#include "item.h"

extern vector<tagItemInfo> m_vecItemInfo;
extern vector<tagItemInfo>::iterator m_iterVecInfo;
extern int g_MonsterLevel;