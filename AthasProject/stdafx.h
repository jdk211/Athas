// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

#include <Windows.h>
#include <cmath>
#include <string>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment( lib, "d3d9.lib")
#pragma comment( lib, "d3dx9.lib")

//
// ��ũ�� ���
//
#define WINSIZE_X	1200		//�ʱ� ������ ���� ũ��
#define WINSIZE_Y	800			//�ʱ� ������ ���� ũ��
#define WINPOS_X	0					//�ʱ� ������ ���� ��ġ X
#define WINPOS_Y	0					//�ʱ� ������ ���� ��ġ Y
#define WIN_CLASS	"Windows Exam"		//������ Ŭ���� �̸�
#define WIN_TITLE	"Windows Exam"		//������ Ÿ��Ʋ �̸�
#define WIN_STYLE	WS_OVERLAPPEDWINDOW	//������ ����

#define ONE_RAD 0.017453f				//���� 1��
#define EPSILON 0.0001f					//������ ������ Float �� �����ɷ� ������.

//
// ��ũ�� �Լ�
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
//��������

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
//RECT �����
inline RECT RectMake(int x, int y, int width, int height)
{
	RECT rc = { x, y, x + width, y + height };
	return rc;
}
//
// ���� ����
//
extern HWND			g_hWnd;		//������ �ڵ� ( �ڽ��� ���α׷����� �����ִ� ������ ��ȣ )
extern HINSTANCE	g_hInst;	//���α׷� �ν��Ͻ� �ڵ� ( OS �� �ο��� ���α׷� ��ȣ )

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
// �Ŵ���
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
// �� 
//
#include "cScene.h"

//UI
#include "cFontManager.h"
#include "cObjectManager.h"


#include "item.h"

extern vector<tagItemInfo> m_vecItemInfo;
extern vector<tagItemInfo>::iterator m_iterVecInfo;
extern int g_MonsterLevel;