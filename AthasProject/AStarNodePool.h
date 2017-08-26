#pragma once

#include <stack>

//AStar 검색에서 사용되는 노드
struct AStarNode
{
	AStarNode* pParent;			//부모 노드가 누구니?  ( 너가 어디로 부터 왔나? )
	cWayPoint* pWayPoint;		//그노드에 물려있는 WayPoint 가 뭐니?
	float	   costFromStart;	//시작지점부터의 비용이 얼마?
	float	   costToGoal;	//골까지의 단순비용이 얼마니?
	float	   totalCost;	//costFromStart + costToGoal

	AStarNode(){
		pParent = NULL;
		pWayPoint = NULL;
		costFromStart = 0.0f;
		costToGoal = 0.0f;
		totalCost = 0.0f;
	}

	AStarNode(cWayPoint* _wayPoint, float _costFromStart, float _costToGoal, float _totalCost, AStarNode* _pParent)
	{
		pParent = _pParent;
		pWayPoint = _wayPoint;
		costFromStart = _costFromStart;
		costToGoal = _costToGoal;
		totalCost = _totalCost;
	}

};


class AStarNodePool
{
public:
	static stack<AStarNode*> poolStack;		//비사용 풀
	static set<AStarNode*> useSet;			//사용되고 있는셋



	//초기화
	static void Initialize(int capacity);

	//해재
	static void Release();

	//얻어오기
	static AStarNode* GetAStarNode();
	static AStarNode* GetAStarNode(cWayPoint* _wayPoint, float _costFromStart, float _costToGoal, float _totalCost, AStarNode* _pParent);

	//반환
	static void GetReturn(AStarNode* returnNode);

	//모두 반환
	static void AllReturn();

};

