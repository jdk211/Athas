#pragma once

#include <string>
#include <vector>

class cWayEdge;
class cCamera;

class cWayPoint
{
public:
	D3DXVECTOR3				position;	//웨이포인트의 위치
	string					name;		//웨이포인트 이름
	vector<cWayEdge*>		ways;		//연결된 웨이 정보


public:
	cWayPoint();
	cWayPoint(string name, D3DXVECTOR3 position);
	~cWayPoint();

	//해당 웨이포인트와 연결
	void LinkWayPoint(cWayPoint* pWayPoint);

	//그린다.
	void DrawGizmos(cCamera* pCam = NULL);

	//웨이경로를 클리어한다.
	void ClearWayEdge();






};

