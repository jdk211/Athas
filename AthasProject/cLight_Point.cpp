#include "stdafx.h"
#include "cLight_Point.h"


cLight_Point::cLight_Point()
{
	//������ ������ ���� ���� �׸���..
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


//���� ���� �Լ�
void cLight_Point::Render()
{
	pLightEffect->SetTechnique("PointLight");

	//�������� ������ġ
	pLightEffect->SetVector(
		"lightPos", &D3DXVECTOR4(this->Transform.GetWorldPosition(), 1));

	//�Ÿ� ����
	pLightEffect->SetFloat("maxRange", maxRange);
	pLightEffect->SetFloat("minRange", minRange);
	pLightEffect->SetFloat("fallOff", fallOff);


	//�÷� ����
	pLightEffect->SetVector(
		"lightColor", (D3DXVECTOR4*)&this->lightColor);

	//����Ʈ ���� ����
	pLightEffect->SetFloat("intensity", this->intensity);

	




	//���� ��Ʈ����
	D3DXMATRIXA16 matWorld = this->Transform.GetFinalMatrix();

	//���� ������ ���� ������
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, this->maxRange, this->maxRange, this->maxRange);

	D3DXMATRIXA16 matFinal = matScale * matWorld;

	//���� ��� ����
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