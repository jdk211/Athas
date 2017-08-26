#pragma once


//2차원 벡터
class MyVector
{
public:
	float x;
	float y;
};

class cCircle
{
public:
	MyVector	centerPos;		//원의 중심위치
	float		radius;			//원의 반지름

public:
	cCircle();
	~cCircle();

	void Render(HDC hdc);
};

