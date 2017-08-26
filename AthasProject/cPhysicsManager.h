#pragma once

#include "cSingletonBase.h"


//���� ����ü
typedef struct tagRay{
	D3DXVECTOR3 origin;			//�������� ���� ��ġ
	D3DXVECTOR3 direction;		//�������� ���� ( ����ȭ �Ǿ��־�� �� )
}Ray, *LPRay;


class cBoundBox;
class cBoundSphere;
class cTransform;
class cXMesh_Static;


class cPhysicsManager : public cSingletonBase<cPhysicsManager>
{
public:
	cPhysicsManager();
	~cPhysicsManager();


	//���̰�  ���Ǿ �浹�ߴ��� Ȯ��
	bool IsRayHitSphere(
		LPRay Ray,				//����
		D3DXVECTOR3* worldCenter,	//���� ����
		float radius,				//��������
		float checkDisance,		//üũ �Ÿ�
		bool innnerOut,			//�����ʿ��� �ٱ������� üũ�Ҳ���?
		D3DXVECTOR3* pHitPos,     //Hit ��ġ ( NULL �̸� ���� �ȵ� )
		D3DXVECTOR3* pHitNormal	  //Hit �� �븻 ( NULL �̸� ���� �ȵ� )
		);


	//���̰�  OBB �ڽ��� �浹�ߴ��� Ȯ��
	bool IsRayHitOBBBox(
		LPRay Ray,
		D3DXVECTOR3* obbMin,		//��ȯ�� �ڽ��� Min ����
		D3DXVECTOR3* obbMax,		//��ȯ�� �ڽ��� Max ����
		D3DXMATRIXA16* obbWorld,	//OBB ���� Matrix
		bool innnerOut,			//�����ʿ��� �ٱ������� üũ�Ҳ���?
		D3DXVECTOR3* pHitPos,     //Hit ��ġ ( NULL �̸� ���� �ȵ� )
		D3DXVECTOR3* pHitNormal	  //Hit �� �븻 ( NULL �̸� ���� �ȵ� )
		);

	//���̰�  �ٿ�� �ڽ��� �浹�ߴ�?
	bool IsRayHitBoundBox(
		LPRay Ray,					//����
		cBoundBox* pBoundBox,		//�ٿ�� �ڽ�
		cTransform* pBoundTrans,	//�ٿ�� �ڽ��� Transform
		bool innnerOut,				//�����ʿ��� �ٱ������� üũ�Ҳ���?
		D3DXVECTOR3* pHitPos,		//Hit ��ġ ( NULL �̸� ���� �ȵ� )
		D3DXVECTOR3* pHitNormal		//Hit �� �븻 ( NULL �̸� ���� �ȵ� )
		);


	//���̰�  �޽���  �� �浹�ߴ�?
	bool IsRayHitXMeshStatic(
		LPRay Ray,					//����
		cXMesh_Static* pMesh,		//üũ�� �޽�
		cTransform* pMeshTrans,		//�޽� Transform
		bool innnerOut,				//�����ʿ��� �ٱ������� üũ�Ҳ���?
		D3DXVECTOR3* pHitPos,		//Hit ��ġ ( NULL �̸� ���� �ȵ� )
		D3DXVECTOR3* pHitNormal		//Hit �� �븻 ( NULL �̸� ���� �ȵ� )
		);



	//2���� �ٿ�� ���� �⵿ ������ ��´�.
	bool IsOverlapSphere(
		cTransform* pTransA, cBoundSphere* pBoundA, cTransform* pTransB, cBoundSphere* pBoundB);
	
	bool IsOverlapBox(
		cTransform* pTransA, cBoundBox* pBoundA, cTransform* pTransB, cBoundBox* pBoundB);


	//2���� �ٿ�忡���� �⵿�����ϰ� ��ġ�� �ʰ� �Ѵ�.
	//moveFactor �� 0 �ϼ��� ��Ĩ�� ���� B �� �����δ�.
	//moveFactor �� 0.5 �̸� ��Ĩ�� ���� A �� B �� �Ȱ��������� �����δ�.
	//moveFactor �� 1 �ϼ��� ��Ĩ�� ���� A �� �����δ�.
	bool IsBlockingSphere(
		cTransform* pTransA, cBoundSphere* pBoundA,
		cTransform* pTransB, cBoundSphere* pBoundB,
		float moveFactor = 0.0f);

	bool IsBlockingBox(
		cTransform* pTransA, cBoundBox* pBoundA,
		cTransform* pTransB, cBoundBox* pBoundB,
		float moveFactor = 0.0f);



	//�� 3���� ���� ����� �����.
	void CreatePlane(
		LPD3DXPLANE pOutPlane, const D3DXVECTOR3* p0, const D3DXVECTOR3* p1, const D3DXVECTOR3* p2);

	//������ �������� �������� �ִܰŸ�
	float PlaneDot(const LPD3DXPLANE pPlane, const D3DXVECTOR3* point);

	//�������� ����� �浹 ��ġ
	bool IntersectRayToPlane(D3DXVECTOR3* pOut, const LPRay pRay, const LPD3DXPLANE pPlane, bool bCheck2Side = false);



};

#define PHYSICS_MGR cPhysicsManager::GetInstance()