#pragma once

#include "cTexQuad.h"

class cCamera;

class cLight
{
public:
	static LPD3DXEFFECT pLightEffect;		//����Ʈ Effect

public:
	cTransform Transform;		//������ Transform
	D3DXCOLOR lightColor;		//���� ����
	float intensity;			//������ ����

public:
	cLight();
	~cLight();

	//���� ���� �Լ�
	virtual void Render() = 0;


	//Texture �� �����Ѵ�.
	static void SetRenderTexture(
		LPDIRECT3DTEXTURE9 normalTex,
		LPDIRECT3DTEXTURE9 depthTex);

	//ViewProjection �� �����Ѵ�
	static void SetCamera(cCamera* pCam);
};

