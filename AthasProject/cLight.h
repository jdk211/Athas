#pragma once

#include "cTexQuad.h"

class cCamera;

class cLight
{
public:
	static LPD3DXEFFECT pLightEffect;		//라이트 Effect

public:
	cTransform Transform;		//광원의 Transform
	D3DXCOLOR lightColor;		//광원 색상
	float intensity;			//광원의 강도

public:
	cLight();
	~cLight();

	//광원 랜더 함수
	virtual void Render() = 0;


	//Texture 를 셋팅한다.
	static void SetRenderTexture(
		LPDIRECT3DTEXTURE9 normalTex,
		LPDIRECT3DTEXTURE9 depthTex);

	//ViewProjection 을 셋팅한다
	static void SetCamera(cCamera* pCam);
};

