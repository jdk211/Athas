#pragma once
#include "cLight.h"


class cRenderTexture;


//
// ���⼺ ����
//
class cLight_Directional : public cLight
{
private:
	cTexQuad		directnalQuad;		//���⼺ ������ �׸��� ���� ��ũ������

	cCamera			lightCamera;		//���⼺ ������ �׸��� ���� �׸��� ���� ī�޶�...
	cRenderTexture*	pShadowMap;			//���̰��� ������ ������ ��

	float			shadowDistance;

public:
	cLight_Directional();
	~cLight_Directional();

	void InitShadowMap(int shadowMapSize, int distance );

	//�ش� �����ÿ� ���� ������ �� ��������
	void BeginShadowRender(cCamera* pViewCam = NULL);

	//�ش� �����ÿ� ���� ������ �� ��������
	void EndShdowRender();

	cCamera* GetLightCamera()
	{
		return &this->lightCamera;
	}

	cRenderTexture* GetShadowRenderTexture()
	{
		return this->pShadowMap;
	}
	

	void SetDistance(float shadowDistance);

	//���� ���� �Լ�
	void Render();

};

