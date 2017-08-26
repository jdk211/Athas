#include "stdafx.h"
#include "cWayPoint.h"
#include "cWayEdge.h"

cWayPoint::cWayPoint()
{
	this->name = "";
	this->position = D3DXVECTOR3(0, 0, 0);
}

cWayPoint::cWayPoint(string name, D3DXVECTOR3 position)
{
	this->name = name;
	this->position = position;
}

cWayPoint::~cWayPoint()
{
	this->ClearWayEdge();
}



//�ش� ��������Ʈ�� ����
void cWayPoint::LinkWayPoint(cWayPoint* pWayPoint)
{
	//�̹� �ִ��� Ȯ��
	for (int i = 0; i < this->ways.size(); i++){

		//�̹� ����� ��ΰ� �����Ѵٸ� ������...
		if (this->ways[i]->pTo == pWayPoint)
		{
			return;
		}
	}

	cWayEdge* newEdge = new cWayEdge();
	newEdge->pFrom = this;
	newEdge->pTo = pWayPoint;
	this->ways.push_back(newEdge);
}

//�׸���.
void cWayPoint::DrawGizmos(cCamera* pCam)
{
	GIZMO_MGR->DrawWireSphere(&this->position, 1.0f, 0xFFFFFFFF);

	if (pCam){

		//ȭ����ġ�� ���
		POINT screenPoint;

		if (pCam->GetScreenPoint(&screenPoint, this->position))
		{
			//ȭ����ġ���� ���̸��� �Ѹ���.
			DXFONT_MGR->PrintText(this->name,
				screenPoint.x,
				screenPoint.y,
				0xFFFFFFFF);
		}
	}

	//����� ����� �׸���.
	for (int i = 0; i < this->ways.size(); i++)
	{

		cWayEdge* edge = this->ways[i];

		//������ �ߴ´�.
		D3DXVECTOR3 from = edge->pFrom->position;
		D3DXVECTOR3 to = edge->pTo->position;



		D3DXVECTOR3 dir = from - to;
		D3DXVec3Normalize(&dir, &dir);
		to += dir;


		GIZMO_MGR->RenderLine(&from, &to, 0xFFFFFFFF);
		GIZMO_MGR->DrawWireSphere(&to, 0.1f, 0xFFFFFFFF);


	}




}

//���̰�θ� Ŭ�����Ѵ�.
void cWayPoint::ClearWayEdge()
{
	for (int i = 0; i < this->ways.size(); i++)
		SAFE_DELETE(this->ways[i]);

	this->ways.clear();

}
