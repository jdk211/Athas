#pragma once


class cWayPoint;

//웨이포인트와 웨이포인트를 연결하는 
//클래스
class cWayEdge
{
public:
	cWayPoint*			pFrom;	//어디로 부터 왔니?
	cWayPoint*			pTo;		//어디로 가니?



public:
	cWayEdge();
	~cWayEdge();

	//거리는 어떻게 되니?
	float GetDistance();
};

