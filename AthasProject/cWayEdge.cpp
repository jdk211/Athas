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


//�Ÿ��� ��� �Ǵ�?
float cWayEdge::GetDistance()
{
	if (pFrom == NULL || pTo == NULL)
		return 0.0f;

	//��������Ʈ ���� �Ÿ��� ��´�.
	return Vector3Distance(&pFrom->position, &pTo->position);

}