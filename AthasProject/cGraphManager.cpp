#include "stdafx.h"

#include "cGraphManager.h"



//����ġ ��
int CompareWayPointNode(WayPointNode a, WayPointNode b){

	if (a.weight < b.weight)
		return -1;
	else if (a.weight > b.weight)
		return 1;
	return 0;

}

//AStarNode TotalCost ��
int CompareAstarNode(AStarNode* a, AStarNode* b){

	if (a->totalCost < b->totalCost)
		return -1;

	else if (a->totalCost > b->totalCost)
		return 1;

	return 0;

}






cGraphManager::cGraphManager()
{
}


cGraphManager::~cGraphManager()
{
}



HRESULT cGraphManager::Init()
{
	AStarNodePool::Initialize(10000);
	return S_OK;
}
void cGraphManager::Release()
{
	AStarNodePool::Release();

}

//�׳� WayPoint ����Ʈ ȭ�鿡 �Ѹ���.
void cGraphManager::ShowWayPoints(vector<cWayPoint*>* pWays, int x, int y, DWORD color)
{
	for (int i = 0; i < pWays->size(); i++)
	{
		DXFONT_MGR->PrintText((*pWays)[i]->name, x, y + i * 20, color);
	}
}


//���� �켱Ž�� 
void cGraphManager::DFSVisit(cWayPoint* pStart, vector<cWayPoint*>* pResult)
{
	//�湮���� �ʱ�ȭ
	visitedSet.clear();


	//��� �� Ŭ����
	pResult->clear();

	//��� �Լ� ȣ��
	DFSRecursive(pStart, pResult);

}



//�ʺ� �켱Ž��
void cGraphManager::BFSVisit(cWayPoint* pStart, vector<cWayPoint*>* pResult)
{
	//�湮���� �ʱ�ȭ
	visitedSet.clear();
	
	//��� �� Ŭ����
	pResult->clear();

	//�ʺ� Ž���� Queue ��	��  ( �̰� Stack ���� �ٲٸ� ���̿켱 Ž���� )
	queue<cWayPoint*> wayQueue;

	//���� WayPoint Ǫ��
	wayQueue.push(pStart);

	//������� �ȴٸ� ���
	while (wayQueue.size() > 0)
	{
		//Queue �� �ִ� ��������Ʈ ����.
		cWayPoint* curWay = wayQueue.front();	//�Ǿտ��ִ°� ����
		wayQueue.pop();				//�տ� ����.


		//�湮�� �Ǿ��ٸ� �Ѱ�....
		if (this->visitedSet.find(curWay) !=
			this->visitedSet.end()){
			continue;
		}


		//���ݳ� �湮ó��
		this->visitedSet.insert(curWay);

		//������� �߰�
		pResult->push_back(curWay);


		//���� ��������Ʈ���� �����ִ� ��Ȯ��
		for (int i = curWay->ways.size() - 1; i >= 0; i--){
			//�������
			cWayPoint* pTo = curWay->ways[i]->pTo;

			//�̹� �湮�Ǿ��ٸ� �ѱ��.
			if (visitedSet.find(pTo) !=
				this->visitedSet.end()){
				continue;
			}

			//�湮�� �ȵǾ��ٸ� Queue �� �߰�
			wayQueue.push(pTo);
		}
	}

}


//����Ʈ�� Table  ( ������ �����ϴµ� ���� ���� ª�� )
//��� ������ mapTable �̸��ϵǴµ�.
//Key toWaypoint ��  Value �� fromWayPoint �̴�.
void cGraphManager::MTS(cWayPoint* pStart, map<cWayPoint*, cWayPoint*>* pResult)
{
	//��� Ŭ����
	pResult->clear();

	//�湮���� �ʱ�ȭ
	visitedSet.clear();


	//����ġ Table
	weightTable.clear();

	//�켱���� ť �ʱ�ȭ
	priorityQueue.Clear();
	priorityQueue.SetCompareFunc(CompareWayPointNode);	//����ġ ���Լ� ����

	//���� ������ �켱���� ť�� �߰�
	//������������ �������� �������� �ʴ´�. ( �������� Node �� Weight ������ �� �� �ִ� )
	priorityQueue.Enqueue(WayPointNode(pStart, 0));

	//����ġ ���̺� ����
	weightTable[pStart] = 0;

	//�켱���� ť�� ������� �ʴٸ�....
	while (priorityQueue.IsEmpty() == false){

		//�켱�������� ��� ����.
		WayPointNode popNode = priorityQueue.Dequeue();

		//����� ��������Ʈ ��´�.
		cWayPoint* pWayPoint = popNode.pWayPoint;

		//�̹� �湮�� �Ǿ��ٸ�...
		if (this->visitedSet.find(pWayPoint) !=
			this->visitedSet.end())
			continue;


		//�湮ó��
		this->visitedSet.insert(pWayPoint);
		
		//�ش��������Ʈ���� ���� �ִ°�
		for (int i = 0; i < pWayPoint->ways.size(); i++)
		{
			//���� ����Ǵ�?
			cWayPoint* pTo = pWayPoint->ways[i]->pTo;

			//����� ���� �̹� �湮�� �Ǿ��ٸ�...
			if (this->visitedSet.find(pTo) !=
				this->visitedSet.end())
				continue;

			//��� ���� �Դ�?
			//cWayPoint* pFrom = pWayPoint->ways[i]->pFrom;
			cWayPoint* pFrom = pWayPoint;


			//�ش� ��� ���� ����ġ
			float weight = pWayPoint->ways[i]->GetDistance();		//����Ʈ��

			//���� ������ ����ġ �ʿ� �߰��� �ȵǾ��ų�?
			//Ȥ�� �߰����� �� ���¶�� ������ ����ġ���� ��ū���� �����ִٸ� ����
			if (this->weightTable.find(pTo) == this->weightTable.end() ||
				this->weightTable[pTo] > weight)
			{
				//����ġ ���̺� ����
				this->weightTable[pTo] = weight;

				//��� ����
				(*pResult)[pTo] = pFrom;

				//���� WayPoint ��� �켱����ť�� �߰�
				this->priorityQueue.Enqueue(WayPointNode(pTo, weight));
			}
		}
	}
}

//���ͽ�Ʈ��. ( ������������ �����ִܰŸ��θ� )
//��� ������ mapTable �̸��ϵǴµ�.
//Key toWaypoint ��  Value �� fromWayPoint �̴�.
void cGraphManager::Dijkstra(cWayPoint* pStart, map<cWayPoint*, cWayPoint*>* pResult)
{
	//��� Ŭ����
	pResult->clear();

	//�湮���� �ʱ�ȭ
	visitedSet.clear();


	//����ġ Table
	weightTable.clear();

	//�켱���� ť �ʱ�ȭ
	priorityQueue.Clear();
	priorityQueue.SetCompareFunc(CompareWayPointNode);	//����ġ ���Լ� ����

	//���� ������ �켱���� ť�� �߰�
	//������������ �������� �������� �ʴ´�. ( �������� Node �� Weight ������ �� �� �ִ� )
	priorityQueue.Enqueue(WayPointNode(pStart, 0));

	//����ġ ���̺� ����
	weightTable[pStart] = 0;

	//�켱���� ť�� ������� �ʴٸ�....
	while (priorityQueue.IsEmpty() == false){

		//�켱�������� ��� ����.
		WayPointNode popNode = priorityQueue.Dequeue();

		//����� ��������Ʈ ��´�.
		cWayPoint* pWayPoint = popNode.pWayPoint;

		//�̹� �湮�� �Ǿ��ٸ�...
		if (this->visitedSet.find(pWayPoint) !=
			this->visitedSet.end())
			continue;


		//�湮ó��
		this->visitedSet.insert(pWayPoint);

		//�ش��������Ʈ���� ���� �ִ°�
		for (int i = 0; i < pWayPoint->ways.size(); i++)
		{
			//���� ����Ǵ�?
			cWayPoint* pTo = pWayPoint->ways[i]->pTo;

			//����� ���� �̹� �湮�� �Ǿ��ٸ�...
			if (this->visitedSet.find(pTo) !=
				this->visitedSet.end())
				continue;

			//��� ���� �Դ�?
			//cWayPoint* pFrom = pWayPoint->ways[i]->pFrom;
			cWayPoint* pFrom = pWayPoint;


			//�ش� ��� ���� ����ġ
			//float weight = pWayPoint->ways[i]->GetDistance();		//����Ʈ��
			float weight = pWayPoint->ways[i]->GetDistance() + this->weightTable[pFrom];		//���ͽ�Ʈ��...

			//���� ������ ����ġ �ʿ� �߰��� �ȵǾ��ų�?
			//Ȥ�� �߰����� �� ���¶�� ������ ����ġ���� ��ū���� �����ִٸ� ����
			if (this->weightTable.find(pTo) == this->weightTable.end() ||
				this->weightTable[pTo] > weight)
			{
				//����ġ ���̺� ����
				this->weightTable[pTo] = weight;

				//��� ����
				(*pResult)[pTo] = pFrom;

				//���� WayPoint ��� �켱����ť�� �߰�
				this->priorityQueue.Enqueue(WayPointNode(pTo, weight));
			}
		}
	}
}



//Astar �˰������� �ִ� ��� ã��
bool cGraphManager::AstarPathFinding(cWayPoint* pStart, cWayPoint* pEnd, vector<cWayPoint*>* pResult)
{
	//���¸���Ʈ�� �����Ҵ�� AStarNode �ϴ� ����
	AStarNodePool::AllReturn();	//���� AStar ��� ��ȯ...
	openList.Clear();

	//���Լ� ����
	openList.SetCompareFunc(CompareAstarNode);

	//�湮 ���� Ŭ����
	visitedSet.clear();

	//������ġ�� ��带 ���� �˻���ġ��....
	AStarNode* pCurNode = AStarNodePool::GetAStarNode(
		pStart,
		0, 0, 0,		//����� ��� 0 �̴�.
		NULL			//�θ�� ����....
		);

	//ó��WayPoint�� �湮ó��
	this->visitedSet.insert(pStart);


	//ó��WayPoint ���� �˻�����
	while (true){

		//���� �˻� ��������Ʈ
		cWayPoint* curWay = pCurNode->pWayPoint;

		//���� ��������Ʈ���� �ֺ��� �����ִ� ���� ����.
		//�����ִ� ����� OpenList �� �߰��Ѵ�...
		for (int i = 0; i < curWay->ways.size(); i++){

			cWayPoint* pTo = curWay->ways[i]->pTo;

			//�̹� �湮�Ǿ��ٸ� �ѱ��.
			if (this->visitedSet.find(pTo) !=
				this->visitedSet.end())
			{
				continue;
			}


			//�湮�Ǿ����� �ʴٸ�...
			else
			{
				//���� ��� �켱���� ť�� �߰�.


				float costFromStart =
					pCurNode->costFromStart + //���ݱ��� �Դ��� �Ÿ�
					curWay->ways[i]->GetDistance(); //To ������ ���


				//������������ �Ÿ��� �ܼ��Ÿ�
				D3DXVECTOR3 dirToTarget = pEnd->position - curWay->position;
				float costToEnd = D3DXVec3Length(&dirToTarget);
				float costTotal = costFromStart + costToEnd;

				//OpenList �� �߰��� ��� �����Ҵ�
				AStarNode* pNewNode = AStarNodePool::GetAStarNode(
					pTo,
					costFromStart,
					costToEnd,
					costTotal,
					pCurNode);


				//���� �߰��� ��尡 �� WayPoint ��?
				if (pTo == pEnd){

					//������ ��忡�� ����..
					AStarNode* pNode = pNewNode;

					//���۱��� Ÿ��ö󰣴�. ( ��Ʈ��ŷ)
					while (pNode->pParent != NULL)
					{
						pResult->push_back(pNode->pWayPoint);
						pNode = pNode->pParent;
					}

					//pResult->reserve(pResult->size());

					//����� reversing ( �ݴ�� )
					for (int i = 0; i < pResult->size() / 2; i++){

						int front = i;
						int back = (pResult->size() - 1) - i;

						if (front >= back)
							break;

						cWayPoint* pTemp;
						pTemp = (*pResult)[back];
						(*pResult)[back] = (*pResult)[front];
						(*pResult)[front] = pTemp;
					}


					return true;
				}

				else{

					//���� ������ openList ���߰�
					this->openList.Enqueue(
						pNewNode);
				}
			}

		}//end For ( ���� �ִ� ���� OpenNode �� �־��� )


		//��� Ž�� ����...
		if (openList.IsEmpty())
			break;

		//���¸���Ʈ�� �߰����� openList ���� ���� total 
		//����� ���� node �� ���� �˻����κ���.
		pCurNode = this->openList.Dequeue();

		//����̰������� ����ǿ�������Ʈ��  �湮ó��
		this->visitedSet.insert(pCurNode->pWayPoint);
	}

	//������� ���´ٸ�
	//��������...
	//start ���� end ������ ���̾���.

	return false;
}

//////////////////////////////////////////////////////////////////////

//���� �켱Ž�� ��Ϳ�
void  cGraphManager::DFSRecursive(cWayPoint* pStart, vector<cWayPoint*>* pResult)
{
	//pStart ��������Ʈ �湮ó��
	visitedSet.insert(pStart);	//�湮 Set �� �߰�.


	//�湮�� ��������Ʈ�̴� pResult �� �߰�����
	pResult->push_back(pStart);


	//Set�� �ݺ���
	set<cWayPoint*>::iterator iter;


	//���� ��������Ʈ���� �����ִ� ��Ȯ��
	for (int i = 0; i < pStart->ways.size(); i++){

		//�������
		cWayPoint* pTo = pStart->ways[i]->pTo;

		//�̹� �湮�Ǿ��ٸ� �ѱ��....
		iter = visitedSet.find(pTo); //Set ���� pTo �� ã��
		if (iter != visitedSet.end())	//�̹� Set ���ִٸ� �湮�� ��������Ʈ ��
			continue;

		//�湮���� �ʾҴٸ� ���
		DFSRecursive(pTo, pResult);
	}



}