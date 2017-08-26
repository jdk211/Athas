#pragma once

class cModelCube
{
	//컬러 큐브 정점 정보
	typedef struct tagMODELVERTEX{
		D3DXVECTOR3 position;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 texcoord;
	}MODELVERTEX, *LPMODELVERTEX;

	LPDIRECT3DVERTEXBUFFER9 vb;	//정점 버퍼
	LPDIRECT3DINDEXBUFFER9 ib;	//인덱스 버퍼

	//정점 선언 정보 버퍼. ( 이놈이 FVF 를 대신한다 )
	LPDIRECT3DVERTEXDECLARATION9 decl;


public:
	cModelCube();
	~cModelCube();


	void Init();
	void Release();
	void Render();
};

