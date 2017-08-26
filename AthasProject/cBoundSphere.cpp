#include "stdafx.h"
#include "cBoundSphere.h"


cBoundSphere::cBoundSphere()
{
}


cBoundSphere::~cBoundSphere()
{
}

//월드 단위의 센터와 반지름을 얻는다.
void cBoundSphere::GetWorldCenterRadius(const cTransform* pTrans, D3DXVECTOR3* outCenter, float* outRaidus)
{
	D3DXMATRIXA16 matFinal = pTrans->GetFinalMatrix();

	//월드 센터
	D3DXVec3TransformCoord(outCenter, &this->localCenter, &matFinal);

	//월드 하프 사이즈
	D3DXVECTOR3 worldHaflSize;
	this->GetWorldHalfSize(pTrans, &worldHaflSize);

	//월드 하프사이즈의 벡터길이가 반지름이된다.
	*outRaidus = D3DXVec3Length(&worldHaflSize);
}

//월드 단위의 하프 사이즈를 얻는다
void cBoundSphere::GetWorldHalfSize(const cTransform* pTrans, D3DXVECTOR3* outHalfSize)
{
	D3DXVECTOR3 scale = pTrans->GetScale();
	outHalfSize->x = this->localHalfSize.x * scale.x;
	outHalfSize->y = this->localHalfSize.y * scale.y;
	outHalfSize->z = this->localHalfSize.z * scale.z;

}

//Bound 정보를 셋팅 한다.
void cBoundSphere::SetBound(const D3DXVECTOR3* pCenter, const D3DXVECTOR3* pHalfSize)
{
	this->localCenter = *pCenter;
	this->localHalfSize = *pHalfSize;
}

//Bound 정보를 셋팅 한다.
void cBoundSphere::SetMinMax(const D3DXVECTOR3* pMin, const D3DXVECTOR3* pMax)
{
	this->localCenter = (*pMin + *pMax) * 0.5f;

	this->localHalfSize = *pMax - this->localCenter;
}

//랜더링한다.
void cBoundSphere::RenderGizmos(const cTransform* pTrans)
{
	//월드 센터와 반지름을 구한다.
	D3DXVECTOR3 worldCenter;
	float radius;

	this->GetWorldCenterRadius(pTrans, &worldCenter, &radius);

	GIZMO_MGR->DrawWireSphere(&worldCenter, radius, 0xFF008000);
}