#include "stdafx.h"

#include "cGraphManager.h"



//가중치 비교
int CompareWayPointNode(WayPointNode a, WayPointNode b){

	if (a.weight < b.weight)
		return -1;
	else if (a.weight > b.weight)
		return 1;
	return 0;

}

//AStarNode TotalCost 비교
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

//그냥 WayPoint 리스트 화면에 뿌린다.
void cGraphManager::ShowWayPoints(vector<cWayPoint*>* pWays, int x, int y, DWORD color)
{
	for (int i = 0; i < pWays->size(); i++)
	{
		DXFONT_MGR->PrintText((*pWays)[i]->name, x, y + i * 20, color);
	}
}


//깊이 우선탐색 
void cGraphManager::DFSVisit(cWayPoint* pStart, vector<cWayPoint*>* pResult)
{
	//방문여부 초기화
	visitedSet.clear();


	//결과 값 클리어
	pResult->clear();

	//재귀 함수 호출
	DFSRecursive(pStart, pResult);

}



//너비 우선탐색
void cGraphManager::BFSVisit(cWayPoint* pStart, vector<cWayPoint*>* pResult)
{
	//방문여부 초기화
	visitedSet.clear();
	
	//결과 값 클리어
	pResult->clear();

	//너비 탐색용 Queue 준	비  ( 이거 Stack 으로 바꾸면 깊이우선 탐색됨 )
	queue<cWayPoint*> wayQueue;

	//시작 WayPoint 푸쉬
	wayQueue.push(pStart);

	//비어있지 안다면 계속
	while (wayQueue.size() > 0)
	{
		//Queue 에 있는 웨이포인트 뺀다.
		cWayPoint* curWay = wayQueue.front();	//맨앞에있는거 참조
		wayQueue.pop();				//앞에 뺀다.


		//방문이 되었다면 넘겨....
		if (this->visitedSet.find(curWay) !=
			this->visitedSet.end()){
			continue;
		}


		//지금놈 방문처리
		this->visitedSet.insert(curWay);

		//결과에도 추가
		pResult->push_back(curWay);


		//현제 웨이포인트에서 갈수있는 길확인
		for (int i = curWay->ways.size() - 1; i >= 0; i--){
			//진행방향
			cWayPoint* pTo = curWay->ways[i]->pTo;

			//이미 방문되었다면 넘긴다.
			if (visitedSet.find(pTo) !=
				this->visitedSet.end()){
				continue;
			}

			//방문이 안되었다면 Queue 에 추가
			wayQueue.push(pTo);
		}
	}

}


//신장트리 Table  ( 모든길을 연결하는데 길이 가장 짧은 )
//결과 값으로 mapTable 이리턴되는데.
//Key toWaypoint 고  Value 가 fromWayPoint 이다.
void cGraphManager::MTS(cWayPoint* pStart, map<cWayPoint*, cWayPoint*>* pResult)
{
	//결과 클리어
	pResult->clear();

	//방문여부 초기화
	visitedSet.clear();


	//가중치 Table
	weightTable.clear();

	//우선순위 큐 초기화
	priorityQueue.Clear();
	priorityQueue.SetCompareFunc(CompareWayPointNode);	//가중치 비교함수 셋팅

	//시작 지점을 우선순위 큐에 추가
	//시작점에대한 연결비용은 존재하지 않는다. ( 연결비용은 Node 의 Weight 값으로 볼 수 있다 )
	priorityQueue.Enqueue(WayPointNode(pStart, 0));

	//가중치 테이블 갱신
	weightTable[pStart] = 0;

	//우선순위 큐가 비어있지 않다면....
	while (priorityQueue.IsEmpty() == false){

		//우선순위에서 노드 뺀다.
		WayPointNode popNode = priorityQueue.Dequeue();

		//노드의 웨이포인트 얻는다.
		cWayPoint* pWayPoint = popNode.pWayPoint;

		//이미 방문이 되었다면...
		if (this->visitedSet.find(pWayPoint) !=
			this->visitedSet.end())
			continue;


		//방문처리
		this->visitedSet.insert(pWayPoint);
		
		//해당웨이포인트에서 갈수 있는곳
		for (int i = 0; i < pWayPoint->ways.size(); i++)
		{
			//어디로 연결되니?
			cWayPoint* pTo = pWayPoint->ways[i]->pTo;

			//연결된 쪽이 이미 방문이 되었다면...
			if (this->visitedSet.find(pTo) !=
				this->visitedSet.end())
				continue;

			//어디서 부터 왔니?
			//cWayPoint* pFrom = pWayPoint->ways[i]->pFrom;
			cWayPoint* pFrom = pWayPoint;


			//해당 길로 가는 가중치
			float weight = pWayPoint->ways[i]->GetDistance();		//신장트리

			//진행 방향이 가중치 맵에 추가가 안되었거나?
			//혹은 추가가가 된 상태라면 지금의 가중치보다 더큰값이 쓰여있다면 갱신
			if (this->weightTable.find(pTo) == this->weightTable.end() ||
				this->weightTable[pTo] > weight)
			{
				//가중치 테이블에 갱신
				this->weightTable[pTo] = weight;

				//결과 갱신
				(*pResult)[pTo] = pFrom;

				//현제 WayPoint 노드 우선순위큐에 추가
				this->priorityQueue.Enqueue(WayPointNode(pTo, weight));
			}
		}
	}
}

//다익스트라. ( 시작지점에서 가장최단거리로만 )
//결과 값으로 mapTable 이리턴되는데.
//Key toWaypoint 고  Value 가 fromWayPoint 이다.
void cGraphManager::Dijkstra(cWayPoint* pStart, map<cWayPoint*, cWayPoint*>* pResult)
{
	//결과 클리어
	pResult->clear();

	//방문여부 초기화
	visitedSet.clear();


	//가중치 Table
	weightTable.clear();

	//우선순위 큐 초기화
	priorityQueue.Clear();
	priorityQueue.SetCompareFunc(CompareWayPointNode);	//가중치 비교함수 셋팅

	//시작 지점을 우선순위 큐에 추가
	//시작점에대한 연결비용은 존재하지 않는다. ( 연결비용은 Node 의 Weight 값으로 볼 수 있다 )
	priorityQueue.Enqueue(WayPointNode(pStart, 0));

	//가중치 테이블 갱신
	weightTable[pStart] = 0;

	//우선순위 큐가 비어있지 않다면....
	while (priorityQueue.IsEmpty() == false){

		//우선순위에서 노드 뺀다.
		WayPointNode popNode = priorityQueue.Dequeue();

		//노드의 웨이포인트 얻는다.
		cWayPoint* pWayPoint = popNode.pWayPoint;

		//이미 방문이 되었다면...
		if (this->visitedSet.find(pWayPoint) !=
			this->visitedSet.end())
			continue;


		//방문처리
		this->visitedSet.insert(pWayPoint);

		//해당웨이포인트에서 갈수 있는곳
		for (int i = 0; i < pWayPoint->ways.size(); i++)
		{
			//어디로 연결되니?
			cWayPoint* pTo = pWayPoint->ways[i]->pTo;

			//연결된 쪽이 이미 방문이 되었다면...
			if (this->visitedSet.find(pTo) !=
				this->visitedSet.end())
				continue;

			//어디서 부터 왔니?
			//cWayPoint* pFrom = pWayPoint->ways[i]->pFrom;
			cWayPoint* pFrom = pWayPoint;


			//해당 길로 가는 가중치
			//float weight = pWayPoint->ways[i]->GetDistance();		//신장트리
			float weight = pWayPoint->ways[i]->GetDistance() + this->weightTable[pFrom];		//다익스트라...

			//진행 방향이 가중치 맵에 추가가 안되었거나?
			//혹은 추가가가 된 상태라면 지금의 가중치보다 더큰값이 쓰여있다면 갱신
			if (this->weightTable.find(pTo) == this->weightTable.end() ||
				this->weightTable[pTo] > weight)
			{
				//가중치 테이블에 갱신
				this->weightTable[pTo] = weight;

				//결과 갱신
				(*pResult)[pTo] = pFrom;

				//현제 WayPoint 노드 우선순위큐에 추가
				this->priorityQueue.Enqueue(WayPointNode(pTo, weight));
			}
		}
	}
}



//Astar 알고리즘으로 최단 경로 찾아
bool cGraphManager::AstarPathFinding(cWayPoint* pStart, cWayPoint* pEnd, vector<cWayPoint*>* pResult)
{
	//오픈리스트에 동적할당된 AStarNode 싹다 제거
	AStarNodePool::AllReturn();	//사용된 AStar 모두 반환...
	openList.Clear();

	//비교함수 셋팅
	openList.SetCompareFunc(CompareAstarNode);

	//방문 여부 클리어
	visitedSet.clear();

	//시작위치의 노드를 현제 검색위치로....
	AStarNode* pCurNode = AStarNodePool::GetAStarNode(
		pStart,
		0, 0, 0,		//비용은 모두 0 이다.
		NULL			//부모는 없다....
		);

	//처음WayPoint를 방문처리
	this->visitedSet.insert(pStart);


	//처음WayPoint 부터 검색시작
	while (true){

		//현재 검색 웨이포인트
		cWayPoint* curWay = pCurNode->pWayPoint;

		//현제 웨이포인트에서 주변에 갈수있는 길을 본다.
		//갈수있는 길들을 OpenList 에 추가한다...
		for (int i = 0; i < curWay->ways.size(); i++){

			cWayPoint* pTo = curWay->ways[i]->pTo;

			//이미 방문되었다면 넘긴다.
			if (this->visitedSet.find(pTo) !=
				this->visitedSet.end())
			{
				continue;
			}


			//방문되어있지 않다면...
			else
			{
				//오픈 노드 우선순위 큐에 추가.


				float costFromStart =
					pCurNode->costFromStart + //지금까지 왔던길 거리
					curWay->ways[i]->GetDistance(); //To 까지의 비용


				//목적지까지의 거리는 단순거리
				D3DXVECTOR3 dirToTarget = pEnd->position - curWay->position;
				float costToEnd = D3DXVec3Length(&dirToTarget);
				float costTotal = costFromStart + costToEnd;

				//OpenList 에 추가할 노드 동적할당
				AStarNode* pNewNode = AStarNodePool::GetAStarNode(
					pTo,
					costFromStart,
					costToEnd,
					costTotal,
					pCurNode);


				//지금 추가된 노드가 골 WayPoint 니?
				if (pTo == pEnd){

					//마지막 노드에서 부터..
					AStarNode* pNode = pNewNode;

					//시작까지 타고올라간다. ( 백트래킹)
					while (pNode->pParent != NULL)
					{
						pResult->push_back(pNode->pWayPoint);
						pNode = pNode->pParent;
					}

					//pResult->reserve(pResult->size());

					//결과값 reversing ( 반대로 )
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

					//위의 정보로 openList 에추가
					this->openList.Enqueue(
						pNewNode);
				}
			}

		}//end For ( 갈수 있는 길모두 OpenNode 에 넣었음 )


		//경로 탐색 실패...
		if (openList.IsEmpty())
			break;

		//오픈리스트에 추가한후 openList 에서 가장 total 
		//비용이 작은 node 를 현재 검색노드로본다.
		pCurNode = this->openList.Dequeue();

		//비용이가장작은 노드의웨이포인트를  방문처리
		this->visitedSet.insert(pCurNode->pWayPoint);
	}

	//여기까지 나온다면
	//비정상리턴...
	//start 에서 end 까지의 길이없다.

	return false;
}

//////////////////////////////////////////////////////////////////////

//깊이 우선탐색 재귀용
void  cGraphManager::DFSRecursive(cWayPoint* pStart, vector<cWayPoint*>* pResult)
{
	//pStart 웨이포인트 방문처리
	visitedSet.insert(pStart);	//방문 Set 에 추가.


	//방문된 웨이포인트이니 pResult 에 추가하자
	pResult->push_back(pStart);


	//Set의 반복자
	set<cWayPoint*>::iterator iter;


	//현제 웨이포인트에서 갈수있는 길확인
	for (int i = 0; i < pStart->ways.size(); i++){

		//진행방향
		cWayPoint* pTo = pStart->ways[i]->pTo;

		//이미 방문되었다면 넘긴다....
		iter = visitedSet.find(pTo); //Set 에서 pTo 를 찾고
		if (iter != visitedSet.end())	//이미 Set 에있다면 방문된 웨이포인트 임
			continue;

		//방문되지 않았다면 재귀
		DFSRecursive(pTo, pResult);
	}



}