#include "stdafx.h"
#include "cFrustum.h"


cFrustum::cFrustum()
{
	//   5-------6
	//  /|      /|
	// 1-------2 |
	// | 4-----|-7
	// |/      |/
	// 0-------3 


	rhwPos[0] = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	rhwPos[1] = D3DXVECTOR3(-1.0f,  1.0f, 0.0f);
	rhwPos[2] = D3DXVECTOR3( 1.0f,  1.0f, 0.0f);
	rhwPos[3] = D3DXVECTOR3( 1.0f, -1.0f, 0.0f);
	rhwPos[4] = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	rhwPos[5] = D3DXVECTOR3(-1.0f,  1.0f, 1.0f);
	rhwPos[6] = D3DXVECTOR3( 1.0f,  1.0f, 1.0f);
	rhwPos[7] = D3DXVECTOR3( 1.0f, -1.0f, 1.0f);
}


cFrustum::~cFrustum()
{
}



//프러스텀을 업데이트 한다.
void cFrustum::UpdateFrustum(D3DXMATRIXA16* matViewProjection)
{
	//뷰프로젝션 행렬의 역
	D3DXMATRIXA16 matInv;
	D3DXMatrixInverse(&matInv, NULL, matViewProjection);

	for (int i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&frustumPos[i], &rhwPos[i], &matInv);


	//   5-------6
	//  /|      /|
	// 1-------2 |
	// | 4-----|-7
	// |/      |/
	// 0-------3 


	//Back
	PHYSICS_MGR->CreatePlane(&this->frustumPlane[0], &frustumPos[0], &frustumPos[1], &frustumPos[2]);
	//Up
	PHYSICS_MGR->CreatePlane(&this->frustumPlane[1], &frustumPos[1], &frustumPos[5], &frustumPos[6]);
	//Bottom
	PHYSICS_MGR->CreatePlane(&this->frustumPlane[2], &frustumPos[4], &frustumPos[0], &frustumPos[3]);
	//Left
	PHYSICS_MGR->CreatePlane(&this->frustumPlane[3], &frustumPos[4], &frustumPos[5], &frustumPos[1]);
	//Right
	PHYSICS_MGR->CreatePlane(&this->frustumPlane[4], &frustumPos[3], &frustumPos[2], &frustumPos[6]);
	//Front
	PHYSICS_MGR->CreatePlane(&this->frustumPlane[5], &frustumPos[7], &frustumPos[6], &frustumPos[5]);



}


//프러스텀 안에 걸져있니?
bool cFrustum::IsInFrustum(D3DXVECTOR3* pCenter, float radius)
{
	//back 평면 제외
	for (int i = 1; i < 6; i++){

		float d = PHYSICS_MGR->PlaneDot(&this->frustumPlane[i], pCenter);

		//최단거리가 구의 반지름 보다 크면 프러스텀안에 있을수 없다
		if (d > radius)
			return false;
	}


	return true;
}

//기즈모를 그려본다.
void cFrustum::RenderGizmos()
{
	//   5-------6
	//  /|      /|
	// 1-------2 |
	// | 4-----|-7
	// |/      |/
	// 0-------3 
	GIZMO_MGR->RenderLine(&frustumPos[0], &frustumPos[1], 0xFFFF00FF);
	GIZMO_MGR->RenderLine(&frustumPos[1], &frustumPos[2], 0xFFFF00FF);
	GIZMO_MGR->RenderLine(&frustumPos[2], &frustumPos[3], 0xFFFF00FF);
	GIZMO_MGR->RenderLine(&frustumPos[3], &frustumPos[0], 0xFFFF00FF);

	GIZMO_MGR->RenderLine(&frustumPos[4], &frustumPos[5], 0xFFFF00FF);
	GIZMO_MGR->RenderLine(&frustumPos[5], &frustumPos[6], 0xFFFF00FF);
	GIZMO_MGR->RenderLine(&frustumPos[6], &frustumPos[7], 0xFFFF00FF);
	GIZMO_MGR->RenderLine(&frustumPos[7], &frustumPos[4], 0xFFFF00FF);

	GIZMO_MGR->RenderLine(&frustumPos[4], &frustumPos[0], 0xFFFF00FF);
	GIZMO_MGR->RenderLine(&frustumPos[5], &frustumPos[1], 0xFFFF00FF);
	GIZMO_MGR->RenderLine(&frustumPos[6], &frustumPos[2], 0xFFFF00FF);
	GIZMO_MGR->RenderLine(&frustumPos[7], &frustumPos[3], 0xFFFF00FF);

}