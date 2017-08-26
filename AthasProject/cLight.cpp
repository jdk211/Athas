#include "stdafx.h"
#include "cLight.h"


LPD3DXEFFECT cLight::pLightEffect = NULL;		//����Ʈ Effect

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


//Texture �� �����Ѵ�.
void cLight::SetRenderTexture(
	LPDIRECT3DTEXTURE9 normalTex,
	LPDIRECT3DTEXTURE9 depthTex)
{
	if (pLightEffect == NULL){
		pLightEffect = RESOURCE_EFFECT->GetResource("./Resources/Effects2/LightRender.fx");
		pLightEffect->SetFloat("halfPixelSizeX", (0.5f / WINSIZE_X));
		pLightEffect->SetFloat("halfPixelSizeY", (0.5f / WINSIZE_Y));
	}

	//���̴��� Texture �� �����Ѵ�.
	pLightEffect->SetTexture("NormalTex", normalTex);
	pLightEffect->SetTexture("DepthTex", depthTex);

}

//ViewProjection �� �����Ѵ�
void cLight::SetCamera(cCamera* pCam) 
{
	if (pLightEffect == NULL){
		pLightEffect = RESOURCE_EFFECT->GetResource("./Resources/Effects2/LightRender.fx");
		pLightEffect->SetFloat("halfPixelSizeX", (0.5f / WINSIZE_X));
		pLightEffect->SetFloat("halfPixelSizeY", (0.5f / WINSIZE_Y));
	}

	//ViewProjection ����� ���Ѵ�.
	D3DXMATRIXA16 matViewProjection = pCam->GetViewProjectionMatrix();

	//ViewProjection �����
	D3DXMATRIXA16 matInvViewProjection;
	D3DXMatrixInverse(&matInvViewProjection, NULL, &matViewProjection);


	//���̴��� ī�޶� ��� ����
	pLightEffect->SetMatrix("matViewProjection", &matViewProjection);

	//���̴��� ī�޶� ����� ����
	pLightEffect->SetMatrix("matInvViewProjection", &matInvViewProjection);


	//�������� ��ġ ����
	pLightEffect->SetVector("camPos", &D3DXVECTOR4(pCam->Transform.GetWorldPosition(), 1));


}