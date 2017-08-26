#pragma once
class cTexQuad
{
	//�������Ǵ� ������ ���ǰ� ����ִ� ���� ����ü....
	typedef struct tagMYVERTEX{
		D3DXVECTOR3		pos;	
		D3DXVECTOR2		uv;		
		enum { FVF = D3DFVF_XYZ | D3DFVF_TEX1 }; 
	}MYVERTEX, *LPMYVERTEX;

	LPDIRECT3DVERTEXBUFFER9			vb;


public:
	cTexQuad();
	~cTexQuad();

	void Init();
	void Release();
	void Render();
};

