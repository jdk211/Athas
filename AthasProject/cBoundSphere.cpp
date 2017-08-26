#include "stdafx.h"
#include "cBoundSphere.h"


cBoundSphere::cBoundSphere()
{
}


cBoundSphere::~cBoundSphere()
{
}

//���� ������ ���Ϳ� �������� ��´�.
void cBoundSphere::GetWorldCenterRadius(const cTransform* pTrans, D3DXVECTOR3* outCenter, float* outRaidus)
{
	D3DXMATRIXA16 matFinal = pTrans->GetFinalMatrix();

	//���� ����
	D3DXVec3TransformCoord(outCenter, &this->localCenter, &matFinal);

	//���� ���� ������
	D3DXVECTOR3 worldHaflSize;
	this->GetWorldHalfSize(pTrans, &worldHaflSize);

	//���� ������������ ���ͱ��̰� �������̵ȴ�.
	*outRaidus = D3DXVec3Length(&worldHaflSize);
}

//���� ������ ���� ����� ��´�
void cBoundSphere::GetWorldHalfSize(const cTransform* pTrans, D3DXVECTOR3* outHalfSize)
{
	D3DXVECTOR3 scale = pTrans->GetScale();
	outHalfSize->x = this->localHalfSize.x * scale.x;
	outHalfSize->y = this->localHalfSize.y * scale.y;
	outHalfSize->z = this->localHalfSize.z * scale.z;

}

//Bound ������ ���� �Ѵ�.
void cBoundSphere::SetBound(const D3DXVECTOR3* pCenter, const D3DXVECTOR3* pHalfSize)
{
	this->localCenter = *pCenter;
	this->localHalfSize = *pHalfSize;
}

//Bound ������ ���� �Ѵ�.
void cBoundSphere::SetMinMax(const D3DXVECTOR3* pMin, const D3DXVECTOR3* pMax)
{
	this->localCenter = (*pMin + *pMax) * 0.5f;

	this->localHalfSize = *pMax - this->localCenter;
}

//�������Ѵ�.
void cBoundSphere::RenderGizmos(const cTransform* pTrans)
{
	//���� ���Ϳ� �������� ���Ѵ�.
	D3DXVECTOR3 worldCenter;
	float radius;

	this->GetWorldCenterRadius(pTrans, &worldCenter, &radius);

	GIZMO_MGR->DrawWireSphere(&worldCenter, radius, 0xFF008000);
}