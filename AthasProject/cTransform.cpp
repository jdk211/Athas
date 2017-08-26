#include "stdafx.h"
#include "cTransform.h"


cTransform::cTransform()
: pChild(NULL), pSibling(NULL), pParent(NULL), bAutoUpdate( true ), m_fMovespd(5.f)
{
	this->Reset();

	this->ShakePosFlag = SHAKE_X | SHAKE_Y | SHAKE_Z;
	this->ShakeRotFlag = SHAKE_X | SHAKE_Y | SHAKE_Z;
}


cTransform::~cTransform()
{
	this->ReleaseParent();
}

//Transform �ʱ�ȭ
void cTransform::Reset(int resetFlag)
{
	if ( resetFlag & RESET_POSITION )
	{
		//��ġ�� �ʱ�ȭ
		this->position = D3DXVECTOR3(0, 0, 0);
	}
	
	if (resetFlag & RESET_SCALE)
	{
		//������ �ʱ�ȭ
		this->scale = D3DXVECTOR3(1, 1, 1);
	}

	if (resetFlag & RESET_ROTATION)
	{
		//������ �ʱ�ȭ
		this->forward = D3DXVECTOR3(0, 0, 1);
		this->up = D3DXVECTOR3(0, 1, 0);
		this->right = D3DXVECTOR3(1, 0, 0);
	}


	//Transform Update!!!!
	this->UpdateTransform();

}
//���̾��Ű �ֻ��� �θ� ��´�.
cTransform*  cTransform::GetRoot()
{
	cTransform* pRoot = this;
	while (pRoot->pParent != NULL)
		pRoot = pRoot->pParent;

	return pRoot;

}

//������ �Ǵٸ� �ڽ��� ���δ�.
void cTransform::AddChild(cTransform* pChild)
{
	//���̷��� �ڽ��� �̹� ���ڽ��̸�...
	if (pChild->pParent == this)
		return;


	//pChild �Ӹ� ���ڽ����� ����������� �Ϻθ�� ���� �����.
	pChild->ReleaseParent();

	//���� ���̷��� pChild �� �θ� ���� ����....

	//���� ���÷� �ٲ���....
	
	//�ڽ��� ���� �����....
	D3DXMATRIXA16 matInvFinal;
	D3DXMatrixInverse(&matInvFinal, NULL, &this->matFinal);

	//Child �� �������.....
	D3DXMATRIXA16 matChildLocal = pChild->matFinal * matInvFinal;

	//Child �� ������� �������....
	memcpy(&pChild->position, &matChildLocal._41, sizeof(D3DXVECTOR3));	//������ġ

	//����ȸ�� �������....
	D3DXVECTOR3 localFront, localUp, localRight;
	memcpy(&localRight, &matChildLocal._11, sizeof(D3DXVECTOR3));
	memcpy(&localUp, &matChildLocal._21, sizeof(D3DXVECTOR3));
	memcpy(&localFront, &matChildLocal._31, sizeof(D3DXVECTOR3));

	//ũ�� ����
	float scaleX, scaleY, scaleZ;
	scaleX = D3DXVec3Length(&localRight);
	scaleY = D3DXVec3Length(&localUp);
	scaleZ = D3DXVec3Length(&localFront);


	//����ȭ ����
	D3DXVec3Normalize(&pChild->right, &localRight);
	D3DXVec3Normalize(&pChild->up, &localUp);
	D3DXVec3Normalize(&pChild->forward, &localFront);

	//������ ����
	pChild->scale.x = scaleX;
	pChild->scale.y = scaleY;
	pChild->scale.z = scaleZ;







	//���� ������ ���ų����̶��...
	if (this->pChild == NULL){
		this->pChild = pChild;
		pChild->pParent = this;
	}

	//�̹� �ڽ����ִٸ�...
	else{

		cTransform* pBrother = this->pChild;

		while (pBrother->pSibling != NULL){
			pBrother = pBrother->pSibling;
		}

		//��������´ٸ� pBrother ���� ���������� �����Ǿ��ִ�.
		
		//�ƿ�� �ݰ���.
		pBrother->pSibling = pChild;

		//���ο� �θ�
		pChild->pParent = this;
	}




	//���ο� ������ ������ ������� Update �ѹ� ��...
	this->UpdateTransform();

}

//Ư�� Transform �� �ٴ´�.
void cTransform::AttachTo(cTransform* pParent)
{
	pParent->AddChild(this);
}

//�θ�� �ȳ�
void cTransform::ReleaseParent()
{
	if (this->pParent == NULL)
		return;


	//�θ��� �ڽ��� ����.
	cTransform* pBrother = this->pParent->pChild;

	//���� �峲�̴�?
	if (this->pParent->pChild == this)
	{
		//�ƿ�� �ʰ� �峲�̴�.
		this->pParent->pChild = this->pSibling;

		//�ƿ�� �ȳ�
		this->pSibling = NULL;

		//�θ�� �ȳ��� �輼��
		this->pParent = NULL;
	}

	//���� �峲�� �ƴ϶��...
	else
	{
		//���� �ٷ� �������� ã��...
		while (pBrother->pSibling != this)
			pBrother = pBrother->pSibling;

		pBrother->pSibling = this->pSibling;
		this->pSibling = NULL;
		this->pParent = NULL;
	}

	//���� ���� ���� ����� ��´�. (������Ʈ�� �Ǳ� ���̴� �θ��� ����� ����� ��������� ���̴� )



	D3DXMATRIXA16 matWorld = this->matFinal;

	//������� �������....
	memcpy(&this->position, &matWorld._41, sizeof(D3DXVECTOR3));	//������ġ

	//����ȸ�� �������....
	D3DXVECTOR3 worldFront, worldUp, worldRight;
	memcpy(&worldRight, &matWorld._11, sizeof(D3DXVECTOR3));
	memcpy(&worldUp, &matWorld._21, sizeof(D3DXVECTOR3));
	memcpy(&worldFront, &matWorld._31, sizeof(D3DXVECTOR3));

	//ũ�� ����
	float scaleX, scaleY, scaleZ;
	scaleX = D3DXVec3Length(&worldRight);
	scaleY = D3DXVec3Length(&worldUp);
	scaleZ = D3DXVec3Length(&worldFront);


	//����ȭ ����
	D3DXVec3Normalize(&this->right, &worldRight);
	D3DXVec3Normalize(&this->up, &worldUp);
	D3DXVec3Normalize(&this->forward, &worldFront);

	//������ ����
	this->scale.x = scaleX;
	this->scale.y = scaleY;
	this->scale.z = scaleZ;




	//������ ������� Update �ѹ� ��...
	this->UpdateTransform();
}




//��ġ�� ���� ��ǥ��� �����Ѵ�. 
void cTransform::SetWorldPosition(float x, float y, float z)
{
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;


	//�θ� �ִٸ�...
	if ( this->pParent != NULL ){

		//�θ� �ִٸ� ����Ǵ� position ���� �θ��� ������� Local Position ���� �Ǿ� �Ѵ�.

		//�θ��� �������
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();

		//�θ��� �����
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);


		//����� ���Ե� �����ǿ� ������� �����Ͽ� ���÷� ��ȯ
		D3DXVec3TransformCoord(
			&this->position,
			&this->position,
			&matInvFinal);
	}


	if (bAutoUpdate)
		this->UpdateTransform();
}
void cTransform::SetWorldPosition(D3DXVECTOR3 pos)
{
	this->position = pos;

	//�θ� �ִٸ�...
	if (this->pParent != NULL){

		//�θ� �ִٸ� ����Ǵ� position ���� �θ��� ������� Local Position ���� �Ǿ� �Ѵ�.

		//�θ��� �������
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();

		//�θ��� �����
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		//����� ���Ե� �����ǿ� ������� �����Ͽ� ���÷� ��ȯ
		D3DXVec3TransformCoord(
			&this->position,
			&this->position,
			&matInvFinal);
	}



	if (bAutoUpdate)
		this->UpdateTransform();
}

//��ġ�� ���� ��ǥ��� �����Ѵ�.  ( �θ� �ִ� ��� ������ �������� ��ġ, �θ� ���ٸ� SetWorldPosition �� �����ѱ�� )
void cTransform::SetLocalPosition(float x, float y, float z)
{
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;

	if (bAutoUpdate)
		this->UpdateTransform();
}
void cTransform::SetLocalPosition(D3DXVECTOR3 pos)
{
	this->position = pos;
	
	if (bAutoUpdate)
		this->UpdateTransform();

}



//���� �������� �̵� ��Ų��.
void cTransform::MovePositionWorld(float dx, float dy, float dz)
{
	//�̵�����
	D3DXVECTOR3 delta(dx, dy, dz);

	//���� �θ� �ִٸ�...
	if (this->pParent != NULL){

		//�θ��� �������
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();

		//�θ��� �����
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		//�̵�����ü�� �κ��� �������� ��������....
		D3DXVec3TransformNormal(
			&delta,
			&delta,
			&matInvFinal);
	}

	this->position += delta;


	if (bAutoUpdate)
		this->UpdateTransform();
}
void cTransform::MovePositionWorld(D3DXVECTOR3 delta)
{
	//���� �θ� �ִٸ�...
	if (this->pParent != NULL){

		//�θ��� �������
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();

		//�θ��� �����
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		//�̵�����ü�� �κ��� �������� ��������....
		D3DXVec3TransformNormal(
			&delta,
			&delta,
			&matInvFinal);
	}

	this->position += delta;
	
	if (bAutoUpdate)
		this->UpdateTransform();
}


//�θ� �ִ� ��� ���� �������� �̵� ��Ų��.
void cTransform::MovePositionLocal(float dx, float dy, float dz)
{
	this->position.x += dx;
	this->position.y += dy;
	this->position.z += dz;

	if (bAutoUpdate)
		this->UpdateTransform();
}
void cTransform::MovePositionLocal(D3DXVECTOR3 delta)
{
	this->position += delta;
	
	if (bAutoUpdate)
		this->UpdateTransform();
}



//�ڽ��� ��������� �̵� ��Ų��.
void cTransform::MovePositionSelf(float dx, float dy, float dz)
{
	D3DXVECTOR3 moveDelta(0, 0, 0);

	moveDelta += this->GetRight() * dx;
	moveDelta += this->GetUp() * dy;
	moveDelta += this->GetForward() * dz;

	this->MovePositionWorld(moveDelta);

}
void cTransform::MovePositionSelf(D3DXVECTOR3 delta)
{
	D3DXVECTOR3 moveDelta(0, 0, 0);

	moveDelta += this->GetRight() * delta.x;
	moveDelta += this->GetUp() * delta.y;
	moveDelta += this->GetForward() * delta.z;

	this->MovePositionWorld(moveDelta);
}


//������ ���� 
void cTransform::SetScale(float x, float y, float z)
{
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;

	if (bAutoUpdate)
		this->UpdateTransform();
}
void cTransform::SetScale(D3DXVECTOR3 scale)
{
	this->scale = scale;

	if (bAutoUpdate)
		this->UpdateTransform();
}

//�����ϸ�
void cTransform::Scaling(float dx, float dy, float dz)
{
	this->scale.x += dx;
	this->scale.y += dy;
	this->scale.z += dz;

	if (bAutoUpdate)
		this->UpdateTransform();
}
void cTransform::Scaling(D3DXVECTOR3 deltaScale)
{
	this->scale += deltaScale;

	if (bAutoUpdate)
		this->UpdateTransform();
}

//���Ϸ� ȸ�� ������ �ش� ȸ�������� ȸ���ض�.
void cTransform::SetRotateWorld(float eAngleX, float eAngleY, float aAngleZ)
{
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat, eAngleY, eAngleX, aAngleZ);

	SetRotateWorld(quat);
}

// ȸ�� ������� �־��ָ� �� ȸ���� ��� ȸ���Ѵ�.
void cTransform::SetRotateWorld(const D3DXQUATERNION& qautWorldRotate)
{
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &qautWorldRotate);

	SetRotateWorld(matRotate);
}

// ȸ�� ����� �־��ָ� �� ȸ�� ��Ĵ�� ȸ���Ѵ�.
void cTransform::SetRotateWorld(const D3DXMATRIXA16& matWorldRotate)
{
	//���� �� �ʱ�ȭ
	D3DXVECTOR3 r = D3DXVECTOR3(1, 0, 0);
	D3DXVECTOR3 u = D3DXVECTOR3(0, 1, 0);
	D3DXVECTOR3 f = D3DXVECTOR3(0, 0, 1);


	
	//���� �θ� �ִٸ� ���������� �������ؾ��ϱ� ������
	//�θ��� ������� �����Ͽ� �����ؾ��Ѵ�.


	//���� �θ� �ִٸ�...
	if (this->pParent != NULL){

		//�θ��� �������
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();

		//�θ��� �����
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		//�̵�����ü�� �κ��� �������� ��������....
		D3DXMATRIXA16 rot = matWorldRotate * matInvFinal;
		
		D3DXVec3TransformNormal(&r, &r, &rot);
		D3DXVec3TransformNormal(&u, &u, &rot);
		D3DXVec3TransformNormal(&f, &f, &rot);

		//�θ����Ϸ����� ���⺤���� ���̰� 1 �� �ƴϰ� �ɼ��� ������
		//�̸� ����ȭ �Ͽ� ����
		D3DXVec3Normalize(&r, &r);
		D3DXVec3Normalize(&u, &u);
		D3DXVec3Normalize(&f, &f);
	}

	else
	{
		D3DXVec3TransformNormal(&r, &r, &matWorldRotate);
		D3DXVec3TransformNormal(&u, &u, &matWorldRotate);
		D3DXVec3TransformNormal(&f, &f, &matWorldRotate);
	}


	this->right = r;
	this->up = u;
	this->forward = f;


	if (bAutoUpdate)
		this->UpdateTransform();
}


//������� �̿��� Ư�� ȸ�������� ȸ������ ������....
void cTransform::SetRotateLocal(float eAngleX, float eAngleY, float aAngleZ)
{
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat, eAngleY, eAngleX, aAngleZ);

	SetRotateLocal(quat);
}

// ȸ�� ������� �־��ָ� �� ȸ���� ��� ȸ���Ѵ�.
void cTransform::SetRotateLocal(const D3DXQUATERNION& qautWorldRotate)
{
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &qautWorldRotate);

	SetRotateLocal(matRotate);
}

// ȸ�� ����� �־��ָ� �� ȸ�� ��Ĵ�� ȸ���Ѵ�.
void cTransform::SetRotateLocal(const D3DXMATRIXA16& matWorldRotate)
{
	//���� �� �ʱ�ȭ
	this->right = D3DXVECTOR3(1, 0, 0);
	this->up = D3DXVECTOR3(0, 1, 0);
	this->forward = D3DXVECTOR3(0, 0, 1);


	//�Ʒ��� �Լ����� Vector3 �� 4x4 Matrix �� ���� Vector3 ����� ��� �Լ����̴�.
	//D3DXVec3TransformCoord	Vector3 ���������п� 1 �� �߰��Ͽ� 4x4 Matrix ���� Matrix �� �̵����б��� ����� ���͸� ��´� ( �ַ� ��ġ ���Ϳ� ��ġ���б��� �����Ҷ� ��� )
	//D3DXVec3TransformNormal	Vector3 ���������п� 0 �� �߰��Ͽ� 4x4 Matrix ���� Matrix �� �̵������� ������ �����ʰ� ȸ���� �����ϸ� ����� ���͸� ��´�.  ( �ַ� ��ġ ���⺤�͸� ȸ�� ��ų������)


	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matWorldRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();
}


//���� ����� ������ �� ���� ��Ĵ�� Transform ���������õȴ�.
void cTransform::SetWorldMatrix(D3DXMATRIXA16* pMat)
{
	//���� ��ķ� ����
	this->matFinal = *pMat;
	this->matLocal = *pMat;

	//���� ��ġ ������Ʈ 
	position = D3DXVECTOR3(
		matFinal._41,
		matFinal._42,
		matFinal._43);

	//���� ������Ʈ 
	D3DXVECTOR3 front = D3DXVECTOR3(
		matFinal._31,
		matFinal._32,
		matFinal._33);
	D3DXVECTOR3 up = D3DXVECTOR3(
		matFinal._21,
		matFinal._22,
		matFinal._23);
	D3DXVECTOR3 right = D3DXVECTOR3(
		matFinal._11,
		matFinal._12,
		matFinal._13);
	D3DXVec3Normalize(&this->forward, &front);
	D3DXVec3Normalize(&this->up, &up);
	D3DXVec3Normalize(&this->right, &right);


	//���̷� ������ �ش�.
	scale = D3DXVECTOR3(
		D3DXVec3Length(&right),
		D3DXVec3Length(&up),
		D3DXVec3Length(&front));


	//���� �θ� �־��ٸ�..
	if (pParent)
	{
		//������ ���Ե� ������ �θ������ ���÷� 

		//�θ� �����
		D3DXMATRIXA16 matParentINVWorld = pParent->GetFinalMatrix();
		D3DXMatrixInverse(&matParentINVWorld, NULL, &matParentINVWorld);

		// ���� ���
		this->matLocal = matFinal * matParentINVWorld;

		//���� ���� ��ġ
		memcpy(&this->position, &matLocal._41, sizeof(D3DXVECTOR3));

		//���� Front, Up, Right
		D3DXVECTOR3 front;
		memcpy(&front, &matLocal._31, sizeof(D3DXVECTOR3));
		D3DXVECTOR3 up;
		memcpy(&up, &matLocal._21, sizeof(D3DXVECTOR3));
		D3DXVECTOR3 right;
		memcpy(&right, &matLocal._11, sizeof(D3DXVECTOR3));

		//�� ������
		float scaleX = D3DXVec3Length(&right);
		float scaleY = D3DXVec3Length(&up);
		float scaleZ = D3DXVec3Length(&front);
		this->scale = D3DXVECTOR3(scaleX, scaleY, scaleZ);

		//������ ���̸� ������ ����ȭ
		//front /= scaleX;
		front *= (1.0f) / scaleZ;
		up *= (1.0f) / scaleY;
		right *= (1.0f) / scaleX;

		this->forward = front;
		this->up = up;
		this->right = right;

	}

	//���� �ڽ��� �ִٸ�....
	cTransform* pCh = this->pChild;
	while (pCh != NULL){
		pCh->UpdateTransform();
		pCh = pChild->pSibling;
	}




}





//���� �������� ȸ�� 
void cTransform::RotateWorld(float dAngleX, float dAngleY, float dAngleZ)
{
	//ȸ�� ����� ��´�.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat, dAngleY, dAngleX, dAngleZ);

	//ȸ�� ��� ��´�.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quat);

	//�θ� �ִٸ�...
	if (this->pParent)
	{
		/*
		//���� ���� ��´�.
		D3DXVECTOR3 r = this->GetRight();
		D3DXVECTOR3 u = this->GetUp();
		D3DXVECTOR3 f = this->GetForward();

		//ȸ����Ĵ�� ������.
		D3DXVec3TransformNormal(&r, &r, &matRotate);
		D3DXVec3TransformNormal(&u, &u, &matRotate);
		D3DXVec3TransformNormal(&f, &f, &matRotate);
		
		//�ٽ� ���÷� ��ȯ�Ͽ� �����Ѵ�.
		
		//�θ��� �����
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &this->pParent->GetFinalMatrix());

		D3DXVec3TransformNormal(&r, &r, &matInvFinal);
		D3DXVec3TransformNormal(&u, &u, &matInvFinal);
		D3DXVec3TransformNormal(&f, &f, &matInvFinal);

		//�θ����Ϸ����� ���⺤���� ���̰� 1 �� �ƴϰ� �ɼ��� ������
		//�̸� ����ȭ �Ͽ� ����
		D3DXVec3Normalize(&this->right, &r);
		D3DXVec3Normalize(&this->up, &u);
		D3DXVec3Normalize(&this->forward, &f);
		*/


		// A * ������ ���� * matRotate * ������ ������
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();
		//�θ��� �����
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		D3DXMATRIXA16 rot = matFinal * matRotate * matInvFinal;
		//ȸ����Ĵ�� ������.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &rot);
		}

	}

	//�θ� ���ٸ�...
	else
	{

		//ȸ����Ĵ�� ������.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
		}
	}

	if (bAutoUpdate)
		this->UpdateTransform();


}
void cTransform::RotateWorld(D3DXVECTOR3 deltaAngle)
{
	//ȸ�� ����� ��´�.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat, 
		deltaAngle.y, deltaAngle.x, deltaAngle.z);

	//ȸ�� ��� ��´�.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quat);

	//�θ� �ִٸ�...
	if (this->pParent)
	{
		/*
		//���� ���� ��´�.
		D3DXVECTOR3 r = this->GetRight();
		D3DXVECTOR3 u = this->GetUp();
		D3DXVECTOR3 f = this->GetForward();

		//ȸ����Ĵ�� ������.
		D3DXVec3TransformNormal(&r, &r, &matRotate);
		D3DXVec3TransformNormal(&u, &u, &matRotate);
		D3DXVec3TransformNormal(&f, &f, &matRotate);

		//�ٽ� ���÷� ��ȯ�Ͽ� �����Ѵ�.

		//�θ��� �����
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &this->pParent->GetFinalMatrix());

		D3DXVec3TransformNormal(&r, &r, &matInvFinal);
		D3DXVec3TransformNormal(&u, &u, &matInvFinal);
		D3DXVec3TransformNormal(&f, &f, &matInvFinal);

		//�θ����Ϸ����� ���⺤���� ���̰� 1 �� �ƴϰ� �ɼ��� ������
		//�̸� ����ȭ �Ͽ� ����
		D3DXVec3Normalize(&this->right, &r);
		D3DXVec3Normalize(&this->up, &u);
		D3DXVec3Normalize(&this->forward, &f);
		*/


		// A * ������ ���� * matRotate * ������ ������
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();
		//�θ��� �����
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		D3DXMATRIXA16 rot = matFinal * matRotate * matInvFinal;
		//ȸ����Ĵ�� ������.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &rot);
		}

	}

	//�θ� ���ٸ�...
	else
	{

		//ȸ����Ĵ�� ������.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
		}
	}


	if (bAutoUpdate)
		this->UpdateTransform();
}

//ȸ�� ����� �־��ָ� ȸ����Ĵ�� ȸ����
void cTransform::RotateWorld(D3DXMATRIXA16& matRotate)
{
	//�θ� �ִٸ�...
	if (this->pParent)
	{
		/*
		//���� ���� ��´�.
		D3DXVECTOR3 r = this->GetRight();
		D3DXVECTOR3 u = this->GetUp();
		D3DXVECTOR3 f = this->GetForward();

		//ȸ����Ĵ�� ������.
		D3DXVec3TransformNormal(&r, &r, &matRotate);
		D3DXVec3TransformNormal(&u, &u, &matRotate);
		D3DXVec3TransformNormal(&f, &f, &matRotate);

		//�ٽ� ���÷� ��ȯ�Ͽ� �����Ѵ�.

		//�θ��� �����
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &this->pParent->GetFinalMatrix());

		D3DXVec3TransformNormal(&r, &r, &matInvFinal);
		D3DXVec3TransformNormal(&u, &u, &matInvFinal);
		D3DXVec3TransformNormal(&f, &f, &matInvFinal);

		//�θ����Ϸ����� ���⺤���� ���̰� 1 �� �ƴϰ� �ɼ��� ������
		//�̸� ����ȭ �Ͽ� ����
		D3DXVec3Normalize(&this->right, &r);
		D3DXVec3Normalize(&this->up, &u);
		D3DXVec3Normalize(&this->forward, &f);
		*/


		// A * ������ ���� * matRotate * ������ ������
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();
		//�θ��� �����
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		D3DXMATRIXA16 rot = matFinal * matRotate * matInvFinal;
		//ȸ����Ĵ�� ������.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &rot);
		}

	}

	//�θ� ���ٸ�...
	else
	{

		//ȸ����Ĵ�� ������.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
		}
	}


	if (bAutoUpdate)
		this->UpdateTransform();
}


//���� �������� ȸ�� 
void cTransform::RotateLocal(float dAngleX, float dAngleY, float dAngleZ)
{
	//ȸ�� ����� ��´�.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat, dAngleY, dAngleX, dAngleZ);

	//ȸ�� ��� ��´�.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quat);


	//ȸ����Ĵ�� ������.
	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();
}
void cTransform::RotateLocal(D3DXVECTOR3 deltaAngle)
{
	//ȸ�� ����� ��´�.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat,
		deltaAngle.y, deltaAngle.x, deltaAngle.z);

	//ȸ�� ��� ��´�.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quat);


	//ȸ����Ĵ�� ������.
	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();
}
void cTransform::RotateLocal(D3DXMATRIXA16& matRotate)
{
	//ȸ����Ĵ�� ������.
	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();
}





//�ڽ��� ��������� ȸ��
void cTransform::RotateSelf(float dAngleX, float dAngleY, float dAngleZ)
{
	//�ڽ��� Right �� �������� ȸ���ϴ� ȸ�� ����� ��´�.
	D3DXMATRIXA16 matRotateX;
	D3DXMatrixRotationAxis(&matRotateX, &this->right, dAngleX);

	//�ڽ��� Up �� �������� ȸ���ϴ� ȸ�� ����� ��´�.
	D3DXMATRIXA16 matRotateY;
	D3DXMatrixRotationAxis(&matRotateY, &this->up, dAngleY);

	//�ڽ��� Front �� �������� ȸ���ϴ� ȸ�� ����� ��´�.
	D3DXMATRIXA16 matRotateZ;
	D3DXMatrixRotationAxis(&matRotateZ, &this->forward, dAngleZ);


	D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;



	//ȸ����Ĵ�� ������.
	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();

}
void cTransform::RotateSelf(D3DXVECTOR3 deltaAngle)
{
	//�ڽ��� Right �� �������� ȸ���ϴ� ȸ�� ����� ��´�.
	D3DXMATRIXA16 matRotateX;
	D3DXMatrixRotationAxis(&matRotateX, &this->right, deltaAngle.x);

	//�ڽ��� Up �� �������� ȸ���ϴ� ȸ�� ����� ��´�.
	D3DXMATRIXA16 matRotateY;
	D3DXMatrixRotationAxis(&matRotateY, &this->up, deltaAngle.y);

	//�ڽ��� Front �� �������� ȸ���ϴ� ȸ�� ����� ��´�.
	D3DXMATRIXA16 matRotateZ;
	D3DXMatrixRotationAxis(&matRotateZ, &this->forward, deltaAngle.z);


	D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

	//ȸ����Ĵ�� ������.
	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();
}



//Ư�� ������ �ٶ󺸰� ȸ���ض�.
void cTransform::LookDirection(const D3DXVECTOR3& dir, const D3DXVECTOR3& Up)
{
	//�ٶ󺼹����� ����ȭ�Ͽ� �������� ����
	D3DXVec3Normalize(&this->forward, &dir);

	//�������� ���Ϳ� ������ �����Ͽ� �����ʺ��͸� ����ȭ�Ѵ�.
	D3DXVec3Cross(&this->right, &Up, &this->forward);
	D3DXVec3Normalize(&this->right, &this->right);

	//������ ���Ϳ� �������͸� �����Ͽ� ��溤�͸� ����ȭ�Ѵ�.
	D3DXVec3Cross(&this->up, &this->forward, &this->right);
	D3DXVec3Normalize(&this->up, &this->up);

	if (bAutoUpdate)
		this->UpdateTransform();
}


//Ư�������� �ٶ󺸴µ� angle ����ŭ�� ȸ�� �ض�
void cTransform::LookDirection(const D3DXVECTOR3& dir, float angle)
{
	//������ ���Ѵ�.
	float distAngle = acos( clamp( D3DXVec3Dot(&this->forward, &dir), -1.0f, 1.0f ) );

	//�������� 0.01 ���̸��̸� ������....
	if (distAngle < 0.01f * ONE_RAD){
		return;
	}

	
	D3DXVECTOR3 axis;

	//�������� ���� 180 �� ���.....
	if (distAngle > 179.9f * ONE_RAD){
		axis = D3DXVECTOR3(0, 1, 0);
	}
	else{
		//����
		D3DXVec3Cross(&axis, &this->forward, &dir);
		D3DXVec3Normalize(&axis, &axis);
	}


	//ȸ�����Ѿ��� ȸ�����...
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationAxis(
		&matRotate,
		&axis,
		min(angle, distAngle) );

	RotateWorld(matRotate);

}


//Ư����ġ�� �ٶ󺸰� ȸ���ض�.
void cTransform::LookPosition(const D3DXVECTOR3& pos, const D3DXVECTOR3& Up)
{
	D3DXVECTOR3 dirToPos = pos - this->position;
	LookDirection(dirToPos, Up);
}



//Ư����ġ��  �ٶ󺸴µ� angle ����ŭ�� ȸ�� �ض�
void cTransform::LookPosition(const D3DXVECTOR3& pos, float angle)
{
	D3DXVECTOR3 dirToPos = pos - this->position;
	D3DXVec3Normalize(&dirToPos, &dirToPos);
	LookDirection(dirToPos, angle);
}



//Transform �� ������Ʈ �Ѵ� ( Trasform �� ������ ���ŵǾ����� ���ȴ� )
void cTransform::UpdateTransform()
{
	D3DXVec3Normalize(&this->right, &this->right);
	D3DXVec3Normalize(&this->up, &this->up);
	D3DXVec3Normalize(&this->forward, &this->forward);

	//Final ���
	D3DXVECTOR3 r = this->right * this->scale.x;
	D3DXVECTOR3 u = this->up * this->scale.y;
	D3DXVECTOR3 f = this->forward * this->scale.z;
	D3DXVECTOR3 p = this->position;

	// matLocal Matrix �� ����
	matLocal._11 = r.x;		matLocal._12 = r.y;		matLocal._13 = r.z;		matLocal._14 = 0;
	matLocal._21 = u.x;		matLocal._22 = u.y;		matLocal._23 = u.z;		matLocal._24 = 0;
	matLocal._31 = f.x;		matLocal._32 = f.y;		matLocal._33 = f.z;		matLocal._34 = 0;
	matLocal._41 = p.x;		matLocal._42 = p.y;		matLocal._43 = p.z;		matLocal._44 = 1;


	//���θ� �ִٸ�...
	if (this->pParent != NULL)
		matFinal = matLocal * this->pParent->GetFinalMatrix();
	else
		matFinal = matLocal;



	//�� �ڽ��� �ִٸ�...
	if (this->pChild != NULL)
		this->pChild->UpdateTransform();

	//�� ���簡 �ִٸ�..
	if (this->pSibling != NULL)
		this->pSibling->UpdateTransform();

}

//�ڽ��� ��� ���� Device �� ������Ʈ �Ѵ�.
void cTransform::UpdateToDevice()
{
	Device->SetTransform(D3DTS_WORLD, &this->matFinal); 
}

//Transform �� ���� ������ �����ϰ� �׷�����.
void cTransform::RenderGizmos()
{
	//�������
	GIZMO_MGR->RenderLine(
		&this->GetWorldPosition(), &(this->GetWorldPosition() + this->GetForward()), 0xFF0000FF);

	//��������
	GIZMO_MGR->RenderLine(
		&this->GetWorldPosition(), &(this->GetWorldPosition() + this->GetRight()), 0xFFFF0000);

	//��������
	GIZMO_MGR->RenderLine(
		&this->GetWorldPosition(), &(this->GetWorldPosition() + this->GetUp()), 0xFF00FF00);
}


void cTransform::DefaultControl(float timeDelta)
{
	/*if (KEY_MGR->IsStayDown(VK_LSHIFT))
	{*/
	D3DXVECTOR3 angle(0, 0, 0);

	if (KEY_MGR->IsStayDown('A')) {
		angle.y -= 1.0f;
	}
	else if (KEY_MGR->IsStayDown('D')) {
		angle.y += 1.0f;
	}

	/*if (KEY_MGR->IsStayDown(VK_UP)){
	angle.x -= 1.0f;
	}
	else if (KEY_MGR->IsStayDown(VK_DOWN)){
	angle.x += 1.0f;
	}*/

	if (angle != D3DXVECTOR3(0, 0, 0)) {

		angle *= timeDelta * 90 * ONE_RAD;
		this->RotateSelf(angle);
	}

	D3DXVECTOR3 move(0, 0, 0);
	if (KEY_MGR->IsStayDown('W')) {
		move.z += 1.0f;
	}
	else if (KEY_MGR->IsStayDown('S')) {
		move.z -= 1.0f;
	}

	if (move != D3DXVECTOR3(0, 0, 0)) {
		D3DXVec3Normalize(&move, &move);
		this->MovePositionSelf(move * timeDelta * 10.0f);
	}
	//}
	/*else
	{
	D3DXVECTOR3 move(0, 0, 0);
	if (KEY_MGR->IsStayDown(VK_LEFT)){
	move.x -= 1.0f;
	}
	else if (KEY_MGR->IsStayDown(VK_RIGHT)){
	move.x += 1.0f;
	}

	if (KEY_MGR->IsStayDown(VK_UP)){
	move.z += 1.0f;
	}
	else if (KEY_MGR->IsStayDown(VK_DOWN)){
	move.z -= 1.0f;
	}

	if (move != D3DXVECTOR3(0, 0, 0)){
	D3DXVec3Normalize(&move, &move);
	this->MovePositionSelf(move * timeDelta * 5.0f);
	}
	}*/
	

	/*if (KEY_MGR->IsStayDown(VK_LSHIFT))
	{
		D3DXVECTOR3 angle(0, 0, 0);

		if (KEY_MGR->IsStayDown(VK_LEFT)){
			angle.y -= 1.0f;
		}
		else if (KEY_MGR->IsStayDown(VK_RIGHT)){
			angle.y += 1.0f;
		}

		if (KEY_MGR->IsStayDown(VK_UP)){
			angle.x -= 1.0f;
		}
		else if (KEY_MGR->IsStayDown(VK_DOWN)){
			angle.x += 1.0f;
		}

		if (angle != D3DXVECTOR3(0, 0, 0)){

			angle *= timeDelta * 90 * ONE_RAD;
			this->RotateSelf(angle);
		}
	}
	else
	{
		D3DXVECTOR3 move(0, 0, 0);
		if (KEY_MGR->IsStayDown(VK_LEFT)){
			move.x -= 1.0f;
		}
		else if (KEY_MGR->IsStayDown(VK_RIGHT)){
			move.x += 1.0f;
		}

		if (KEY_MGR->IsStayDown(VK_UP)){
			move.z += 1.0f;
		}
		else if (KEY_MGR->IsStayDown(VK_DOWN)){
			move.z -= 1.0f;
		}

		if (KEY_MGR->IsStayDown('E')){
			move.y += 1.0f;
		}
		else if (KEY_MGR->IsStayDown('C')){
			move.y -= 1.0f;
		}

		if (move != D3DXVECTOR3(0, 0, 0)){
			D3DXVec3Normalize(&move, &move);
			this->MovePositionSelf(move * timeDelta * 15.0f);
		}
	}
*/
}

void cTransform::MouseDefaultControl(float timeDelta)
{
	static float hAngle = 0.0f;
	static float vAngle = 0.0f;

	float mouseSensitivityX = 0.5f;
	float mouseSensitivityY = 0.5f;

	if (KEY_MGR->IsOnceDown('M'))
	{
		m_fMovespd += 1.0f;
	}
	else if (KEY_MGR->IsOnceDown('N'))
	{
		m_fMovespd -= 1.0f;
		if (m_fMovespd < 0.f)
		{
			m_fMovespd = 1.0f;
		}
	}

	//���콺 ���� ��ư�� ���ʷ� ��������
	if (KEY_MGR->IsOnceDown(VK_RBUTTON))
	{
		//Ŀ���� Ŭ���̾�Ʈ ���ͷ� �̵���Ų��.
		POINT setPos;
		setPos.x = WINSIZE_X / 2;
		setPos.y = WINSIZE_Y / 2;

		//Ŭ���̾�Ʈ������ ��ġ�� ����ũž ������ ��ġ��..
		ClientToScreen(g_hWnd, &setPos);

		//����ũž ������ ���콺 ��ġ����
		SetCursorPos(setPos.x, setPos.y);
	}
	//���콺 ���� ��ư�� ������ ������
	else if (KEY_MGR->IsStayDown(VK_RBUTTON))
	{
		D3DXVECTOR3 moveDir(0, 0, 0);

		if (KEY_MGR->IsStayDown('W')){
			moveDir.z += 0.1f;
		}
		else if (KEY_MGR->IsStayDown('S')){
			moveDir.z -= 0.1f;
		}

		if (KEY_MGR->IsStayDown('A')){
			moveDir.x -= 0.1f;
		}
		else if (KEY_MGR->IsStayDown('D')){
			moveDir.x += 0.1f;
		}

		if (moveDir != D3DXVECTOR3(0, 0, 0))
		{
			D3DXVec3Normalize(&moveDir, &moveDir);

			this->MovePositionSelf(
				moveDir * m_fMovespd * timeDelta);
		}

		//���콺 ���� ��ġ
		POINT centerPos;
		centerPos.x = WINSIZE_X / 2;
		centerPos.y = WINSIZE_Y / 2;

		//���� ���콺 ��ġ
		POINT nowPos = GetMousePos();
		
		//���콺 ������
		float deltaMouseX = static_cast<float>(nowPos.x - centerPos.x);
		float deltaMouseY = static_cast<float>(nowPos.y - centerPos.y);

		//���� ����
		hAngle += ( deltaMouseX * mouseSensitivityX ) * ONE_RAD;

		//������ ����
		vAngle += (deltaMouseY * mouseSensitivityY)  * ONE_RAD;

		//������ ����
		vAngle = clamp(vAngle, -85 * ONE_RAD, 85 * ONE_RAD);

		this->SetRotateWorld(
			vAngle, hAngle, 0.0f);

		//�ٽ� ���콺 ���� ��ġ

		//Ŭ���̾�Ʈ������ ��ġ�� ����ũž ������ ��ġ��..
		ClientToScreen(g_hWnd, &centerPos);

		//����ũž ������ ���콺 ��ġ����
		SetCursorPos(centerPos.x, centerPos.y);
	}



}

// ------------------------------------------------------------------------
// Get ����
// ------------------------------------------------------------------------

//���� ��ġ�� ��´�.
D3DXVECTOR3 cTransform::GetWorldPosition() const
{
	if (this->pParent != NULL){
		//���������� ����� �������� �θ��� ��������� ��´�.
		D3DXMATRIXA16 matParentFinal = this->pParent->GetFinalMatrix();

		D3DXVECTOR3 worldPos;

		D3DXVec3TransformCoord(&worldPos, &this->position, &matParentFinal);

		return worldPos;
	}

	return this->position;
}

//�κ��� �ִ� ��� �θ��� �������� ������ġ�� ��´�.
D3DXVECTOR3 cTransform::GetLocalPosition() const
{
	return this->position;
}

//��������� ��´�.
D3DXMATRIXA16 cTransform::GetFinalMatrix() const
{
	return this->matFinal;
}

//���� ��´�. 
D3DXVECTOR3 cTransform::GetForward(bool bNormalize) const
{
	D3DXVECTOR3 result;

	if (bNormalize)
		result = this->forward;

	else
		result = this->forward * this->scale.z;


	//�θ� �ִٸ�...
	if (this->pParent != NULL)
	{
		//���������� ����� �������� �θ��� ��������� ��´�.
		D3DXMATRIXA16 matParentFinal = this->pParent->GetFinalMatrix();
		D3DXVec3TransformNormal(&result, &result, &matParentFinal);
	}
	D3DXVec3Normalize(&result, &result);


	return result;

}
D3DXVECTOR3 cTransform::GetUp(bool bNormalize ) const
{
	D3DXVECTOR3 result;

	if (bNormalize)
		result = this->up;

	else
		result = this->up * this->scale.y;


	//�θ� �ִٸ�...
	if (this->pParent != NULL)
	{
		//���������� ����� �������� �θ��� ��������� ��´�.
		D3DXMATRIXA16 matParentFinal = this->pParent->GetFinalMatrix();
		D3DXVec3TransformNormal(&result, &result, &matParentFinal);
	}

	D3DXVec3Normalize(&result, &result);

	return result;
}
D3DXVECTOR3 cTransform::GetRight(bool bNormalize) const
{
	D3DXVECTOR3 result;

	if (bNormalize)
		result = this->right;

	else
		result = this->right * this->scale.x;


	//�θ� �ִٸ�...
	if (this->pParent != NULL)
	{
		//���������� ����� �������� �θ��� ��������� ��´�.
		D3DXMATRIXA16 matParentFinal = this->pParent->GetFinalMatrix();
		D3DXVec3TransformNormal(&result, &result, &matParentFinal);
	}

	D3DXVec3Normalize(&result, &result);

	return result;
}



//�������� ���� ���� ��´�.
void cTransform::GetScaledAxies(D3DXVECTOR3* pVecArr) const
{
	pVecArr[0] = this->GetRight(false);
	pVecArr[1] = this->GetUp(false);
	pVecArr[2] = this->GetForward(false);	
}

//�������� �ȸ��� ���� ��´�.
void cTransform::GetUnitAxies(D3DXVECTOR3* pVecArr) const
{
	pVecArr[0] = this->GetRight();
	pVecArr[1] = this->GetUp();
	pVecArr[2] = this->GetForward();
}


D3DXVECTOR3 cTransform::GetScaledAxis(int axisNum) const
{
	D3DXVECTOR3 result;
	result = this->axis[axisNum] * ((float*)&this->scale)[axisNum];

	//�θ� �ִٸ�...
	if (this->pParent != NULL)
	{
		//���������� ����� �������� �θ��� ��������� ��´�.
		D3DXMATRIXA16 matParentFinal = this->pParent->GetFinalMatrix();
		D3DXVec3TransformNormal(&result, &result, &matParentFinal);
	}

	return result;
}
D3DXVECTOR3 cTransform::GetUnitAxis(int axisNum) const
{
	D3DXVECTOR3 result;
	result =  this->axis[axisNum];

	//�θ� �ִٸ�...
	if (this->pParent != NULL)
	{
		//���������� ����� �������� �θ��� ��������� ��´�.
		D3DXMATRIXA16 matParentFinal = this->pParent->GetFinalMatrix();
		D3DXVec3TransformNormal(&result, &result, &matParentFinal);
	}

	return result;

}

//���� ��Ŀ��� ȸ����� ���и� �����´�.
D3DXMATRIXA16 cTransform::GetWorldRotateMatrix() const
{
	D3DXMATRIXA16 matRotate;
	D3DXMatrixIdentity(&matRotate);
	memcpy(&matRotate._11, &this->GetRight(), sizeof(D3DXVECTOR3));
	memcpy(&matRotate._21, &this->GetUp(), sizeof(D3DXVECTOR3));
	memcpy(&matRotate._31, &this->GetForward(), sizeof(D3DXVECTOR3));

	return matRotate;
}

//���������� ��´�.
D3DXQUATERNION cTransform::GetWorldQuaternion() const
{
	//ȸ�� ����� ��´�.
	D3DXMATRIXA16 matRotate = this->GetWorldRotateMatrix();
	
	//ȸ����ķ� ȸ��������� ��´�.
	D3DXQUATERNION rot;
	D3DXQuaternionRotationMatrix(&rot, &matRotate);

	return rot;
}

//�������� ��´�.
D3DXVECTOR3  cTransform::GetScale() const
{
	D3DXVECTOR3 scale;

	scale.x = D3DXVec3Length(&D3DXVECTOR3( matFinal._11, matFinal._12, matFinal._13 ) );
	scale.y = D3DXVec3Length(&D3DXVECTOR3(matFinal._21, matFinal._22, matFinal._23));
	scale.z = D3DXVec3Length(&D3DXVECTOR3(matFinal._31, matFinal._32, matFinal._33));


	return scale;
}

// ------------------------------------------------------------------------
// ���� ����
// ------------------------------------------------------------------------

//Ư�� Ÿ�� Transform �� ���� ��ġ ���������� �Ѵ�.
void cTransform::InterpolatePosition(const cTransform& target, float factor)
{
	D3DXVECTOR3 newPos;
	
	//newPos.x = lerp(this->position.x, target.position.x, factor);
	//newPos.y = lerp(this->position.y, target.position.y, factor);
	//newPos.z = lerp(this->position.z, target.position.z, factor);
	D3DXVec3Lerp( &newPos, &this->position, &target.position, factor);

	this->position = newPos;

	this->UpdateTransform();

}
void cTransform::InterpolatePosition(D3DXVECTOR3 targetPos, float factor)
{
	D3DXVECTOR3 newPos;

	D3DXVec3Lerp(&newPos, &this->position, &targetPos, factor);

	this->position = newPos;

	this->UpdateTransform();
}

void cTransform::InterpolatePosition(const cTransform& start, const cTransform& end, float factor)
{
	D3DXVECTOR3 newPos;

	D3DXVec3Lerp(&newPos, &start.position, &end.position, factor);

	this->position = newPos;

	this->UpdateTransform();

}
void cTransform::InterpolatePosition(D3DXVECTOR3 startPos, D3DXVECTOR3 endPos, float factor)
{
	D3DXVECTOR3 newPos;

	D3DXVec3Lerp(&newPos, &startPos, &endPos, factor);

	this->position = newPos;

	this->UpdateTransform();
}

//Ư�� Ÿ�� Transform �� ���� ȸ�������� �Ѵ�.
void cTransform::InterpolateRoatation(const cTransform& target, float factor)
{
	//factor �� 0 ~ 1 ������ ���̴�.
	factor = clamp(factor, 0.0f, 1.0f);

	//���� ȸ�������
	D3DXQUATERNION myRot = this->GetWorldQuaternion();
	//Ÿ�� ȸ�������
	D3DXQUATERNION targetRot = target.GetWorldQuaternion();

	//������ �����
	D3DXQUATERNION result;

	if (FLOAT_EQUAL(factor, 0.0f)){

		return;
	}
	
	else if (FLOAT_EQUAL(factor, 1.0f)){

		result = target.GetWorldQuaternion();
	}

	else{

		//��������� ȸ�� ������ �Ѵ�.
		D3DXQuaternionSlerp(
			&result,		//���
			&myRot,			//���� ���۰�
			&targetRot,		//���� ����
			factor);
	}

	//����� ������ ����
	this->SetRotateWorld(result);

}
void cTransform::InterpolateRoatation(D3DXQUATERNION quat, float factor)
{

	//factor �� 0 ~ 1 ������ ���̴�.
	factor = clamp(factor, 0.0f, 1.0f);

	//���� ȸ�������
	D3DXQUATERNION myRot = this->GetWorldQuaternion();
	
	//������ �����
	D3DXQUATERNION result;

	if (FLOAT_EQUAL(factor, 0.0f)){

		return;
	}

	else if (FLOAT_EQUAL(factor, 1.0f)){

		result = quat;
	}

	else{

		//��������� ȸ�� ������ �Ѵ�.
		D3DXQuaternionSlerp(
			&result,		//���
			&myRot,			//���� ���۰�
			&quat,			//���� ����
			factor);
	}

	//����� ������ ����
	this->SetRotateWorld(result);

}
void cTransform::InterpolateRoatation(const cTransform& start, const cTransform& end, float factor)
{

	//factor �� 0 ~ 1 ������ ���̴�.
	factor = clamp(factor, 0.0f, 1.0f);

	//������ �����
	D3DXQUATERNION result;

	if (FLOAT_EQUAL(factor, 0.0f)){

		result = start.GetWorldQuaternion();
	}

	else if (FLOAT_EQUAL(factor, 1.0f)){

		result = end.GetWorldQuaternion();
	}

	else{

		//��������� ȸ�� ������ �Ѵ�.
		D3DXQuaternionSlerp(
			&result,		//���
			&start.GetWorldQuaternion(),			//���� ���۰�
			&end.GetWorldQuaternion(),		//���� ����
			factor);
	}

	
	//����� ������ ����
	this->SetRotateWorld(result);
}
void cTransform::InterpolateRoatation(D3DXQUATERNION rotStart, D3DXQUATERNION rotEnd, float factor)
{

	//factor �� 0 ~ 1 ������ ���̴�.
	factor = clamp(factor, 0.0f, 1.0f);

	//������ �����
	D3DXQUATERNION result;


	if (FLOAT_EQUAL(factor, 0.0f)){

		result = rotStart;
	}

	else if (FLOAT_EQUAL(factor, 1.0f)){

		result = rotEnd;
	}

	else{

		//��������� ȸ�� ������ �Ѵ�.
		D3DXQuaternionSlerp(
			&result,		//���
			&rotStart,			//���� ���۰�
			&rotEnd,		//���� ����
			factor);
	}

	
	//����� ������ ����
	this->SetRotateWorld(result);
}


//
// Shake ����..................
//

//����ũ ����� ������ Update �� ���־�� �Ѵ�.
void cTransform::ShakeUpdate(float timeDelta)
{
	//��ġ�� ���� ��鸲 �Ŀ��� �ִٸ�..
	if (this->ShakePosPower > 0.0f && this->ShakePosFlag != 0)
	{
		//��鸲�� ������ �ǵ鿩���Ѵ�..
		D3DXVECTOR3 localPos(0, 0, 0);
		if (this->ShakePosFlag & SHAKE_X)
			localPos.x = GetRandom(-this->ShakePosPower, this->ShakePosPower);
		if (this->ShakePosFlag & SHAKE_Y)
			localPos.y = GetRandom(-this->ShakePosPower, this->ShakePosPower);
		if (this->ShakePosFlag & SHAKE_Z)
			localPos.z = GetRandom(-this->ShakePosPower, this->ShakePosPower);

		//���� ���������� ����
		this->SetLocalPosition(localPos);

		//����ũ �Ŀ� ���...
		this->ShakePosPower -= this->ShakePosRelease * timeDelta;

		if (this->ShakePosPower < 0.0f){
			this->ShakePosPower = 0.0f;
			//����ġ
			this->SetLocalPosition(0, 0, 0);
		}

	}

	//ȸ���� ���� ��鸲 �Ŀ��� �ִٸ�..(���� ShakeRotPower ��  ȸ�����̵ȴ� )
	if (this->ShakeRotPower > 0.0f && this->ShakeRotFlag != 0)
	{
		//��鸲�� ������ �ǵ鿩���Ѵ�..
		D3DXVECTOR3 localRot(0, 0, 0);
		if (this->ShakeRotFlag & SHAKE_X)
			localRot.x = GetRandom(-this->ShakeRotPower, this->ShakeRotPower);
		if (this->ShakeRotFlag & SHAKE_Y)
			localRot.y = GetRandom(-this->ShakeRotPower, this->ShakeRotPower);
		if (this->ShakeRotFlag & SHAKE_Z)
			localRot.z = GetRandom(-this->ShakeRotPower, this->ShakeRotPower);

		//���� �����̼����� ����
		this->SetRotateLocal(localRot.x, localRot.y, localRot.z);

		//����ũ �Ŀ� ���...
		this->ShakeRotPower -= this->ShakeRotRelease * timeDelta;

		if (this->ShakeRotPower < 0.0f){
			this->ShakeRotPower = 0.0f;
			this->SetRotateLocal(0, 0, 0);
		}



	}
}

//��ġ�� ���� ��鸲�� �ش�.
void cTransform::ShakePos(float shakePower, float shakeRelase)
{
	this->ShakePosPower = shakePower;
	this->ShakePosRelease = shakeRelase;
}

//ȸ���� ���� ��鸲�� �ش�.
void cTransform::ShakeRot(float shakePower, float shakeRelase)
{
	this->ShakeRotPower = shakePower;
	this->ShakeRotRelease = shakeRelase;
}

//��鸲 �� ����
void cTransform::SetShakePosFlag(int flag)
{
	this->ShakePosFlag = flag;
}
void cTransform::SetShakeRotFlag(int flag)
{
	this->ShakeRotFlag = flag;
}

