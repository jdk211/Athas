#pragma once

class cColorCube
{
private:

	//�÷� ť�� ���� ����
	typedef struct tagCOLORVERTEX{
		D3DXVECTOR3 position;
		DWORD color;
		enum { FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE };
	}COLORVERTEX, *LPCOLORVERTEX;

	LPDIRECT3DVERTEXBUFFER9 vb;	//���� ����
	LPDIRECT3DINDEXBUFFER9 ib;	//�ε��� ����


public:
	cColorCube();
	~cColorCube();

	void Init();
	void Release();
	void Render();

};

