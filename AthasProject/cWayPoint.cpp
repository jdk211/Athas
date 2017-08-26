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



//해당 웨이포인트와 연결
void cWayPoint::LinkWayPoint(cWayPoint* pWayPoint)
{
	//이미 있는지 확인
	for (int i = 0; i < this->ways.size(); i++){

		//이미 연결된 경로가 존재한다면 하지마...
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

//그린다.
void cWayPoint::DrawGizmos(cCamera* pCam)
{
	GIZMO_MGR->DrawWireSphere(&this->position, 1.0f, 0xFFFFFFFF);

	if (pCam){

		//화면위치를 얻어
		POINT screenPoint;

		if (pCam->GetScreenPoint(&screenPoint, this->position))
		{
			//화면위치에다 내이름을 뿌리자.
			DXFONT_MGR->PrintText(this->name,
				screenPoint.x,
				screenPoint.y,
				0xFFFFFFFF);
		}
	}

	//연결된 길들을 그린다.
	for (int i = 0; i < this->ways.size(); i++)
	{

		cWayEdge* edge = this->ways[i];

		//라인을 긋는다.
		D3DXVECTOR3 from = edge->pFrom->position;
		D3DXVECTOR3 to = edge->pTo->position;



		D3DXVECTOR3 dir = from - to;
		D3DXVec3Normalize(&dir, &dir);
		to += dir;


		GIZMO_MGR->RenderLine(&from, &to, 0xFFFFFFFF);
		GIZMO_MGR->DrawWireSphere(&to, 0.1f, 0xFFFFFFFF);


	}




}

//웨이경로를 클리어한다.
void cWayPoint::ClearWayEdge()
{
	for (int i = 0; i < this->ways.size(); i++)
		SAFE_DELETE(this->ways[i]);

	this->ways.clear();

}
