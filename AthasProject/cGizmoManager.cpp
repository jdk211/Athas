#include "stdafx.h"
#include "cGizmoManager.h"


cGizmoManager::cGizmoManager()
:pGridVertices( NULL)
{
}


cGizmoManager::~cGizmoManager()
{
}


//매니져 초기화 
HRESULT cGizmoManager::Init(void)
{
	return S_OK;
}

//매니져 해재
void cGizmoManager::Release(void)
{
	SAFE_DELETE_ARR(pGridVertices);
}

//그리드 초기화
void cGizmoManager::InitGrid(int gridCellNum, float gridCellSize)
{
	//
	// 그리드 생성
	//
	this->gridCellNum = gridCellNum;
	this->gridCellSize = gridCellSize;

	//그리드 총정점 갯수
	int verticesNum = (this->gridCellNum * 2 + 1 ) * 4;
	verticesNum += 2;
	verticesNum += 12;


	//그리드 라인갯수 
	this->gridLineNum = verticesNum / 2;


	//정점 갯수만큼 만들어
	this->pGridVertices = new GIZMOVERTEX[verticesNum];

	//그리드 라인길이
	float lineLength = gridCellSize * gridCellNum * 2;
	float halfLineLength = lineLength * 0.5f;

	//Z 라인 정점 배치
	unsigned int vidx = 0;
	for (int z = -gridCellNum; z <= gridCellNum; z++)
	{
		DWORD color = 0xff808080;
		if (z == 0){
			color = 0xFFFF0000;
		}

		//z 왼쪽
		this->pGridVertices[vidx].color = color;
		this->pGridVertices[vidx].position =
			D3DXVECTOR3(-halfLineLength, 0, z * this->gridCellSize);
		vidx++;


		//z 오른쪽
		this->pGridVertices[vidx].color = color;
		this->pGridVertices[vidx].position =
			D3DXVECTOR3( halfLineLength, 0, z * this->gridCellSize);
		vidx++;

	}

	//X 라인 정점 배치
	for (int x = -gridCellNum; x <= gridCellNum; x++)
	{
		DWORD color = 0xff808080;
		if (x == 0){
			color = 0xff0000ff;
		}


		//x 정면
		this->pGridVertices[vidx].color = color;
		this->pGridVertices[vidx++].position = D3DXVECTOR3(x * this->gridCellSize, 0, halfLineLength);
		//x 뒤
		this->pGridVertices[vidx].color = color;
		this->pGridVertices[vidx++].position = D3DXVECTOR3(x * this->gridCellSize, 0, -halfLineLength);
	}


	//y 축작대기 추가
	//y 위쪽
	this->pGridVertices[vidx].color = 0xFF00FF00;
	this->pGridVertices[vidx].position = D3DXVECTOR3(0, halfLineLength, 0);
	vidx++;
	//y 아래쪽
	this->pGridVertices[vidx].color = 0xFF00FF00;
	this->pGridVertices[vidx].position = D3DXVECTOR3(0, -halfLineLength, 0);
	vidx++;

	//각 축 화살표
	//X 축
	this->pGridVertices[vidx].color = 0xffff0000;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(halfLineLength, 0, 0);
	this->pGridVertices[vidx].color = 0xffff0000;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(halfLineLength - this->gridCellSize, 0, this->gridCellSize);
	this->pGridVertices[vidx].color = 0xffff0000;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(halfLineLength, 0, 0);
	this->pGridVertices[vidx].color = 0xffff0000;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(halfLineLength - this->gridCellSize, 0, -this->gridCellSize);


	//Y 축
	this->pGridVertices[vidx].color = 0xff00ff00;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(0, halfLineLength, 0);
	this->pGridVertices[vidx].color = 0xff00ff00;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(this->gridCellSize, halfLineLength - this->gridCellSize, 0);
	this->pGridVertices[vidx].color = 0xff00ff00;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(0, halfLineLength, 0);
	this->pGridVertices[vidx].color = 0xff00ff00;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(-this->gridCellSize, halfLineLength - this->gridCellSize, 0);

	//Z 축
	this->pGridVertices[vidx].color = 0xff0000ff;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(0, 0, halfLineLength);
	this->pGridVertices[vidx].color = 0xff0000ff;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(this->gridCellSize, 0, halfLineLength - this->gridCellSize);
	this->pGridVertices[vidx].color = 0xff0000ff;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(0, 0, halfLineLength);
	this->pGridVertices[vidx].color = 0xff0000ff;
	this->pGridVertices[vidx++].position = D3DXVECTOR3(-this->gridCellSize, 0, halfLineLength - this->gridCellSize);

}

//그리드 랜더 함수
void cGizmoManager::RenderGrid()
{

	//그리드가 셋팅되어있다면...
	if (this->pGridVertices){

		//
		// GridRender
		//

		//월드 행렬을 초기화 하고 그린다.
		D3DXMATRIXA16 matGridWorld;
		D3DXMatrixIdentity(&matGridWorld);
		Device->SetTransform(D3DTS_WORLD, &matGridWorld);

		Device->SetFVF(GIZMOVERTEX::FVF);
		Device->DrawPrimitiveUP(
			D3DPT_LINELIST,
			this->gridLineNum,
			this->pGridVertices,
			sizeof(GIZMOVERTEX));
	}

}

//라인 랜더
void cGizmoManager::RenderLine(const D3DXVECTOR3* pStart, const D3DXVECTOR3* pEnd, DWORD color)
{


#ifdef _DEBUG

	//이전 World 행렬에 셋팅된 행렬값을 얻어놓는다.
	D3DXMATRIXA16 matPrevWorld;
	Device->GetTransform(D3DTS_WORLD, &matPrevWorld);

	//Device 의 월드 초기화
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	Device->SetTransform(D3DTS_WORLD, &matWorld);

	lineVertices[0].position = *pStart;
	lineVertices[1].position = *pEnd;
	lineVertices[0].color = color;
	lineVertices[1].color = color;

	Device->SetFVF(GIZMOVERTEX::FVF);
	Device->DrawPrimitiveUP(
		D3DPT_LINELIST,
		1,
		lineVertices,
		sizeof(GIZMOVERTEX));


	//이전 상태로 돌리자
	Device->SetTransform(D3DTS_WORLD, &matPrevWorld);

#endif


}

void cGizmoManager::DrawWireSphere(D3DXVECTOR3* worldCenter, float radius, DWORD color)
{

#ifdef _DEBUG	

	//이전에 셋팅된 월드 행렬
	D3DXMATRIXA16 matPrevWorld;
	Device->GetTransform(D3DTS_WORLD, &matPrevWorld);		//그리기전 행렬상태가 어떠니?

	//월드 초기화
	D3DXMATRIXA16 matIden;
	D3DXMatrixIdentity(&matIden);
	Device->SetTransform(D3DTS_WORLD, &matIden);

	//라이팅을 끄기전에 현제 라이팅 셋팅여부가 어떻게 되는지 기억하자.
	DWORD dwPrevLight = 0;
	Device->GetRenderState(D3DRS_LIGHTING, &dwPrevLight);



	float angle = 0;

	GIZMOVERTEX vertices[108];
	for (int i = 0; i < 36; i++)
	{
		int idxX = i;
		int idxY = 36 + i;
		int idxZ = 72 + i;

		angle = i * 10 * ONE_RAD;

		vertices[idxX].position = *worldCenter + D3DXVECTOR3(0, sin(angle), cos(angle)) * radius;
		vertices[idxY].position = *worldCenter + D3DXVECTOR3(cos(angle), 0, sin(angle)) * radius;
		vertices[idxZ].position = *worldCenter + D3DXVECTOR3(cos(angle), sin(angle), 0) * radius;

		vertices[idxX].color = color;
		vertices[idxY].color = color;
		vertices[idxZ].color = color;
	}

	WORD indices[216];
	int idxX = 0;
	int idxY = 36;
	int idxZ = 72;

	//indices[idxY * 2] = idxY;
	//indices[idxY * 2 + 1] = idxY + 1;
	//indices[idxZ * 2] = idxZ;
	//indices[idxZ * 2 + 1] = idxZ + 1;

	for (int i = 0; i < 36; i++){
		idxX = i;
		indices[idxX * 2] = idxX;
		indices[idxX * 2 + 1] = (idxX + 1) % 36;

		idxY = 36 + i;
		indices[idxY * 2] = idxX + 36;
		indices[idxY * 2 + 1] = ((idxX + 1) % 36) + 36;

		/*idxZ = 72 + i;
		indices[idxZ * 2] = idxX + 72;
		indices[idxZ * 2 + 1] = ((idxX + 1) % 36) + 72;*/
	}
	//FVF 셋팅
	Device->SetFVF(GIZMOVERTEX::FVF);

	Device->DrawIndexedPrimitiveUP(
		D3DPT_LINELIST,
		0,
		108,
		216 / 2,
		indices,
		D3DFMT_INDEX16,
		vertices,
		sizeof(GIZMOVERTEX));


	//라이팅 이전상태로 돌려라...
	Device->SetRenderState(D3DRS_LIGHTING, dwPrevLight);

	//월드 행렬 이전 상태롤 돌려라..
	Device->SetTransform(D3DTS_WORLD, &matPrevWorld);
#endif
}


void  cGizmoManager::DrawWireObbBox(
	D3DXVECTOR3* min, D3DXVECTOR3* max, D3DXMATRIXA16* mat, DWORD color)
{


	D3DXVECTOR3 p[8] = {
		D3DXVECTOR3(min->x, min->y, min->z),
		D3DXVECTOR3(min->x, max->y, min->z),
		D3DXVECTOR3(max->x, max->y, min->z),
		D3DXVECTOR3(max->x, min->y, min->z),

		D3DXVECTOR3(min->x, min->y, max->z),
		D3DXVECTOR3(min->x, max->y, max->z),
		D3DXVECTOR3(max->x, max->y, max->z),
		D3DXVECTOR3(max->x, min->y, max->z)
	};


	//위에것을 월드 변환한다.
	for (int i = 0; i < 8; i++)
		D3DXVec3TransformCoord(p + i, p + i, mat);
	
	this->RenderLine(p + 0, p + 1, color);
	this->RenderLine(p + 1, p + 2, color);
	this->RenderLine(p + 2, p + 3, color);
	this->RenderLine(p + 3, p + 0, color);

	this->RenderLine(p + 4, p + 5, color);
	this->RenderLine(p + 5, p + 6, color);
	this->RenderLine(p + 6, p + 7, color);
	this->RenderLine(p + 7, p + 4, color);

	this->RenderLine(p + 4, p + 0, color);
	this->RenderLine(p + 5, p + 1, color);
	this->RenderLine(p + 6, p + 2, color);
	this->RenderLine(p + 7, p + 3, color);


}