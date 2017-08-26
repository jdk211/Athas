#include "stdafx.h"
#include "cBoundBox.h"


cBoundBox::cBoundBox()
{
	m_Color = D3DCOLOR_XRGB(255, 255, 0);
}


cBoundBox::~cBoundBox()
{
}

//���� �� ��ȯ�� 8 ���� ���� ��´�. ( outBoxPos 8 �� �迭������ )
void cBoundBox::GetWorldBox(const cTransform* pTrans, D3DXVECTOR3* outBoxPos)
{
	//   5-------6
	//  /|      /|
	// 1-------2 |
	// | 4-----|-7
	// |/      |/
	// 0-------3 

	//���� 8 ���� ������ ���Ѵ�
	D3DXVECTOR3 Vertices[8];
	Vertices[0] = D3DXVECTOR3(this->localMinPos.x, this->localMinPos.y, this->localMinPos.z);
	Vertices[1] = D3DXVECTOR3(this->localMinPos.x, this->localMaxPos.y, this->localMinPos.z);
	Vertices[2] = D3DXVECTOR3(this->localMaxPos.x, this->localMaxPos.y, this->localMinPos.z);
	Vertices[3] = D3DXVECTOR3(this->localMaxPos.x, this->localMinPos.y, this->localMinPos.z);
	Vertices[4] = D3DXVECTOR3(this->localMinPos.x, this->localMinPos.y, this->localMaxPos.z);
	Vertices[5] = D3DXVECTOR3(this->localMinPos.x, this->localMaxPos.y, this->localMaxPos.z);
	Vertices[6] = D3DXVECTOR3(this->localMaxPos.x, this->localMaxPos.y, this->localMaxPos.z);
	Vertices[7] = D3DXVECTOR3(this->localMaxPos.x, this->localMinPos.y, this->localMaxPos.z);

	//���� �� ��ȯ
	D3DXMATRIXA16 matFinal = pTrans->GetFinalMatrix();
	for (int i = 0; i < 8; i++){
		D3DXVec3TransformCoord(outBoxPos + i, Vertices + i, &matFinal);
	}

}

//���� �� ��ȯ�� 8 ���� ���� MinMax �� ��´�.
void cBoundBox::GetWorldAABBMinMax(const cTransform* pTrans, D3DXVECTOR3* min, D3DXVECTOR3* max)
{
	//��ȯ�� �ڽ��� ������ġ 8 ���� ��´�.
	D3DXVECTOR3 worldPos[8];
	GetWorldBox(pTrans, worldPos);

	//���� 8 ���� �ִ� �ּҸ� ������.
	*min = worldPos[0];
	*max = worldPos[0];

	for (int i = 1; i < 8; i++){


		// X ����
		if (min->x > worldPos[i].x){
			min->x = worldPos[i].x;
		}
		else if (max->x < worldPos[i].x){
			max->x = worldPos[i].x;
		}

		// Y ����
		if (min->y > worldPos[i].y){
			min->y = worldPos[i].y;
		}
		else if (max->y < worldPos[i].y){
			max->y = worldPos[i].y;
		}

		// Z ����
		if (min->z > worldPos[i].z){
			min->z = worldPos[i].z;
		}
		else if (max->z < worldPos[i].z){
			max->z = worldPos[i].z;
		}
	}
}

//���� ����
void cBoundBox::SetBound(const D3DXVECTOR3* pCenter, const D3DXVECTOR3* pHalfSize)
{
	//�θ��� SetBound �� ����....
	D3DXVECTOR3 sphereSize = *pHalfSize;
	//sphereSize = sphereSize * 2;
	cBoundSphere::SetBound(pCenter, &sphereSize);

	this->localMinPos = (*pCenter) - (*pHalfSize);
	this->localMaxPos = (*pCenter) + (*pHalfSize);

}
void cBoundBox::SetMinMax(const D3DXVECTOR3* pMin, const D3DXVECTOR3* pMax)
{
	this->localMinPos = *pMin;
	this->localMaxPos = *pMax;


	//�θ��� SetMinMax �� ����....
	cBoundSphere::SetMinMax(pMin, pMax);


}

//�������Ѵ�.
void cBoundBox::RenderGizmos(const cTransform* pTrans) 
{
	//�θ��� RenderGizmos �� ����....
	//cBoundSphere::RenderGizmos(pTrans);

	GIZMO_MGR->DrawWireObbBox(
		&this->localMinPos,
		&this->localMaxPos,
		&pTrans->GetFinalMatrix(), m_Color);



	/*
	//��ȯ�� �ڽ��� ���� MinMax �� ����.
	D3DXVECTOR3 min;
	D3DXVECTOR3 max;
	this->GetWorldAABBMinMax(pTrans, &min, &max);


	DWORD color = 0xFF008000;
	GIZMO_MGR->RenderLine(&D3DXVECTOR3(min.x, min.y, min.z), &D3DXVECTOR3(max.x, min.y, min.z), color);
	GIZMO_MGR->RenderLine(&D3DXVECTOR3(max.x, min.y, min.z), &D3DXVECTOR3(max.x, max.y, min.z), color);
	GIZMO_MGR->RenderLine(&D3DXVECTOR3(max.x, max.y, min.z), &D3DXVECTOR3(min.x, max.y, min.z), color);
	GIZMO_MGR->RenderLine(&D3DXVECTOR3(min.x, max.y, min.z), &D3DXVECTOR3(min.x, min.y, min.z), color);

	GIZMO_MGR->RenderLine(&D3DXVECTOR3(min.x, min.y, max.z), &D3DXVECTOR3(max.x, min.y, max.z), color);
	GIZMO_MGR->RenderLine(&D3DXVECTOR3(max.x, min.y, max.z), &D3DXVECTOR3(max.x, max.y, max.z), color);
	GIZMO_MGR->RenderLine(&D3DXVECTOR3(max.x, max.y, max.z), &D3DXVECTOR3(min.x, max.y, max.z), color);
	GIZMO_MGR->RenderLine(&D3DXVECTOR3(min.x, max.y, max.z), &D3DXVECTOR3(min.x, min.y, max.z), color);

	GIZMO_MGR->RenderLine(&D3DXVECTOR3(min.x, min.y, min.z), &D3DXVECTOR3(min.x, min.y, max.z), color);
	GIZMO_MGR->RenderLine(&D3DXVECTOR3(max.x, min.y, min.z), &D3DXVECTOR3(max.x, min.y, max.z), color);
	GIZMO_MGR->RenderLine(&D3DXVECTOR3(max.x, max.y, min.z), &D3DXVECTOR3(max.x, max.y, max.z), color);
	GIZMO_MGR->RenderLine(&D3DXVECTOR3(min.x, max.y, min.z), &D3DXVECTOR3(min.x, max.y, max.z), color);*/

}

