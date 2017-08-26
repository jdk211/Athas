#pragma once

#include <string>
#include <vector>

class cWayEdge;
class cCamera;

class cWayPoint
{
public:
	D3DXVECTOR3				position;	//��������Ʈ�� ��ġ
	string					name;		//��������Ʈ �̸�
	vector<cWayEdge*>		ways;		//����� ���� ����


public:
	cWayPoint();
	cWayPoint(string name, D3DXVECTOR3 position);
	~cWayPoint();

	//�ش� ��������Ʈ�� ����
	void LinkWayPoint(cWayPoint* pWayPoint);

	//�׸���.
	void DrawGizmos(cCamera* pCam = NULL);

	//���̰�θ� Ŭ�����Ѵ�.
	void ClearWayEdge();






};

