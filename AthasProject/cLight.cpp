#include "stdafx.h"
#include "cLight.h"


LPD3DXEFFECT cLight::pLightEffect = NULL;		//라이트 Effect

cLight::cLight()
{
	if (pLightEffect == NULL){
		pLightEffect = RESOURCE_EFFECT->GetResource("./Resources/Effects2/LightRender.fx");
		pLightEffect->SetFloat("halfPixelSizeX", ( 0.5f / WINSIZE_X ) );
		pLightEffect->SetFloat("halfPixelSizeY", ( 0.5f / WINSIZE_Y ) );

	}

	lightColor = D3DXCOLOR(1, 1, 1, 1);
	intensity = 1.0f;

}


cLight::~cLight()
{
}


//Texture 를 셋팅한다.
void cLight::SetRenderTexture(
	LPDIRECT3DTEXTURE9 normalTex,
	LPDIRECT3DTEXTURE9 depthTex)
{
	if (pLightEffect == NULL){
		pLightEffect = RESOURCE_EFFECT->GetResource("./Resources/Effects2/LightRender.fx");
		pLightEffect->SetFloat("halfPixelSizeX", (0.5f / WINSIZE_X));
		pLightEffect->SetFloat("halfPixelSizeY", (0.5f / WINSIZE_Y));
	}

	//셰이더에 Texture 를 셋팅한다.
	pLightEffect->SetTexture("NormalTex", normalTex);
	pLightEffect->SetTexture("DepthTex", depthTex);

}

//ViewProjection 을 셋팅한다
void cLight::SetCamera(cCamera* pCam) 
{
	if (pLightEffect == NULL){
		pLightEffect = RESOURCE_EFFECT->GetResource("./Resources/Effects2/LightRender.fx");
		pLightEffect->SetFloat("halfPixelSizeX", (0.5f / WINSIZE_X));
		pLightEffect->SetFloat("halfPixelSizeY", (0.5f / WINSIZE_Y));
	}

	//ViewProjection 행렬을 구한다.
	D3DXMATRIXA16 matViewProjection = pCam->GetViewProjectionMatrix();

	//ViewProjection 역행렬
	D3DXMATRIXA16 matInvViewProjection;
	D3DXMatrixInverse(&matInvViewProjection, NULL, &matViewProjection);


	//셰이더에 카메라 행렬 셋팅
	pLightEffect->SetMatrix("matViewProjection", &matViewProjection);

	//셰이더에 카메라 역행렬 셋팅
	pLightEffect->SetMatrix("matInvViewProjection", &matInvViewProjection);


	//시점자의 위치 셋팅
	pLightEffect->SetVector("camPos", &D3DXVECTOR4(pCam->Transform.GetWorldPosition(), 1));


}