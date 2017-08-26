#pragma once

//각 축번호 Define
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2



//초기화 플레그 ( 비트 플레그용 )
#define RESET_POSITION	1
#define RESET_ROTATION	2
#define RESET_SCALE		4


//쉐이크 허용축
#define SHAKE_X 1
#define SHAKE_Y 2
#define SHAKE_Z 4




//3D 공간에 배치되는 물체의 
//공간정보를 유용하게 해주는 클래스
class cTransform
{
protected:
	
	//축정보는 Union 으로 묵여 axis 배열로도 접근 가능하고, right, up, forward 로 도 접근가능하다.
	// ( 부모가 있으면 부모에대한 상태적인 로컬방향 없으면 월드방향 )
	union {

		//바라보는 방향 값 아래의 3개는 반드시 직교화 And 정규화 되어있어야 한다.
		//항상정규화 되어있다.
		struct{
			D3DXVECTOR3 right;
			D3DXVECTOR3 up;
			D3DXVECTOR3 forward;
		};

		struct{
			D3DXVECTOR3 axis[3];
		};
	};

	//스케일 값 
	D3DXVECTOR3		scale;

	//위치값  ( 부모가 있으면 부모에대한 상태적인 로컬위치 없으면 월드 위치 )
	D3DXVECTOR3		position;

	//로컬행렬
	D3DXMATRIXA16	matLocal;

	//최종 행렬
	D3DXMATRIXA16	matFinal;
	
	cTransform*		pChild;		//자식 Transform
	cTransform*		pSibling;	//형재 Transform
	cTransform*		pParent;	//부모 Transform
	

	//
	// Shake 관련
	//
	float ShakePosPower;			//현제 위치에 대한 흔들림 파워
	float ShakeRotPower;			//현제 회전에 대한 흔들림 파워

	float ShakePosRelease;			//위치 파워 초당 해제량
	float ShakeRotRelease;			//회전 파워 초당 해제량

	int ShakePosFlag;				//위치 허용축 플레그
	int ShakeRotFlag;				//회전 허용축 플레그

	float m_fMovespd;	//카메라 이동 속도

public:
	bool			bAutoUpdate;


public:
	cTransform();
	~cTransform();

	//Transform 초기화
	void Reset(int resetFlag = -1);

	//하이어라키 최상위 부모를 얻는다.
	cTransform* GetRoot();

	//내꺼에 또다른 자식을 붙인다. ( 숙제 위치 유지한체 붙이기 )
	void AddChild(cTransform* pChild);

	//특정 Transform 에 붙는다.  ( 숙제 위치 유지한체 붙기 )
	void AttachTo(cTransform* pParent);
	
	//부모와 안녕	( 숙제 위치 유지한체 떨어지기 )
	void ReleaseParent();
	
	//위치를 월드 좌표계로 셋팅한다. 
	void SetWorldPosition(float x, float y, float z);
	void SetWorldPosition(D3DXVECTOR3 pos);

	//위치를 로컬 좌표계로 셋팅한다.  ( 부모가 있는 경우 무모의 상태적인 위치, 부모가 없다면 SetWorldPosition 와 동일한기능 )
	void SetLocalPosition(float x, float y, float z);
	void SetLocalPosition(D3DXVECTOR3 pos);


	//월드 기준으로 이동 시킨다.
	void MovePositionWorld(float dx, float dy, float dz);
	void MovePositionWorld(D3DXVECTOR3 delta);

	//부모가 있는 경우 로컬 기준으로 이동 시킨다.
	void MovePositionLocal(float dx, float dy, float dz);
	void MovePositionLocal(D3DXVECTOR3 delta);

	//자신의 축기준으로 이동 시킨다.
	void MovePositionSelf(float dx, float dy, float dz);
	void MovePositionSelf(D3DXVECTOR3 delta);

	//스케일 셋팅 
	void SetScale(float x, float y, float z);
	void SetScale(D3DXVECTOR3 scale);

	//스케일링
	void Scaling(float dx, float dy, float dz);
	void Scaling(D3DXVECTOR3 deltaScale);
	
	//월드 행렬을 넣으면 그 월드 행렬대로 Transform 정보가셋팅된다.
	void SetWorldMatrix(D3DXMATRIXA16* pMat);
	
	//카메라 이동 스피드
	void SetCameraMoveSpd(float fSpd) { m_fMovespd = fSpd; }


	//사원수를 이용한 특정 회전값으로 회전량을 가져라....
	void SetRotateWorld(float eAngleX, float eAngleY, float aAngleZ);

	// 회전 사원수를 넣어주면 그 회전값 대로 회전한다.
	void SetRotateWorld(const D3DXQUATERNION& qautWorldRotate);

	// 회전 행렬을 넣어주면 그 회전 행렬대로 회전한다.
	void SetRotateWorld(const D3DXMATRIXA16& matWorldRotate);

	//사원수를 이용한 특정 회전값으로 회전량을 가져라....
	void SetRotateLocal(float eAngleX, float eAngleY, float aAngleZ);

	// 회전 사원수를 넣어주면 그 회전값 대로 회전한다.
	void SetRotateLocal(const D3DXQUATERNION& qautWorldRotate);

	// 회전 행렬을 넣어주면 그 회전 행렬대로 회전한다.
	void SetRotateLocal(const D3DXMATRIXA16& matWorldRotate);


	//월드 기준으로 회전 
	void RotateWorld(float dAngleX, float dAngleY, float dAngleZ);
	void RotateWorld(D3DXVECTOR3 deltaAngle);
	void RotateWorld(D3DXMATRIXA16& matRotate);		//회전 행렬을 넣어주면 회전행렬대로 회전함

	//로컬 기준으로 회전 
	void RotateLocal(float dAngleX, float dAngleY, float dAngleZ);
	void RotateLocal(D3DXVECTOR3 deltaAngle);
	void RotateLocal(D3DXMATRIXA16& matRotate);		//회전 행렬을 넣어주면 회전행렬대로 회전함

	//자신의 축기준으로 회전
	void RotateSelf(float dAngleX, float dAngleY, float dAngleZ);
	void RotateSelf(D3DXVECTOR3 deltaAngle);




	//특정 방향을 바라보게 회전해라. 	( 숙제 월드 단위로 바라보게 )
	void LookDirection(const D3DXVECTOR3& dir, const D3DXVECTOR3& Up = D3DXVECTOR3(0, 1, 0));

	//특정방향을 바라보는데 angle 각만큼만 회전 해라 ( 숙제 월드 단위로 바라보게 )
	void LookDirection(const D3DXVECTOR3& dir, float angle);

	//특정위치를 바라보게 회전해라. ( 숙제 월드 단위로 바라보게 )
	void LookPosition(const D3DXVECTOR3& pos, const D3DXVECTOR3& Up = D3DXVECTOR3(0, 1, 0));

	//특정위치를  바라보는데 angle 각만큼만 회전 해라 ( 숙제 월드 단위로 바라보게 )
	void LookPosition(const D3DXVECTOR3& pos, float angle);


	//Transform 을 업데이트 한다 ( Trasform 의 정보가 갱신되었을대 사용된다 )
	void UpdateTransform();

	//자신의 행렬 값을 Device 에 업데이트 한다.
	void UpdateToDevice();

	//Transform 의 현제 방위를 간단하게 그려본다.
	void RenderGizmos();


	void DefaultControl(float timeDelta);

	void MouseDefaultControl(float timeDelta);


	// ------------------------------------------------------------------------
	// Get 관련
	// ------------------------------------------------------------------------

	//월드 위치를 얻는다.
	D3DXVECTOR3 GetWorldPosition() const;		

	//부보가 있는 경우 부모의 상태적인 로컬위치를 얻는다.
	D3DXVECTOR3 GetLocalPosition() const;		

	//최종행렬을 얻는다.
	D3DXMATRIXA16 GetFinalMatrix() const;		

	//축을 얻는다. 
	D3DXVECTOR3 GetForward(bool bNormalize = true) const;		
	D3DXVECTOR3 GetUp(bool bNormalize = true) const;			
	D3DXVECTOR3 GetRight(bool bNormalize = true) const;			
	void GetScaledAxies(D3DXVECTOR3* pVecArr) const;			
	void GetUnitAxies(D3DXVECTOR3* pVecArr) const;				
	D3DXVECTOR3 GetScaledAxis(int axisNum) const;				
	D3DXVECTOR3 GetUnitAxis(int axisNum) const;					

	//월드 행렬에서 회전행렬 성분만 가져온다.
	D3DXMATRIXA16 GetWorldRotateMatrix() const;

	//월드사원수를 얻는다.
	D3DXQUATERNION GetWorldQuaternion() const;

	//스케일을 얻는다.
	D3DXVECTOR3 GetScale() const;

	// ------------------------------------------------------------------------
	// 보간 관련
	// ------------------------------------------------------------------------

	//특정 타겟 Transform 에 대해 위치 선형보간을 한다.
	void InterpolatePosition(const cTransform& target, float factor);
	void InterpolatePosition(D3DXVECTOR3 targetPos, float factor);
	void InterpolatePosition(const cTransform& start, const cTransform& end, float factor);
	void InterpolatePosition(D3DXVECTOR3 startPos, D3DXVECTOR3 endPos, float factor);

	//특정 타겟 Transform 에 대해 회전보간을 한다.
	void InterpolateRoatation(const cTransform& target, float factor);
	void InterpolateRoatation(D3DXQUATERNION quat, float factor);
	void InterpolateRoatation(const cTransform& start, const cTransform& end, float factor);
	void InterpolateRoatation(D3DXQUATERNION rotStart, D3DXQUATERNION rotEnd, float factor);



	//
	// Shake 관련..................
	//

	//쉐이크 명령이 있으면 Update 를 해주어야 한다.
	void ShakeUpdate(float timeDelta);

	//위치에 대한 흔들림을 준다.
	void ShakePos(float shakePower, float shakeRelase);

	//회전에 대한 흔들림을 준다.
	void ShakeRot(float shakePower, float shakeRelase);

	//흔들림 축 설정
	void SetShakePosFlag(int flag);
	void SetShakeRotFlag(int flag);




};

