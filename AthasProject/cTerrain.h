#pragma once

class cQuadTree;
//////////////////////////////////////////
#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#define BYTE    unsigned char
//////////////////////////////////////////
class cTerrain
{
public:

	//Terrain 정점 구조체
	typedef struct tagTERRAINVERTEX {
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 binormal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR2 baseUV;			//0~1 UV
		D3DXVECTOR2 tileUV;			//타일 UV 
	}TERRAINVERTEX, *LPTERRAINVERTEX;

	//Terrain 인덱스 구조체 ( 요고 구조체 하나가 삼각형 하나를 담당한다 )
	typedef struct tagTERRAINTRI {
		DWORD dw0;
		DWORD dw1;
		DWORD dw2;
	}TERRAINTRI, *LPTERRAINTRI;


	//Texture 3 장 
	typedef struct tagTERRAINTEXTURE {
		LPDIRECT3DTEXTURE9 diffuse;
		LPDIRECT3DTEXTURE9 specular;
		LPDIRECT3DTEXTURE9 normal;
	}TERRAINTEXTURE, *LPTERRAINTEXTURE;

private:
	LPDIRECT3DVERTEXBUFFER9				m_pTerrainVB;		//터레인 정점 버퍼
	LPDIRECT3DINDEXBUFFER9				m_pTerrainIB;		//터레인 인덱스 버퍼
	LPDIRECT3DVERTEXDECLARATION9		m_pTerrainDecl;		//터레인 정점 Decl

	float				m_fHeightScale;		//높이스케일 ( 픽셀컬러가 255 일때 높이 )
	float				m_fCellScale;		//셀하나의 크기

	int					m_nVerNumX;					//가로 정점수
	int					m_nVerNumZ;					//깊이 정점수
	int					m_nTotalVerNum;				//총 정점수

	int					m_nCellNumX;				//가로 셀수
	int					m_nCellNumZ;				//깊이 셀수
	int					m_nTotalCellNum;			//총 셀수
	int					m_nTotalTri;				//삼각형 갯수

	LPTERRAINVERTEX						m_pTerrainVertices;	//터레인 정점정보 ( 생성된 Terrain 의 정점정보를 따로 싹다 가지고 있는다 )
	LPTERRAINTRI						m_pTerrainTri;
	LPDIRECT3DTEXTURE9					m_pHeightMap;		//터레인 높이 맵

	float				m_fTerrainSizeZ;			//터레인 깊이 사이즈
	float				m_fTerrainSizeX;			//터레인 가로 사이즈


													//정확한 높이 계산을 위해 필요하다...
	float				m_fTerrainStartX;			//터레인 시작 위치X
	float				m_fTerrainStartZ;			//터레인 시작 위치Z
	float				m_fTerrainEndX;
	float				m_fTerrainEndZ;

	cQuadTree*			m_pQuadTree;

	LPD3DXEFFECT		m_pTerrainEffect;

	TERRAINTEXTURE		m_pTexTile[4];					//터레인 각 층 타일
	LPDIRECT3DTEXTURE9  m_pTexSlat;						//스플렛팅 Texture

														/////////////////////////////////////////////////////////////
private:
	//bmp 관련

	FILE *infile;				//맵 정보를 담는다

	BITMAPFILEHEADER hf;		// BMP Header Information
	BITMAPINFOHEADER hInfo;

	RGBQUAD hRGB[256];
	BYTE *lpImg;
	int rwsize;

private:
	char*					m_cpMapPath;

	int						m_nMapSize;			//맵 사이즈			// ex) 257
	int						m_nTileNum;			//맵 타일의 사이즈	// ex) 257 - 1
	int						m_nTotalVertex;		//점의 사이즈		// ex) 257 * 257

	LPPNTVERTEX				m_pVertex;			//맵 vertex
	vector<DWORD>			m_vecIndex;			//vertex의 순서 index
	LPD3DXMESH				m_pMesh;			//Mesh로 그린다

	const char*				m_sTxtPath;			//Height txtfile path
												/////////////////////////////////////////////////////////////

public:
	cTerrain();
	~cTerrain();

	//초기화 
	HRESULT Init(
		const char* szHeightMapName,		//높이맵 이름
		const char* szTile_0,				// 타일 이미지 0
		const char* szTile_1,				// 타일 이미지 1
		const char* szTile_2,				// 타일 이미지 2
		const char* szTile_3,				// 타일 이미지 3
		const char* szTileSplat,			// 타일 이미지 컨트롤 ( RGB Black )
		const char* heightTxtPath,			// 
		float cellSize,						// 하나의 셀크기
		float heightScale,					// 높이 스케일
		int smoothLevel,					// 스무싱 처리 횟수
		int tileNum							// 타일링 갯수
	);


	//해제
	void Release();

	//일반 랜더
	void Render(cCamera* pCam);

	//그림자 랜더
	void RenderShadow(cCamera* pLightCam);

	//랜더
	void RenderWire(cCamera* pCam);

	//해당 X, Z 위치의 지형의 높이를 얻는다.
	float GetHeight(float x, float z);
	bool GetHeight(IN float x, OUT float y, IN float z);

	//해당 X, Z 위치의 경사 벡터를 얻는다.
	bool GetSlant(D3DXVECTOR3* pOut, float gravityPower, float x, float z);


	//Ray 히트 위치를 얻는다.
	bool IsIntersectRayMoosic(D3DXVECTOR3* pOut, const LPRay pRay);

	//Ray 히트 위치를 얻는다.
	bool IsIntersectRay(D3DXVECTOR3* pOut, const LPRay pRay);


	D3DXVECTOR3 GetRandomPos();

private:

	//터레인 만들기
	HRESULT CreateTerrain(int smooth, int tileNum);

	//터레인 스무싱
	void SmoothTerrain(int passed);
};

