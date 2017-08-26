#include "stdafx.h"
#include "cCircle.h"


cCircle::cCircle()
{
	centerPos.x = 0.0f;
	centerPos.y = 0.0f;

	radius = 0.0f;
}


cCircle::~cCircle()
{
}


void cCircle::Render(HDC hdc)
{
	int left = (int)(this->centerPos.x - this->radius);
	int right = (int)(this->centerPos.x + this->radius);
	int top = (int)(this->centerPos.y - this->radius);
	int bottom = (int)(this->centerPos.y + this->radius);

	Ellipse(hdc, left, top, right, bottom);
}