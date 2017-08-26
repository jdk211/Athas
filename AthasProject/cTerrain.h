#pragma once

class cQuadTree;
//////////////////////////////////////////
#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#define BYTE    unsigned char
//////////////////////////////////////////
class cTerrain
{
public:

	//Terrain ���� ����ü
	typedef struct tagTERRAINVERTEX {
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 binormal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR2 baseUV;			//0~1 UV
		D3DXVECTOR2 tileUV;			//Ÿ�� UV 
	}TERRAINVERTEX, *LPTERRAINVERTEX;

	//Terrain �ε��� ����ü ( ��� ����ü �ϳ��� �ﰢ�� �ϳ��� ����Ѵ� )
	typedef struct tagTERRAINTRI {
		DWORD dw0;
		DWORD dw1;
		DWORD dw2;
	}TERRAINTRI, *LPTERRAINTRI;


	//Texture 3 �� 
	typedef struct tagTERRAINTEXTURE {
		LPDIRECT3DTEXTURE9 diffuse;
		LPDIRECT3DTEXTURE9 specular;
		LPDIRECT3DTEXTURE9 normal;
	}TERRAINTEXTURE, *LPTERRAINTEXTURE;

private:
	LPDIRECT3DVERTEXBUFFER9				m_pTerrainVB;		//�ͷ��� ���� ����
	LPDIRECT3DINDEXBUFFER9				m_pTerrainIB;		//�ͷ��� �ε��� ����
	LPDIRECT3DVERTEXDECLARATION9		m_pTerrainDecl;		//�ͷ��� ���� Decl

	float				m_fHeightScale;		//���̽����� ( �ȼ��÷��� 255 �϶� ���� )
	float				m_fCellScale;		//���ϳ��� ũ��

	int					m_nVerNumX;					//���� ������
	int					m_nVerNumZ;					//���� ������
	int					m_nTotalVerNum;				//�� ������

	int					m_nCellNumX;				//���� ����
	int					m_nCellNumZ;				//���� ����
	int					m_nTotalCellNum;			//�� ����
	int					m_nTotalTri;				//�ﰢ�� ����

	LPTERRAINVERTEX						m_pTerrainVertices;	//�ͷ��� �������� ( ������ Terrain �� ���������� ���� �ϴ� ������ �ִ´� )
	LPTERRAINTRI						m_pTerrainTri;
	LPDIRECT3DTEXTURE9					m_pHeightMap;		//�ͷ��� ���� ��

	float				m_fTerrainSizeZ;			//�ͷ��� ���� ������
	float				m_fTerrainSizeX;			//�ͷ��� ���� ������


													//��Ȯ�� ���� ����� ���� �ʿ��ϴ�...
	float				m_fTerrainStartX;			//�ͷ��� ���� ��ġX
	float				m_fTerrainStartZ;			//�ͷ��� ���� ��ġZ
	float				m_fTerrainEndX;
	float				m_fTerrainEndZ;

	cQuadTree*			m_pQuadTree;

	LPD3DXEFFECT		m_pTerrainEffect;

	TERRAINTEXTURE		m_pTexTile[4];					//�ͷ��� �� �� Ÿ��
	LPDIRECT3DTEXTURE9  m_pTexSlat;						//���÷��� Texture

														/////////////////////////////////////////////////////////////
private:
	//bmp ����

	FILE *infile;				//�� ������ ��´�

	BITMAPFILEHEADER hf;		// BMP Header Information
	BITMAPINFOHEADER hInfo;

	RGBQUAD hRGB[256];
	BYTE *lpImg;
	int rwsize;

private:
	char*					m_cpMapPath;

	int						m_nMapSize;			//�� ������			// ex) 257
	int						m_nTileNum;			//�� Ÿ���� ������	// ex) 257 - 1
	int						m_nTotalVertex;		//���� ������		// ex) 257 * 257

	LPPNTVERTEX				m_pVertex;			//�� vertex
	vector<DWORD>			m_vecIndex;			//vertex�� ���� index
	LPD3DXMESH				m_pMesh;			//Mesh�� �׸���

	const char*				m_sTxtPath;			//Height txtfile path
												/////////////////////////////////////////////////////////////

public:
	cTerrain();
	~cTerrain();

	//�ʱ�ȭ 
	HRESULT Init(
		const char* szHeightMapName,		//���̸� �̸�
		const char* szTile_0,				// Ÿ�� �̹��� 0
		const char* szTile_1,				// Ÿ�� �̹��� 1
		const char* szTile_2,				// Ÿ�� �̹��� 2
		const char* szTile_3,				// Ÿ�� �̹��� 3
		const char* szTileSplat,			// Ÿ�� �̹��� ��Ʈ�� ( RGB Black )
		const char* heightTxtPath,			// 
		float cellSize,						// �ϳ��� ��ũ��
		float heightScale,					// ���� ������
		int smoothLevel,					// ������ ó�� Ƚ��
		int tileNum							// Ÿ�ϸ� ����
	);


	//����
	void Release();

	//�Ϲ� ����
	void Render(cCamera* pCam);

	//�׸��� ����
	void RenderShadow(cCamera* pLightCam);

	//����
	void RenderWire(cCamera* pCam);

	//�ش� X, Z ��ġ�� ������ ���̸� ��´�.
	float GetHeight(float x, float z);
	bool GetHeight(IN float x, OUT float y, IN float z);

	//�ش� X, Z ��ġ�� ��� ���͸� ��´�.
	bool GetSlant(D3DXVECTOR3* pOut, float gravityPower, float x, float z);


	//Ray ��Ʈ ��ġ�� ��´�.
	bool IsIntersectRayMoosic(D3DXVECTOR3* pOut, const LPRay pRay);

	//Ray ��Ʈ ��ġ�� ��´�.
	bool IsIntersectRay(D3DXVECTOR3* pOut, const LPRay pRay);


	D3DXVECTOR3 GetRandomPos();

private:

	//�ͷ��� �����
	HRESULT CreateTerrain(int smooth, int tileNum);

	//�ͷ��� ������
	void SmoothTerrain(int passed);
};

