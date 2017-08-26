#pragma once

//�� ���ȣ Define
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2



//�ʱ�ȭ �÷��� ( ��Ʈ �÷��׿� )
#define RESET_POSITION	1
#define RESET_ROTATION	2
#define RESET_SCALE		4


//����ũ �����
#define SHAKE_X 1
#define SHAKE_Y 2
#define SHAKE_Z 4




//3D ������ ��ġ�Ǵ� ��ü�� 
//���������� �����ϰ� ���ִ� Ŭ����
class cTransform
{
protected:
	
	//�������� Union ���� ���� axis �迭�ε� ���� �����ϰ�, right, up, forward �� �� ���ٰ����ϴ�.
	// ( �θ� ������ �θ𿡴��� �������� ���ù��� ������ ������� )
	union {

		//�ٶ󺸴� ���� �� �Ʒ��� 3���� �ݵ�� ����ȭ And ����ȭ �Ǿ��־�� �Ѵ�.
		//�׻�����ȭ �Ǿ��ִ�.
		struct{
			D3DXVECTOR3 right;
			D3DXVECTOR3 up;
			D3DXVECTOR3 forward;
		};

		struct{
			D3DXVECTOR3 axis[3];
		};
	};

	//������ �� 
	D3DXVECTOR3		scale;

	//��ġ��  ( �θ� ������ �θ𿡴��� �������� ������ġ ������ ���� ��ġ )
	D3DXVECTOR3		position;

	//�������
	D3DXMATRIXA16	matLocal;

	//���� ���
	D3DXMATRIXA16	matFinal;
	
	cTransform*		pChild;		//�ڽ� Transform
	cTransform*		pSibling;	//���� Transform
	cTransform*		pParent;	//�θ� Transform
	

	//
	// Shake ����
	//
	float ShakePosPower;			//���� ��ġ�� ���� ��鸲 �Ŀ�
	float ShakeRotPower;			//���� ȸ���� ���� ��鸲 �Ŀ�

	float ShakePosRelease;			//��ġ �Ŀ� �ʴ� ������
	float ShakeRotRelease;			//ȸ�� �Ŀ� �ʴ� ������

	int ShakePosFlag;				//��ġ ����� �÷���
	int ShakeRotFlag;				//ȸ�� ����� �÷���

	float m_fMovespd;	//ī�޶� �̵� �ӵ�

public:
	bool			bAutoUpdate;


public:
	cTransform();
	~cTransform();

	//Transform �ʱ�ȭ
	void Reset(int resetFlag = -1);

	//���̾��Ű �ֻ��� �θ� ��´�.
	cTransform* GetRoot();

	//������ �Ǵٸ� �ڽ��� ���δ�. ( ���� ��ġ ������ü ���̱� )
	void AddChild(cTransform* pChild);

	//Ư�� Transform �� �ٴ´�.  ( ���� ��ġ ������ü �ٱ� )
	void AttachTo(cTransform* pParent);
	
	//�θ�� �ȳ�	( ���� ��ġ ������ü �������� )
	void ReleaseParent();
	
	//��ġ�� ���� ��ǥ��� �����Ѵ�. 
	void SetWorldPosition(float x, float y, float z);
	void SetWorldPosition(D3DXVECTOR3 pos);

	//��ġ�� ���� ��ǥ��� �����Ѵ�.  ( �θ� �ִ� ��� ������ �������� ��ġ, �θ� ���ٸ� SetWorldPosition �� �����ѱ�� )
	void SetLocalPosition(float x, float y, float z);
	void SetLocalPosition(D3DXVECTOR3 pos);


	//���� �������� �̵� ��Ų��.
	void MovePositionWorld(float dx, float dy, float dz);
	void MovePositionWorld(D3DXVECTOR3 delta);

	//�θ� �ִ� ��� ���� �������� �̵� ��Ų��.
	void MovePositionLocal(float dx, float dy, float dz);
	void MovePositionLocal(D3DXVECTOR3 delta);

	//�ڽ��� ��������� �̵� ��Ų��.
	void MovePositionSelf(float dx, float dy, float dz);
	void MovePositionSelf(D3DXVECTOR3 delta);

	//������ ���� 
	void SetScale(float x, float y, float z);
	void SetScale(D3DXVECTOR3 scale);

	//�����ϸ�
	void Scaling(float dx, float dy, float dz);
	void Scaling(D3DXVECTOR3 deltaScale);
	
	//���� ����� ������ �� ���� ��Ĵ�� Transform ���������õȴ�.
	void SetWorldMatrix(D3DXMATRIXA16* pMat);
	
	//ī�޶� �̵� ���ǵ�
	void SetCameraMoveSpd(float fSpd) { m_fMovespd = fSpd; }


	//������� �̿��� Ư�� ȸ�������� ȸ������ ������....
	void SetRotateWorld(float eAngleX, float eAngleY, float aAngleZ);

	// ȸ�� ������� �־��ָ� �� ȸ���� ��� ȸ���Ѵ�.
	void SetRotateWorld(const D3DXQUATERNION& qautWorldRotate);

	// ȸ�� ����� �־��ָ� �� ȸ�� ��Ĵ�� ȸ���Ѵ�.
	void SetRotateWorld(const D3DXMATRIXA16& matWorldRotate);

	//������� �̿��� Ư�� ȸ�������� ȸ������ ������....
	void SetRotateLocal(float eAngleX, float eAngleY, float aAngleZ);

	// ȸ�� ������� �־��ָ� �� ȸ���� ��� ȸ���Ѵ�.
	void SetRotateLocal(const D3DXQUATERNION& qautWorldRotate);

	// ȸ�� ����� �־��ָ� �� ȸ�� ��Ĵ�� ȸ���Ѵ�.
	void SetRotateLocal(const D3DXMATRIXA16& matWorldRotate);


	//���� �������� ȸ�� 
	void RotateWorld(float dAngleX, float dAngleY, float dAngleZ);
	void RotateWorld(D3DXVECTOR3 deltaAngle);
	void RotateWorld(D3DXMATRIXA16& matRotate);		//ȸ�� ����� �־��ָ� ȸ����Ĵ�� ȸ����

	//���� �������� ȸ�� 
	void RotateLocal(float dAngleX, float dAngleY, float dAngleZ);
	void RotateLocal(D3DXVECTOR3 deltaAngle);
	void RotateLocal(D3DXMATRIXA16& matRotate);		//ȸ�� ����� �־��ָ� ȸ����Ĵ�� ȸ����

	//�ڽ��� ��������� ȸ��
	void RotateSelf(float dAngleX, float dAngleY, float dAngleZ);
	void RotateSelf(D3DXVECTOR3 deltaAngle);




	//Ư�� ������ �ٶ󺸰� ȸ���ض�. 	( ���� ���� ������ �ٶ󺸰� )
	void LookDirection(const D3DXVECTOR3& dir, const D3DXVECTOR3& Up = D3DXVECTOR3(0, 1, 0));

	//Ư�������� �ٶ󺸴µ� angle ����ŭ�� ȸ�� �ض� ( ���� ���� ������ �ٶ󺸰� )
	void LookDirection(const D3DXVECTOR3& dir, float angle);

	//Ư����ġ�� �ٶ󺸰� ȸ���ض�. ( ���� ���� ������ �ٶ󺸰� )
	void LookPosition(const D3DXVECTOR3& pos, const D3DXVECTOR3& Up = D3DXVECTOR3(0, 1, 0));

	//Ư����ġ��  �ٶ󺸴µ� angle ����ŭ�� ȸ�� �ض� ( ���� ���� ������ �ٶ󺸰� )
	void LookPosition(const D3DXVECTOR3& pos, float angle);


	//Transform �� ������Ʈ �Ѵ� ( Trasform �� ������ ���ŵǾ����� ���ȴ� )
	void UpdateTransform();

	//�ڽ��� ��� ���� Device �� ������Ʈ �Ѵ�.
	void UpdateToDevice();

	//Transform �� ���� ������ �����ϰ� �׷�����.
	void RenderGizmos();


	void DefaultControl(float timeDelta);

	void MouseDefaultControl(float timeDelta);


	// ------------------------------------------------------------------------
	// Get ����
	// ------------------------------------------------------------------------

	//���� ��ġ�� ��´�.
	D3DXVECTOR3 GetWorldPosition() const;		

	//�κ��� �ִ� ��� �θ��� �������� ������ġ�� ��´�.
	D3DXVECTOR3 GetLocalPosition() const;		

	//��������� ��´�.
	D3DXMATRIXA16 GetFinalMatrix() const;		

	//���� ��´�. 
	D3DXVECTOR3 GetForward(bool bNormalize = true) const;		
	D3DXVECTOR3 GetUp(bool bNormalize = true) const;			
	D3DXVECTOR3 GetRight(bool bNormalize = true) const;			
	void GetScaledAxies(D3DXVECTOR3* pVecArr) const;			
	void GetUnitAxies(D3DXVECTOR3* pVecArr) const;				
	D3DXVECTOR3 GetScaledAxis(int axisNum) const;				
	D3DXVECTOR3 GetUnitAxis(int axisNum) const;					

	//���� ��Ŀ��� ȸ����� ���и� �����´�.
	D3DXMATRIXA16 GetWorldRotateMatrix() const;

	//���������� ��´�.
	D3DXQUATERNION GetWorldQuaternion() const;

	//�������� ��´�.
	D3DXVECTOR3 GetScale() const;

	// ------------------------------------------------------------------------
	// ���� ����
	// ------------------------------------------------------------------------

	//Ư�� Ÿ�� Transform �� ���� ��ġ ���������� �Ѵ�.
	void InterpolatePosition(const cTransform& target, float factor);
	void InterpolatePosition(D3DXVECTOR3 targetPos, float factor);
	void InterpolatePosition(const cTransform& start, const cTransform& end, float factor);
	void InterpolatePosition(D3DXVECTOR3 startPos, D3DXVECTOR3 endPos, float factor);

	//Ư�� Ÿ�� Transform �� ���� ȸ�������� �Ѵ�.
	void InterpolateRoatation(const cTransform& target, float factor);
	void InterpolateRoatation(D3DXQUATERNION quat, float factor);
	void InterpolateRoatation(const cTransform& start, const cTransform& end, float factor);
	void InterpolateRoatation(D3DXQUATERNION rotStart, D3DXQUATERNION rotEnd, float factor);



	//
	// Shake ����..................
	//

	//����ũ ����� ������ Update �� ���־�� �Ѵ�.
	void ShakeUpdate(float timeDelta);

	//��ġ�� ���� ��鸲�� �ش�.
	void ShakePos(float shakePower, float shakeRelase);

	//ȸ���� ���� ��鸲�� �ش�.
	void ShakeRot(float shakePower, float shakeRelase);

	//��鸲 �� ����
	void SetShakePosFlag(int flag);
	void SetShakeRotFlag(int flag);




};

