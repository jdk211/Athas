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

	//쉐도우맵 준비 ( 깊이 값을 쓴다 )
	this->pShadowMap = new cRenderTexture();
	this->pShadowMap->Init(shadowMapSize, shadowMapSize, D3DFMT_R32F);	

	//방향성 광원의 카메라는 직교로 쏜다. 종횡비는 1.0f;
	this->lightCamera.Ortho = true;
	this->lightCamera.AspectRatio = 1.0f;
	this->lightCamera.OrthoSize = distance;
	this->lightCamera.Far = distance * 2.0f;

	this->shadowDistance = distance;

}

//해당 라이팅에 대한 쉐도우 맵 랜더시작
void cLight_Directional::BeginShadowRender(cCamera* pViewCam)
{
	this->pShadowMap->BeginRender(0, 0xFFFFFFFF,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, TRUE);

	if (pViewCam != NULL)
	{
		//라이트 카메라를 최대한 메인카메라 앞에존재하게끔 위치를 설정한다.

		//뷰 카메라의 위치
		D3DXVECTOR3 camPos = pViewCam->Transform.GetWorldPosition();

		//뷰 카메라의 정면
		D3DXVECTOR3 camFront = pViewCam->Transform.GetForward();

		//광원의 방향
		D3DXVECTOR3 lightDir = this->Transform.GetForward();

		//카메라의 위치와 광원의 방향을따져 광원의 방향을유지한체 시점방향 앞에 라이트 투영영역이 유지되게 한다.
		D3DXVECTOR3 lightCamPos = camPos +
			(camFront * (shadowDistance * 0.5f)) +		//쉐도우 거리에 절반만큼 카메라 정면으로 이동
			(-lightDir * shadowDistance);				//라이트 반대 방향으로 쉐도우 거리 만큼이동,


		//라이트 카메라를 lightCamPos 위치에 있게
		this->lightCamera.Transform.SetWorldPosition(lightCamPos);
		
		//방향을 라이트 방향과 같게.
		this->lightCamera.Transform.LookDirection(lightDir);

	}// end if
	





	//라이트 카메라에대한 행렬 업데이트
	this->lightCamera.UpdateCamera();

}

//해당 라이팅에 대한 쉐도우 맵 랜더종료
void cLight_Directional::EndShdowRender()
{
	this->pShadowMap->EndRender();
}
void cLight_Directional::SetDistance(float shadowDistance)
{
	//방향성 광원의 카메라는 직교로 쏜다. 종횡비는 1.0f;
	this->lightCamera.Ortho = true;
	this->lightCamera.AspectRatio = 1.0f;
	this->lightCamera.OrthoSize = shadowDistance;
	this->lightCamera.Far = shadowDistance * 2.0f;

	this->shadowDistance = shadowDistance;
}


//광원 랜더 함수
void cLight_Directional::Render()
{
	pLightEffect->SetTechnique("DirectionLight");




	//방항성광원 방향 셋팅
	pLightEffect->SetVector(
		"lightDir", &D3DXVECTOR4(this->Transform.GetForward(), 1));

	//컬러 셋팅
	pLightEffect->SetVector(
		"lightColor", (D3DXVECTOR4*)&this->lightColor);

	//라이트 강도 셋팅
	pLightEffect->SetFloat("intensity", this->intensity);


	//라이트 카메라의 ViewProjection
	pLightEffect->SetMatrix("matDirectionLightViewProj", 
		&this->lightCamera.GetViewProjectionMatrix());

	//쉐도우 맵
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