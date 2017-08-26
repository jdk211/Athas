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
	//���� ����
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


	//���� ������
	D3DXVECTOR3 cubePos[24];		//ť���� ������ġ
	D3DXVECTOR3 cubeNor[24];		//ť���� �븻
	D3DXVECTOR3 cubeBin[24];		//ť���� ���̳븻
	D3DXVECTOR3 cubeTan[24];		//ť���� ź��Ʈ
	D3DXVECTOR2 cubeUV[24];			//ť���� UV


	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationY(&matRotate, -90 * ONE_RAD);


	//
	// ������ ��ġ�� UV �� ��������.
	//


	//
	// �޸�
	//

	//���ۿ� 4������
	memcpy(cubePos, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV, quadTexcoord, sizeof(D3DXVECTOR2)* 4);


	//
	// ������
	//
	for (int i = 0; i < 4; i++){
		D3DXVec3TransformCoord(&quadPosition[i], &quadPosition[i], &matRotate);
	}

	//���ۿ� 4������
	memcpy(cubePos + 4, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV + 4, quadTexcoord, sizeof(D3DXVECTOR2)* 4);




	//
	// ����
	//
	for (int i = 0; i < 4; i++){
		D3DXVec3TransformCoord(&quadPosition[i], &quadPosition[i], &matRotate);
	}

	//���ۿ� 4������
	memcpy(cubePos + 8, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV + 8, quadTexcoord, sizeof(D3DXVECTOR2)* 4);


	//
	// �¸�
	//
	for (int i = 0; i < 4; i++){
		D3DXVec3TransformCoord(&quadPosition[i], &quadPosition[i], &matRotate);
	}

	//���ۿ� 4������
	memcpy(cubePos + 12, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV + 12, quadTexcoord, sizeof(D3DXVECTOR2)* 4);

	//
	// ����
	//
	D3DXMatrixRotationZ(&matRotate, -90 * ONE_RAD);
	for (int i = 0; i < 4; i++){
		D3DXVec3TransformCoord(&quadPosition[i], &quadPosition[i], &matRotate);
	}

	//���ۿ� 4������
	memcpy(cubePos + 16, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV + 16, quadTexcoord, sizeof(D3DXVECTOR2)* 4);

	//
	// �Ʒ���
	//
	D3DXMatrixRotationZ(&matRotate, -180 * ONE_RAD);
	for (int i = 0; i < 4; i++){
		D3DXVec3TransformCoord(&quadPosition[i], &quadPosition[i], &matRotate);
	}

	//���ۿ� 4������
	memcpy(cubePos + 20, quadPosition, sizeof(D3DXVECTOR3)* 4);
	memcpy(cubeUV + 20, quadTexcoord, sizeof(D3DXVECTOR2)* 4);


	//������� ���� ������  ��ġ�� UV �� ���Ǿ���.



	//
	// �ε���
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



	//�븻�� �������.
	ComputeNormal(
		cubeNor,		//��� ����� ���� �迭
		cubePos,		//������ġ
		24,				//��������
		index,			//�ε��� �迭 DWORD
		36				//�ε��� �迭 ����
		);


	//���̳븻�� ź��Ʈ�� �������.
	ComputeTangentAngBinormal(
		cubeTan,		//����� ���� Tangent �迭
		cubeBin,		//����� ���� Binormal �迭
		cubePos,		//���� ������
		cubeNor,		//���� �븻
		cubeUV,			//���� UV
		index,			//���� �ε��� �迭
		12,				//������ ����
		24);			//���� ����


	
	//
	// ���� ���� ����
	//
	Device->CreateVertexBuffer(
		sizeof(MODELVERTEX)* 24,
		D3DUSAGE_WRITEONLY,
		0,					//FVF ������ ���� 0 �� ����.
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
	// �ε��� ���� ����
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
	// ���� Declaration ����
	//

	//������ ������ �迭������ ������.
	D3DVERTEXELEMENT9 vertElem[6];		//������ ���� ������ + 1 �� ��ŭ �迭�� �����.

	//��������ü�� ó�� ���� ������ Position �̴�.
	vertElem[0].Stream = 0;		//Stream �� 0
	vertElem[0].Offset = 0;		//�޸��� ���� Byte ���� 0
	vertElem[0].Type = D3DDECLTYPE_FLOAT3;		//�ڷ��� ����.. Vector3 �� FLOAT3
	vertElem[0].Method = D3DDECLMETHOD_DEFAULT;		//�ϴ� ������  D3DDECLMETHOD_DEFAUL
	vertElem[0].Usage = D3DDECLUSAGE_POSITION;		//���� Ÿ�� ���� ��ġ
	vertElem[0].UsageIndex = 0;						//UsageIndex �ϴ� 0


	//��������ü�� ���� ���� ������ tangent �̴�.
	vertElem[1].Stream = 0;		//Stream �� 0
	vertElem[1].Offset = 12;	//�޸��� ���� Byte ���� 0
	vertElem[1].Type = D3DDECLTYPE_FLOAT3;		//�ڷ��� ����.. Vector3 �� FLOAT3
	vertElem[1].Method = D3DDECLMETHOD_DEFAULT;		//�ϴ� ������  D3DDECLMETHOD_DEFAUL
	vertElem[1].Usage = D3DDECLUSAGE_TANGENT;		//���� Ÿ�� ���� ��ġ
	vertElem[1].UsageIndex = 0;						//UsageIndex �ϴ� 0


	//��������ü�� ���� ���� ������ binormal �̴�.
	vertElem[2].Stream = 0;		//Stream �� 0
	vertElem[2].Offset = 24;	//�޸��� ���� Byte ���� 0
	vertElem[2].Type = D3DDECLTYPE_FLOAT3;		//�ڷ��� ����.. Vector3 �� FLOAT3
	vertElem[2].Method = D3DDECLMETHOD_DEFAULT;		//�ϴ� ������  D3DDECLMETHOD_DEFAUL
	vertElem[2].Usage = D3DDECLUSAGE_BINORMAL;		//���� Ÿ�� ���� ��ġ
	vertElem[2].UsageIndex = 0;						//UsageIndex �ϴ� 0
	
	// ��������ü�� ���� ���� ������ normal �̴�.
	vertElem[3].Stream = 0;		//Stream �� 0
	vertElem[3].Offset = 36;	//�޸��� ���� Byte ���� 0
	vertElem[3].Type = D3DDECLTYPE_FLOAT3;		//�ڷ��� ����.. Vector3 �� FLOAT3
	vertElem[3].Method = D3DDECLMETHOD_DEFAULT;		//�ϴ� ������  D3DDECLMETHOD_DEFAUL
	vertElem[3].Usage = D3DDECLUSAGE_NORMAL;		//���� Ÿ�� ���� ��ġ
	vertElem[3].UsageIndex = 0;						//UsageIndex �ϴ� 0

	// ��������ü�� ���� ���� ������ normal �̴�.
	vertElem[4].Stream = 0;		//Stream �� 0
	vertElem[4].Offset = 48;	//�޸��� ���� Byte ���� 0
	vertElem[4].Type = D3DDECLTYPE_FLOAT2;		//�ڷ��� ����.. Vector2 �� FLOAT2
	vertElem[4].Method = D3DDECLMETHOD_DEFAULT;		//�ϴ� ������  D3DDECLMETHOD_DEFAUL
	vertElem[4].Usage = D3DDECLUSAGE_TEXCOORD;		//���� Ÿ�� ���� ��ġ
	vertElem[4].UsageIndex = 0;						//UsageIndex �ϴ� 0

	//�������� �˸���.
	//���̻� ����
	D3DVERTEXELEMENT9 end = D3DDECL_END(); //{0xFF,0,D3DDECLTYPE_UNUSED,0,0,0}
	vertElem[5] = end;


	//LPDIRECT3DVERTEXDECLARATION9 ����
	Device->CreateVertexDeclaration(
		vertElem,		//���������� �������� �迭
		&this->decl		//���� LPDIRECT3DVERTEXDECLARATION9 ������
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
	
	//FVF ��ſ� ���°�
	Device->SetVertexDeclaration(this->decl);
	
	Device->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);
}