#pragma once


class cWayPoint;

//��������Ʈ�� ��������Ʈ�� �����ϴ� 
//Ŭ����
class cWayEdge
{
public:
	cWayPoint*			pFrom;	//���� ���� �Դ�?
	cWayPoint*			pTo;		//���� ����?



public:
	cWayEdge();
	~cWayEdge();

	//�Ÿ��� ��� �Ǵ�?
	float GetDistance();
};

