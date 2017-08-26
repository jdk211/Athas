#include "stdafx.h"
#include "AStarNodePool.h"

stack<AStarNode*> AStarNodePool::poolStack;		//비사용 풀
set<AStarNode*> AStarNodePool::useSet;			//사용되고 있는셋


//초기화
void AStarNodePool::Initialize(int capacity)
{
	for (int i = 0; i < capacity; i++){
		poolStack.push(new AStarNode());
	}
}

//해재
void AStarNodePool::Release()
{
	//모두 리턴
	AllReturn();

	while (poolStack.empty() == false)
	{
		AStarNode* pPopNode = poolStack.top();
		poolStack.pop();

		delete pPopNode;
	}

}

//얻어오기
AStarNode* AStarNodePool::GetAStarNode()
{
	if (poolStack.empty()){
		for (int i = 0; i < 100; i++){
			poolStack.push(new AStarNode());
		}
	}

	AStarNode* pPopNode = poolStack.top();
	poolStack.pop();

	pPopNode->costFromStart = 0;
	pPopNode->costToGoal = 0;
	pPopNode->totalCost = 0;
	pPopNode->pWayPoint = NULL;
	pPopNode->pParent = NULL;

	//사용 여부 신고
	useSet.insert(pPopNode);

	return pPopNode;

}
AStarNode* AStarNodePool::GetAStarNode(cWayPoint* _wayPoint, float _costFromStart, float _costToGoal, float _totalCost, AStarNode* _pParent)
{
	if (poolStack.empty()){
		for (int i = 0; i < 100; i++){
			poolStack.push(new AStarNode());
		}
	}

	AStarNode* pPopNode = poolStack.top();
	poolStack.pop();

	pPopNode->costFromStart = _costFromStart;
	pPopNode->costToGoal = _costToGoal;
	pPopNode->totalCost = _totalCost;
	pPopNode->pWayPoint = _wayPoint;
	pPopNode->pParent = _pParent;

	//사용 여부 신고
	useSet.insert(pPopNode);

	return pPopNode;

}

//반환
void AStarNodePool::GetReturn(AStarNode* returnNode)
{
	set<AStarNode*>::iterator find = useSet.find(returnNode);
	if (find != useSet.end()){

		//찾은 AStarNode 풀로 반환....
		poolStack.push((*find));

		useSet.erase(find);
	}


	
}

//모두 반환
void AStarNodePool::AllReturn()
{
	set<AStarNode*>::iterator iter;
	for (iter = useSet.begin(); iter != useSet.end(); ++iter)
	{
		//찾은 AStarNode 풀로 반환....
		poolStack.push((*iter));
	}
	useSet.clear();
}