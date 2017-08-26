#include "stdafx.h"
#include "cTexQuad.h"


cTexQuad::cTexQuad()
{
	Init();
}


cTexQuad::~cTexQuad()
{
	Release();
}


void cTexQuad::Init()
{

	//Quad 하나 만든다.
	// 1---2
	// |  /|
	// | / |
	// |/  |
	// 0---3

	MYVERTEX Quad[6];
	Quad[0].pos = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	Quad[1].pos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	Quad[2].pos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	Quad[3].pos = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	Quad[4].pos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	Quad[5].pos = D3DXVECTOR3(1.0f, -1.0f, 0.0f);

	Quad[0].uv = D3DXVECTOR2(0, 1);
	Quad[1].uv = D3DXVECTOR2(0, 0);
	Quad[2].uv = D3DXVECTOR2(1, 0);
	Quad[3].uv = D3DXVECTOR2(0, 1);
	Quad[4].uv = D3DXVECTOR2(1, 0);
	Quad[5].uv = D3DXVECTOR2(1, 1);

	//정점 버퍼 셋팅
	Device->CreateVertexBuffer(
		sizeof(MYVERTEX)* 6,
		D3DUSAGE_WRITEONLY,
		MYVERTEX::FVF,
		D3DPOOL_DEFAULT,
		&this->vb,
		NULL);

	LPMYVERTEX p = NULL;
	this->vb->Lock(0, 0, (void**)&p, 0);
	memcpy(p, Quad, sizeof(MYVERTEX)* 6);
	this->vb->Unlock();



}
void cTexQuad::Release()
{
	SAFE_RELEASE(this->vb);
}

void cTexQuad::Render()
{
	Device->SetStreamSource(0, this-> vb, 0, sizeof(MYVERTEX));
	Device->SetFVF(MYVERTEX::FVF);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
}