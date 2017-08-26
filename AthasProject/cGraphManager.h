#pragma once

#include "cSingletonBase.h"
#include "cWayPoint.h"
#include "cWayEdge.h"
#include <set>
#include <queue>
#include "cPriorityQueue.h"
#include "AStarNodePool.h"

//다익스트라나 신장트리를 구성할때
//사용되는 WayPointNode
struct WayPointNode {

	cWayPoint*		pWayPoint;		//해당 노드에 물려있는 웨이포인트
	float			weight;			//가중치

	WayPointNode()
		:pWayPoint(NULL), weight(0.0f)
	{
	}

	WayPointNode(cWayPoint* _pWayPoint, float _weight)
		:pWayPoint(_pWayPoint), weight(_weight)
	{
	}
};





class cGraphManager : public cSingletonBase<cGraphManager>
{
private:
	set<cWayPoint*>					visitedSet;		//방문된 WayPoint 의 기록이 남는다.
	map<cWayPoint*, float>			weightTable;	//가중치 갱신 Table
	cPriorityQueue<WayPointNode>	priorityQueue;	//우선순위큐 ( 신장트리나 다익스트라 만들때 사용됨 )

	cPriorityQueue<AStarNode*>		openList;		//Astar 검색시 경로가능성이 있는 wayPoint 들 ( total 비용이 가장 작은 애들부터 나온다 )

public:
	cGraphManager();
	~cGraphManager();

	HRESULT Init();
	void Release();

	//그냥 WayPoint 리스트 화면에 뿌린다.
	void ShowWayPoints(vector<cWayPoint*>* pWays, int x, int y, DWORD color);

	//깊이 우선탐색 
	void DFSVisit(cWayPoint* pStart, vector<cWayPoint*>* pResult);

	//너비 우선탐색
	void BFSVisit(cWayPoint* pStart, vector<cWayPoint*>* pResult);

	//신장트리 Table  ( 모든길을 연결하는데 길이 가장 짧은 )
	//결과 값으로 mapTable 이리턴되는데.
	//Key toWaypoint 고  Value 가 fromWayPoint 이다.
	void MTS(cWayPoint* pStart, map<cWayPoint*, cWayPoint*>* pResult);

	//다익스트라. ( 시작지점에서 가장최단거리로만 )
	//결과 값으로 mapTable 이리턴되는데.
	//Key toWaypoint 고  Value 가 fromWayPoint 이다.
	void Dijkstra(cWayPoint* pStart, map<cWayPoint*, cWayPoint*>* pResult);


	//Astar 알고리즘으로 최단 경로 찾아
	bool AstarPathFinding(cWayPoint* pStart, cWayPoint* pEnd, vector<cWayPoint*>* pResult);




private:
	//깊이 우선탐색 재귀용
	void DFSRecursive(cWayPoint* pStart, vector<cWayPoint*>* pResult);



};

#define GRAPH_MGR cGraphManager::GetInstance()