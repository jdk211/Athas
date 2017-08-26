#pragma once

#include <stack>

//AStar �˻����� ���Ǵ� ���
struct AStarNode
{
	AStarNode* pParent;			//�θ� ��尡 ������?  ( �ʰ� ���� ���� �Գ�? )
	cWayPoint* pWayPoint;		//�׳�忡 �����ִ� WayPoint �� ����?
	float	   costFromStart;	//�������������� ����� ��?
	float	   costToGoal;	//������� �ܼ������ �󸶴�?
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
	static stack<AStarNode*> poolStack;		//���� Ǯ
	static set<AStarNode*> useSet;			//���ǰ� �ִ¼�



	//�ʱ�ȭ
	static void Initialize(int capacity);

	//����
	static void Release();

	//������
	static AStarNode* GetAStarNode();
	static AStarNode* GetAStarNode(cWayPoint* _wayPoint, float _costFromStart, float _costToGoal, float _totalCost, AStarNode* _pParent);

	//��ȯ
	static void GetReturn(AStarNode* returnNode);

	//��� ��ȯ
	static void AllReturn();

};

