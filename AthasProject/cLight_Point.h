#pragma once
#include "cLight.h"
class cLight_Point : public cLight
{
public:
	float maxRange;		//���� ������ �޴� �ִ� �Ÿ�
	float minRange;		//���� ������ �޴� �ּ� �Ÿ�
	float fallOff;		//���� ��ġ ( �� ��ġ�� ������ �ް��� ������ )

	LPD3DXMESH pLightSphere;		//����Ʈ ��


public:
	cLight_Point();
	~cLight_Point();

	virtual void Render();

};

