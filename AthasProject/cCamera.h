#pragma once

class cFrustum;

class cCamera 
{
public:
	cTransform Transform;

	float AspectRatio;	//화면 종횡비
	float Near;			//카메라 Near
	float Far;			//카메라 Far
	float FOV;			//카메라 화각 ( 라디안 )

	bool Ortho;			//이게 false 이면 원근 투영, true 이면 직교투영
	float OrthoSize;	//직교크기 세로크기가 됨

	cFrustum* pFrustum;

private:

	D3DXMATRIXA16 matView;				//뷰 행렬
	D3DXMATRIXA16 matProjection;		//투영 행렬
	D3DXMATRIXA16 matViewProjection;	//뷰 * 투영 행렬
	D3DXVECTOR3   vecPosition;
	bool		  isTrue;


public:
	cCamera();
	~cCamera();


	//카메라 행렬을 업데이트 한다.
	void UpdateCamera();

	//카메라 행렬을 Device 에 적용한다.
	void UpdateToDevice();


	//해당 카메라로 랜더링한다.
	void BeginRender( DWORD clearFlag );


	//3차원 월드 포지션에 대한 스크린위치를 알아낸다. (숙제)
	bool GetScreenPoint(POINT* pOut, const D3DXVECTOR3& worldPos);


	//화면의 위치를 가지고 카메라의 레이를 얻는다
	void GetWorldRay(LPRay pOutRay, POINT ptScreen);
	void GetWorldRay(LPRay pOutRay, D3DXVECTOR2 screenPos);

	cTransform GetTransForm()
	{
		return Transform;
	}


	D3DXMATRIXA16 GetViewMatrix()
	{
		return this->matView;
	}

	D3DXMATRIXA16 GetProjectionMatrix()
	{
		return this->matProjection;
	}

	D3DXMATRIXA16 GetViewProjectionMatrix()
	{
		return this->matViewProjection;
	}

	void SetVecPosition(D3DXVECTOR3 vec)
	{
		isTrue = true; 
		vecPosition = vec;
	}

	void SetCameraFar(float fFar) { Far = fFar; }

	void RenderFrustumGizmo();
};

