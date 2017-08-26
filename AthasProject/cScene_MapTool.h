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
		MODE_HEIGHT,	//���̸�
		MODE_BUILD,		//�ǹ���ġ
		MODE_EDIT,		//�ǹ�����
		MODE_DELETE		//�ǹ�����
	};
private:
	//bmp ����

	FILE *infile;				//�� ������ ��´�

	BITMAPFILEHEADER hf;		// BMP Header Information
	BITMAPINFOHEADER hInfo;

	RGBQUAD hRGB[256];
	BYTE *lpImg;
	int rwsize;

private:
	cLight_Directional*		m_pDirectionalLight;
	cQuadTree*				m_pQuadTree;		//���� Ʈ��

	char*					m_cpMapName;		//���� �� �� �̸�
	char*					m_cpMapPath;		//���� �� �� �ּ�

	int						m_nMapSize;			//�� ������			// ex) 257
	int						m_nTileNum;			//�� Ÿ���� ������	// ex) 257 - 1
	int						m_nTotalVertex;		//���� ������		// ex) 257 * 257

	float					m_fGapSize;			//�� ũ��
	int						m_nHeightSize;		//�÷��� ����
	int						m_nBrushRadius;		//�귯�� ������

	LPPNTVERTEX				m_pVertex;			//�� vertex
	vector<DWORD>			m_vecIndex;			//vertex�� ���� index
	LPD3DXMESH				m_pMesh;			//Mesh�� �׸���

	Ray						m_ClickRay;			//Ŭ���� Ray
	D3DXVECTOR3				m_vPickingPos;		//��ŷ ������
	D3DXVECTOR3				m_vCircleCenter;	//�귯�� �� �߽�

	cXMesh_Static*			m_pTestHome;		//�ǹ� test
	char*					m_strSkinPath;		//�ǹ� ��Ų �ּ�
	cTransform				m_BTestTrans;

	vector<LPSTBUILD>		m_vecSetBuild;		//�ǹ�

	MODE_STATE				m_stModeState;		//���� ��� ����
	string					m_strMode;			//���� ��� print

	int						m_nSelectBuildNum;	//���� ������ �ǹ� ��ȣ
	bool					m_isBuildPick;		//�ǹ��� ��ġ ��������
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

	float _Lerp(float a, float b, float t) { return a - (a*t) + (b*t); }			// ��������

	virtual void	OnClick(cUIButton* pSender) override;
};

