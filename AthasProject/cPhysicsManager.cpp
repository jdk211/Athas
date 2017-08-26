#include "stdafx.h"
#include "cPhysicsManager.h"
#include "cBoundBox.h"

cPhysicsManager::cPhysicsManager()
{
}


cPhysicsManager::~cPhysicsManager()
{
}

//레이가  스피어에 충돌했는지 확인
bool cPhysicsManager::IsRayHitSphere(
	LPRay Ray,				//레이
	D3DXVECTOR3* worldCenter,	//월드 센터
	float radius,				//구반지름
	float checkDisance,		//체크 거리
	bool innnerOut,			//구안쪽에서 바깥쪽으로 체크할꺼니?
	D3DXVECTOR3* pHitPos,     //Hit 위치 ( NULL 이면 대입 안됨 )
	D3DXVECTOR3* pHitNormal	  //Hit 의 노말 ( NULL 이면 대입 안됨 )
	)
{
	//레이의 오리진에서 부터 구의 센터까지의 방향벡터
	D3DXVECTOR3 dirToCenter = *worldCenter - Ray->origin;
	
	//dirToCenter 길이 제곱
	float dsq = D3DXVec3LengthSq(&dirToCenter);

	//반지름의 제곱
	float rSq = radius * radius;

	//투영길이
	float x = D3DXVec3Dot(&dirToCenter, &Ray->direction);

	//레이 시작점이 구안에있는지...
	bool inRayStartSphereIn = dsq < rSq;


	//x 가 음수라면... 구안쪽에서 충돌 체크를 안한다면 반드시 실패한다.
	if ((innnerOut == false && x < 0.0f) ||
		(innnerOut == false && inRayStartSphereIn )){	//레이의 위치가 구안에있다면 실패
		return false;
	}

	float ySq = dsq - x * x;

	//충돌아님....
	if (ySq > rSq)
		return false;

	//여기까지 왔다면.. 충돌.
	
	//충돌 위치를 얻고 싶다면....
	if (pHitPos != NULL)
	{
		float q = sqrt( rSq - ySq );



		//히트 거리는
		if (inRayStartSphereIn)
			q *= -1.0f;		//구안에있다면 q 를 반대 방향으로...

		float hitDist = x - q;

		//히트 위치는
		(*pHitPos) = Ray->origin + Ray->direction * hitDist;

		//히트 노말
		if (pHitNormal != NULL)
		{
			(*pHitNormal) = (*pHitPos) - (*worldCenter);

			//정규화
			D3DXVec3Normalize(pHitNormal, pHitNormal);
		}
	
	}


	return true;
}

//레이가  OBB 박스에 충돌했는지 확인
bool cPhysicsManager::IsRayHitOBBBox(
	LPRay Ray,
	D3DXVECTOR3* obbMin,		//변환전 박스의 Min 포스
	D3DXVECTOR3* obbMax,		//변환전 박스의 Max 포스
	D3DXMATRIXA16* obbWorld,	//OBB 월드 Matrix
	bool innnerOut,			//구안쪽에서 바깥쪽으로 체크할꺼니?
	D3DXVECTOR3* pHitPos,     //Hit 위치 ( NULL 이면 대입 안됨 )
	D3DXVECTOR3* pHitNormal	  //Hit 의 노말 ( NULL 이면 대입 안됨 )
	)
{

	//양면 체크를 위해 태어났다 왜냐? 양면체크를 하면 충돌지점이 2개 가 나오니깐.
	static vector<D3DXVECTOR3> hits;
	static vector<D3DXVECTOR3> hitNormals;
	hits.clear();
	hitNormals.clear();

	//레이를 역으로 땡긴다.
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

	//8 개의 점 구한다.
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

	//뒤면 평면
	this->CreatePlane(&plane, p + 0, p + 1, p + 2);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.x >= min->x && hitPos.x <= max->x &&
			hitPos.y >= min->y && hitPos.y <= max->y)
		{
			//안쪽에서도 체크시
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));
			}

			//바깥쪽에서면 체크시...
			else
			{
				//월드로 땡겨서 넣어줘
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//월드로 땡겨서 넣어줘
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(0, 0, -1), obbWorld);
				}

				return true;
			}

			
		}
	}

	//앞면 평면
	this->CreatePlane(&plane, p + 7, p + 6, p + 5);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.x >= min->x && hitPos.x <= max->x &&
			hitPos.y >= min->y && hitPos.y <= max->y)
		{
			//안쪽에서도 체크시
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));

			}

			//바깥쪽에서면 체크시...
			else
			{
				//월드로 땡겨서 넣어줘
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//월드로 땡겨서 넣어줘
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(plane.a, plane.b, plane.c), obbWorld);
				}

				return true;
			}
		}
	}


	//히트 지점이 2개 이상인경우
	if (hits.size() >= 2)
	{
		//가까운거 히트 위치임...
		
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

		//월드로 땡겨서 넣어줘
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hitPosResult, obbWorld);
		}


		//월드로 땡겨서 넣어줘
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNorResult, obbWorld);
		}
		return true;

	}


	//위면 평면
	this->CreatePlane(&plane, p + 1, p + 5, p + 6);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.x >= min->x && hitPos.x <= max->x &&
			hitPos.z >= min->z && hitPos.z <= max->z)
		{
			//안쪽에서도 체크시
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));
			}

			//바깥쪽에서면 체크시...
			else
			{
				//월드로 땡겨서 넣어줘
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//월드로 땡겨서 넣어줘
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(plane.a, plane.b, plane.c), obbWorld);
				}

				return true;
			}
		}
	}

	//히트 지점이 2개 이상인경우
	if (hits.size() >= 2)
	{
		//가까운거 히트 위치임...

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

		//월드로 땡겨서 넣어줘
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hitPosResult, obbWorld);
		}


		//월드로 땡겨서 넣어줘
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNorResult, obbWorld);
		}
		return true;

	}


	//아래면 평면
	this->CreatePlane(&plane, p + 4, p + 0, p + 3);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.x >= min->x && hitPos.x <= max->x &&
			hitPos.z >= min->z && hitPos.z <= max->z)
		{
			//안쪽에서도 체크시
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));

			}

			//바깥쪽에서면 체크시...
			else
			{
				//월드로 땡겨서 넣어줘
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//월드로 땡겨서 넣어줘
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(plane.a, plane.b, plane.c), obbWorld);
				}

				return true;
			}
		}
	}

	//히트 지점이 2개 이상인경우
	if (hits.size() >= 2)
	{
		//가까운거 히트 위치임...

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

		//월드로 땡겨서 넣어줘
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hitPosResult, obbWorld);
		}


		//월드로 땡겨서 넣어줘
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNorResult, obbWorld);
		}
		return true;

	}



	//우면 평면
	this->CreatePlane(&plane, p + 3, p + 2, p + 6);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.z >= min->z && hitPos.z <= max->z &&
			hitPos.y >= min->y && hitPos.y <= max->y)
		{
			//안쪽에서도 체크시
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));

			}

			//바깥쪽에서면 체크시...
			else
			{
				//월드로 땡겨서 넣어줘
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//월드로 땡겨서 넣어줘
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(plane.a, plane.b, plane.c), obbWorld);
				}

				return true;
			}
		}
	}

	//히트 지점이 2개 이상인경우
	if (hits.size() >= 2)
	{
		//가까운거 히트 위치임...

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

		//월드로 땡겨서 넣어줘
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hitPosResult, obbWorld);
		}


		//월드로 땡겨서 넣어줘
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNorResult, obbWorld);
		}
		return true;

	}


	//좌면 평면
	this->CreatePlane(&plane, p + 5, p + 1, p + 0);
	if (this->IntersectRayToPlane(&hitPos, &localRay, &plane, innnerOut))
	{
		if (hitPos.z >= min->z && hitPos.z <= max->z &&
			hitPos.y >= min->y && hitPos.y <= max->y)
		{
			//안쪽에서도 체크시
			if (innnerOut)
			{
				hits.push_back(hitPos);
				hitNormals.push_back(D3DXVECTOR3(plane.a, plane.b, plane.c));
			}

			//바깥쪽에서면 체크시...
			else
			{
				//월드로 땡겨서 넣어줘
				if (pHitPos != NULL){
					D3DXVec3TransformCoord(pHitPos, &hitPos, obbWorld);
				}


				//월드로 땡겨서 넣어줘
				if (pHitNormal != NULL){
					D3DXVec3TransformNormal(pHitNormal, &D3DXVECTOR3(plane.a, plane.b, plane.c), obbWorld);
				}

				return true;
			}
		}
	}


	//히트 지점이 2개 이상인경우
	if (hits.size() >= 2)
	{
		//가까운거 히트 위치임...

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

		//월드로 땡겨서 넣어줘
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hitPosResult, obbWorld);
		}


		//월드로 땡겨서 넣어줘
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNorResult, obbWorld);
		}
		return true;

	}


	else if (hits.size() == 1)
	{
		//월드로 땡겨서 넣어줘
		if (pHitPos != NULL){
			D3DXVec3TransformCoord(pHitPos, &hits[0], obbWorld);
		}


		//월드로 땡겨서 넣어줘
		if (pHitNormal != NULL){
			D3DXVec3TransformNormal(pHitNormal, &hitNormals[0], obbWorld);
		}
		return true;
	}



	return false;

	
}



//레이가  바운드 박스에 충돌했니?
bool cPhysicsManager::IsRayHitBoundBox(
	LPRay Ray,					//레이
	cBoundBox* pBoundBox,		//바운드 박스
	cTransform* pBoundTrans,	//바운드 박스의 Transform
	bool innnerOut,				//구안쪽에서 바깥쪽으로 체크할꺼니?
	D3DXVECTOR3* pHitPos,		//Hit 위치 ( NULL 이면 대입 안됨 )
	D3DXVECTOR3* pHitNormal		//Hit 의 노말 ( NULL 이면 대입 안됨 )
	)
{
	//월드 센터랑 반지름을 얻는다.
	D3DXVECTOR3 center;
	float radius;
	pBoundBox->GetWorldCenterRadius(pBoundTrans, &center, &radius);

	//구충돌부터
	if (IsRayHitSphere(Ray, &center, radius, 10000.0f, true, NULL, NULL) == false)
		return false;

	//사각까지 충돌체크 필요
	return IsRayHitOBBBox(
		Ray,
		&pBoundBox->localMinPos,
		&pBoundBox->localMaxPos,
		&pBoundTrans->GetFinalMatrix(),
		innnerOut,
		pHitPos,
		pHitNormal);



}

//레이가  메쉬에  에 충돌했니?
bool cPhysicsManager::IsRayHitXMeshStatic(
	LPRay Ray,					//레이
	cXMesh_Static* pMesh,		//체크할 메시
	cTransform* pMeshTrans,		//메시 Transform
	bool innnerOut,				//구안쪽에서 바깥쪽으로 체크할꺼니?
	D3DXVECTOR3* pHitPos,		//Hit 위치 ( NULL 이면 대입 안됨 )
	D3DXVECTOR3* pHitNormal		//Hit 의 노말 ( NULL 이면 대입 안됨 )
	)
{
	//메시의 Default 바운드 박스와 충돌체크 하고
	if (IsRayHitBoundBox(
		Ray,
		pMesh->GetDefaultBound(),
		pMeshTrans,
		true,
		NULL,
		NULL) == false)
		return false;


	//레이를 Mesh 의 로컬로 땡긴후 사용
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

	//메쉬의 서브셋대로 모두 체크
	for (DWORD i = 0; i < pMesh->GetMaterialNum(); i++)
	{

		//레이와 메쉬의 서브셋간의 충돌체크를 해준다.
		D3DXIntersectSubset(
			pMesh->GetMesh(),
			i,				//체클할 메시의 서브셋 인덱스
			&ori,			//레이 오리진 ( Mesh 로컬로 땡긴거 )
			&dir,			//레이 방향 ( Mesh 로컬로 땡긴거 )
			&bHit,			//충돌 성공여부
			&FaceIndex,		//충돌된 면의 인덱스
			NULL,			//충돌 삼각형의 U
			NULL,			//충돌 삼각형의 V
			&hitDist,		//충돌 거리
			NULL,
			&hitCount);		//다단 히트시 충돌 카운드...

		
		
			if (bHit){

				if (pHitPos != NULL){

					*pHitPos = ori + dir * hitDist;
					//월드로...
					D3DXVec3TransformCoord(pHitPos, pHitPos, &pMeshTrans->GetFinalMatrix());
				}

				if (pHitNormal != NULL){

					*pHitNormal = pMesh->GetFaceNormal(FaceIndex);
					//월드로...
					D3DXVec3TransformNormal(pHitNormal, pHitNormal, &pMeshTrans->GetFinalMatrix());
				}


				return true;
			}

	}

	return false;

}



//2개의 바운드 대한 출동 정보를 얻는다.
bool  cPhysicsManager::IsOverlapSphere(
	cTransform* pTransA, cBoundSphere* pBoundA, cTransform* pTransB, cBoundSphere* pBoundB)
{
	//알아서 만들어....
	return false;
}

bool cPhysicsManager::IsOverlapBox(
	cTransform* pTransA, cBoundBox* pBoundA, cTransform* pTransB, cBoundBox* pBoundB)
{
	//에시당초 외접구끼리 충돌안되었다면 Box끼리는 죽었다 깨어나도 충돌 안된다...
	//if (IsOverlapSphere(pTransA, pBoundA, pTransB, pBoundB) == false)
	//	return false;

	//
	// OBB 충돌 체크 ( 여기서 부터 개 토 나옴 ..... )
	//


	//배열순서
	//X = 0, Y = 1, Z = 2;
	//OBB 충돌에 필요한 충돌 구조체 (월드 단위의 정보)
	struct OBB{
		D3DXVECTOR3 center;		//중심점
		D3DXVECTOR3 axis[3];	//축방향	
		float halfLength[3];	//각축에 대한 절반 크기
	};

	//
	// A 바운딩에 대한 충돌 구조체
	//
	OBB obbA;

	//A 바운드의 센터
	D3DXVECTOR3 centerA;
	float radiusA;
	pBoundA->GetWorldCenterRadius(pTransA, &centerA, &radiusA);

	//월드 센터
	obbA.center = centerA;

	//각 축 방향
	obbA.axis[0] = pTransA->GetRight();
	obbA.axis[1] = pTransA->GetUp();
	obbA.axis[2] = pTransA->GetForward();

	//각축의 길이
	D3DXVECTOR3 halfSizeA;
	pBoundA->GetWorldHalfSize(pTransA, &halfSizeA);
	obbA.halfLength[0] = halfSizeA.x;
	obbA.halfLength[1] = halfSizeA.y;
	obbA.halfLength[2] = halfSizeA.z;

	//
	// B 바운딩에 대한 충돌 구조체
	//
	OBB obbB;

	//A 바운드의 센터
	D3DXVECTOR3 centerB;
	float radiusB;
	pBoundB->GetWorldCenterRadius(pTransB, &centerB, &radiusB);

	//월드 센터
	obbB.center = centerB;

	//각 축 방향
	obbB.axis[0] = pTransB->GetRight();
	obbB.axis[1] = pTransB->GetUp();
	obbB.axis[2] = pTransB->GetForward();

	//각축의 길이
	D3DXVECTOR3 halfSizeB;
	pBoundB->GetWorldHalfSize(pTransB, &halfSizeB);
	obbB.halfLength[0] = halfSizeB.x;
	obbB.halfLength[1] = halfSizeB.y;
	obbB.halfLength[2] = halfSizeB.z;


	//
	// OBB 충돌
	//
	float cos[3][3];				//각축 차에대한 대한 코사인 값  [A축][B축]  ( [0][1] => 이인덱스는 A의 X 축과 B의 Y 축의 각차에 대한 cos 값이다, A 의 X 와 B 의 Y 에대한 내적값 )
	float absCos[3][3];				//각축 차에대한 대한 코사인 절대값  [A축][B축]  ( [0][1] => 이인덱스는 A의 X 축과 B의 Y 축의 각차에 대한 cos 절대 값이다 )
	float dist[3];					//A 바운드 각축으로 A 중심점에서 B 의 중심점벡터를 투영한 투영길이	

	const float cutOff = 0.9999f;		//수직 판단을 하기위한 컷오프 값 ( 어느 한 축의 cos 결과 값이 이보다 크다면 두 충돌체는 한축이 평행하다는 예기 )
	bool existParallelPair = false;		//한 축이라도 평행하나?


	//A 에서 B 의 방향벡터
	D3DXVECTOR3 D = obbB.center - obbA.center;

	float r, r0, r1;			//r0 과 r1 의 합이 r 보다 작으면 충돌 실패 



	for (int a = 0; a < 3; a++)
	{
		for (int b = 0; b < 3; b++)
		{
			cos[a][b] = D3DXVec3Dot(&obbA.axis[a], &obbB.axis[b]);
			absCos[a][b] = abs(cos[a][b]);

			//한축이 서로 교차 되는 지확인
			if (absCos[a][b] > cutOff) existParallelPair = true;

		}

		//센터끼리의 방향벡터를 A 바운드 Axis 의 투영한 거리
		dist[a] = D3DXVec3Dot(&obbA.axis[a], &D);
	}


	//
	// A 바운드 박스에 X 축을 기준으로 한 연산
	//

	//r 은 dist[0] 의 절대 값이 된다.
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
	// A 바운드 박스에 Y 축을 기준으로 한 연산
	//

	//r 은 dist[1] 의 절대 값이 된다.
	r = abs(dist[1]);

	//r0 
	r0 = obbA.halfLength[1];

	//r1 
	r1 = absCos[1][0] * obbB.halfLength[0] +
		absCos[1][1] * obbB.halfLength[1] +
		absCos[1][2] * obbB.halfLength[2];

	if (r > r0 + r1) return false;


	//
	// A 바운드 박스에 Z 축을 기준으로 한 연산
	//

	//r 은 dist[2] 의 절대 값이 된다.
	r = abs(dist[2]);

	//r0 
	r0 = obbA.halfLength[2];

	//r1 
	r1 = absCos[2][0] * obbB.halfLength[0] +
		absCos[2][1] * obbB.halfLength[1] +
		absCos[2][2] * obbB.halfLength[2];

	if (r > r0 + r1) return false;




	//
	// B 바운드 박스에 X 축을 기준으로 한 연산
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
	// B 바운드 박스에 Y 축을 기준으로 한 연산
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
	// B 바운드 박스에 Z 축을 기준으로 한 연산
	//
	r = abs(D3DXVec3Dot(&obbB.axis[2], &D));

	//r0 
	r0 = absCos[0][2] * obbA.halfLength[0] +
		absCos[1][2] * obbA.halfLength[1] +
		absCos[2][2] * obbA.halfLength[2];

	//r1 
	r1 = obbB.halfLength[2];
	if (r > r0 + r1) return false;


	//여기까지왔는데 실패되지 않았다.. 그러면 existParallelPair true 이면
	//한축이 평행하다는 예기인데 이러면 분리축 6 번만 검색하면된다....
	if (existParallelPair) return true;


	//////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////


	//A의 X 축 B 의 X 축에 대한 외적의 충돌 체크
	r = abs(dist[2] * cos[1][0] - dist[1] * cos[2][0]);
	r0 = obbA.halfLength[1] * absCos[2][0] + obbA.halfLength[2] * absCos[1][0];
	r1 = obbB.halfLength[1] * absCos[0][2] + obbB.halfLength[2] * absCos[0][1];
	if (r > r0 + r1)
		return false;


	//A의 X 축 B 의 Y 축에 대한 외적의 충돌 체크
	r = abs(dist[2] * cos[1][1] - dist[1] * cos[2][1]);
	r0 = obbA.halfLength[1] * absCos[2][1] + obbA.halfLength[2] * absCos[1][1];
	r1 = obbB.halfLength[0] * absCos[0][2] + obbB.halfLength[2] * absCos[0][0];
	if (r > r0 + r1)
		return false;


	//A의 X 축 B 의 Z 축에 대한 외적의 충돌 체크
	r = abs(dist[2] * cos[1][2] - dist[1] * cos[2][2]);
	r0 = obbA.halfLength[1] * absCos[2][2] + obbA.halfLength[2] * absCos[1][2];
	r1 = obbB.halfLength[0] * absCos[0][1] + obbB.halfLength[1] * absCos[0][0];
	if (r > r0 + r1)
		return false;

	/////////////////////////////////////////////////////////////////

	//A의 Y 축 B 의 X 축에 대한 외적의 충돌 체크
	r = abs(dist[0] * cos[2][0] - dist[2] * cos[0][0]);
	r0 = obbA.halfLength[0] * absCos[2][0] + obbA.halfLength[2] * absCos[0][0];
	r1 = obbB.halfLength[1] * absCos[1][2] + obbB.halfLength[2] * absCos[1][1];
	if (r > r0 + r1)
		return false;

	//A의 Y 축 B 의 Y 축에 대한 외적의 충돌 체크
	r = abs(dist[0] * cos[2][1] - dist[2] * cos[0][1]);
	r0 = obbA.halfLength[0] * absCos[2][1] + obbA.halfLength[2] * absCos[0][1];
	r1 = obbB.halfLength[0] * absCos[1][2] + obbB.halfLength[2] * absCos[1][0];
	if (r > r0 + r1)
		return false;

	//A의 Y 축 B 의 Z 축에 대한 외적의 충돌 체크
	r = abs(dist[0] * cos[2][2] - dist[2] * cos[0][2]);
	r0 = obbA.halfLength[0] * absCos[2][2] + obbA.halfLength[2] * absCos[0][2];
	r1 = obbB.halfLength[0] * absCos[1][1] + obbB.halfLength[1] * absCos[1][0];
	if (r > r0 + r1)
		return false;

	/////////////////////////////////////////////////////////////////	 

	//A의 Z 축 B 의 X 축에 대한 외적의 충돌 체크
	r = abs(dist[1] * cos[0][0] - dist[0] * cos[1][0]);
	r0 = obbA.halfLength[0] * absCos[1][0] + obbA.halfLength[1] * absCos[0][0];
	r1 = obbB.halfLength[1] * absCos[2][2] + obbB.halfLength[2] * absCos[2][1];
	if (r > r0 + r1)
		return false;

	//A의 Z 축 B 의 Y 축에 대한 외적의 충돌 체크
	r = abs(dist[1] * cos[0][1] - dist[0] * cos[1][1]);
	r0 = obbA.halfLength[0] * absCos[1][1] + obbA.halfLength[1] * absCos[0][1];
	r1 = obbB.halfLength[0] * absCos[2][2] + obbB.halfLength[2] * absCos[2][0];
	if (r > r0 + r1)
		return false;

	//A의 Z 축 B 의 Z 축에 대한 외적의 충돌 체크
	r = abs(dist[1] * cos[0][2] - dist[0] * cos[1][2]);
	r0 = obbA.halfLength[0] * absCos[1][2] + obbA.halfLength[1] * absCos[0][2];
	r1 = obbB.halfLength[0] * absCos[2][1] + obbB.halfLength[1] * absCos[2][0];
	if (r > r0 + r1)
		return false;

	//여기까지 클리어 했다면  당신은 충돌 용자... ( PS 충돌 됐다는 예기다 )
	return true;

}



//2개의 바운드에대한 출동검출하고 겹치지 않게 한다.
//moveFactor 가 0 일수록 겹칩에 대해 B 가 움직인다.
//moveFactor 가 0.5 이면 겹칩에 대해 A 와 B 가 똑같은량으로 움직인다.
//moveFactor 가 1 일수록 겹칩에 대해 A 가 움직인다.
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
	//둘이 충돌되지 않았으면 할필요없다
	if (IsOverlapBox(pTransA, pBoundA, pTransB, pBoundB) == false)
		return false;//둘이 충돌되지 않았으면 할필요없다

	//둘이 부디쳤스니 밀어내야한다...
	
	//A의 Min Max
	D3DXVECTOR3 minA = pBoundA->localMinPos;
	D3DXVECTOR3 maxA = pBoundA->localMaxPos;

	//B의 Min Max
	D3DXVECTOR3 minB = pBoundB->localMinPos;
	D3DXVECTOR3 maxB = pBoundB->localMaxPos;

	//
	// A 를 가만히 두고 B 을 A 의 상대적인 기준으로 변경하여 계산
	//


	//B 의 로컬 사각 8 점

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

	//A 의 월드 역행렬
	D3DXMATRIXA16 matWorldAInv;
	D3DXMatrixInverse(&matWorldAInv, NULL, &pTransA->GetFinalMatrix());

	//B 의 월드 행렬
	D3DXMATRIXA16 matWorldB = pTransB->GetFinalMatrix();

	//B 월드 만큼 가고 A 의 역으로 다시 움직인 행렬
	D3DXMATRIXA16 mat = matWorldB * matWorldAInv;

	//B pos 에 적용
	for (int i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&pos[i], &pos[i], &mat);


	//이이후 Pos 들은 A 대한 B 의 상대적인 위치값들이된다.

	//최종적으로 적용된 B pos 를 가지고 min max 를 AABB 형태로 다시 갱신 하자
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

	//사각 형 구조체
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

	//겹칩량의 사각형
	fRect rcInter;
	rcInter.left = max(rcA.left, rcB.left);
	rcInter.right = min(rcA.right, rcB.right);
	rcInter.bottom = max(rcA.bottom, rcB.bottom);
	rcInter.top = min(rcA.top, rcB.top);
	rcInter.back = max(rcA.back, rcB.back);
	rcInter.front = min(rcA.front, rcB.front);

	//각축의 겹칩량을 구하고 그중 가장 작은 축으로 B 이동시킨다.
	float interX = rcInter.right - rcInter.left;
	float interY = rcInter.top - rcInter.bottom;
	float interZ = rcInter.front - rcInter.back;
	float minInter = (interX < interY) ? ((interX < interZ) ? interX : interZ) : ((interY < interZ) ? interY : interZ);
	
	//밀량
	float moveLengthA = minInter;

	//미는 방향
	D3DXVECTOR3 moveDirA(0, 0, 0);

	//X 축의 겹칩량이 제일 작다면..
	if (minInter == interX)
	{
		//A 의 왼쪽으로 밀어야 한다면....
		if (FLOAT_EQUAL(rcInter.left, rcA.left))
			moveDirA = -pTransA->GetRight();
		//A 의 오른쪽으로 밀어야 한다면....
		else if (FLOAT_EQUAL(rcInter.right, rcA.right))
			moveDirA = pTransA->GetRight();

	}

	//Y 축의 겹칩량이 제일 작다면..
	else if (minInter == interY)
	{
		//A 의 위으로 밀어야 한다면....
		if (FLOAT_EQUAL(rcInter.top, rcA.top))
			moveDirA = pTransA->GetUp();

		//A 의 아래으로 밀어야 한다면....
		else if (FLOAT_EQUAL(rcInter.bottom, rcA.bottom))
			moveDirA = -pTransA->GetUp();
	}

	//Z 축의 겹침량이 제일 작다면..
	else if (minInter == interZ)
	{
		//A 의 정면으로 밀어야 한다면....
		if (FLOAT_EQUAL(rcInter.front, rcA.front))
			moveDirA = pTransA->GetForward();

		//A 의 뒤으로 밀어야 한다면....
		else if (FLOAT_EQUAL(rcInter.back, rcA.back))
			moveDirA = -pTransA->GetForward();
	}

	//여기까지온다면 밀량과 미는 방향이 계산된다.
	//moveDirA
	//moveLengthA
	//위의 두값은 A 를 가만히 두고 B 를 A 역으로 계산한 값이된다.

	//
	// 아래의 연산은 B 를 가만히 두고 A 를 B 역으로 계산한 값이된다.
	//
	D3DXVECTOR3 moveDirB(0, 0, 0);
	float moveLengthB = 0.0f;

	//A의 Min Max
	minA = pBoundA->localMinPos;
	maxA = pBoundA->localMaxPos;

	//B의 Min Max
	minB = pBoundB->localMinPos;
	maxB = pBoundB->localMaxPos;

	//A 의 로컬 사각 8 점

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

	//B 의 월드 역행렬
	D3DXMATRIXA16 matWorldBInv;
	D3DXMatrixInverse(&matWorldBInv, NULL, &pTransB->GetFinalMatrix());

	//A 의 월드 행렬
	D3DXMATRIXA16 matWorldA = pTransA->GetFinalMatrix();

	//A 월드 만큼 가고 B 의 역으로 다시 움직인 행렬
	mat = matWorldA * matWorldBInv;

	//A pos 에 적용
	for (int i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&pos[i], &pos[i], &mat);

	//최종적으로 적용된 A pos 를 가지고 min max 를 갱신 하자
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


	//겹칩량의 사각형
	rcInter.left = max(rcA.left, rcB.left);
	rcInter.right = min(rcA.right, rcB.right);
	rcInter.bottom = max(rcA.bottom, rcB.bottom);
	rcInter.top = min(rcA.top, rcB.top);
	rcInter.back = max(rcA.back, rcB.back);
	rcInter.front = min(rcA.front, rcB.front);


	//각축의 겹칩량을 구하고 그중 가장 작은 축으로 B 이동시킨다.
	interX = rcInter.right - rcInter.left;
	interY = rcInter.top - rcInter.bottom;
	interZ = rcInter.front - rcInter.back;
	minInter = (interX < interY) ? ((interX < interZ) ? interX : interZ) : ((interY < interZ) ? interY : interZ);

	moveLengthB = minInter;

	//X 축의 겹칩량이 제일 작다면..
	if (minInter == interX)
	{
		//B 의 왼쪽으로 밀어야 한다면....
		if (FLOAT_EQUAL(rcInter.left, rcB.left))
			moveDirB = -pTransB->GetRight();

		//B 의 오른쪽으로 밀어야 한다면....
		else if (FLOAT_EQUAL(rcInter.right, rcB.right))
			moveDirB = pTransB->GetRight();
	}
	//Y 축의 겹칩량이 제일 작다면..
	else if (minInter == interY)
	{
		//B 의 위으로 밀어야 한다면....
		if (FLOAT_EQUAL(rcInter.top, rcB.top))
			moveDirB = pTransB->GetUp();

		//B 의 아래으로 밀어야 한다면....
		else if (FLOAT_EQUAL(rcInter.bottom, rcB.bottom))
			moveDirB = -pTransB->GetUp();

	}

	//Z 축의 겹침량이 제일 작다면..
	else if (minInter == interZ)
	{
		//B 의 정면으로 밀어야 한다면....
		if (FLOAT_EQUAL(rcInter.front, rcB.front))
			moveDirB = pTransB->GetForward();

		//B 의 뒤으로 밀어야 한다면....
		else if (FLOAT_EQUAL(rcInter.back, rcB.back))
			moveDirB = -pTransB->GetForward();
	}


	//여기까지온다면 아래의 4 개의 변수가 계산된 것이다....
	//D3DXVECTOR3 moveDirA( 0, 0, 0 );
	//float moveLengthA = minInter;
	//D3DXVECTOR3 moveDirB( 0, 0, 0 );
	//float moveLengthB = 0.0f;

	//밀량이 작은쪽으로...
	if (moveLengthB > moveLengthA)
	{
		//moveDirA 와 moveLengthA 가 유효

		//A 와 B 의 스케일 적용
		//여긴 A 를 가만히 두고 B 를 건들인거다
		//따라서 A 의 스케일이 2 였고 B 의 스케일 이 1 이였다고 가정하였을때
		//A 의 역행렬로 계산되어 A 의 스케일이 1 B 의 스케일이 0.5 로 계산된 길이이다
		//그러므로 각 축의 길이는
		//Ascale 값을 곱하여 계산하는 것이 올바르다.
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
		//moveDirB 와 moveLengthB 가 유효

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





//점 3개로 무한 평면을 만든다.
void  cPhysicsManager::CreatePlane(
	LPD3DXPLANE pOutPlane, const D3DXVECTOR3* p0, const D3DXVECTOR3* p1, const D3DXVECTOR3* p2)
{
	D3DXVECTOR3 edge1 = *p1 - *p0;
	D3DXVECTOR3 edge2 = *p2 - *p0;
	D3DXVECTOR3 normal;
	D3DXVec3Cross(&normal, &edge1, &edge2);
	D3DXVec3Normalize(&normal, &normal); 

	//무한 평면의 노말 벡터
	pOutPlane->a = normal.x;
	pOutPlane->b = normal.y;
	pOutPlane->c = normal.z;


	/*
	//평면상의 점 아무거나 하나....
	D3DXVECTOR3 point = *p0;		//세점중 아무거나 하나 넣어도 무방함.....
	D3DXVECTOR3 dirToZero = D3DXVECTOR3(0, 0, 0) - point;
	pOutPlane->d = D3DXVec3Dot(&normal, &dirToZero);
	*/


	pOutPlane->d = -D3DXVec3Dot(&normal, p0 );



}


//임의의 한점에서 평면까지의 최단거리
float cPhysicsManager::PlaneDot(
	const LPD3DXPLANE pPlane, const D3DXVECTOR3* point)
{
	//평면의 노말
	D3DXVECTOR3 normal(pPlane->a, pPlane->b, pPlane->c);
	
	/*
	//평면 상의 임의의 한점???
	D3DXVECTOR3 p = normal * -pPlane->d;
	//평면상의 임의의 한점으로 부터 매개변수로 받은 점까지의 방향벡터
	D3DXVECTOR3 dir = *point - p;

	//내적
	float distance = D3DXVec3Dot(&normal, &dir);
	*/


	float distance = D3DXVec3Dot(&normal, point) + pPlane->d;



	return distance;
}


//반직선과 평면의 충돌 위치
bool cPhysicsManager::IntersectRayToPlane(D3DXVECTOR3* pOut, const LPRay pRay, const LPD3DXPLANE pPlane, bool bCheck2Side)
{
	//노말 벡터
	D3DXVECTOR3 normal(pPlane->a, pPlane->b, pPlane->c);

	//각도에 의한 cos 결과값.....
	float dot2 = D3DXVec3Dot(&normal, &pRay->direction);		//광선의 방향과 평면의 법선 방향의 각차의 cos 값

	//dot2 가 0 이란예기는 반직선의 방향과 평면의 방향이 직교한다는 예기인데...
	//이는 즉 평면과 반직선은 평행하다는 예기가 된다.
	if ( FLOAT_EQUAL(dot2, 0.0f) ) 
		return false;

	//반직선의 시작점에서 평면까지으 최단거리
	float d = D3DXVec3Dot(&normal, &pRay->origin) + pPlane->d;


	//양면 체크를 안한다면...
	if (bCheck2Side == false)
	{
		//레이의 시작점이 평면 안쪽에 있어거나 노말방향과 예각을 이룬다면 실패
		if (d < 0.0f || dot2 > 0.0f){
			return false;
		}
	}


	//양면체크를 한다면...
	else
	{
		//레이가 평면 안쪽에있을때 평면의 방향과 레이방향이
		//둔각을 이룬다면 실패
		if (d < 0.0f && dot2 < 0.0f){
			return false;
		}

		//레이가 평면 밖에있을때 평면의 방향과 레이방향이
		//예각을 이룬다면 실패
		else if (d >= 0.0f && dot2 > 0.0f){
			return false;
		}

	}


	// t 는 Ray 가 평면에 충돌하였을때 RayOrigin 에서 부터 충돌지점까지의 거리이다.
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
	
	//반직선방향으로 충돌지점까지의 거리
	float t = d / -dot2;


	//충돌 위치
	*pOut = pRay->origin + (pRay->direction * t);


	return true;

}