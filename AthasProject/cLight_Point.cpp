#include "stdafx.h"
#include "cLight_Point.h"


cLight_Point::cLight_Point()
{
	//점광원 랜더에 사용될 구를 그리자..
	D3DXCreateSphere(
		Device,
		1.0f,
		30, 30,
		&pLightSphere,
		NULL);

	this->maxRange = 10.0f;
	this->minRange = 2.0f;
	this->fallOff = 1.0f;
}


cLight_Point::~cLight_Point()
{
	SAFE_RELEASE(pLightSphere);
}


//광원 랜더 함수
void cLight_Point::Render()
{
	pLightEffect->SetTechnique("PointLight");

	//점광원의 광원위치
	pLightEffect->SetVector(
		"lightPos", &D3DXVECTOR4(this->Transform.GetWorldPosition(), 1));

	//거리 셋팅
	pLightEffect->SetFloat("maxRange", maxRange);
	pLightEffect->SetFloat("minRange", minRange);
	pLightEffect->SetFloat("fallOff", fallOff);


	//컬러 셋팅
	pLightEffect->SetVector(
		"lightColor", (D3DXVECTOR4*)&this->lightColor);

	//라이트 강도 셋팅
	pLightEffect->SetFloat("intensity", this->intensity);

	




	//월드 매트릭스
	D3DXMATRIXA16 matWorld = this->Transform.GetFinalMatrix();

	//광원 영역에 따른 스케일
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, this->maxRange, this->maxRange, this->maxRange);

	D3DXMATRIXA16 matFinal = matScale * matWorld;

	//월드 행렬 셋팅
	pLightEffect->SetMatrix("matWorld", &matFinal);


	UINT pass;
	pLightEffect->Begin(&pass, 0);
	for (UINT i = 0; i < pass; i++)
	{
		pLightEffect->BeginPass(i);
		
		this->pLightSphere->DrawSubset(0);

		pLightEffect->EndPass();
	}
	pLightEffect->End();
}