#pragma once

#include "cScene.h"
#include "cBoundBox.h"
#include <vector>

class cLight_Directional;
class cQuadTree;

#define	MAP_NAME_TOWN	"TOWN"
#define MAP_NAME_CRAFT	"CRAFT"
#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#define BYTE    unsigned char

class cScene_MapTool : public cScene
{
public:
	typedef struct ST_BUILD
	{
		cXMesh_Static*	skin;
		char*			skinPath;
		cTransform		trans;
		D3DXVECTOR3		position;
		cBoundBox*		box;
	}STBUILDE, *LPSTBUILD;

	struct BRUSHVERTEX
	{
		D3DXVECTOR3 p;
		DWORD color;
	};
#define D3DFVF_BRUSHVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

	enum MODE_STATE
	{
		MODE_HEIGHT,	//높이맵
		MODE_BUILD,		//건물설치
		MODE_EDIT,		//건물수정
		MODE_DELETE		//건물삭제
	};
private:
	//bmp 관련

	FILE *infile;				//맵 정보를 담는다

	BITMAPFILEHEADER hf;		// BMP Header Information
	BITMAPINFOHEADER hInfo;

	RGBQUAD hRGB[256];
	BYTE *lpImg;
	int rwsize;

private:
	cLight_Directional*		m_pDirectionalLight;
	cQuadTree*				m_pQuadTree;		//쿼드 트리

	char*					m_cpMapName;		//변경 할 맵 이름
	char*					m_cpMapPath;		//변경 할 맵 주소

	int						m_nMapSize;			//맵 사이즈			// ex) 257
	int						m_nTileNum;			//맵 타일의 사이즈	// ex) 257 - 1
	int						m_nTotalVertex;		//점의 사이즈		// ex) 257 * 257

	float					m_fGapSize;			//셀 크기
	int						m_nHeightSize;		//올려줄 높이
	int						m_nBrushRadius;		//브러스 반지름

	LPPNTVERTEX				m_pVertex;			//맵 vertex
	vector<DWORD>			m_vecIndex;			//vertex의 순서 index
	LPD3DXMESH				m_pMesh;			//Mesh로 그린다

	Ray						m_ClickRay;			//클린한 Ray
	D3DXVECTOR3				m_vPickingPos;		//픽킹 포지션
	D3DXVECTOR3				m_vCircleCenter;	//브러시 원 중심

	cXMesh_Static*			m_pTestHome;		//건물 test
	char*					m_strSkinPath;		//건물 스킨 주소
	cTransform				m_BTestTrans;

	vector<LPSTBUILD>		m_vecSetBuild;		//건물

	MODE_STATE				m_stModeState;		//현재 모드 상태
	string					m_strMode;			//현재 모드 print

	int						m_nSelectBuildNum;	//현재 선택한 건물 번호
	bool					m_isBuildPick;		//건물을 설치 가능한지
	cUIObject*				m_pUIRoot;
	LPD3DXSPRITE			m_pSprite;

	POINT					ptMouse;

	float U;
	float V;
	float Dist;

public:
	cScene_MapTool();
	~cScene_MapTool();

	virtual HRESULT Scene_Init();
	virtual void	Scene_Release();
	virtual void	Scene_Update(float timeDelta);
	virtual void	Scene_Render();
	virtual void	Scene_LightRender();

	void	BrushCircleDraw();
	bool	BMPFileOpen();
	void	CreateGround();
	void	DeleteBuild();
	void	EditBuild();
	void	FixGroundHeight();
	void	FixBrushSize();
	void	ModeSelect();
	void	ModeUpdate();
	void	PickBulid();
	void	PickGround();
	void	PickisPossible();
	void	PrintText();
	void	ResetMesh();
	void	SaveMap();
	void	SaveBuild();
	int		SelectBuildNum();

	float GetHeightY(float x, float z);

	bool IsIntersectRay(OUT D3DXVECTOR3* pVector, IN const LPRay pRay);

	float _Lerp(float a, float b, float t) { return a - (a*t) + (b*t); }			// 선형보간

	virtual void	OnClick(cUIButton* pSender) override;
};

