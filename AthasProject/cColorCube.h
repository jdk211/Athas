#pragma once

class cColorCube
{
private:

	//컬러 큐브 정점 정보
	typedef struct tagCOLORVERTEX{
		D3DXVECTOR3 position;
		DWORD color;
		enum { FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE };
	}COLORVERTEX, *LPCOLORVERTEX;

	LPDIRECT3DVERTEXBUFFER9 vb;	//정점 버퍼
	LPDIRECT3DINDEXBUFFER9 ib;	//인덱스 버퍼


public:
	cColorCube();
	~cColorCube();

	void Init();
	void Release();
	void Render();

};

