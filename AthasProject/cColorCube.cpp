#include "stdafx.h"
#include "cColorCube.h"


cColorCube::cColorCube()
: vb(NULL), ib( NULL )
{
}


cColorCube::~cColorCube()
{
}

void cColorCube::Init()
{
	//쿼드 정점
	COLORVERTEX quad[4];
	quad[0].position = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
	quad[1].position = D3DXVECTOR3(-0.5f, 0.5f, -0.5f);
	quad[2].position = D3DXVECTOR3(0.5f, 0.5f, -0.5f);
	quad[3].position = D3DXVECTOR3(0.5f, -0.5f, -0.5f);

	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationY(&matRotate, -90 * ONE_RAD);



	//
	// 정점 버퍼 생성
	//
	Device->CreateVertexBuffer(
		sizeof(COLORVERTEX)* 24,
		D3DUSAGE_WRITEONLY,
		COLORVERTEX::FVF,
		D3DPOOL_DEFAULT,
		&this->vb,
		NULL);

	LPCOLORVERTEX pVertice = NULL;
	this->vb->Lock(0, 0, (void**)&pVertice, 0);

	//
	// 뒷면
	//
	quad[0].color = 0xFF000080;
	quad[1].color = 0xFF000080;
	quad[2].color = 0xFF000080;
	quad[3].color = 0xFF000080;

	//버퍼에 4개만써
	memcpy(pVertice, quad, sizeof(COLORVERTEX)* 4);


	//
	// 우측면
	//
	for (int i = 0; i < 4; i++)
		D3DXVec3TransformCoord(&quad[i].position, &quad[i].position, &matRotate);
	quad[0].color = 0xFFFF0000;
	quad[1].color = 0xFFFF0000;
	quad[2].color = 0xFFFF0000;
	quad[3].color = 0xFFFF0000;

	//버퍼에 4개만써
	memcpy(pVertice + 4, quad, sizeof(COLORVERTEX)* 4);



	//
	// 정면
	//
	for (int i = 0; i < 4; i++)
		D3DXVec3TransformCoord(&quad[i].position, &quad[i].position, &matRotate);
	quad[0].color = 0xFF0000FF;
	quad[1].color = 0xFF0000FF;
	quad[2].color = 0xFF0000FF;
	quad[3].color = 0xFF0000FF;

	//버퍼에 4개만써
	memcpy(pVertice + 8, quad, sizeof(COLORVERTEX)* 4);


	//
	// 좌면
	//
	for (int i = 0; i < 4; i++)
		D3DXVec3TransformCoord(&quad[i].position, &quad[i].position, &matRotate);
	quad[0].color = 0xFF800000;
	quad[1].color = 0xFF800000;
	quad[2].color = 0xFF800000;
	quad[3].color = 0xFF800000;

	//버퍼에 4개만써
	memcpy(pVertice + 12, quad, sizeof(COLORVERTEX)* 4);


	//
	// 윗면
	//
	D3DXMatrixRotationZ(&matRotate, -90 * ONE_RAD);
	for (int i = 0; i < 4; i++)
		D3DXVec3TransformCoord(&quad[i].position, &quad[i].position, &matRotate);
	quad[0].color = 0xFF00FF00;
	quad[1].color = 0xFF00FF00;
	quad[2].color = 0xFF00FF00;
	quad[3].color = 0xFF00FF00;

	//버퍼에 4개만써
	memcpy(pVertice + 16, quad, sizeof(COLORVERTEX)* 4);


	//
	// 아랫면
	//
	D3DXMatrixRotationZ(&matRotate, -180 * ONE_RAD);
	for (int i = 0; i < 4; i++)
		D3DXVec3TransformCoord(&quad[i].position, &quad[i].position, &matRotate);
	quad[0].color = 0xFF008000;
	quad[1].color = 0xFF008000;
	quad[2].color = 0xFF008000;
	quad[3].color = 0xFF008000;

	//버퍼에 4개만써
	memcpy(pVertice + 20, quad, sizeof(COLORVERTEX)* 4);

	this->vb->Unlock();

	//
	// 인덱스 버퍼 생성
	//
	Device->CreateIndexBuffer(
		sizeof(DWORD)* 36,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,
		&this->ib,
		NULL);

	DWORD* pIndices = NULL;
	this->ib->Lock(0, 0, (void**)&pIndices, 0);

	for (int i = 0; i < 6; i++){
		DWORD i0 = i * 4 + 0;
		DWORD i1 = i * 4 + 1;
		DWORD i2 = i * 4 + 2;
		DWORD i3 = i * 4 + 3;

		pIndices[i * 6 + 0] = i0;
		pIndices[i * 6 + 1] = i1;
		pIndices[i * 6 + 2] = i2;
		pIndices[i * 6 + 3] = i0;
		pIndices[i * 6 + 4] = i2;
		pIndices[i * 6 + 5] = i3;
	}

	this->ib->Unlock();

}
void cColorCube::Release()
{
	SAFE_RELEASE(this->vb);
	SAFE_RELEASE(this->ib);
}

void cColorCube::Render()
{
	Device->SetStreamSource(0, this->vb, 0, sizeof(COLORVERTEX));
	Device->SetIndices(this->ib);
	Device->SetFVF(COLORVERTEX::FVF);
	Device->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

}

