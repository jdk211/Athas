#include "StdAfx.h"
#include "MyVector3.h"
#include "MyVector3Function.h"

namespace MYD3D
{
	//����
	FLOAT MYD3DXVec3Dot( CONST MYD3DXVECTOR3* pV1, CONST MYD3DXVECTOR3* pV2 )
	{
		return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
	}

	//���� ( pOut ���, pV1 ����, pV2 ���� )
	MYD3DXVECTOR3* MYD3DXVec3Cross( MYD3DXVECTOR3* pOut, CONST MYD3DXVECTOR3* pV1, CONST MYD3DXVECTOR3* pV2 )
	{

		pOut->x = (pV1->y * pV2->z) - (pV1->z * pV2->y);
		pOut->y = (pV1->z * pV2->x) - (pV1->x * pV2->z);
		pOut->z = (pV1->x * pV2->y) - (pV1->y * pV2->x);
		return pOut;
	}

	//����
	FLOAT MYD3DXVec3Length( CONST MYD3DXVECTOR3* pV1 )
	{
		//return sqrt( MYD3DXVec3Dot(pV1, pV1) );
		return sqrt(pV1->x * pV1->x + pV1->y * pV1->y + pV1->z * pV1->z);
	}

	//����ȭ pOut ��� pV1 ���
	MYD3DXVECTOR3* MYD3DXVec3Normalize( MYD3DXVECTOR3* pOut, CONST MYD3DXVECTOR3* pV1 )
	{
		FLOAT fLength = MYD3DXVec3Length(pV1);
		FLOAT inv = 1.0f / fLength;
		pOut->x *= inv;
		pOut->y *= inv;
		pOut->z *= inv;

		return pOut;
	}

	//���� ���ͱ��Ҷ� ������ �Լ� pV1 ���� pV2 �� ���� ���� ���͸� ��ȯ�Ѵ�.
	MYD3DXVECTOR3* MYD3DXVec3Subtract( MYD3DXVECTOR3* pOut, CONST MYD3DXVECTOR3* pV1, CONST MYD3DXVECTOR3* pV2 )
	{
		pOut->x = pV1->x - pV2->x;
		pOut->y = pV1->y - pV2->y;
		pOut->z = pV1->z - pV2->z;

		return pOut;
	}

	//������ �ڻ��� ������ ������ �̾Ƴ��� ��� ( �κ��Ͱ� ����ȭ �Ǿ��־�� �Ѵ� )
	FLOAT MYD3DXVec3DotAngle( CONST MYD3DXVECTOR3* pV1, CONST MYD3DXVECTOR3* pV2 )
	{
		float dot = MYD3DXVec3Dot(pV1, pV2);
		if (dot > 1.0f)
			dot = 1.0f;
		else if (dot < -1.0f)
			dot = -1.0f;

		return acos(dot);
	}
}
