#include "stdafx.h"
#include "cCamera.h"
#include "cFrustum.h"

cCamera::cCamera()
: AspectRatio((float)WINSIZE_X / WINSIZE_Y),
Near(0.1f),
Far(100),
FOV(60 * ONE_RAD),
Ortho(false),
OrthoSize(5.0f),
vecPosition(D3DXVECTOR3(0,0,0)),
isTrue(false)
{
	this->pFrustum = new cFrustum();
}

cCamera::~cCamera()
{
	delete this->pFrustum;
}

//ī�޶� ����� ������Ʈ �Ѵ�.
void cCamera::UpdateCamera()
{
	//View ���
	D3DXMatrixInverse(
		&matView,
		NULL,
		&this->Transform.GetFinalMatrix());

	//������� 
	if (this->Ortho )
	{
		D3DXMatrixOrthoLH(
			&matProjection,
			OrthoSize * this->AspectRatio,	//�������� ���� ������
			OrthoSize,					//�������� ���� ������
			this->Near,				//ī�޶� Near
			this->Far				//ī�޶� Far
			);

		
	}
	else
	{
		//D3DXMatrixPerspectiveFovLH ���� ��������� �����.
		D3DXMatrixPerspectiveFovLH(
			&matProjection,
			this->FOV,
			this->AspectRatio,
			this->Near,
			this->Far);

	}




	this->matViewProjection = 
		this->matView * this->matProjection;

	//�������ҵ� ������Ʈ �Ѵ�.
	this->pFrustum->UpdateFrustum(
		&this->matViewProjection);

}

//ī�޶� ����� Device �� �����Ѵ�.
void cCamera::UpdateToDevice()
{
	/*if (isTrue)
	{
		D3DXVECTOR3 m_vLookAt = D3DXVECTOR3(0, 0, 0);
		D3DXVECTOR3 m_vEye = D3DXVECTOR3(0, 5, -10);
		D3DXVECTOR3 m_vUp = D3DXVECTOR3(0, 1, 0);
		m_vLookAt = vecPosition;
		m_vEye = m_vEye + vecPosition;
		
		(&matView, &m_vEye, &m_vLookAt, &m_vUp);
	}*/
	Device->SetTransform(D3DTS_VIEW, &this->matView);
	Device->SetTransform(D3DTS_PROJECTION, &this->matProjection);
}

//3���� ���� �����ǿ� ���� ��ũ����ġ�� �˾Ƴ���. (����)
bool cCamera::GetScreenPoint(POINT* pOut, const D3DXVECTOR3& worldPos)
{
	//ī�޶��� ViewProjection ����� ��´�.
	D3DXMATRIXA16 matViewProj = this->matViewProjection;

	D3DXVECTOR3 clipPos;
	D3DXVec3TransformCoord(&clipPos, &worldPos, &matViewProj);

	//�������� �� clipPos;
	//				*-------*<-( 1, 1, 1 )
	//			   /|      /|
	//			  *-------* |
	//            | *-----|-*
	//		      |/      |/
	//(-1,-1, 0)->*-------* 


	//ȭ�� ���鿡 ������ ����...
	if (clipPos.z < 0.0f || clipPos.z > 1.0f)
	{
		return false;
	}



	//-1 ~ 1 ������ ������ 0 ~ 1 ������ ������...
	float screenPosX = (clipPos.x + 1.0f) * 0.5f;
	float screenPosY = 1.0f - ( (clipPos.y + 1.0f) * 0.5f );	//������ ��ǥ�� �� �ٲ۴�.


	pOut->x = static_cast<long>( screenPosX * WINSIZE_X );
	pOut->y = static_cast<long>( screenPosY * WINSIZE_Y );


	return true;
}

//ȭ���� ��ġ�� ������ ī�޶��� ���̸� ��´�
void cCamera::GetWorldRay(LPRay pOutRay, POINT ptScreen)
{
	D3DXVECTOR2 screenPos;
	screenPos.x = ptScreen.x;
	screenPos.y = ptScreen.y;

	GetWorldRay(pOutRay, screenPos);
}
void cCamera::GetWorldRay(LPRay pOutRay, D3DXVECTOR2 screenPos)
{
	//��ũ���� ��ġ ������ ���� ( 0 ~ 1 ���̷� ������ �ȴ� )
	float factorX = screenPos.x / WINSIZE_X;
	float factorY = 1.0f - (screenPos.y / WINSIZE_Y);	//��ũ�� ��ġ�� ���� 0 �̴� �ݴ�� �ٲ۴�.	

	//�̰��� ( -1 �� 1 ������ ������ )
	factorX = factorX * 2.0f - 1.0f;
	factorY = factorY * 2.0f - 1.0f;

	//
	// ���� ����
	//
	D3DXVECTOR3 direction(factorX, factorY, 1.0f);

	//���� ����� ����
	D3DXMATRIXA16 matInvProj;
	D3DXMatrixInverse(&matInvProj, NULL, &this->matProjection);
	D3DXVec3TransformCoord(&direction, &direction, &matInvProj);

	//�俪 ��� ����
	D3DXMATRIXA16 matInvView;
	D3DXMatrixInverse(&matInvView, NULL, &this->matView);
	D3DXVec3TransformNormal(&direction, &direction, &matInvView);

	//����ȭ
	D3DXVec3Normalize(&direction, &direction);

	//����
	pOutRay->origin = this->Transform.GetWorldPosition();		
	pOutRay->direction = direction;

}

//�ش� ī�޶�� �������Ѵ�.
void cCamera::BeginRender(DWORD clearFlag)
{
	if (clearFlag != 0){
		Device->Clear(
			0,				//û���� ������ D3DRECT �迭 ����		( ��ü Ŭ���� 0 )
			NULL,			//û���� ������ D3DRECT �迭 ������		( ��ü Ŭ���� NULL )
			clearFlag,		//û�ҵ� ���� �÷��� ( D3DCLEAR_TARGET �÷�����, D3DCLEAR_ZBUFFER ���̹���, D3DCLEAR_STENCIL ���ٽǹ���
			0xFF202020,			//�÷����۸� û���ϰ� ä���� ����( 0xAARRGGBB )
			1.0f,				//���̹��۸� û���Ұ� ( 0 ~ 1, 0 �� ī�޶󿡼� ���ϰ���� 1 �� ī�޶󿡼� ���� �� )
			0					//���ٽ� ���۸� ä�ﰪ
			);
	}


	this->UpdateCamera();
	this->UpdateToDevice();
}

void cCamera::RenderFrustumGizmo()
{
	this->pFrustum->RenderGizmos();
}