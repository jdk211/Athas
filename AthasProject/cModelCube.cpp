#include "stdafx.h"
#include "cModelCube.h"


cModelCube::cModelCube()
{
}


cModelCube::~cModelCube()
{
}

void cModelCube::Init()
{
	//쿼드 정점
	D3DXVECTOR3 quadPosition[4];
	D3DXVECTOR2 quadTexcoord[4];
	quadPosition[0] = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
	quadPosition[1] = D3DXVECTOR3(-0.5f, 0.5f, -0.5f);
	quadPosition[2] = D3DXVECTOR3(0.5f, 0.5f, -0.5f);
	quadPosition[3] = D3DXVECTOR3(0.5f, -0.5f, -0.5f);
	quadTexcoord[0] = D3DXVECTOR2(0, 1);
	quadTexcoord[1] = D3DXVECTOR2(0, 0);
	quadTexcoord[2] = D3DXVECTOR2(1, 0);
	quadTexcoord[3] = D3DXVECTOR2(1, 1);


	//계산된 정점들
	D3DXVECTOR3 cubePos[24];		//큐브의 정점위치
	D3DXVECTOR3 cubeNor[24];		//큐브의 노말
	D3DXVECTOR3 cubeBin[24];		//큐브의 바이노말
	D3DXVECTOR3 cubeTan[24];		//큐브의 탄젠트
	D3DXVECTOR2 cubeUV[24];			//큐브의 UV


	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationY(&matRotate, -90 * ONE_RAD);


	//
	// 정점의 위치와 UV 만 대입하자.
	//


	//
	// 뒷면
	//

	//버퍼에 4개만써
	memcpy(cubePos, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV, quadTexcoord, sizeof(D3DXVECTOR2)* 4);


	//
	// 우측면
	//
	for (int i = 0; i < 4; i++){
		D3DXVec3TransformCoord(&quadPosition[i], &quadPosition[i], &matRotate);
	}

	//버퍼에 4개만써
	memcpy(cubePos + 4, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV + 4, quadTexcoord, sizeof(D3DXVECTOR2)* 4);




	//
	// 정면
	//
	for (int i = 0; i < 4; i++){
		D3DXVec3TransformCoord(&quadPosition[i], &quadPosition[i], &matRotate);
	}

	//버퍼에 4개만써
	memcpy(cubePos + 8, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV + 8, quadTexcoord, sizeof(D3DXVECTOR2)* 4);


	//
	// 좌면
	//
	for (int i = 0; i < 4; i++){
		D3DXVec3TransformCoord(&quadPosition[i], &quadPosition[i], &matRotate);
	}

	//버퍼에 4개만써
	memcpy(cubePos + 12, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV + 12, quadTexcoord, sizeof(D3DXVECTOR2)* 4);

	//
	// 윗면
	//
	D3DXMatrixRotationZ(&matRotate, -90 * ONE_RAD);
	for (int i = 0; i < 4; i++){
		D3DXVec3TransformCoord(&quadPosition[i], &quadPosition[i], &matRotate);
	}

	//버퍼에 4개만써
	memcpy(cubePos + 16, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV + 16, quadTexcoord, sizeof(D3DXVECTOR2)* 4);

	//
	// 아랫면
	//
	D3DXMatrixRotationZ(&matRotate, -180 * ONE_RAD);
	for (int i = 0; i < 4; i++){
		D3DXVec3TransformCoord(&quadPosition[i], &quadPosition[i], &matRotate);
	}

	//버퍼에 4개만써
	memcpy(cubePos + 20, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV + 20, quadTexcoord, sizeof(D3DXVECTOR2)* 4);


	//여기까지 오면 정점의  위치와 UV 가 계산되었다.



	//
	// 인덱스
	//

	DWORD index[36];
	for (int i = 0; i < 6; i++){
		DWORD i0 = i * 4 + 0;
		DWORD i1 = i * 4 + 1;
		DWORD i2 = i * 4 + 2;
		DWORD i3 = i * 4 + 3;
		index[i * 6 + 0] = i0;
		index[i * 6 + 1] = i1;
		index[i * 6 + 2] = i2;
		index[i * 6 + 3] = i0;
		index[i * 6 + 4] = i2;
		index[i * 6 + 5] = i3;
	}



	//노말을 계산하자.
	ComputeNormal(
		cubeNor,		//계산 결과를 넣을 배열
		cubePos,		//정점위치
		24,				//정점갯수
		index,			//인덱스 배열 DWORD
		36				//인덱스 배열 갯수
		);


	//바이노말과 탄젠트를 계산하자.
	ComputeTangentAngBinormal(
		cubeTan,		//결과로 얻을 Tangent 배열
		cubeBin,		//결과로 얻을 Binormal 배열
		cubePos,		//정점 포지션
		cubeNor,		//정점 노말
		cubeUV,			//정점 UV
		index,			//정점 인덱스 배열
		12,				//폴리곤 갯수
		24);			//정점 갯수


	
	//
	// 정점 버퍼 생성
	//
	Device->CreateVertexBuffer(
		sizeof(MODELVERTEX)* 24,
		D3DUSAGE_WRITEONLY,
		0,					//FVF 정보가 없어 0 을 쓴다.
		D3DPOOL_DEFAULT,
		&this->vb,
		NULL);

	LPMODELVERTEX pVertice = NULL;
	this->vb->Lock(0, 0, (void**)&pVertice, 0);

	for (int i = 0; i < 24; i++)
	{
		pVertice[i].position = cubePos[i];
		pVertice[i].normal = cubeNor[i];
		pVertice[i].binormal = cubeBin[i];
		pVertice[i].tangent = cubeTan[i];
		pVertice[i].texcoord = cubeUV[i];
	}
	
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
	
	memcpy(pIndices, index, sizeof(DWORD)* 36);

	this->ib->Unlock();


	//
	// 정점 Declaration 생성
	//

	//정점의 구조의 배열정보를 만들자.
	D3DVERTEXELEMENT9 vertElem[6];		//정점의 정보 갯수의 + 1 개 만큼 배열을 만든다.

	//정점구조체의 처음 들어가는 정보는 Position 이다.
	vertElem[0].Stream = 0;		//Stream 은 0
	vertElem[0].Offset = 0;		//메모리의 시작 Byte 단위 0
	vertElem[0].Type = D3DDECLTYPE_FLOAT3;		//자료의 형태.. Vector3 는 FLOAT3
	vertElem[0].Method = D3DDECLMETHOD_DEFAULT;		//일단 무조건  D3DDECLMETHOD_DEFAUL
	vertElem[0].Usage = D3DDECLUSAGE_POSITION;		//정보 타입 정점 위치
	vertElem[0].UsageIndex = 0;						//UsageIndex 일단 0


	//정점구조체의 다음 들어가는 정보는 tangent 이다.
	vertElem[1].Stream = 0;		//Stream 은 0
	vertElem[1].Offset = 12;	//메모리의 시작 Byte 단위 0
	vertElem[1].Type = D3DDECLTYPE_FLOAT3;		//자료의 형태.. Vector3 는 FLOAT3
	vertElem[1].Method = D3DDECLMETHOD_DEFAULT;		//일단 무조건  D3DDECLMETHOD_DEFAUL
	vertElem[1].Usage = D3DDECLUSAGE_TANGENT;		//정보 타입 정점 위치
	vertElem[1].UsageIndex = 0;						//UsageIndex 일단 0


	//정점구조체의 다음 들어가는 정보는 binormal 이다.
	vertElem[2].Stream = 0;		//Stream 은 0
	vertElem[2].Offset = 24;	//메모리의 시작 Byte 단위 0
	vertElem[2].Type = D3DDECLTYPE_FLOAT3;		//자료의 형태.. Vector3 는 FLOAT3
	vertElem[2].Method = D3DDECLMETHOD_DEFAULT;		//일단 무조건  D3DDECLMETHOD_DEFAUL
	vertElem[2].Usage = D3DDECLUSAGE_BINORMAL;		//정보 타입 정점 위치
	vertElem[2].UsageIndex = 0;						//UsageIndex 일단 0
	
	// 정점구조체의 다음 들어가는 정보는 normal 이다.
	vertElem[3].Stream = 0;		//Stream 은 0
	vertElem[3].Offset = 36;	//메모리의 시작 Byte 단위 0
	vertElem[3].Type = D3DDECLTYPE_FLOAT3;		//자료의 형태.. Vector3 는 FLOAT3
	vertElem[3].Method = D3DDECLMETHOD_DEFAULT;		//일단 무조건  D3DDECLMETHOD_DEFAUL
	vertElem[3].Usage = D3DDECLUSAGE_NORMAL;		//정보 타입 정점 위치
	vertElem[3].UsageIndex = 0;						//UsageIndex 일단 0

	// 정점구조체의 다음 들어가는 정보는 normal 이다.
	vertElem[4].Stream = 0;		//Stream 은 0
	vertElem[4].Offset = 48;	//메모리의 시작 Byte 단위 0
	vertElem[4].Type = D3DDECLTYPE_FLOAT2;		//자료의 형태.. Vector2 는 FLOAT2
	vertElem[4].Method = D3DDECLMETHOD_DEFAULT;		//일단 무조건  D3DDECLMETHOD_DEFAUL
	vertElem[4].Usage = D3DDECLUSAGE_TEXCOORD;		//정보 타입 정점 위치
	vertElem[4].UsageIndex = 0;						//UsageIndex 일단 0

	//마지막을 알린다.
	//더이상 없어
	D3DVERTEXELEMENT9 end = D3DDECL_END(); //{0xFF,0,D3DDECLTYPE_UNUSED,0,0,0}
	vertElem[5] = end;


	//LPDIRECT3DVERTEXDECLARATION9 생성
	Device->CreateVertexDeclaration(
		vertElem,		//위에서만든 정점정보 배열
		&this->decl		//얻어올 LPDIRECT3DVERTEXDECLARATION9 포인터
		);




}
void cModelCube::Release()
{
	SAFE_RELEASE(this->vb);
	SAFE_RELEASE(this->ib);
	SAFE_RELEASE(this->decl);
}

void cModelCube::Render()
{
	Device->SetStreamSource(0, this->vb, 0, sizeof(MODELVERTEX));
	Device->SetIndices(this->ib);
	//Device->SetFVF(MODELVERTEX::FVF);
	
	//FVF 대신에 들어가는거
	Device->SetVertexDeclaration(this->decl);
	
	Device->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);
}