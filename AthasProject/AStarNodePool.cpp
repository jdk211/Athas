#include "stdafx.h"
#include "AStarNodePool.h"

stack<AStarNode*> AStarNodePool::poolStack;		//���� Ǯ
set<AStarNode*> AStarNodePool::useSet;			//���ǰ� �ִ¼�


//�ʱ�ȭ
void AStarNodePool::Initialize(int capacity)
{
	for (int i = 0; i < capacity; i++){
		poolStack.push(new AStarNode());
	}
}

//����
void AStarNodePool::Release()
{
	//��� ����
	AllReturn();

	while (poolStack.empty() == false)
	{
		AStarNode* pPopNode = poolStack.top();
		poolStack.pop();

		delete pPopNode;
	}

}

//������
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

	//��� ���� �Ű�
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

	//��� ���� �Ű�
	useSet.insert(pPopNode);

	return pPopNode;

}

//��ȯ
void AStarNodePool::GetReturn(AStarNode* returnNode)
{
	set<AStarNode*>::iterator find = useSet.find(returnNode);
	if (find != useSet.end()){

		//ã�� AStarNode Ǯ�� ��ȯ....
		poolStack.push((*find));

		useSet.erase(find);
	}


	
}

//��� ��ȯ
void AStarNodePool::AllReturn()
{
	set<AStarNode*>::iterator iter;
	for (iter = useSet.begin(); iter != useSet.end(); ++iter)
	{
		//ã�� AStarNode Ǯ�� ��ȯ....
		poolStack.push((*iter));
	}
	useSet.clear();
}