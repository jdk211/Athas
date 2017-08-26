#include "stdafx.h"
#include "cLight_Directional.h"
#include "cRenderTexture.h"

cLight_Directional::cLight_Directional()
:pShadowMap(NULL)
{
}


cLight_Directional::~cLight_Directional()
{
}

void cLight_Directional::InitShadowMap(int shadowMapSize, int distance)
{
	if (this->pShadowMap != NULL){
		this->pShadowMap->Release();
		SAFE_DELETE(this->pShadowMap);
	}

	//������� �غ� ( ���� ���� ���� )
	this->pShadowMap = new cRenderTexture();
	this->pShadowMap->Init(shadowMapSize, shadowMapSize, D3DFMT_R32F);	

	//���⼺ ������ ī�޶�� ������ ���. ��Ⱦ��� 1.0f;
	this->lightCamera.Ortho = true;
	this->lightCamera.AspectRatio = 1.0f;
	this->lightCamera.OrthoSize = distance;
	this->lightCamera.Far = distance * 2.0f;

	this->shadowDistance = distance;

}

//�ش� �����ÿ� ���� ������ �� ��������
void cLight_Directional::BeginShadowRender(cCamera* pViewCam)
{
	this->pShadowMap->BeginRender(0, 0xFFFFFFFF,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, TRUE);

	if (pViewCam != NULL)
	{
		//����Ʈ ī�޶� �ִ��� ����ī�޶� �տ������ϰԲ� ��ġ�� �����Ѵ�.

		//�� ī�޶��� ��ġ
		D3DXVECTOR3 camPos = pViewCam->Transform.GetWorldPosition();

		//�� ī�޶��� ����
		D3DXVECTOR3 camFront = pViewCam->Transform.GetForward();

		//������ ����
		D3DXVECTOR3 lightDir = this->Transform.GetForward();

		//ī�޶��� ��ġ�� ������ ���������� ������ ������������ü �������� �տ� ����Ʈ ���������� �����ǰ� �Ѵ�.
		D3DXVECTOR3 lightCamPos = camPos +
			(camFront * (shadowDistance * 0.5f)) +		//������ �Ÿ��� ���ݸ�ŭ ī�޶� �������� �̵�
			(-lightDir * shadowDistance);				//����Ʈ �ݴ� �������� ������ �Ÿ� ��ŭ�̵�,


		//����Ʈ ī�޶� lightCamPos ��ġ�� �ְ�
		this->lightCamera.Transform.SetWorldPosition(lightCamPos);
		
		//������ ����Ʈ ����� ����.
		this->lightCamera.Transform.LookDirection(lightDir);

	}// end if
	





	//����Ʈ ī�޶󿡴��� ��� ������Ʈ
	this->lightCamera.UpdateCamera();

}

//�ش� �����ÿ� ���� ������ �� ��������
void cLight_Directional::EndShdowRender()
{
	this->pShadowMap->EndRender();
}
void cLight_Directional::SetDistance(float shadowDistance)
{
	//���⼺ ������ ī�޶�� ������ ���. ��Ⱦ��� 1.0f;
	this->lightCamera.Ortho = true;
	this->lightCamera.AspectRatio = 1.0f;
	this->lightCamera.OrthoSize = shadowDistance;
	this->lightCamera.Far = shadowDistance * 2.0f;

	this->shadowDistance = shadowDistance;
}


//���� ���� �Լ�
void cLight_Directional::Render()
{
	pLightEffect->SetTechnique("DirectionLight");




	//���׼����� ���� ����
	pLightEffect->SetVector(
		"lightDir", &D3DXVECTOR4(this->Transform.GetForward(), 1));

	//�÷� ����
	pLightEffect->SetVector(
		"lightColor", (D3DXVECTOR4*)&this->lightColor);

	//����Ʈ ���� ����
	pLightEffect->SetFloat("intensity", this->intensity);


	//����Ʈ ī�޶��� ViewProjection
	pLightEffect->SetMatrix("matDirectionLightViewProj", 
		&this->lightCamera.GetViewProjectionMatrix());

	//������ ��
	pLightEffect->SetTexture("ShadowTex", this->pShadowMap->GetTexture() );

	UINT pass;
	pLightEffect->Begin(&pass, 0);
	for (UINT i = 0; i < pass; i++)
	{
		pLightEffect->BeginPass(i);
		directnalQuad.Render();
		pLightEffect->EndPass();
	}
	pLightEffect->End();

	
}