#pragma once


//2���� ����
class MyVector
{
public:
	float x;
	float y;
};

class cCircle
{
public:
	MyVector	centerPos;		//���� �߽���ġ
	float		radius;			//���� ������

public:
	cCircle();
	~cCircle();

	void Render(HDC hdc);
};

