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

//카메라 행렬을 업데이트 한다.
void cCamera::UpdateCamera()
{
	//View 행렬
	D3DXMatrixInverse(
		&matView,
		NULL,
		&this->Transform.GetFinalMatrix());

	//투영행렬 
	if (this->Ortho )
	{
		D3DXMatrixOrthoLH(
			&matProjection,
			OrthoSize * this->AspectRatio,	//직교투영 가로 사이즈
			OrthoSize,					//직교투영 세로 사이즈
			this->Near,				//카메라 Near
			this->Far				//카메라 Far
			);

		
	}
	else
	{
		//D3DXMatrixPerspectiveFovLH 원근 투영행렬을 만든다.
		D3DXMatrixPerspectiveFovLH(
			&matProjection,
			this->FOV,
			this->AspectRatio,
			this->Near,
			this->Far);

	}




	this->matViewProjection = 
		this->matView * this->matProjection;

	//프러스텀도 업데이트 한다.
	this->pFrustum->UpdateFrustum(
		&this->matViewProjection);

}

//카메라 행렬을 Device 에 적용한다.
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

//3차원 월드 포지션에 대한 스크린위치를 알아낸다. (숙제)
bool cCamera::GetScreenPoint(POINT* pOut, const D3DXVECTOR3& worldPos)
{
	//카메라의 ViewProjection 행렬을 얻는다.
	D3DXMATRIXA16 matViewProj = this->matViewProjection;

	D3DXVECTOR3 clipPos;
	D3DXVec3TransformCoord(&clipPos, &worldPos, &matViewProj);

	//동차공간 을 clipPos;
	//				*-------*<-( 1, 1, 1 )
	//			   /|      /|
	//			  *-------* |
	//            | *-----|-*
	//		      |/      |/
	//(-1,-1, 0)->*-------* 


	//화면 정면에 없으면 실패...
	if (clipPos.z < 0.0f || clipPos.z > 1.0f)
	{
		return false;
	}



	//-1 ~ 1 사이의 범위를 0 ~ 1 사이의 범위로...
	float screenPosX = (clipPos.x + 1.0f) * 0.5f;
	float screenPosY = 1.0f - ( (clipPos.y + 1.0f) * 0.5f );	//윈도우 좌표계 로 바꾼다.


	pOut->x = static_cast<long>( screenPosX * WINSIZE_X );
	pOut->y = static_cast<long>( screenPosY * WINSIZE_Y );


	return true;
}

//화면의 위치를 가지고 카메라의 레이를 얻는다
void cCamera::GetWorldRay(LPRay pOutRay, POINT ptScreen)
{
	D3DXVECTOR2 screenPos;
	screenPos.x = ptScreen.x;
	screenPos.y = ptScreen.y;

	GetWorldRay(pOutRay, screenPos);
}
void cCamera::GetWorldRay(LPRay pOutRay, D3DXVECTOR2 screenPos)
{
	//스크린의 위치 비율을 얻자 ( 0 ~ 1 사이로 나오게 된다 )
	float factorX = screenPos.x / WINSIZE_X;
	float factorY = 1.0f - (screenPos.y / WINSIZE_Y);	//스크린 위치는 상이 0 이니 반대로 바꾼다.	

	//이것을 ( -1 과 1 사이의 비율로 )
	factorX = factorX * 2.0f - 1.0f;
	factorY = factorY * 2.0f - 1.0f;

	//
	// 동차 방향
	//
	D3DXVECTOR3 direction(factorX, factorY, 1.0f);

	//투영 역행렬 적용
	D3DXMATRIXA16 matInvProj;
	D3DXMatrixInverse(&matInvProj, NULL, &this->matProjection);
	D3DXVec3TransformCoord(&direction, &direction, &matInvProj);

	//뷰역 행렬 적용
	D3DXMATRIXA16 matInvView;
	D3DXMatrixInverse(&matInvView, NULL, &this->matView);
	D3DXVec3TransformNormal(&direction, &direction, &matInvView);

	//정규화
	D3DXVec3Normalize(&direction, &direction);

	//대입
	pOutRay->origin = this->Transform.GetWorldPosition();		
	pOutRay->direction = direction;

}

//해당 카메라로 랜더링한다.
void cCamera::BeginRender(DWORD clearFlag)
{
	if (clearFlag != 0){
		Device->Clear(
			0,				//청소할 영역의 D3DRECT 배열 갯수		( 전체 클리어 0 )
			NULL,			//청소할 영역의 D3DRECT 배열 포인터		( 전체 클리어 NULL )
			clearFlag,		//청소될 버퍼 플레그 ( D3DCLEAR_TARGET 컬러버퍼, D3DCLEAR_ZBUFFER 깊이버퍼, D3DCLEAR_STENCIL 스텐실버퍼
			0xFF202020,			//컬러버퍼를 청소하고 채워질 색상( 0xAARRGGBB )
			1.0f,				//깊이버퍼를 청소할값 ( 0 ~ 1, 0 이 카메라에서 제일가까운 1 이 카메라에서 제일 먼 )
			0					//스텐실 버퍼를 채울값
			);
	}


	this->UpdateCamera();
	this->UpdateToDevice();
}

void cCamera::RenderFrustumGizmo()
{
	this->pFrustum->RenderGizmos();
}