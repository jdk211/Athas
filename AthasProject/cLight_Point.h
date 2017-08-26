#pragma once
#include "cLight.h"
class cLight_Point : public cLight
{
public:
	float maxRange;		//빛의 영향을 받는 최대 거리
	float minRange;		//빛의 영향을 받는 최소 거리
	float fallOff;		//감쇠 수치 ( 이 수치가 높으면 급격히 감소함 )

	LPD3DXMESH pLightSphere;		//라이트 구


public:
	cLight_Point();
	~cLight_Point();

	virtual void Render();

};

