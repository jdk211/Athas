#pragma once

#include "cSingletonBase.h"
#include "cWayPoint.h"
#include "cWayEdge.h"
#include <set>
#include <queue>
#include "cPriorityQueue.h"
#include "AStarNodePool.h"

//���ͽ�Ʈ�� ����Ʈ���� �����Ҷ�
//���Ǵ� WayPointNode
struct WayPointNode {

	cWayPoint*		pWayPoint;		//�ش� ��忡 �����ִ� ��������Ʈ
	float			weight;			//����ġ

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
	set<cWayPoint*>					visitedSet;		//�湮�� WayPoint �� ����� ���´�.
	map<cWayPoint*, float>			weightTable;	//����ġ ���� Table
	cPriorityQueue<WayPointNode>	priorityQueue;	//�켱����ť ( ����Ʈ���� ���ͽ�Ʈ�� ���鶧 ���� )

	cPriorityQueue<AStarNode*>		openList;		//Astar �˻��� ��ΰ��ɼ��� �ִ� wayPoint �� ( total ����� ���� ���� �ֵ���� ���´� )

public:
	cGraphManager();
	~cGraphManager();

	HRESULT Init();
	void Release();

	//�׳� WayPoint ����Ʈ ȭ�鿡 �Ѹ���.
	void ShowWayPoints(vector<cWayPoint*>* pWays, int x, int y, DWORD color);

	//���� �켱Ž�� 
	void DFSVisit(cWayPoint* pStart, vector<cWayPoint*>* pResult);

	//�ʺ� �켱Ž��
	void BFSVisit(cWayPoint* pStart, vector<cWayPoint*>* pResult);

	//����Ʈ�� Table  ( ������ �����ϴµ� ���� ���� ª�� )
	//��� ������ mapTable �̸��ϵǴµ�.
	//Key toWaypoint ��  Value �� fromWayPoint �̴�.
	void MTS(cWayPoint* pStart, map<cWayPoint*, cWayPoint*>* pResult);

	//���ͽ�Ʈ��. ( ������������ �����ִܰŸ��θ� )
	//��� ������ mapTable �̸��ϵǴµ�.
	//Key toWaypoint ��  Value �� fromWayPoint �̴�.
	void Dijkstra(cWayPoint* pStart, map<cWayPoint*, cWayPoint*>* pResult);


	//Astar �˰������� �ִ� ��� ã��
	bool AstarPathFinding(cWayPoint* pStart, cWayPoint* pEnd, vector<cWayPoint*>* pResult);




private:
	//���� �켱Ž�� ��Ϳ�
	void DFSRecursive(cWayPoint* pStart, vector<cWayPoint*>* pResult);



};

#define GRAPH_MGR cGraphManager::GetInstance()