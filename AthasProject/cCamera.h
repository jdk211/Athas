#pragma once

class cFrustum;

class cCamera 
{
public:
	cTransform Transform;

	float AspectRatio;	//ȭ�� ��Ⱦ��
	float Near;			//ī�޶� Near
	float Far;			//ī�޶� Far
	float FOV;			//ī�޶� ȭ�� ( ���� )

	bool Ortho;			//�̰� false �̸� ���� ����, true �̸� ��������
	float OrthoSize;	//����ũ�� ����ũ�Ⱑ ��

	cFrustum* pFrustum;

private:

	D3DXMATRIXA16 matView;				//�� ���
	D3DXMATRIXA16 matProjection;		//���� ���
	D3DXMATRIXA16 matViewProjection;	//�� * ���� ���
	D3DXVECTOR3   vecPosition;
	bool		  isTrue;


public:
	cCamera();
	~cCamera();


	//ī�޶� ����� ������Ʈ �Ѵ�.
	void UpdateCamera();

	//ī�޶� ����� Device �� �����Ѵ�.
	void UpdateToDevice();


	//�ش� ī�޶�� �������Ѵ�.
	void BeginRender( DWORD clearFlag );


	//3���� ���� �����ǿ� ���� ��ũ����ġ�� �˾Ƴ���. (����)
	bool GetScreenPoint(POINT* pOut, const D3DXVECTOR3& worldPos);


	//ȭ���� ��ġ�� ������ ī�޶��� ���̸� ��´�
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

