#include "stdafx.h"
#include "cPhysicsManager.h"
#include "cBoundBox.h"

cPhysicsManager::cPhysicsManager()
{
}


cPhysicsManager::~cPhysicsManager()
{
}

//���̰�  ���Ǿ �浹�ߴ��� Ȯ��
bool cPhysicsManager::IsRayHitSphere(
	LPRay Ray,				//����
	D3DXVECTOR3* worldCenter,	//���� ����
	float radius,				//��������
	float checkDisance,		//üũ �Ÿ�
	bool innnerOut,			//�����ʿ��� �ٱ������� üũ�Ҳ���?
	D3DXVECTOR3* pHitPos,     //Hit ��ġ ( NULL �̸� ���� �ȵ� )
	D3DXVECTOR3* pHitNormal	  //Hit �� �븻 ( NULL �̸� ���� �ȵ� )
	)
{
	//������ ���������� ���� ���� ���ͱ����� ���⺤��
	D3DXVECTOR3 dirToCenter = *worldCenter - Ray->origin;
	
	//dirToCenter ���� ����
	float dsq = D3DXVec3LengthSq(&dirToCenter);

	//�������� ����
	float rSq = radius * radius;

	//��������
	float x = D3DXVec3Dot(&dirToCenter, &Ray->direction);

	//���� �������� ���ȿ��ִ���...
	bool inRayStartSphereIn = dsq < rSq;


	//x �� �������... �����ʿ��� �浹 üũ�� ���Ѵٸ� �ݵ�� �����Ѵ�.
	if ((innnerOut == false && x < 0.0f) ||
		(innnerOut == false && inRayStartSphereIn )){	//������ ��ġ�� ���ȿ��ִٸ� ����
		return false;
	}

	float ySq = dsq - x * x;

	//�浹�ƴ�....
	if (ySq > rSq)
		return false;

	//������� �Դٸ�.. �浹.
	
	//�浹 ��ġ�� ��� �ʹٸ�....
	if (pHitPos != NULL)
	{
		float q = sqrt( rSq - ySq );



		//��Ʈ �Ÿ���
		if (inRayStartSphereIn)
			q *= -1.0f;		//���ȿ��ִٸ� q �� �ݴ� ��������...

		float hitDist = x - q;

		//��Ʈ ��ġ��
		(*pHitPos) = Ray->origin + Ray->direction * hitDist;

		//��Ʈ �븻
		if (pHitNormal != NULL)
		{
			(*pHitNormal) = (*pHitPos) - (*worldCenter);

			//����ȭ
			D3DXVec3Normalize(pHitNormal, pHitNormal);
		}
	
	}


	return true;
}

//���̰�  OBB �ڽ��� �浹�ߴ��� Ȯ��
bool cPhysicsManager::IsRayHitOBBBox(
	LPRay Ray,
	D3DXVECTOR3* obbMin,		//��ȯ�� �ڽ��� Min ����
	D3DXVECTOR3* obbMax,		//��ȯ�� �ڽ��� Max ����
	D3DXMATRIXA16* obbWorld,	//OBB ���� Matrix
	bool innnerOut,			//�����ʿ��� �ٱ������� üũ�Ҳ���?
	D3DXVECTOR3* pHitPos,     //Hit ��ġ ( NULL �̸� ���� �ȵ� )
	D3DXVECTOR3* pHitNormal	  //Hit �� �븻 ( NULL �̸� ���� �ȵ� )
	)
{

	//��� üũ�� ���� �¾�� �ֳ�? ���üũ�� �ϸ� �浹������ 2�� �� �����ϱ�.
	static vector<D3DXVECTOR3> hits;
	static vector<D3DXVECTOR3> hitNormals;
	hits.clear();
	hitNormals.clear();

	//���̸� ������ �����.
	D3DXMATRIXA16 invMat;
	D3DXMatrixInverse(&invMat, NULL, obbWorld);

	D3DXVECTOR3 ori;
	D3DXVec3TransformCoord(&ori, &Ray->origin, &invMat);
	D3DXVECTOR3 dir;
	D3DXVec3TransformNormal(&dir, &Ray->direction, &invMat);

	tagRay localRay;
	localRay.origin = ori;
	localRay.direction = dir;

	D3DXVECTOR3* min = obbMin;
	D3DXVECTOR3* max = obbMax;

	//   5-------6
	//  /|      /|
	// 1-------2 |
	// | 4-----|-7
	// |/      |/
	// 0-------3 

	//8 ���� �� ���Ѵ�.
	D3DXVECTOR3 p[8] = {
		D3DXVECTOR3(min->x, min->y, min->z),
		D3DXVECTOR3(min->x, max->y, min->z),
		D3DXVECTOR3(max->x, max->y, min->z),
		D3DXVECTOR3(max->x, min->y, min->z),

		D3DXVECTOR3(min->x, min->y, max->z),
		D3DXVECTOR3(min->x, max->y, max->z),
		D3DXVECTOR3(max->x, max->y, max->z),
		D3DXVECTOR3(max->x, min->y, max->z)
	};

	D3DXPLANE plane;

	D3DXVECTOR3 hitPos;
	D3DXVECTOR3 hitNormal;

	//�ڸ� ���
	this->CreatePlane(&plane, p + 0, p + 1, p + 2);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.x >= min->x && hitPos.x <= max->x &&
			hitPos.y >= min->y && hitPos.y <= max->y)
		{
			//���ʿ����� üũ��
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));
			}

			//�ٱ��ʿ����� üũ��...
			else
			{
				//����� ���ܼ� �־���
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//����� ���ܼ� �־���
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(0, 0, -1), obbWorld);
				}

				return true;
			}

			
		}
	}

	//�ո� ���
	this->CreatePlane(&plane, p + 7, p + 6, p + 5);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.x >= min->x && hitPos.x <= max->x &&
			hitPos.y >= min->y && hitPos.y <= max->y)
		{
			//���ʿ����� üũ��
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));

			}

			//�ٱ��ʿ����� üũ��...
			else
			{
				//����� ���ܼ� �־���
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//����� ���ܼ� �־���
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(plane.a, plane.b, plane.c), obbWorld);
				}

				return true;
			}
		}
	}


	//��Ʈ ������ 2�� �̻��ΰ��
	if (hits.size() >= 2)
	{
		//������ ��Ʈ ��ġ��...
		
		float distSq0 = Vector3DistanceSq(&localRay.origin, &hits[0]);
		float distSq1 = Vector3DistanceSq(&localRay.origin, &hits[1]);

		D3DXVECTOR3 hitPosResult;
		D3DXVECTOR3 hitNorResult;
		if (distSq0 < distSq1){
			hitPosResult = hits[0];
			hitNorResult = hitNormals[0];
		}
		else{
			hitPosResult = hits[1];
			hitNorResult = hitNormals[1	];
		}

		//����� ���ܼ� �־���
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hitPosResult, obbWorld);
		}


		//����� ���ܼ� �־���
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNorResult, obbWorld);
		}
		return true;

	}


	//���� ���
	this->CreatePlane(&plane, p + 1, p + 5, p + 6);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.x >= min->x && hitPos.x <= max->x &&
			hitPos.z >= min->z && hitPos.z <= max->z)
		{
			//���ʿ����� üũ��
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));
			}

			//�ٱ��ʿ����� üũ��...
			else
			{
				//����� ���ܼ� �־���
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//����� ���ܼ� �־���
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(plane.a, plane.b, plane.c), obbWorld);
				}

				return true;
			}
		}
	}

	//��Ʈ ������ 2�� �̻��ΰ��
	if (hits.size() >= 2)
	{
		//������ ��Ʈ ��ġ��...

		float distSq0 = Vector3DistanceSq(&localRay.origin, &hits[0]);
		float distSq1 = Vector3DistanceSq(&localRay.origin, &hits[1]);

		D3DXVECTOR3 hitPosResult;
		D3DXVECTOR3 hitNorResult;
		if (distSq0 < distSq1){
			hitPosResult = hits[0];
			hitNorResult = hitNormals[0];
		}
		else{
			hitPosResult = hits[1];
			hitNorResult = hitNormals[1];
		}

		//����� ���ܼ� �־���
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hitPosResult, obbWorld);
		}


		//����� ���ܼ� �־���
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNorResult, obbWorld);
		}
		return true;

	}


	//�Ʒ��� ���
	this->CreatePlane(&plane, p + 4, p + 0, p + 3);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.x >= min->x && hitPos.x <= max->x &&
			hitPos.z >= min->z && hitPos.z <= max->z)
		{
			//���ʿ����� üũ��
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));

			}

			//�ٱ��ʿ����� üũ��...
			else
			{
				//����� ���ܼ� �־���
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//����� ���ܼ� �־���
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(plane.a, plane.b, plane.c), obbWorld);
				}

				return true;
			}
		}
	}

	//��Ʈ ������ 2�� �̻��ΰ��
	if (hits.size() >= 2)
	{
		//������ ��Ʈ ��ġ��...

		float distSq0 = Vector3DistanceSq(&localRay.origin, &hits[0]);
		float distSq1 = Vector3DistanceSq(&localRay.origin, &hits[1]);

		D3DXVECTOR3 hitPosResult;
		D3DXVECTOR3 hitNorResult;
		if (distSq0 < distSq1){
			hitPosResult = hits[0];
			hitNorResult = hitNormals[0];
		}
		else{
			hitPosResult = hits[1];
			hitNorResult = hitNormals[1];
		}

		//����� ���ܼ� �־���
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hitPosResult, obbWorld);
		}


		//����� ���ܼ� �־���
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNorResult, obbWorld);
		}
		return true;

	}



	//��� ���
	this->CreatePlane(&plane, p + 3, p + 2, p + 6);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.z >= min->z && hitPos.z <= max->z &&
			hitPos.y >= min->y && hitPos.y <= max->y)
		{
			//���ʿ����� üũ��
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));

			}

			//�ٱ��ʿ����� üũ��...
			else
			{
				//����� ���ܼ� �־���
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//����� ���ܼ� �־���
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(plane.a, plane.b, plane.c), obbWorld);
				}

				return true;
			}
		}
	}

	//��Ʈ ������ 2�� �̻��ΰ��
	if (hits.size() >= 2)
	{
		//������ ��Ʈ ��ġ��...

		float distSq0 = Vector3DistanceSq(&localRay.origin, &hits[0]);
		float distSq1 = Vector3DistanceSq(&localRay.origin, &hits[1]);

		D3DXVECTOR3 hitPosResult;
		D3DXVECTOR3 hitNorResult;
		if (distSq0 < distSq1){
			hitPosResult = hits[0];
			hitNorResult = hitNormals[0];
		}
		else{
			hitPosResult = hits[1];
			hitNorResult = hitNormals[1];
		}

		//����� ���ܼ� �־���
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hitPosResult, obbWorld);
		}


		//����� ���ܼ� �־���
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNorResult, obbWorld);
		}
		return true;

	}


	//�¸� ���
	this->CreatePlane(&plane, p + 5, p + 1, p + 0);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.z >= min->z && hitPos.z <= max->z &&
			hitPos.y >= min->y && hitPos.y <= max->y)
		{
			//���ʿ����� üũ��
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));
			}

			//�ٱ��ʿ����� üũ��...
			else
			{
				//����� ���ܼ� �־���
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//����� ���ܼ� �־���
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(plane.a, plane.b, plane.c), obbWorld);
				}

				return true;
			}
		}
	}


	//��Ʈ ������ 2�� �̻��ΰ��
	if (hits.size() >= 2)
	{
		//������ ��Ʈ ��ġ��...

		float distSq0 = Vector3DistanceSq(&localRay.origin, &hits[0]);
		float distSq1 = Vector3DistanceSq(&localRay.origin, &hits[1]);

		D3DXVECTOR3 hitPosResult;
		D3DXVECTOR3 hitNorResult;
		if (distSq0 < distSq1){
			hitPosResult = hits[0];
			hitNorResult = hitNormals[0];
		}
		else{
			hitPosResult = hits[1];
			hitNorResult = hitNormals[1];
		}

		//����� ���ܼ� �־���
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hitPosResult, obbWorld);
		}


		//����� ���ܼ� �־���
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNorResult, obbWorld);
		}
		return true;

	}


	else if (hits.size() == 1)
	{
		//����� ���ܼ� �־���
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hits[0], obbWorld);
		}


		//����� ���ܼ� �־���
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNormals[0], obbWorld);
		}
		return true;
	}



	return false;

	
}



//���̰�  �ٿ�� �ڽ��� �浹�ߴ�?
bool cPhysicsManager::IsRayHitBoundBox(
	LPRay Ray,					//����
	cBoundBox* pBoundBox,		//�ٿ�� �ڽ�
	cTransform* pBoundTrans,	//�ٿ�� �ڽ��� Transform
	bool innnerOut,				//�����ʿ��� �ٱ������� üũ�Ҳ���?
	D3DXVECTOR3* pHitPos,		//Hit ��ġ ( NULL �̸� ���� �ȵ� )
	D3DXVECTOR3* pHitNormal		//Hit �� �븻 ( NULL �̸� ���� �ȵ� )
	)
{
	//���� ���Ͷ� �������� ��´�.
	D3DXVECTOR3 center;
	float radius;
	pBoundBox->GetWorldCenterRadius(pBoundTrans, &center, &radius);

	//���浹����
	if (IsRayHitSphere(Ray, &center, radius, 10000.0f, true, NULL, NULL) == false)
		return false;

	//�簢���� �浹üũ �ʿ�
	return IsRayHitOBBBox(
		Ray,
		&pBoundBox->localMinPos,
		&pBoundBox->localMaxPos,
		&pBoundTrans->GetFinalMatrix(),
		innnerOut,
		pHitPos,
		pHitNormal);



}

//���̰�  �޽���  �� �浹�ߴ�?
bool cPhysicsManager::IsRayHitXMeshStatic(
	LPRay Ray,					//����
	cXMesh_Static* pMesh,		//üũ�� �޽�
	cTransform* pMeshTrans,		//�޽� Transform
	bool innnerOut,				//�����ʿ��� �ٱ������� üũ�Ҳ���?
	D3DXVECTOR3* pHitPos,		//Hit ��ġ ( NULL �̸� ���� �ȵ� )
	D3DXVECTOR3* pHitNormal		//Hit �� �븻 ( NULL �̸� ���� �ȵ� )
	)
{
	//�޽��� Default �ٿ�� �ڽ��� �浹üũ �ϰ�
	if (IsRayHitBoundBox(
		Ray,
		pMesh->GetDefaultBound(),
		pMeshTrans,
		true,
		NULL,
		NULL) == false)
		return false;


	//���̸� Mesh �� ���÷� ������ ���
	D3DXMATRIXA16 invMat;
	D3DXMatrixInverse(&invMat, NULL, &pMeshTrans->GetFinalMatrix() );

	D3DXVECTOR3 ori;
	D3DXVec3TransformCoord(&ori, &Ray->origin, &invMat);
	D3DXVECTOR3 dir;
	D3DXVec3TransformNormal(&dir, &Ray->direction, &invMat);



	BOOL	 bHit;
	DWORD	 FaceIndex;
	float	 hitDist;
	DWORD	 hitCount;

	//�޽��� ����´�� ��� üũ
	for (DWORD i = 0; i < pMesh->GetMaterialNum(); i++)
	{

		//���̿� �޽��� ����°��� �浹üũ�� ���ش�.
		D3DXIntersectSubset(
			pMesh->GetMesh(),
			i,				//üŬ�� �޽��� ����� �ε���
			&ori,			//���� ������ ( Mesh ���÷� ����� )
			&dir,			//���� ���� ( Mesh ���÷� ����� )
			&bHit,			//�浹 ��������
			&FaceIndex,		//�浹�� ���� �ε���
			NULL,			//�浹 �ﰢ���� U
			NULL,			//�浹 �ﰢ���� V
			&hitDist,		//�浹 �Ÿ�
			NULL,
			&hitCount);		//�ٴ� ��Ʈ�� �浹 ī���...

		
		
			if (bHit){

				if (pHitPos != NULL){

					*pHitPos = ori + dir * hitDist;
					//�����...
					D3DXVec3TransformCoord(pHitPos, pHitPos, &pMeshTrans->GetFinalMatrix());
				}

				if (pHitNormal != NULL){

					*pHitNormal = pMesh->GetFaceNormal(FaceIndex);
					//�����...
					D3DXVec3TransformNormal(pHitNormal, pHitNormal, &pMeshTrans->GetFinalMatrix());
				}


				return true;
			}

	}

	return false;

}



//2���� �ٿ�� ���� �⵿ ������ ��´�.
bool  cPhysicsManager::IsOverlapSphere(
	cTransform* pTransA, cBoundSphere* pBoundA, cTransform* pTransB, cBoundSphere* pBoundB)
{
	//�˾Ƽ� �����....
	return false;
}

bool cPhysicsManager::IsOverlapBox(
	cTransform* pTransA, cBoundBox* pBoundA, cTransform* pTransB, cBoundBox* pBoundB)
{
	//���ô��� ���������� �浹�ȵǾ��ٸ� Box������ �׾��� ����� �浹 �ȵȴ�...
	//if (IsOverlapSphere(pTransA, pBoundA, pTransB, pBoundB) == false)
	//	return false;

	//
	// OBB �浹 üũ ( ���⼭ ���� �� �� ���� ..... )
	//


	//�迭����
	//X = 0, Y = 1, Z = 2;
	//OBB �浹�� �ʿ��� �浹 ����ü (���� ������ ����)
	struct OBB{
		D3DXVECTOR3 center;		//�߽���
		D3DXVECTOR3 axis[3];	//�����	
		float halfLength[3];	//���࿡ ���� ���� ũ��
	};

	//
	// A �ٿ���� ���� �浹 ����ü
	//
	OBB obbA;

	//A �ٿ���� ����
	D3DXVECTOR3 centerA;
	float radiusA;
	pBoundA->GetWorldCenterRadius(pTransA, &centerA, &radiusA);

	//���� ����
	obbA.center = centerA;

	//�� �� ����
	obbA.axis[0] = pTransA->GetRight();
	obbA.axis[1] = pTransA->GetUp();
	obbA.axis[2] = pTransA->GetForward();

	//������ ����
	D3DXVECTOR3 halfSizeA;
	pBoundA->GetWorldHalfSize(pTransA, &halfSizeA);
	obbA.halfLength[0] = halfSizeA.x;
	obbA.halfLength[1] = halfSizeA.y;
	obbA.halfLength[2] = halfSizeA.z;

	//
	// B �ٿ���� ���� �浹 ����ü
	//
	OBB obbB;

	//A �ٿ���� ����
	D3DXVECTOR3 centerB;
	float radiusB;
	pBoundB->GetWorldCenterRadius(pTransB, &centerB, &radiusB);

	//���� ����
	obbB.center = centerB;

	//�� �� ����
	obbB.axis[0] = pTransB->GetRight();
	obbB.axis[1] = pTransB->GetUp();
	obbB.axis[2] = pTransB->GetForward();

	//������ ����
	D3DXVECTOR3 halfSizeB;
	pBoundB->GetWorldHalfSize(pTransB, &halfSizeB);
	obbB.halfLength[0] = halfSizeB.x;
	obbB.halfLength[1] = halfSizeB.y;
	obbB.halfLength[2] = halfSizeB.z;


	//
	// OBB �浹
	//
	float cos[3][3];				//���� �������� ���� �ڻ��� ��  [A��][B��]  ( [0][1] => ���ε����� A�� X ��� B�� Y ���� ������ ���� cos ���̴�, A �� X �� B �� Y ������ ������ )
	float absCos[3][3];				//���� �������� ���� �ڻ��� ���밪  [A��][B��]  ( [0][1] => ���ε����� A�� X ��� B�� Y ���� ������ ���� cos ���� ���̴� )
	float dist[3];					//A �ٿ�� �������� A �߽������� B �� �߽������͸� ������ ��������	

	const float cutOff = 0.9999f;		//���� �Ǵ��� �ϱ����� �ƿ��� �� ( ��� �� ���� cos ��� ���� �̺��� ũ�ٸ� �� �浹ü�� ������ �����ϴٴ� ���� )
	bool existParallelPair = false;		//�� ���̶� �����ϳ�?


	//A ���� B �� ���⺤��
	D3DXVECTOR3 D = obbB.center - obbA.center;

	float r, r0, r1;			//r0 �� r1 �� ���� r ���� ������ �浹 ���� 



	for (int a = 0; a < 3; a++)
	{
		for (int b = 0; b < 3; b++)
		{
			cos[a][b] = D3DXVec3Dot(&obbA.axis[a], &obbB.axis[b]);
			absCos[a][b] = abs(cos[a][b]);

			//������ ���� ���� �Ǵ� ��Ȯ��
			if (absCos[a][b] > cutOff) existParallelPair = true;

		}

		//���ͳ����� ���⺤�͸� A �ٿ�� Axis �� ������ �Ÿ�
		dist[a] = D3DXVec3Dot(&obbA.axis[a], &D);
	}


	//
	// A �ٿ�� �ڽ��� X ���� �������� �� ����
	//

	//r �� dist[0] �� ���� ���� �ȴ�.
	r = abs(dist[0]);

	//r0 
	r0 = obbA.halfLength[0];

	/*
	r1 = abs(D3DXVec3Dot(&obbA.axis[0], &(obbB.axis[0] * obbB.halfLength[0]))) +
		abs(D3DXVec3Dot(&obbA.axis[0], &(obbB.axis[1] * obbB.halfLength[1]))) +
		abs(D3DXVec3Dot(&obbA.axis[0], &(obbB.axis[2] * obbB.halfLength[2])));
		*/
	/*
	r1 = abs(D3DXVec3Dot(&obbA.axis[0], &obbB.axis[0])) * obbB.halfLength[0] +
		abs(D3DXVec3Dot(&obbA.axis[0], &obbB.axis[1])) * obbB.halfLength[1] +
		abs(D3DXVec3Dot(&obbA.axis[0], &obbB.axis[2])) * obbB.halfLength[2];
	*/
	r1 = absCos[0][0] * obbB.halfLength[0] +
		 absCos[0][1] * obbB.halfLength[1] +
		 absCos[0][2] * obbB.halfLength[2];

	if (r > r0 + r1) return false;


	//
	// A �ٿ�� �ڽ��� Y ���� �������� �� ����
	//

	//r �� dist[1] �� ���� ���� �ȴ�.
	r = abs(dist[1]);

	//r0 
	r0 = obbA.halfLength[1];

	//r1 
	r1 = absCos[1][0] * obbB.halfLength[0] +
		absCos[1][1] * obbB.halfLength[1] +
		absCos[1][2] * obbB.halfLength[2];

	if (r > r0 + r1) return false;


	//
	// A �ٿ�� �ڽ��� Z ���� �������� �� ����
	//

	//r �� dist[2] �� ���� ���� �ȴ�.
	r = abs(dist[2]);

	//r0 
	r0 = obbA.halfLength[2];

	//r1 
	r1 = absCos[2][0] * obbB.halfLength[0] +
		absCos[2][1] * obbB.halfLength[1] +
		absCos[2][2] * obbB.halfLength[2];

	if (r > r0 + r1) return false;




	//
	// B �ٿ�� �ڽ��� X ���� �������� �� ����
	//
	r = abs(D3DXVec3Dot(&obbB.axis[0], &D));

	//r0 
	r0 = absCos[0][0] * obbA.halfLength[0] +
		 absCos[1][0] * obbA.halfLength[1] +
		 absCos[2][0] * obbA.halfLength[2];

	//r1 
	r1 = obbB.halfLength[0];

	if (r > r0 + r1) return false;

	//
	// B �ٿ�� �ڽ��� Y ���� �������� �� ����
	//
	r = abs(D3DXVec3Dot(&obbB.axis[1], &D));

	//r0 
	r0 = absCos[0][1] * obbA.halfLength[0] +
		absCos[1][1] * obbA.halfLength[1] +
		absCos[2][1] * obbA.halfLength[2];

	//r1 
	r1 = obbB.halfLength[1];
	if (r > r0 + r1) return false;


	//
	// B �ٿ�� �ڽ��� Z ���� �������� �� ����
	//
	r = abs(D3DXVec3Dot(&obbB.axis[2], &D));

	//r0 
	r0 = absCos[0][2] * obbA.halfLength[0] +
		absCos[1][2] * obbA.halfLength[1] +
		absCos[2][2] * obbA.halfLength[2];

	//r1 
	r1 = obbB.halfLength[2];
	if (r > r0 + r1) return false;


	//��������Դµ� ���е��� �ʾҴ�.. �׷��� existParallelPair true �̸�
	//������ �����ϴٴ� �����ε� �̷��� �и��� 6 ���� �˻��ϸ�ȴ�....
	if (existParallelPair) return true;


	//////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////


	//A�� X �� B �� X �࿡ ���� ������ �浹 üũ
	r = abs(dist[2] * cos[1][0] - dist[1] * cos[2][0]);
	r0 = obbA.halfLength[1] * absCos[2][0] + obbA.halfLength[2] * absCos[1][0];
	r1 = obbB.halfLength[1] * absCos[0][2] + obbB.halfLength[2] * absCos[0][1];
	if (r > r0 + r1)
		return false;


	//A�� X �� B �� Y �࿡ ���� ������ �浹 üũ
	r = abs(dist[2] * cos[1][1] - dist[1] * cos[2][1]);
	r0 = obbA.halfLength[1] * absCos[2][1] + obbA.halfLength[2] * absCos[1][1];
	r1 = obbB.halfLength[0] * absCos[0][2] + obbB.halfLength[2] * absCos[0][0];
	if (r > r0 + r1)
		return false;


	//A�� X �� B �� Z �࿡ ���� ������ �浹 üũ
	r = abs(dist[2] * cos[1][2] - dist[1] * cos[2][2]);
	r0 = obbA.halfLength[1] * absCos[2][2] + obbA.halfLength[2] * absCos[1][2];
	r1 = obbB.halfLength[0] * absCos[0][1] + obbB.halfLength[1] * absCos[0][0];
	if (r > r0 + r1)
		return false;

	/////////////////////////////////////////////////////////////////

	//A�� Y �� B �� X �࿡ ���� ������ �浹 üũ
	r = abs(dist[0] * cos[2][0] - dist[2] * cos[0][0]);
	r0 = obbA.halfLength[0] * absCos[2][0] + obbA.halfLength[2] * absCos[0][0];
	r1 = obbB.halfLength[1] * absCos[1][2] + obbB.halfLength[2] * absCos[1][1];
	if (r > r0 + r1)
		return false;

	//A�� Y �� B �� Y �࿡ ���� ������ �浹 üũ
	r = abs(dist[0] * cos[2][1] - dist[2] * cos[0][1]);
	r0 = obbA.halfLength[0] * absCos[2][1] + obbA.halfLength[2] * absCos[0][1];
	r1 = obbB.halfLength[0] * absCos[1][2] + obbB.halfLength[2] * absCos[1][0];
	if (r > r0 + r1)
		return false;

	//A�� Y �� B �� Z �࿡ ���� ������ �浹 üũ
	r = abs(dist[0] * cos[2][2] - dist[2] * cos[0][2]);
	r0 = obbA.halfLength[0] * absCos[2][2] + obbA.halfLength[2] * absCos[0][2];
	r1 = obbB.halfLength[0] * absCos[1][1] + obbB.halfLength[1] * absCos[1][0];
	if (r > r0 + r1)
		return false;

	/////////////////////////////////////////////////////////////////	 

	//A�� Z �� B �� X �࿡ ���� ������ �浹 üũ
	r = abs(dist[1] * cos[0][0] - dist[0] * cos[1][0]);
	r0 = obbA.halfLength[0] * absCos[1][0] + obbA.halfLength[1] * absCos[0][0];
	r1 = obbB.halfLength[1] * absCos[2][2] + obbB.halfLength[2] * absCos[2][1];
	if (r > r0 + r1)
		return false;

	//A�� Z �� B �� Y �࿡ ���� ������ �浹 üũ
	r = abs(dist[1] * cos[0][1] - dist[0] * cos[1][1]);
	r0 = obbA.halfLength[0] * absCos[1][1] + obbA.halfLength[1] * absCos[0][1];
	r1 = obbB.halfLength[0] * absCos[2][2] + obbB.halfLength[2] * absCos[2][0];
	if (r > r0 + r1)
		return false;

	//A�� Z �� B �� Z �࿡ ���� ������ �浹 üũ
	r = abs(dist[1] * cos[0][2] - dist[0] * cos[1][2]);
	r0 = obbA.halfLength[0] * absCos[1][2] + obbA.halfLength[1] * absCos[0][2];
	r1 = obbB.halfLength[0] * absCos[2][1] + obbB.halfLength[1] * absCos[2][0];
	if (r > r0 + r1)
		return false;

	//������� Ŭ���� �ߴٸ�  ����� �浹 ����... ( PS �浹 �ƴٴ� ����� )
	return true;

}



//2���� �ٿ�忡���� �⵿�����ϰ� ��ġ�� �ʰ� �Ѵ�.
//moveFactor �� 0 �ϼ��� ��Ĩ�� ���� B �� �����δ�.
//moveFactor �� 0.5 �̸� ��Ĩ�� ���� A �� B �� �Ȱ��������� �����δ�.
//moveFactor �� 1 �ϼ��� ��Ĩ�� ���� A �� �����δ�.
bool cPhysicsManager::IsBlockingSphere(
	cTransform* pTransA, cBoundSphere* pBoundA,
	cTransform* pTransB, cBoundSphere* pBoundB,
	float moveFactor)
{
	return false;
}

bool cPhysicsManager::IsBlockingBox(
	cTransform* pTransA, cBoundBox* pBoundA,
	cTransform* pTransB, cBoundBox* pBoundB,
	float moveFactor)
{
	//���� �浹���� �ʾ����� ���ʿ����
	if (IsOverlapBox(pTransA, pBoundA, pTransB, pBoundB) == false)
		return false;//���� �浹���� �ʾ����� ���ʿ����

	//���� �ε��ƽ��� �о���Ѵ�...
	
	//A�� Min Max
	D3DXVECTOR3 minA = pBoundA->localMinPos;
	D3DXVECTOR3 maxA = pBoundA->localMaxPos;

	//B�� Min Max
	D3DXVECTOR3 minB = pBoundB->localMinPos;
	D3DXVECTOR3 maxB = pBoundB->localMaxPos;

	//
	// A �� ������ �ΰ� B �� A �� ������� �������� �����Ͽ� ���
	//


	//B �� ���� �簢 8 ��

	//       5-------6 Max
	//      /|      /|
	//     1-------2 |
	//     | 4-----|-7
	//     |/      |/
	// Min 0-------3

	D3DXVECTOR3 pos[8];
	pos[0] = D3DXVECTOR3(minB.x, minB.y, minB.z);
	pos[1] = D3DXVECTOR3(minB.x, maxB.y, minB.z);
	pos[2] = D3DXVECTOR3(maxB.x, maxB.y, minB.z);
	pos[3] = D3DXVECTOR3(maxB.x, minB.y, minB.z);
	pos[4] = D3DXVECTOR3(minB.x, minB.y, maxB.z);
	pos[5] = D3DXVECTOR3(minB.x, maxB.y, maxB.z);
	pos[6] = D3DXVECTOR3(maxB.x, maxB.y, maxB.z);
	pos[7] = D3DXVECTOR3(maxB.x, minB.y, maxB.z);

	//A �� ���� �����
	D3DXMATRIXA16 matWorldAInv;
	D3DXMatrixInverse(&matWorldAInv, NULL, &pTransA->GetFinalMatrix());

	//B �� ���� ���
	D3DXMATRIXA16 matWorldB = pTransB->GetFinalMatrix();

	//B ���� ��ŭ ���� A �� ������ �ٽ� ������ ���
	D3DXMATRIXA16 mat = matWorldB * matWorldAInv;

	//B pos �� ����
	for (int i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&pos[i], &pos[i], &mat);


	//������ Pos ���� A ���� B �� ������� ��ġ�����̵ȴ�.

	//���������� ����� B pos �� ������ min max �� AABB ���·� �ٽ� ���� ����
	minB = pos[0];
	maxB = pos[0];
	for (int i = 1; i < 8; i++){
		if (pos[i].x < minB.x) minB.x = pos[i].x;
		if (pos[i].y < minB.y) minB.y = pos[i].y;
		if (pos[i].z < minB.z) minB.z = pos[i].z;
		if (pos[i].x > maxB.x) maxB.x = pos[i].x;
		if (pos[i].y > maxB.y) maxB.y = pos[i].y;
		if (pos[i].z > maxB.z) maxB.z = pos[i].z;
	}

	//�簢 �� ����ü
	struct fRect{
		float left;		//-X
		float right;	//+X
		float bottom;   //-Y
		float top;		//+Y
		float back;		//-Z
		float front;	//+Z
	};

	fRect rcA = { minA.x, maxA.x, minA.y, maxA.y, minA.z, maxA.z };
	fRect rcB = { minB.x, maxB.x, minB.y, maxB.y, minB.z, maxB.z };

	//��Ĩ���� �簢��
	fRect rcInter;
	rcInter.left = max(rcA.left, rcB.left);
	rcInter.right = min(rcA.right, rcB.right);
	rcInter.bottom = max(rcA.bottom, rcB.bottom);
	rcInter.top = min(rcA.top, rcB.top);
	rcInter.back = max(rcA.back, rcB.back);
	rcInter.front = min(rcA.front, rcB.front);

	//������ ��Ĩ���� ���ϰ� ���� ���� ���� ������ B �̵���Ų��.
	float interX = rcInter.right - rcInter.left;
	float interY = rcInter.top - rcInter.bottom;
	float interZ = rcInter.front - rcInter.back;
	float minInter = (interX < interY) ? ((interX < interZ) ? interX : interZ) : ((interY < interZ) ? interY : interZ);
	
	//�з�
	float moveLengthA = minInter;

	//�̴� ����
	D3DXVECTOR3 moveDirA(0, 0, 0);

	//X ���� ��Ĩ���� ���� �۴ٸ�..
	if (minInter == interX)
	{
		//A �� �������� �о�� �Ѵٸ�....
		if (FLOAT_EQUAL(rcInter.left, rcA.left))
			moveDirA = -pTransA->GetRight();
		//A �� ���������� �о�� �Ѵٸ�....
		else if (FLOAT_EQUAL(rcInter.right, rcA.right))
			moveDirA = pTransA->GetRight();

	}

	//Y ���� ��Ĩ���� ���� �۴ٸ�..
	else if (minInter == interY)
	{
		//A �� ������ �о�� �Ѵٸ�....
		if (FLOAT_EQUAL(rcInter.top, rcA.top))
			moveDirA = pTransA->GetUp();

		//A �� �Ʒ����� �о�� �Ѵٸ�....
		else if (FLOAT_EQUAL(rcInter.bottom, rcA.bottom))
			moveDirA = -pTransA->GetUp();
	}

	//Z ���� ��ħ���� ���� �۴ٸ�..
	else if (minInter == interZ)
	{
		//A �� �������� �о�� �Ѵٸ�....
		if (FLOAT_EQUAL(rcInter.front, rcA.front))
			moveDirA = pTransA->GetForward();

		//A �� ������ �о�� �Ѵٸ�....
		else if (FLOAT_EQUAL(rcInter.back, rcA.back))
			moveDirA = -pTransA->GetForward();
	}

	//��������´ٸ� �з��� �̴� ������ ���ȴ�.
	//moveDirA
	//moveLengthA
	//���� �ΰ��� A �� ������ �ΰ� B �� A ������ ����� ���̵ȴ�.

	//
	// �Ʒ��� ������ B �� ������ �ΰ� A �� B ������ ����� ���̵ȴ�.
	//
	D3DXVECTOR3 moveDirB(0, 0, 0);
	float moveLengthB = 0.0f;

	//A�� Min Max
	minA = pBoundA->localMinPos;
	maxA = pBoundA->localMaxPos;

	//B�� Min Max
	minB = pBoundB->localMinPos;
	maxB = pBoundB->localMaxPos;

	//A �� ���� �簢 8 ��

	//       5-------6 Max
	//      /|      /|
	//     1-------2 |
	//     | 4-----|-7
	//     |/      |/
	// Min 0-------3

	pos[0] = D3DXVECTOR3(minA.x, minA.y, minA.z);
	pos[1] = D3DXVECTOR3(minA.x, maxA.y, minA.z);
	pos[2] = D3DXVECTOR3(maxA.x, maxA.y, minA.z);
	pos[3] = D3DXVECTOR3(maxA.x, minA.y, minA.z);
	pos[4] = D3DXVECTOR3(minA.x, minA.y, maxA.z);
	pos[5] = D3DXVECTOR3(minA.x, maxA.y, maxA.z);
	pos[6] = D3DXVECTOR3(maxA.x, maxA.y, maxA.z);
	pos[7] = D3DXVECTOR3(maxA.x, minA.y, maxA.z);

	//B �� ���� �����
	D3DXMATRIXA16 matWorldBInv;
	D3DXMatrixInverse(&matWorldBInv, NULL, &pTransB->GetFinalMatrix());

	//A �� ���� ���
	D3DXMATRIXA16 matWorldA = pTransA->GetFinalMatrix();

	//A ���� ��ŭ ���� B �� ������ �ٽ� ������ ���
	mat = matWorldA * matWorldBInv;

	//A pos �� ����
	for (int i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&pos[i], &pos[i], &mat);

	//���������� ����� A pos �� ������ min max �� ���� ����
	minA = pos[0];
	maxA = pos[0];
	for (int i = 1; i < 8; i++){
		if (pos[i].x < minA.x) minA.x = pos[i].x;
		if (pos[i].y < minA.y) minA.y = pos[i].y;
		if (pos[i].z < minA.z) minA.z = pos[i].z;
		if (pos[i].x > maxA.x) maxA.x = pos[i].x;
		if (pos[i].y > maxA.y) maxA.y = pos[i].y;
		if (pos[i].z > maxA.z) maxA.z = pos[i].z;
	}
	//rcA = { minA.x, maxA.x, minA.y, maxA.y, minA.z, maxA.z };
	//rcB = { minB.x, maxB.x, minB.y, maxB.y, minB.z, maxB.z };

	rcA.left = minA.x;		rcA.right = maxA.x;			rcA.bottom = minA.y;			rcA.top = maxA.y;			rcA.back = minA.z;			rcA.front = maxA.z;
	rcB.left = minB.x;		rcB.right = maxB.x;			rcB.bottom = minB.y;			rcB.top = maxB.y;			rcB.back = minB.z;			rcB.front = maxB.z;


	//��Ĩ���� �簢��
	rcInter.left = max(rcA.left, rcB.left);
	rcInter.right = min(rcA.right, rcB.right);
	rcInter.bottom = max(rcA.bottom, rcB.bottom);
	rcInter.top = min(rcA.top, rcB.top);
	rcInter.back = max(rcA.back, rcB.back);
	rcInter.front = min(rcA.front, rcB.front);


	//������ ��Ĩ���� ���ϰ� ���� ���� ���� ������ B �̵���Ų��.
	interX = rcInter.right - rcInter.left;
	interY = rcInter.top - rcInter.bottom;
	interZ = rcInter.front - rcInter.back;
	minInter = (interX < interY) ? ((interX < interZ) ? interX : interZ) : ((interY < interZ) ? interY : interZ);

	moveLengthB = minInter;

	//X ���� ��Ĩ���� ���� �۴ٸ�..
	if (minInter == interX)
	{
		//B �� �������� �о�� �Ѵٸ�....
		if (FLOAT_EQUAL(rcInter.left, rcB.left))
			moveDirB = -pTransB->GetRight();

		//B �� ���������� �о�� �Ѵٸ�....
		else if (FLOAT_EQUAL(rcInter.right, rcB.right))
			moveDirB = pTransB->GetRight();
	}
	//Y ���� ��Ĩ���� ���� �۴ٸ�..
	else if (minInter == interY)
	{
		//B �� ������ �о�� �Ѵٸ�....
		if (FLOAT_EQUAL(rcInter.top, rcB.top))
			moveDirB = pTransB->GetUp();

		//B �� �Ʒ����� �о�� �Ѵٸ�....
		else if (FLOAT_EQUAL(rcInter.bottom, rcB.bottom))
			moveDirB = -pTransB->GetUp();

	}

	//Z ���� ��ħ���� ���� �۴ٸ�..
	else if (minInter == interZ)
	{
		//B �� �������� �о�� �Ѵٸ�....
		if (FLOAT_EQUAL(rcInter.front, rcB.front))
			moveDirB = pTransB->GetForward();

		//B �� ������ �о�� �Ѵٸ�....
		else if (FLOAT_EQUAL(rcInter.back, rcB.back))
			moveDirB = -pTransB->GetForward();
	}


	//��������´ٸ� �Ʒ��� 4 ���� ������ ���� ���̴�....
	//D3DXVECTOR3 moveDirA( 0, 0, 0 );
	//float moveLengthA = minInter;
	//D3DXVECTOR3 moveDirB( 0, 0, 0 );
	//float moveLengthB = 0.0f;

	//�з��� ����������...
	if (moveLengthB > moveLengthA)
	{
		//moveDirA �� moveLengthA �� ��ȿ

		//A �� B �� ������ ����
		//���� A �� ������ �ΰ� B �� �ǵ��ΰŴ�
		//���� A �� �������� 2 ���� B �� ������ �� 1 �̿��ٰ� �����Ͽ�����
		//A �� ����ķ� ���Ǿ� A �� �������� 1 B �� �������� 0.5 �� ���� �����̴�
		//�׷��Ƿ� �� ���� ���̴�
		//Ascale ���� ���Ͽ� ����ϴ� ���� �ùٸ���.
		D3DXVECTOR3 scaleA = pTransA->GetScale();


		




		pTransB->GetRoot()->MovePositionWorld(
			moveDirA.x * moveLengthA * (1.0 - moveFactor) * scaleA.x,
			moveDirA.y * moveLengthA * (1.0 - moveFactor) * scaleA.y,
			moveDirA.z * moveLengthA * (1.0 - moveFactor) * scaleA.z);


		pTransA->GetRoot()->MovePositionWorld(
			-moveDirA.x * moveLengthA * moveFactor * scaleA.x,
			-moveDirA.y * moveLengthA * moveFactor * scaleA.y,
			-moveDirA.z * moveLengthA * moveFactor * scaleA.z);
	}

	else
	{
		//moveDirB �� moveLengthB �� ��ȿ

		D3DXVECTOR3 scaleB = pTransB->GetScale();

		pTransA->GetRoot()->MovePositionWorld(
			moveDirB.x * moveLengthB * moveFactor  * scaleB.x,
			moveDirB.y * moveLengthB * moveFactor  * scaleB.y,
			moveDirB.z * moveLengthB * moveFactor  * scaleB.z);

		pTransB->GetRoot()->MovePositionWorld(
			-moveDirB.x * moveLengthB * (1.0 - moveFactor) *scaleB.x,
			-moveDirB.y * moveLengthB * (1.0 - moveFactor) *scaleB.y,
			-moveDirB.z * moveLengthB * (1.0 - moveFactor) *scaleB.z);

	}




	return true;
}





//�� 3���� ���� ����� �����.
void  cPhysicsManager::CreatePlane(
	LPD3DXPLANE pOutPlane, const D3DXVECTOR3* p0, const D3DXVECTOR3* p1, const D3DXVECTOR3* p2)
{
	D3DXVECTOR3 edge1 = *p1 - *p0;
	D3DXVECTOR3 edge2 = *p2 - *p0;
	D3DXVECTOR3 normal;
	D3DXVec3Cross(&normal, &edge1, &edge2);
	D3DXVec3Normalize(&normal, &normal); 

	//���� ����� �븻 ����
	pOutPlane->a = normal.x;
	pOutPlane->b = normal.y;
	pOutPlane->c = normal.z;


	/*
	//������ �� �ƹ��ų� �ϳ�....
	D3DXVECTOR3 point = *p0;		//������ �ƹ��ų� �ϳ� �־ ������.....
	D3DXVECTOR3 dirToZero = D3DXVECTOR3(0, 0, 0) - point;
	pOutPlane->d = D3DXVec3Dot(&normal, &dirToZero);
	*/


	pOutPlane->d = -D3DXVec3Dot(&normal, p0 );



}


//������ �������� �������� �ִܰŸ�
float cPhysicsManager::PlaneDot(
	const LPD3DXPLANE pPlane, const D3DXVECTOR3* point)
{
	//����� �븻
	D3DXVECTOR3 normal(pPlane->a, pPlane->b, pPlane->c);
	
	/*
	//��� ���� ������ ����???
	D3DXVECTOR3 p = normal * -pPlane->d;
	//������ ������ �������� ���� �Ű������� ���� �������� ���⺤��
	D3DXVECTOR3 dir = *point - p;

	//����
	float distance = D3DXVec3Dot(&normal, &dir);
	*/


	float distance = D3DXVec3Dot(&normal, point) + pPlane->d;



	return distance;
}


//�������� ����� �浹 ��ġ
bool cPhysicsManager::IntersectRayToPlane(D3DXVECTOR3* pOut, const LPRay pRay, const LPD3DXPLANE pPlane, bool bCheck2Side)
{
	//�븻 ����
	D3DXVECTOR3 normal(pPlane->a, pPlane->b, pPlane->c);

	//������ ���� cos �����.....
	float dot2 = D3DXVec3Dot(&normal, &pRay->direction);		//������ ����� ����� ���� ������ ������ cos ��

	//dot2 �� 0 �̶������ �������� ����� ����� ������ �����Ѵٴ� �����ε�...
	//�̴� �� ���� �������� �����ϴٴ� ���Ⱑ �ȴ�.
	if ( FLOAT_EQUAL(dot2, 0.0f) ) 
		return false;

	//�������� ���������� �������� �ִܰŸ�
	float d = D3DXVec3Dot(&normal, &pRay->origin) + pPlane->d;


	//��� üũ�� ���Ѵٸ�...
	if (bCheck2Side == false)
	{
		//������ �������� ��� ���ʿ� �־�ų� �븻����� ������ �̷�ٸ� ����
		if (d < 0.0f || dot2 > 0.0f){
			return false;
		}
	}


	//���üũ�� �Ѵٸ�...
	else
	{
		//���̰� ��� ���ʿ������� ����� ����� ���̹�����
		//�а��� �̷�ٸ� ����
		if (d < 0.0f && dot2 < 0.0f){
			return false;
		}

		//���̰� ��� �ۿ������� ����� ����� ���̹�����
		//������ �̷�ٸ� ����
		else if (d >= 0.0f && dot2 > 0.0f){
			return false;
		}

	}


	// t �� Ray �� ��鿡 �浹�Ͽ����� RayOrigin ���� ���� �浹���������� �Ÿ��̴�.
	//      |
	//     /|
	//    t |
	//   /  |
	//  /   |
	// *--d-|

	// acos( d / t )  = Theta
	// cos( acos( d / t ) ) = cos( Theta )
	// d / t = cos( Theta )
	// d / t = -dot2;
	// d = -dot2 * t;
	// t = d / -dot2;
	
	//�������������� �浹���������� �Ÿ�
	float t = d / -dot2;


	//�浹 ��ġ
	*pOut = pRay->origin + (pRay->direction * t);


	return true;

}