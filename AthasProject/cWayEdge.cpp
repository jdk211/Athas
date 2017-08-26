#include "stdafx.h"
#include "cWayEdge.h"
#include "cWayPoint.h"

cWayEdge::cWayEdge()
: pFrom(NULL), pTo(NULL)
{
}


cWayEdge::~cWayEdge()
{
}


//거리는 어떻게 되니?
float cWayEdge::GetDistance()
{
	if (pFrom == NULL || pTo == NULL)
		return 0.0f;

	//웨이포인트 간의 거리를 얻는다.
	return Vector3Distance(&pFrom->position, &pTo->position);

}