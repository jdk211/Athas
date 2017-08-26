#pragma once
class cTexQuad
{
	//랜더링되는 정점의 정의가 담겨있는 정점 구조체....
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

