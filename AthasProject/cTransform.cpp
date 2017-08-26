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

//Transform 초기화
void cTransform::Reset(int resetFlag)
{
	if ( resetFlag & RESET_POSITION )
	{
		//위치값 초기화
		this->position = D3DXVECTOR3(0, 0, 0);
	}
	
	if (resetFlag & RESET_SCALE)
	{
		//스케일 초기화
		this->scale = D3DXVECTOR3(1, 1, 1);
	}

	if (resetFlag & RESET_ROTATION)
	{
		//방향축 초기화
		this->forward = D3DXVECTOR3(0, 0, 1);
		this->up = D3DXVECTOR3(0, 1, 0);
		this->right = D3DXVECTOR3(1, 0, 0);
	}


	//Transform Update!!!!
	this->UpdateTransform();

}
//하이어라키 최상위 부모를 얻는다.
cTransform*  cTransform::GetRoot()
{
	cTransform* pRoot = this;
	while (pRoot->pParent != NULL)
		pRoot = pRoot->pParent;

	return pRoot;

}

//내꺼에 또다른 자식을 붙인다.
void cTransform::AddChild(cTransform* pChild)
{
	//붙이려는 자식이 이미 내자식이면...
	if (pChild->pParent == this)
		return;


	//pChild 임마 내자식으로 들어오고싶으면 니부모와 연을 끊어라.
	pChild->ReleaseParent();

	//현재 붙이려는 pChild 는 부모가 없는 상태....

	//나의 로컬로 바꾸자....
	
	//자신의 최종 역행렬....
	D3DXMATRIXA16 matInvFinal;
	D3DXMatrixInverse(&matInvFinal, NULL, &this->matFinal);

	//Child 의 로컬행렬.....
	D3DXMATRIXA16 matChildLocal = pChild->matFinal * matInvFinal;

	//Child 의 최종행렬 장기적출....
	memcpy(&pChild->position, &matChildLocal._41, sizeof(D3DXVECTOR3));	//로컬위치

	//로컬회전 장기적출....
	D3DXVECTOR3 localFront, localUp, localRight;
	memcpy(&localRight, &matChildLocal._11, sizeof(D3DXVECTOR3));
	memcpy(&localUp, &matChildLocal._21, sizeof(D3DXVECTOR3));
	memcpy(&localFront, &matChildLocal._31, sizeof(D3DXVECTOR3));

	//크기 적출
	float scaleX, scaleY, scaleZ;
	scaleX = D3DXVec3Length(&localRight);
	scaleY = D3DXVec3Length(&localUp);
	scaleZ = D3DXVec3Length(&localFront);


	//정규화 대입
	D3DXVec3Normalize(&pChild->right, &localRight);
	D3DXVec3Normalize(&pChild->up, &localUp);
	D3DXVec3Normalize(&pChild->forward, &localFront);

	//스케일 대입
	pChild->scale.x = scaleX;
	pChild->scale.y = scaleY;
	pChild->scale.z = scaleZ;







	//내가 쓸쓸한 독거노인이라면...
	if (this->pChild == NULL){
		this->pChild = pChild;
		pChild->pParent = this;
	}

	//이미 자식이있다면...
	else{

		cTransform* pBrother = this->pChild;

		while (pBrother->pSibling != NULL){
			pBrother = pBrother->pSibling;
		}

		//여기까지온다면 pBrother 에는 막내형님이 참조되어있다.
		
		//아우야 반갑다.
		pBrother->pSibling = pChild;

		//새로운 부모
		pChild->pParent = this;
	}




	//새로운 가족을 맞이한 기념으로 Update 한방 콜...
	this->UpdateTransform();

}

//특정 Transform 에 붙는다.
void cTransform::AttachTo(cTransform* pParent)
{
	pParent->AddChild(this);
}

//부모와 안녕
void cTransform::ReleaseParent()
{
	if (this->pParent == NULL)
		return;


	//부모의 자식을 본다.
	cTransform* pBrother = this->pParent->pChild;

	//내가 장남이니?
	if (this->pParent->pChild == this)
	{
		//아우야 너가 장남이다.
		this->pParent->pChild = this->pSibling;

		//아우야 안녕
		this->pSibling = NULL;

		//부모님 안녕히 계세요
		this->pParent = NULL;
	}

	//내가 장남이 아니라면...
	else
	{
		//나의 바로 윗형님을 찾자...
		while (pBrother->pSibling != this)
			pBrother = pBrother->pSibling;

		pBrother->pSibling = this->pSibling;
		this->pSibling = NULL;
		this->pParent = NULL;
	}

	//현재 나의 최종 행렬을 얻는다. (업데이트가 되기 전이니 부모의 행렬이 적용된 최종행렬일 것이다 )



	D3DXMATRIXA16 matWorld = this->matFinal;

	//최종행렬 장기적출....
	memcpy(&this->position, &matWorld._41, sizeof(D3DXVECTOR3));	//월드위치

	//월드회전 장기적출....
	D3DXVECTOR3 worldFront, worldUp, worldRight;
	memcpy(&worldRight, &matWorld._11, sizeof(D3DXVECTOR3));
	memcpy(&worldUp, &matWorld._21, sizeof(D3DXVECTOR3));
	memcpy(&worldFront, &matWorld._31, sizeof(D3DXVECTOR3));

	//크기 적출
	float scaleX, scaleY, scaleZ;
	scaleX = D3DXVec3Length(&worldRight);
	scaleY = D3DXVec3Length(&worldUp);
	scaleZ = D3DXVec3Length(&worldFront);


	//정규화 대입
	D3DXVec3Normalize(&this->right, &worldRight);
	D3DXVec3Normalize(&this->up, &worldUp);
	D3DXVec3Normalize(&this->forward, &worldFront);

	//스케일 대입
	this->scale.x = scaleX;
	this->scale.y = scaleY;
	this->scale.z = scaleZ;




	//독립한 기념으로 Update 한방 콜...
	this->UpdateTransform();
}




//위치를 월드 좌표계로 셋팅한다. 
void cTransform::SetWorldPosition(float x, float y, float z)
{
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;


	//부모가 있다면...
	if ( this->pParent != NULL ){

		//부모가 있다면 저장되는 position 값은 부모의 상대적인 Local Position 값이 되야 한다.

		//부모의 최종행렬
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();

		//부모의 역행렬
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);


		//월드로 대입된 포지션에 역행렬을 적용하여 로컬로 변환
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

	//부모가 있다면...
	if (this->pParent != NULL){

		//부모가 있다면 저장되는 position 값은 부모의 상대적인 Local Position 값이 되야 한다.

		//부모의 최종행렬
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();

		//부모의 역행렬
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		//월드로 대입된 포지션에 역행렬을 적용하여 로컬로 변환
		D3DXVec3TransformCoord(
			&this->position,
			&this->position,
			&matInvFinal);
	}



	if (bAutoUpdate)
		this->UpdateTransform();
}

//위치를 로컬 좌표계로 셋팅한다.  ( 부모가 있는 경우 무모의 상태적인 위치, 부모가 없다면 SetWorldPosition 와 동일한기능 )
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



//월드 기준으로 이동 시킨다.
void cTransform::MovePositionWorld(float dx, float dy, float dz)
{
	//이동벡터
	D3DXVECTOR3 delta(dx, dy, dz);

	//만약 부모가 있다면...
	if (this->pParent != NULL){

		//부모의 최종행렬
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();

		//부모의 역행렬
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		//이동량자체를 부보의 상태적인 방향으로....
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
	//만약 부모가 있다면...
	if (this->pParent != NULL){

		//부모의 최종행렬
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();

		//부모의 역행렬
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		//이동량자체를 부보의 상태적인 방향으로....
		D3DXVec3TransformNormal(
			&delta,
			&delta,
			&matInvFinal);
	}

	this->position += delta;
	
	if (bAutoUpdate)
		this->UpdateTransform();
}


//부모가 있는 경우 로컬 기준으로 이동 시킨다.
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



//자신의 축기준으로 이동 시킨다.
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


//스케일 셋팅 
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

//스케일링
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

//오일러 회전 값으로 해당 회전값으로 회전해라.
void cTransform::SetRotateWorld(float eAngleX, float eAngleY, float aAngleZ)
{
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat, eAngleY, eAngleX, aAngleZ);

	SetRotateWorld(quat);
}

// 회전 사원수를 넣어주면 그 회전값 대로 회전한다.
void cTransform::SetRotateWorld(const D3DXQUATERNION& qautWorldRotate)
{
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &qautWorldRotate);

	SetRotateWorld(matRotate);
}

// 회전 행렬을 넣어주면 그 회전 행렬대로 회전한다.
void cTransform::SetRotateWorld(const D3DXMATRIXA16& matWorldRotate)
{
	//나의 축 초기화
	D3DXVECTOR3 r = D3DXVECTOR3(1, 0, 0);
	D3DXVECTOR3 u = D3DXVECTOR3(0, 1, 0);
	D3DXVECTOR3 f = D3DXVECTOR3(0, 0, 1);


	
	//만약 부모가 있다면 로컬정보로 저장을해야하기 때문에
	//부모의 역행렬을 적용하여 대입해야한다.


	//만약 부모가 있다면...
	if (this->pParent != NULL){

		//부모의 최종행렬
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();

		//부모의 역행렬
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		//이동량자체를 부보의 상태적인 방향으로....
		D3DXMATRIXA16 rot = matWorldRotate * matInvFinal;
		
		D3DXVec3TransformNormal(&r, &r, &rot);
		D3DXVec3TransformNormal(&u, &u, &rot);
		D3DXVec3TransformNormal(&f, &f, &rot);

		//부모스케일로인해 방향벡터의 길이가 1 이 아니게 될수도 있으니
		//이를 정규화 하여 대입
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


//사원수를 이용한 특정 회전값으로 회전량을 가져라....
void cTransform::SetRotateLocal(float eAngleX, float eAngleY, float aAngleZ)
{
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat, eAngleY, eAngleX, aAngleZ);

	SetRotateLocal(quat);
}

// 회전 사원수를 넣어주면 그 회전값 대로 회전한다.
void cTransform::SetRotateLocal(const D3DXQUATERNION& qautWorldRotate)
{
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &qautWorldRotate);

	SetRotateLocal(matRotate);
}

// 회전 행렬을 넣어주면 그 회전 행렬대로 회전한다.
void cTransform::SetRotateLocal(const D3DXMATRIXA16& matWorldRotate)
{
	//나의 축 초기화
	this->right = D3DXVECTOR3(1, 0, 0);
	this->up = D3DXVECTOR3(0, 1, 0);
	this->forward = D3DXVECTOR3(0, 0, 1);


	//아래의 함수들은 Vector3 에 4x4 Matrix 를 곱한 Vector3 결과를 얻는 함수들이다.
	//D3DXVec3TransformCoord	Vector3 마지막성분에 1 을 추가하여 4x4 Matrix 곱해 Matrix 의 이동성분까지 적용된 벡터를 얻는다 ( 주로 위치 벡터에 위치성분까지 적용할때 사용 )
	//D3DXVec3TransformNormal	Vector3 마지막성분에 0 을 추가하여 4x4 Matrix 곱해 Matrix 의 이동성분은 적용이 되지않고 회전과 스케일만 적용된 벡터를 얻는다.  ( 주로 위치 방향벡터를 회전 시킬때적용)


	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matWorldRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();
}


//월드 행렬을 넣으면 그 월드 행렬대로 Transform 정보가셋팅된다.
void cTransform::SetWorldMatrix(D3DXMATRIXA16* pMat)
{
	//월드 행렬로 셋팅
	this->matFinal = *pMat;
	this->matLocal = *pMat;

	//월드 위치 업데이트 
	position = D3DXVECTOR3(
		matFinal._41,
		matFinal._42,
		matFinal._43);

	//방위 업데이트 
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


	//길이로 스케일 준다.
	scale = D3DXVECTOR3(
		D3DXVec3Length(&right),
		D3DXVec3Length(&up),
		D3DXVec3Length(&front));


	//만약 부모가 있었다면..
	if (pParent)
	{
		//위에서 대입된 정보를 부모기준의 로컬로 

		//부모 역행렬
		D3DXMATRIXA16 matParentINVWorld = pParent->GetFinalMatrix();
		D3DXMatrixInverse(&matParentINVWorld, NULL, &matParentINVWorld);

		// 로컬 행렬
		this->matLocal = matFinal * matParentINVWorld;

		//나의 로컬 위치
		memcpy(&this->position, &matLocal._41, sizeof(D3DXVECTOR3));

		//나의 Front, Up, Right
		D3DXVECTOR3 front;
		memcpy(&front, &matLocal._31, sizeof(D3DXVECTOR3));
		D3DXVECTOR3 up;
		memcpy(&up, &matLocal._21, sizeof(D3DXVECTOR3));
		D3DXVECTOR3 right;
		memcpy(&right, &matLocal._11, sizeof(D3DXVECTOR3));

		//각 스케일
		float scaleX = D3DXVec3Length(&right);
		float scaleY = D3DXVec3Length(&up);
		float scaleZ = D3DXVec3Length(&front);
		this->scale = D3DXVECTOR3(scaleX, scaleY, scaleZ);

		//벡터의 길이를 나누면 정규화
		//front /= scaleX;
		front *= (1.0f) / scaleZ;
		up *= (1.0f) / scaleY;
		right *= (1.0f) / scaleX;

		this->forward = front;
		this->up = up;
		this->right = right;

	}

	//내가 자식이 있다면....
	cTransform* pCh = this->pChild;
	while (pCh != NULL){
		pCh->UpdateTransform();
		pCh = pChild->pSibling;
	}




}





//월드 기준으로 회전 
void cTransform::RotateWorld(float dAngleX, float dAngleY, float dAngleZ)
{
	//회전 사원수 얻는다.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat, dAngleY, dAngleX, dAngleZ);

	//회전 행렬 얻는다.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quat);

	//부모가 있다면...
	if (this->pParent)
	{
		/*
		//월드 축을 얻는다.
		D3DXVECTOR3 r = this->GetRight();
		D3DXVECTOR3 u = this->GetUp();
		D3DXVECTOR3 f = this->GetForward();

		//회전행렬대로 돌린다.
		D3DXVec3TransformNormal(&r, &r, &matRotate);
		D3DXVec3TransformNormal(&u, &u, &matRotate);
		D3DXVec3TransformNormal(&f, &f, &matRotate);
		
		//다시 로컬로 변환하여 대입한다.
		
		//부모의 역행렬
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &this->pParent->GetFinalMatrix());

		D3DXVec3TransformNormal(&r, &r, &matInvFinal);
		D3DXVec3TransformNormal(&u, &u, &matInvFinal);
		D3DXVec3TransformNormal(&f, &f, &matInvFinal);

		//부모스케일로인해 방향벡터의 길이가 1 이 아니게 될수도 있으니
		//이를 정규화 하여 대입
		D3DXVec3Normalize(&this->right, &r);
		D3DXVec3Normalize(&this->up, &u);
		D3DXVec3Normalize(&this->forward, &f);
		*/


		// A * 무모의 월드 * matRotate * 무모의 역월드
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();
		//부모의 역행렬
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		D3DXMATRIXA16 rot = matFinal * matRotate * matInvFinal;
		//회전행렬대로 돌린다.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &rot);
		}

	}

	//부모가 없다면...
	else
	{

		//회전행렬대로 돌린다.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
		}
	}

	if (bAutoUpdate)
		this->UpdateTransform();


}
void cTransform::RotateWorld(D3DXVECTOR3 deltaAngle)
{
	//회전 사원수 얻는다.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat, 
		deltaAngle.y, deltaAngle.x, deltaAngle.z);

	//회전 행렬 얻는다.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quat);

	//부모가 있다면...
	if (this->pParent)
	{
		/*
		//월드 축을 얻는다.
		D3DXVECTOR3 r = this->GetRight();
		D3DXVECTOR3 u = this->GetUp();
		D3DXVECTOR3 f = this->GetForward();

		//회전행렬대로 돌린다.
		D3DXVec3TransformNormal(&r, &r, &matRotate);
		D3DXVec3TransformNormal(&u, &u, &matRotate);
		D3DXVec3TransformNormal(&f, &f, &matRotate);

		//다시 로컬로 변환하여 대입한다.

		//부모의 역행렬
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &this->pParent->GetFinalMatrix());

		D3DXVec3TransformNormal(&r, &r, &matInvFinal);
		D3DXVec3TransformNormal(&u, &u, &matInvFinal);
		D3DXVec3TransformNormal(&f, &f, &matInvFinal);

		//부모스케일로인해 방향벡터의 길이가 1 이 아니게 될수도 있으니
		//이를 정규화 하여 대입
		D3DXVec3Normalize(&this->right, &r);
		D3DXVec3Normalize(&this->up, &u);
		D3DXVec3Normalize(&this->forward, &f);
		*/


		// A * 무모의 월드 * matRotate * 무모의 역월드
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();
		//부모의 역행렬
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		D3DXMATRIXA16 rot = matFinal * matRotate * matInvFinal;
		//회전행렬대로 돌린다.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &rot);
		}

	}

	//부모가 없다면...
	else
	{

		//회전행렬대로 돌린다.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
		}
	}


	if (bAutoUpdate)
		this->UpdateTransform();
}

//회전 행렬을 넣어주면 회전행렬대로 회전함
void cTransform::RotateWorld(D3DXMATRIXA16& matRotate)
{
	//부모가 있다면...
	if (this->pParent)
	{
		/*
		//월드 축을 얻는다.
		D3DXVECTOR3 r = this->GetRight();
		D3DXVECTOR3 u = this->GetUp();
		D3DXVECTOR3 f = this->GetForward();

		//회전행렬대로 돌린다.
		D3DXVec3TransformNormal(&r, &r, &matRotate);
		D3DXVec3TransformNormal(&u, &u, &matRotate);
		D3DXVec3TransformNormal(&f, &f, &matRotate);

		//다시 로컬로 변환하여 대입한다.

		//부모의 역행렬
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &this->pParent->GetFinalMatrix());

		D3DXVec3TransformNormal(&r, &r, &matInvFinal);
		D3DXVec3TransformNormal(&u, &u, &matInvFinal);
		D3DXVec3TransformNormal(&f, &f, &matInvFinal);

		//부모스케일로인해 방향벡터의 길이가 1 이 아니게 될수도 있으니
		//이를 정규화 하여 대입
		D3DXVec3Normalize(&this->right, &r);
		D3DXVec3Normalize(&this->up, &u);
		D3DXVec3Normalize(&this->forward, &f);
		*/


		// A * 무모의 월드 * matRotate * 무모의 역월드
		D3DXMATRIXA16 matFinal = this->pParent->GetFinalMatrix();
		//부모의 역행렬
		D3DXMATRIXA16 matInvFinal;
		D3DXMatrixInverse(&matInvFinal, NULL, &matFinal);

		D3DXMATRIXA16 rot = matFinal * matRotate * matInvFinal;
		//회전행렬대로 돌린다.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &rot);
		}

	}

	//부모가 없다면...
	else
	{

		//회전행렬대로 돌린다.
		for (int i = 0; i < 3; i++){
			D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
		}
	}


	if (bAutoUpdate)
		this->UpdateTransform();
}


//로컬 기준으로 회전 
void cTransform::RotateLocal(float dAngleX, float dAngleY, float dAngleZ)
{
	//회전 사원수 얻는다.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat, dAngleY, dAngleX, dAngleZ);

	//회전 행렬 얻는다.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quat);


	//회전행렬대로 돌린다.
	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();
}
void cTransform::RotateLocal(D3DXVECTOR3 deltaAngle)
{
	//회전 사원수 얻는다.
	D3DXQUATERNION quat;
	D3DXQuaternionRotationYawPitchRoll(&quat,
		deltaAngle.y, deltaAngle.x, deltaAngle.z);

	//회전 행렬 얻는다.
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &quat);


	//회전행렬대로 돌린다.
	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();
}
void cTransform::RotateLocal(D3DXMATRIXA16& matRotate)
{
	//회전행렬대로 돌린다.
	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();
}





//자신의 축기준으로 회전
void cTransform::RotateSelf(float dAngleX, float dAngleY, float dAngleZ)
{
	//자신의 Right 축 기준으로 회전하는 회전 행렬을 얻는다.
	D3DXMATRIXA16 matRotateX;
	D3DXMatrixRotationAxis(&matRotateX, &this->right, dAngleX);

	//자신의 Up 축 기준으로 회전하는 회전 행렬을 얻는다.
	D3DXMATRIXA16 matRotateY;
	D3DXMatrixRotationAxis(&matRotateY, &this->up, dAngleY);

	//자신의 Front 축 기준으로 회전하는 회전 행렬을 얻는다.
	D3DXMATRIXA16 matRotateZ;
	D3DXMatrixRotationAxis(&matRotateZ, &this->forward, dAngleZ);


	D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;



	//회전행렬대로 돌린다.
	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();

}
void cTransform::RotateSelf(D3DXVECTOR3 deltaAngle)
{
	//자신의 Right 축 기준으로 회전하는 회전 행렬을 얻는다.
	D3DXMATRIXA16 matRotateX;
	D3DXMatrixRotationAxis(&matRotateX, &this->right, deltaAngle.x);

	//자신의 Up 축 기준으로 회전하는 회전 행렬을 얻는다.
	D3DXMATRIXA16 matRotateY;
	D3DXMatrixRotationAxis(&matRotateY, &this->up, deltaAngle.y);

	//자신의 Front 축 기준으로 회전하는 회전 행렬을 얻는다.
	D3DXMATRIXA16 matRotateZ;
	D3DXMatrixRotationAxis(&matRotateZ, &this->forward, deltaAngle.z);


	D3DXMATRIXA16 matRotate = matRotateY * matRotateX * matRotateZ;

	//회전행렬대로 돌린다.
	for (int i = 0; i < 3; i++){
		D3DXVec3TransformNormal(&axis[i], &axis[i], &matRotate);
	}

	if (bAutoUpdate)
		this->UpdateTransform();
}



//특정 방향을 바라보게 회전해라.
void cTransform::LookDirection(const D3DXVECTOR3& dir, const D3DXVECTOR3& Up)
{
	//바라볼방향을 정규화하여 정면으로 대입
	D3DXVec3Normalize(&this->forward, &dir);

	//상방기준의 벡터와 정면을 외적하여 오른쪽벡터를 직교화한다.
	D3DXVec3Cross(&this->right, &Up, &this->forward);
	D3DXVec3Normalize(&this->right, &this->right);

	//정면의 벡터와 우측벡터를 외적하여 상방벡터를 직교화한다.
	D3DXVec3Cross(&this->up, &this->forward, &this->right);
	D3DXVec3Normalize(&this->up, &this->up);

	if (bAutoUpdate)
		this->UpdateTransform();
}


//특정방향을 바라보는데 angle 각만큼만 회전 해라
void cTransform::LookDirection(const D3DXVECTOR3& dir, float angle)
{
	//각차를 구한다.
	float distAngle = acos( clamp( D3DXVec3Dot(&this->forward, &dir), -1.0f, 1.0f ) );

	//각도차가 0.01 도미만이면 하지마....
	if (distAngle < 0.01f * ONE_RAD){
		return;
	}

	
	D3DXVECTOR3 axis;

	//각도차가 거의 180 도 라면.....
	if (distAngle > 179.9f * ONE_RAD){
		axis = D3DXVECTOR3(0, 1, 0);
	}
	else{
		//외적
		D3DXVec3Cross(&axis, &this->forward, &dir);
		D3DXVec3Normalize(&axis, &axis);
	}


	//회전시켜야할 회전행렬...
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationAxis(
		&matRotate,
		&axis,
		min(angle, distAngle) );

	RotateWorld(matRotate);

}


//특정위치를 바라보게 회전해라.
void cTransform::LookPosition(const D3DXVECTOR3& pos, const D3DXVECTOR3& Up)
{
	D3DXVECTOR3 dirToPos = pos - this->position;
	LookDirection(dirToPos, Up);
}



//특정위치를  바라보는데 angle 각만큼만 회전 해라
void cTransform::LookPosition(const D3DXVECTOR3& pos, float angle)
{
	D3DXVECTOR3 dirToPos = pos - this->position;
	D3DXVec3Normalize(&dirToPos, &dirToPos);
	LookDirection(dirToPos, angle);
}



//Transform 을 업데이트 한다 ( Trasform 의 정보가 갱신되었을대 사용된다 )
void cTransform::UpdateTransform()
{
	D3DXVec3Normalize(&this->right, &this->right);
	D3DXVec3Normalize(&this->up, &this->up);
	D3DXVec3Normalize(&this->forward, &this->forward);

	//Final 행렬
	D3DXVECTOR3 r = this->right * this->scale.x;
	D3DXVECTOR3 u = this->up * this->scale.y;
	D3DXVECTOR3 f = this->forward * this->scale.z;
	D3DXVECTOR3 p = this->position;

	// matLocal Matrix 에 적용
	matLocal._11 = r.x;		matLocal._12 = r.y;		matLocal._13 = r.z;		matLocal._14 = 0;
	matLocal._21 = u.x;		matLocal._22 = u.y;		matLocal._23 = u.z;		matLocal._24 = 0;
	matLocal._31 = f.x;		matLocal._32 = f.y;		matLocal._33 = f.z;		matLocal._34 = 0;
	matLocal._41 = p.x;		matLocal._42 = p.y;		matLocal._43 = p.z;		matLocal._44 = 1;


	//내부모가 있다면...
	if (this->pParent != NULL)
		matFinal = matLocal * this->pParent->GetFinalMatrix();
	else
		matFinal = matLocal;



	//내 자식이 있다면...
	if (this->pChild != NULL)
		this->pChild->UpdateTransform();

	//내 형재가 있다면..
	if (this->pSibling != NULL)
		this->pSibling->UpdateTransform();

}

//자신의 행렬 값을 Device 에 업데이트 한다.
void cTransform::UpdateToDevice()
{
	Device->SetTransform(D3DTS_WORLD, &this->matFinal); 
}

//Transform 의 현제 방위를 간단하게 그려본다.
void cTransform::RenderGizmos()
{
	//정면라인
	GIZMO_MGR->RenderLine(
		&this->GetWorldPosition(), &(this->GetWorldPosition() + this->GetForward()), 0xFF0000FF);

	//우측라인
	GIZMO_MGR->RenderLine(
		&this->GetWorldPosition(), &(this->GetWorldPosition() + this->GetRight()), 0xFFFF0000);

	//우측라인
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

	//마우스 우측 버튼을 최초로 눌렀을때
	if (KEY_MGR->IsOnceDown(VK_RBUTTON))
	{
		//커서를 클라이언트 센터로 이동시킨다.
		POINT setPos;
		setPos.x = WINSIZE_X / 2;
		setPos.y = WINSIZE_Y / 2;

		//클라이언트기준의 위치를 데스크탑 기준의 위치로..
		ClientToScreen(g_hWnd, &setPos);

		//데스크탑 기준의 마우스 위치셋팅
		SetCursorPos(setPos.x, setPos.y);
	}
	//마우스 우측 버튼을 누르고 있을때
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

		//마우스 센터 위치
		POINT centerPos;
		centerPos.x = WINSIZE_X / 2;
		centerPos.y = WINSIZE_Y / 2;

		//현재 마우스 위치
		POINT nowPos = GetMousePos();
		
		//마우스 변위량
		float deltaMouseX = static_cast<float>(nowPos.x - centerPos.x);
		float deltaMouseY = static_cast<float>(nowPos.y - centerPos.y);

		//수평각 적용
		hAngle += ( deltaMouseX * mouseSensitivityX ) * ONE_RAD;

		//수직각 적용
		vAngle += (deltaMouseY * mouseSensitivityY)  * ONE_RAD;

		//수직각 막아
		vAngle = clamp(vAngle, -85 * ONE_RAD, 85 * ONE_RAD);

		this->SetRotateWorld(
			vAngle, hAngle, 0.0f);

		//다시 마우스 센터 위치

		//클라이언트기준의 위치를 데스크탑 기준의 위치로..
		ClientToScreen(g_hWnd, &centerPos);

		//데스크탑 기준의 마우스 위치셋팅
		SetCursorPos(centerPos.x, centerPos.y);
	}



}

// ------------------------------------------------------------------------
// Get 관련
// ------------------------------------------------------------------------

//월드 위치를 얻는다.
D3DXVECTOR3 cTransform::GetWorldPosition() const
{
	if (this->pParent != NULL){
		//로컬정보를 월드로 빼기위해 부모의 최종행렬을 얻는다.
		D3DXMATRIXA16 matParentFinal = this->pParent->GetFinalMatrix();

		D3DXVECTOR3 worldPos;

		D3DXVec3TransformCoord(&worldPos, &this->position, &matParentFinal);

		return worldPos;
	}

	return this->position;
}

//부보가 있는 경우 부모의 상태적인 로컬위치를 얻는다.
D3DXVECTOR3 cTransform::GetLocalPosition() const
{
	return this->position;
}

//최종행렬을 얻는다.
D3DXMATRIXA16 cTransform::GetFinalMatrix() const
{
	return this->matFinal;
}

//축을 얻는다. 
D3DXVECTOR3 cTransform::GetForward(bool bNormalize) const
{
	D3DXVECTOR3 result;

	if (bNormalize)
		result = this->forward;

	else
		result = this->forward * this->scale.z;


	//부모가 있다면...
	if (this->pParent != NULL)
	{
		//로컬정보를 월드로 빼기위해 부모의 최종행렬을 얻는다.
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


	//부모가 있다면...
	if (this->pParent != NULL)
	{
		//로컬정보를 월드로 빼기위해 부모의 최종행렬을 얻는다.
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


	//부모가 있다면...
	if (this->pParent != NULL)
	{
		//로컬정보를 월드로 빼기위해 부모의 최종행렬을 얻는다.
		D3DXMATRIXA16 matParentFinal = this->pParent->GetFinalMatrix();
		D3DXVec3TransformNormal(&result, &result, &matParentFinal);
	}

	D3DXVec3Normalize(&result, &result);

	return result;
}



//스케일이 먹은 축을 얻는다.
void cTransform::GetScaledAxies(D3DXVECTOR3* pVecArr) const
{
	pVecArr[0] = this->GetRight(false);
	pVecArr[1] = this->GetUp(false);
	pVecArr[2] = this->GetForward(false);	
}

//스케일이 안먹은 축을 얻는다.
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

	//부모가 있다면...
	if (this->pParent != NULL)
	{
		//로컬정보를 월드로 빼기위해 부모의 최종행렬을 얻는다.
		D3DXMATRIXA16 matParentFinal = this->pParent->GetFinalMatrix();
		D3DXVec3TransformNormal(&result, &result, &matParentFinal);
	}

	return result;
}
D3DXVECTOR3 cTransform::GetUnitAxis(int axisNum) const
{
	D3DXVECTOR3 result;
	result =  this->axis[axisNum];

	//부모가 있다면...
	if (this->pParent != NULL)
	{
		//로컬정보를 월드로 빼기위해 부모의 최종행렬을 얻는다.
		D3DXMATRIXA16 matParentFinal = this->pParent->GetFinalMatrix();
		D3DXVec3TransformNormal(&result, &result, &matParentFinal);
	}

	return result;

}

//월드 행렬에서 회전행렬 성분만 가져온다.
D3DXMATRIXA16 cTransform::GetWorldRotateMatrix() const
{
	D3DXMATRIXA16 matRotate;
	D3DXMatrixIdentity(&matRotate);
	memcpy(&matRotate._11, &this->GetRight(), sizeof(D3DXVECTOR3));
	memcpy(&matRotate._21, &this->GetUp(), sizeof(D3DXVECTOR3));
	memcpy(&matRotate._31, &this->GetForward(), sizeof(D3DXVECTOR3));

	return matRotate;
}

//월드사원수를 얻는다.
D3DXQUATERNION cTransform::GetWorldQuaternion() const
{
	//회전 행렬을 얻는다.
	D3DXMATRIXA16 matRotate = this->GetWorldRotateMatrix();
	
	//회전행렬로 회전사원수를 얻는다.
	D3DXQUATERNION rot;
	D3DXQuaternionRotationMatrix(&rot, &matRotate);

	return rot;
}

//스케일을 얻는다.
D3DXVECTOR3  cTransform::GetScale() const
{
	D3DXVECTOR3 scale;

	scale.x = D3DXVec3Length(&D3DXVECTOR3( matFinal._11, matFinal._12, matFinal._13 ) );
	scale.y = D3DXVec3Length(&D3DXVECTOR3(matFinal._21, matFinal._22, matFinal._23));
	scale.z = D3DXVec3Length(&D3DXVECTOR3(matFinal._31, matFinal._32, matFinal._33));


	return scale;
}

// ------------------------------------------------------------------------
// 보간 관련
// ------------------------------------------------------------------------

//특정 타겟 Transform 에 대해 위치 선형보간을 한다.
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

//특정 타겟 Transform 에 대해 회전보간을 한다.
void cTransform::InterpolateRoatation(const cTransform& target, float factor)
{
	//factor 는 0 ~ 1 사이의 값이다.
	factor = clamp(factor, 0.0f, 1.0f);

	//내꺼 회전사원수
	D3DXQUATERNION myRot = this->GetWorldQuaternion();
	//타겟 회전사원수
	D3DXQUATERNION targetRot = target.GetWorldQuaternion();

	//보간된 사원수
	D3DXQUATERNION result;

	if (FLOAT_EQUAL(factor, 0.0f)){

		return;
	}
	
	else if (FLOAT_EQUAL(factor, 1.0f)){

		result = target.GetWorldQuaternion();
	}

	else{

		//사원수끼리 회전 보간을 한다.
		D3DXQuaternionSlerp(
			&result,		//결과
			&myRot,			//보간 시작값
			&targetRot,		//보간 끝값
			factor);
	}

	//결과를 나에게 셋팅
	this->SetRotateWorld(result);

}
void cTransform::InterpolateRoatation(D3DXQUATERNION quat, float factor)
{

	//factor 는 0 ~ 1 사이의 값이다.
	factor = clamp(factor, 0.0f, 1.0f);

	//내꺼 회전사원수
	D3DXQUATERNION myRot = this->GetWorldQuaternion();
	
	//보간된 사원수
	D3DXQUATERNION result;

	if (FLOAT_EQUAL(factor, 0.0f)){

		return;
	}

	else if (FLOAT_EQUAL(factor, 1.0f)){

		result = quat;
	}

	else{

		//사원수끼리 회전 보간을 한다.
		D3DXQuaternionSlerp(
			&result,		//결과
			&myRot,			//보간 시작값
			&quat,			//보간 끝값
			factor);
	}

	//결과를 나에게 셋팅
	this->SetRotateWorld(result);

}
void cTransform::InterpolateRoatation(const cTransform& start, const cTransform& end, float factor)
{

	//factor 는 0 ~ 1 사이의 값이다.
	factor = clamp(factor, 0.0f, 1.0f);

	//보간된 사원수
	D3DXQUATERNION result;

	if (FLOAT_EQUAL(factor, 0.0f)){

		result = start.GetWorldQuaternion();
	}

	else if (FLOAT_EQUAL(factor, 1.0f)){

		result = end.GetWorldQuaternion();
	}

	else{

		//사원수끼리 회전 보간을 한다.
		D3DXQuaternionSlerp(
			&result,		//결과
			&start.GetWorldQuaternion(),			//보간 시작값
			&end.GetWorldQuaternion(),		//보간 끝값
			factor);
	}

	
	//결과를 나에게 셋팅
	this->SetRotateWorld(result);
}
void cTransform::InterpolateRoatation(D3DXQUATERNION rotStart, D3DXQUATERNION rotEnd, float factor)
{

	//factor 는 0 ~ 1 사이의 값이다.
	factor = clamp(factor, 0.0f, 1.0f);

	//보간된 사원수
	D3DXQUATERNION result;


	if (FLOAT_EQUAL(factor, 0.0f)){

		result = rotStart;
	}

	else if (FLOAT_EQUAL(factor, 1.0f)){

		result = rotEnd;
	}

	else{

		//사원수끼리 회전 보간을 한다.
		D3DXQuaternionSlerp(
			&result,		//결과
			&rotStart,			//보간 시작값
			&rotEnd,		//보간 끝값
			factor);
	}

	
	//결과를 나에게 셋팅
	this->SetRotateWorld(result);
}


//
// Shake 관련..................
//

//쉐이크 명령이 있으면 Update 를 해주어야 한다.
void cTransform::ShakeUpdate(float timeDelta)
{
	//위치에 대한 흔들림 파워가 있다면..
	if (this->ShakePosPower > 0.0f && this->ShakePosFlag != 0)
	{
		//흔들림은 로컬을 건들여야한다..
		D3DXVECTOR3 localPos(0, 0, 0);
		if (this->ShakePosFlag & SHAKE_X)
			localPos.x = GetRandom(-this->ShakePosPower, this->ShakePosPower);
		if (this->ShakePosFlag & SHAKE_Y)
			localPos.y = GetRandom(-this->ShakePosPower, this->ShakePosPower);
		if (this->ShakePosFlag & SHAKE_Z)
			localPos.z = GetRandom(-this->ShakePosPower, this->ShakePosPower);

		//로컬 포지션으로 셋팅
		this->SetLocalPosition(localPos);

		//쉐이크 파워 깐다...
		this->ShakePosPower -= this->ShakePosRelease * timeDelta;

		if (this->ShakePosPower < 0.0f){
			this->ShakePosPower = 0.0f;
			//원위치
			this->SetLocalPosition(0, 0, 0);
		}

	}

	//회전에 대한 흔들림 파워가 있다면..(참고 ShakeRotPower 는  회전각이된다 )
	if (this->ShakeRotPower > 0.0f && this->ShakeRotFlag != 0)
	{
		//흔들림은 로컬을 건들여야한다..
		D3DXVECTOR3 localRot(0, 0, 0);
		if (this->ShakeRotFlag & SHAKE_X)
			localRot.x = GetRandom(-this->ShakeRotPower, this->ShakeRotPower);
		if (this->ShakeRotFlag & SHAKE_Y)
			localRot.y = GetRandom(-this->ShakeRotPower, this->ShakeRotPower);
		if (this->ShakeRotFlag & SHAKE_Z)
			localRot.z = GetRandom(-this->ShakeRotPower, this->ShakeRotPower);

		//로컬 로테이션으로 셋팅
		this->SetRotateLocal(localRot.x, localRot.y, localRot.z);

		//쉐이크 파워 깐다...
		this->ShakeRotPower -= this->ShakeRotRelease * timeDelta;

		if (this->ShakeRotPower < 0.0f){
			this->ShakeRotPower = 0.0f;
			this->SetRotateLocal(0, 0, 0);
		}



	}
}

//위치에 대한 흔들림을 준다.
void cTransform::ShakePos(float shakePower, float shakeRelase)
{
	this->ShakePosPower = shakePower;
	this->ShakePosRelease = shakeRelase;
}

//회전에 대한 흔들림을 준다.
void cTransform::ShakeRot(float shakePower, float shakeRelase)
{
	this->ShakeRotPower = shakePower;
	this->ShakeRotRelease = shakeRelase;
}

//흔들림 축 설정
void cTransform::SetShakePosFlag(int flag)
{
	this->ShakePosFlag = flag;
}
void cTransform::SetShakeRotFlag(int flag)
{
	this->ShakeRotFlag = flag;
}

