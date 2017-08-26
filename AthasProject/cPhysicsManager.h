#pragma once

#include "cSingletonBase.h"


//레이 구조체
typedef struct tagRay{
	D3DXVECTOR3 origin;			//반직선의 시작 위치
	D3DXVECTOR3 direction;		//반직선의 방향 ( 정규화 되어있어야 함 )
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


	//레이가  스피어에 충돌했는지 확인
	bool IsRayHitSphere(
		LPRay Ray,				//레이
		D3DXVECTOR3* worldCenter,	//월드 센터
		float radius,				//구반지름
		float checkDisance,		//체크 거리
		bool innnerOut,			//구안쪽에서 바깥쪽으로 체크할꺼니?
		D3DXVECTOR3* pHitPos,     //Hit 위치 ( NULL 이면 대입 안됨 )
		D3DXVECTOR3* pHitNormal	  //Hit 의 노말 ( NULL 이면 대입 안됨 )
		);


	//레이가  OBB 박스에 충돌했는지 확인
	bool IsRayHitOBBBox(
		LPRay Ray,
		D3DXVECTOR3* obbMin,		//변환전 박스의 Min 포스
		D3DXVECTOR3* obbMax,		//변환전 박스의 Max 포스
		D3DXMATRIXA16* obbWorld,	//OBB 월드 Matrix
		bool innnerOut,			//구안쪽에서 바깥쪽으로 체크할꺼니?
		D3DXVECTOR3* pHitPos,     //Hit 위치 ( NULL 이면 대입 안됨 )
		D3DXVECTOR3* pHitNormal	  //Hit 의 노말 ( NULL 이면 대입 안됨 )
		);

	//레이가  바운드 박스에 충돌했니?
	bool IsRayHitBoundBox(
		LPRay Ray,					//레이
		cBoundBox* pBoundBox,		//바운드 박스
		cTransform* pBoundTrans,	//바운드 박스의 Transform
		bool innnerOut,				//구안쪽에서 바깥쪽으로 체크할꺼니?
		D3DXVECTOR3* pHitPos,		//Hit 위치 ( NULL 이면 대입 안됨 )
		D3DXVECTOR3* pHitNormal		//Hit 의 노말 ( NULL 이면 대입 안됨 )
		);


	//레이가  메쉬에  에 충돌했니?
	bool IsRayHitXMeshStatic(
		LPRay Ray,					//레이
		cXMesh_Static* pMesh,		//체크할 메시
		cTransform* pMeshTrans,		//메시 Transform
		bool innnerOut,				//구안쪽에서 바깥쪽으로 체크할꺼니?
		D3DXVECTOR3* pHitPos,		//Hit 위치 ( NULL 이면 대입 안됨 )
		D3DXVECTOR3* pHitNormal		//Hit 의 노말 ( NULL 이면 대입 안됨 )
		);



	//2개의 바운드 대한 출동 정보를 얻는다.
	bool IsOverlapSphere(
		cTransform* pTransA, cBoundSphere* pBoundA, cTransform* pTransB, cBoundSphere* pBoundB);
	
	bool IsOverlapBox(
		cTransform* pTransA, cBoundBox* pBoundA, cTransform* pTransB, cBoundBox* pBoundB);


	//2개의 바운드에대한 출동검출하고 겹치지 않게 한다.
	//moveFactor 가 0 일수록 겹칩에 대해 B 가 움직인다.
	//moveFactor 가 0.5 이면 겹칩에 대해 A 와 B 가 똑같은량으로 움직인다.
	//moveFactor 가 1 일수록 겹칩에 대해 A 가 움직인다.
	bool IsBlockingSphere(
		cTransform* pTransA, cBoundSphere* pBoundA,
		cTransform* pTransB, cBoundSphere* pBoundB,
		float moveFactor = 0.0f);

	bool IsBlockingBox(
		cTransform* pTransA, cBoundBox* pBoundA,
		cTransform* pTransB, cBoundBox* pBoundB,
		float moveFactor = 0.0f);



	//점 3개로 무한 평면을 만든다.
	void CreatePlane(
		LPD3DXPLANE pOutPlane, const D3DXVECTOR3* p0, const D3DXVECTOR3* p1, const D3DXVECTOR3* p2);

	//임의의 한점에서 평면까지의 최단거리
	float PlaneDot(const LPD3DXPLANE pPlane, const D3DXVECTOR3* point);

	//반직선과 평면의 충돌 위치
	bool IntersectRayToPlane(D3DXVECTOR3* pOut, const LPRay pRay, const LPD3DXPLANE pPlane, bool bCheck2Side = false);



};

#define PHYSICS_MGR cPhysicsManager::GetInstance()