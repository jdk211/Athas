#pragma once

class cModelCube
{
	//�÷� ť�� ���� ����
	typedef struct tagMODELVERTEX{
		D3DXVECTOR3 position;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 texcoord;
	}MODELVERTEX, *LPMODELVERTEX;

	LPDIRECT3DVERTEXBUFFER9 vb;	//���� ����
	LPDIRECT3DINDEXBUFFER9 ib;	//�ε��� ����

	//���� ���� ���� ����. ( �̳��� FVF �� ����Ѵ� )
	LPDIRECT3DVERTEXDECLARATION9 decl;


public:
	cModelCube();
	~cModelCube();


	void Init();
	void Release();
	void Render();
};

