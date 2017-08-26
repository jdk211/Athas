#include "StdAfx.h"
#include "MyVector3.h"
#include "MyVector3Function.h"
#include "MyMatrix.h"
#include "MyMatrixFunction.h"

namespace MYD3D
{
	// 행렬을 포인트에 반영하는 함수 ( D3DXVec3TransformCoord)
	MYD3DXVECTOR3* MYD3DXVec3TransformCoord(MYD3DXVECTOR3* pOut, CONST MYD3DXVECTOR3* pV, CONST MYD3DXMATRIX* pM)
	{
		//이동 성분까지 적용이 된다.
		float x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + 1 * pM->_41;
		float y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + 1 * pM->_42;
		float z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + 1 * pM->_43;
		
		//변환가중치
		float w = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + 1 * pM->_44;

		//변환가중치를 모든 xyz 에 나누어 리턴시킨다 ( 만약 변환행의 곱으로 사용하면 안해도 상관없지만, 투영행렬변환까지 생각한다면 꼭 해야된다 )
		pOut->x = x / w;
		pOut->y = y / w;
		pOut->z = z / w;

		return pOut;
	}

	// 행렬을 벡터에 반영하는 함수 ( D3DXVec3TranformNormal)
	MYD3DXVECTOR3* MYD3DXVec3TransformNormal(MYD3DXVECTOR3* pOut, CONST MYD3DXVECTOR3* pV, CONST MYD3DXMATRIX* pM)
	{
		//이동 성분은 적용되지 않는다.
		FLOAT x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31;
		FLOAT y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32;
		FLOAT z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33;

		pOut->x = x;
		pOut->y = y;
		pOut->z = z;

		return pOut;

	}

	// 행렬 곱
	MYD3DXMATRIX* MYD3DXMatrixMultiply(MYD3DXMATRIX *pOut, CONST MYD3DXMATRIX *pM1, CONST MYD3DXMATRIX *pM2)
	{
		//초기화
		memset(pOut, 0, sizeof(MYD3DXMATRIX));

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					pOut->m[i][j] += pM1->m[i][k] * pM2->m[k][j];
				}
			}
		}
		return pOut;
	}

	//항등 행렬
	MYD3DXMATRIX* MYD3DXMatrixIdentity(MYD3DXMATRIX *pOut)
	{
		ZeroMemory(pOut, sizeof(MYD3DXMATRIX));
		for (int i = 0; i < 4; i++)
			pOut->m[i][i] = 1.0f;

		return pOut;
	}

	// 전치 행렬 : 행과 열을 교환한다.
	MYD3DXMATRIX* MYD3DXMatrixTranspose(MYD3DXMATRIX *pOut, CONST MYD3DXMATRIX *pM)
	{
		MYD3DXMATRIX matTemp;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				matTemp.m[i][j] = pM->m[j][i];
			}
		}

		memcpy(pOut, &matTemp, sizeof(MYD3DXMATRIX));

		return pOut;
	}

	// 이동 행렬
	MYD3DXMATRIX* MYD3DXMatrixTranslation(MYD3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z)
	{
		pOut->_11 = 1;		pOut->_12 = 0;		pOut->_13 = 0;		pOut->_14 = 0;
		pOut->_21 = 0;		pOut->_22 = 1;		pOut->_23 = 0;		pOut->_24 = 0;
		pOut->_31 = 0;		pOut->_32 = 0;		pOut->_33 = 1;		pOut->_34 = 0;
		pOut->_41 = x;		pOut->_42 = y;		pOut->_43 = z;		pOut->_44 = 1;

		return pOut;
	}

	///< X 회전 행렬 : _22 _23 _32 _33 / cos sin -sin cos
	MYD3DXMATRIX* MYD3DXMatrixRotationX(MYD3DXMATRIX *pOut, FLOAT Angle)
	{
		///< 항등 행렬로 초기화
		MYD3DXMatrixIdentity(pOut);

		pOut->_22 = cosf(Angle);
		pOut->_23 = sinf(Angle);
		pOut->_32 = -sinf(Angle);
		pOut->_33 = cosf(Angle);

		return pOut;
	}

	// Y 회전 행렬 : _11 , _13 , _31 , _33 / cos -sin sin cos
	MYD3DXMATRIX* MYD3DXMatrixRotationY(MYD3DXMATRIX *pOut, FLOAT Angle)
	{
		// 항등 행렬로 초기화
		MYD3DXMatrixIdentity(pOut);

		pOut->_11 = cosf(Angle);
		pOut->_13 = -sinf(Angle);
		pOut->_31 = sinf(Angle);
		pOut->_33 = cosf(Angle);

		return pOut;
	}
	// Z 회전 행렬
	MYD3DXMATRIX* MYD3DXMatrixRotationZ(MYD3DXMATRIX *pOut, FLOAT Angle)
	{
		// 항등 행렬로 초기화
		MYD3DXMatrixIdentity(pOut);

		pOut->_11 = cosf(Angle);
		pOut->_12 = sinf(Angle);
		pOut->_21 = -sinf(Angle);
		pOut->_22 = cosf(Angle);

		return pOut;
	}

	// 축회전 
	// MYD3DXMATRIX* pOut : 반환
	// MYD3DXVECTOR3* vAxis : 회전 기준 축
	// FLOAT fAngle : 회전 각( Rad )
	MYD3DXMATRIX* MYD3DXMatrixRotationAxis(MYD3DXMATRIX* pOut, MYD3DXVECTOR3* vAxis, FLOAT fAngle)
	{
		FLOAT			fSinAngle = sinf(fAngle);
		FLOAT			fCosAngle = cosf(fAngle);
		FLOAT			fDivAngle = 1 - fCosAngle;

		pOut->_11 = fDivAngle * vAxis->x * vAxis->x + fCosAngle;
		pOut->_12 = fDivAngle * vAxis->x * vAxis->y + vAxis->z * fSinAngle;
		pOut->_13 = fDivAngle * vAxis->x * vAxis->z - vAxis->y * fSinAngle;

		pOut->_21 = fDivAngle * vAxis->x * vAxis->y - vAxis->z * fSinAngle;
		pOut->_22 = fDivAngle * vAxis->y * vAxis->y + fCosAngle;
		pOut->_23 = fDivAngle * vAxis->y * vAxis->z + vAxis->x * fSinAngle;

		pOut->_31 = fDivAngle * vAxis->x * vAxis->y + vAxis->y * fSinAngle;
		pOut->_32 = fDivAngle * vAxis->y * vAxis->z - vAxis->x * fSinAngle;
		pOut->_33 = fDivAngle * vAxis->z * vAxis->z + fCosAngle;

		pOut->_14 = pOut->_24 = pOut->_34 = pOut->_41 = pOut->_42 = pOut->_43 = 0;

		pOut->_44 = 1;

		return pOut;
	}

	// 크기 변형 행렬
	MYD3DXMATRIX* MYD3DXMatrixScaling(MYD3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz)
	{
		ZeroMemory(pOut, sizeof(MYD3DMATRIX));

		pOut->_11 = sx;
		pOut->_22 = sy;
		pOut->_33 = sz;
		pOut->_44 = 1;

		return pOut;
	}

	//역행렬
	MYD3DXMATRIX* MYD3DXMatrixInverse(MYD3DXMATRIX *pOut, FLOAT* pDerterminant, CONST MYD3DXMATRIX* pM)
	{
		///< 행렬식 : 판별식 
		FLOAT detpM =
			pM->_11 * pM->_22 * pM->_33 * pM->_44 +
			pM->_11 * pM->_23 * pM->_34 * pM->_42 +
			pM->_11 * pM->_24 * pM->_32 * pM->_43 +

			pM->_12 * pM->_21 * pM->_34 * pM->_43 +
			pM->_12 * pM->_23 * pM->_31 * pM->_44 +
			pM->_12 * pM->_24 * pM->_33 * pM->_41 +

			pM->_13 * pM->_21 * pM->_32 * pM->_44 +
			pM->_13 * pM->_22 * pM->_34 * pM->_41 +
			pM->_13 * pM->_24 * pM->_31 * pM->_42 +

			pM->_14 * pM->_21 * pM->_33 * pM->_42 +
			pM->_14 * pM->_22 * pM->_31 * pM->_43 +
			pM->_14 * pM->_23 * pM->_32 * pM->_41 -

			pM->_11 * pM->_22 * pM->_34 * pM->_43 -
			pM->_11 * pM->_23 * pM->_32 * pM->_44 -
			pM->_11 * pM->_24 * pM->_33 * pM->_42 -

			pM->_12 * pM->_21 * pM->_33 * pM->_44 -
			pM->_12 * pM->_23 * pM->_34 * pM->_41 -
			pM->_12 * pM->_24 * pM->_31 * pM->_43 -

			pM->_13 * pM->_21 * pM->_34 * pM->_42 -
			pM->_13 * pM->_22 * pM->_31 * pM->_44 -
			pM->_13 * pM->_24 * pM->_32 * pM->_41 -

			pM->_14 * pM->_21 * pM->_32 * pM->_43 -
			pM->_14 * pM->_22 * pM->_33 * pM->_41 -
			pM->_14 * pM->_23 * pM->_31 * pM->_42;

		if (detpM == 0)	//판별식 판별 실패
		{
			return NULL;
		}
		else
		{
			pOut->_11 = (
				(pM->_22 * pM->_33 * pM->_44) +
				(pM->_23 * pM->_34 * pM->_42) +
				(pM->_24 * pM->_32 * pM->_43) -
				(pM->_22 * pM->_34 * pM->_43) -
				(pM->_23 * pM->_32 * pM->_44) -
				(pM->_24 * pM->_33 * pM->_42)) / detpM;

			pOut->_12 = (
				(pM->_12 * pM->_34 * pM->_43) +
				(pM->_13 * pM->_32 * pM->_44) +
				(pM->_14 * pM->_33 * pM->_42) -
				(pM->_12 * pM->_33 * pM->_44) -
				(pM->_13 * pM->_34 * pM->_42) -
				(pM->_14 * pM->_32 * pM->_43)) / detpM;

			pOut->_13 = (
				(pM->_12 * pM->_23 * pM->_44) +
				(pM->_13 * pM->_24 * pM->_42) +
				(pM->_14 * pM->_22 * pM->_43) -
				(pM->_12 * pM->_24 * pM->_43) -
				(pM->_13 * pM->_22 * pM->_44) -
				(pM->_14 * pM->_23 * pM->_42)) / detpM;

			pOut->_14 = (
				(pM->_12 * pM->_24 * pM->_33) +
				(pM->_13 * pM->_22 * pM->_34) +
				(pM->_14 * pM->_23 * pM->_32) -
				(pM->_12 * pM->_23 * pM->_34) -
				(pM->_13 * pM->_24 * pM->_32) -
				(pM->_14 * pM->_22 * pM->_33)) / detpM;

			pOut->_21 = (
				(pM->_21 * pM->_34 * pM->_43) +
				(pM->_23 * pM->_31 * pM->_44) +
				(pM->_24 * pM->_33 * pM->_41) -
				(pM->_21 * pM->_33 * pM->_44) -
				(pM->_23 * pM->_34 * pM->_41) -
				(pM->_24 * pM->_31 * pM->_43)) / detpM;

			pOut->_22 = (
				(pM->_11 * pM->_33 * pM->_44) +
				(pM->_13 * pM->_34 * pM->_41) +
				(pM->_14 * pM->_31 * pM->_43) -
				(pM->_11 * pM->_34 * pM->_43) -
				(pM->_13 * pM->_31 * pM->_44) -
				(pM->_14 * pM->_33 * pM->_41)) / detpM;

			pOut->_23 = (
				(pM->_11 * pM->_24 * pM->_43) +
				(pM->_13 * pM->_21 * pM->_44) +
				(pM->_14 * pM->_23 * pM->_41) -
				(pM->_11 * pM->_23 * pM->_44) -
				(pM->_13 * pM->_24 * pM->_41) -
				(pM->_14 * pM->_21 * pM->_43)) / detpM;

			pOut->_24 = (
				(pM->_11 * pM->_23 * pM->_34) +
				(pM->_13 * pM->_24 * pM->_31) +
				(pM->_14 * pM->_21 * pM->_33) -
				(pM->_11 * pM->_24 * pM->_33) -
				(pM->_13 * pM->_21 * pM->_34) -
				(pM->_14 * pM->_23 * pM->_31)) / detpM;

			pOut->_31 = (
				(pM->_21 * pM->_32 * pM->_44) +
				(pM->_22 * pM->_34 * pM->_41) +
				(pM->_24 * pM->_31 * pM->_42) -
				(pM->_21 * pM->_34 * pM->_42) -
				(pM->_22 * pM->_31 * pM->_44) -
				(pM->_24 * pM->_32 * pM->_41)) / detpM;

			pOut->_32 = (
				(pM->_11 * pM->_34 * pM->_42) +
				(pM->_12 * pM->_31 * pM->_44) +
				(pM->_14 * pM->_32 * pM->_41) -
				(pM->_11 * pM->_32 * pM->_44) -
				(pM->_12 * pM->_34 * pM->_41) -
				(pM->_14 * pM->_31 * pM->_42)) / detpM;

			pOut->_33 = (
				(pM->_11 * pM->_22 * pM->_44) +
				(pM->_12 * pM->_24 * pM->_41) +
				(pM->_14 * pM->_21 * pM->_42) -
				(pM->_11 * pM->_24 * pM->_42) -
				(pM->_12 * pM->_21 * pM->_44) -
				(pM->_14 * pM->_22 * pM->_41)) / detpM;

			pOut->_34 = (
				(pM->_11 * pM->_24 * pM->_32) +
				(pM->_12 * pM->_21 * pM->_34) +
				(pM->_14 * pM->_22 * pM->_31) -
				(pM->_11 * pM->_22 * pM->_34) -
				(pM->_12 * pM->_24 * pM->_31) -
				(pM->_14 * pM->_21 * pM->_32)) / detpM;

			pOut->_41 = (
				(pM->_21 * pM->_33 * pM->_42) +
				(pM->_22 * pM->_31 * pM->_43) +
				(pM->_23 * pM->_32 * pM->_41) -
				(pM->_21 * pM->_32 * pM->_43) -
				(pM->_22 * pM->_33 * pM->_41) -
				(pM->_23 * pM->_31 * pM->_42)) / detpM;

			pOut->_42 = (
				(pM->_11 * pM->_32 * pM->_43) +
				(pM->_12 * pM->_33 * pM->_41) +
				(pM->_13 * pM->_31 * pM->_42) -
				(pM->_11 * pM->_33 * pM->_42) -
				(pM->_12 * pM->_31 * pM->_43) -
				(pM->_13 * pM->_32 * pM->_41)) / detpM;

			pOut->_43 = (
				(pM->_11 * pM->_23 * pM->_42) +
				(pM->_12 * pM->_21 * pM->_43) +
				(pM->_13 * pM->_22 * pM->_41) -
				(pM->_11 * pM->_22 * pM->_43) -
				(pM->_12 * pM->_23 * pM->_41) -
				(pM->_13 * pM->_21 * pM->_42)) / detpM;

			pOut->_44 = (
				(pM->_11 * pM->_22 * pM->_33) +
				(pM->_12 * pM->_23 * pM->_31) +
				(pM->_13 * pM->_21 * pM->_32) -
				(pM->_11 * pM->_23 * pM->_32) -
				(pM->_12 * pM->_21 * pM->_33) -
				(pM->_13 * pM->_22 * pM->_31)) / detpM;
		}

		return pOut;
	}

	// 카메라 좌표계로 변환
	MYD3DXMATRIX* MYD3DXMatrixLookAtLH(MYD3DXMATRIX *pOut,
		CONST MYD3DXVECTOR3 *pEye, CONST MYD3DXVECTOR3 *pAt, CONST MYD3DXVECTOR3 *pUp)
	{
		MYD3DXVECTOR3 XAxis, YAxis, ZAxis;

		//정면 앞을 가리키는 Z 기저벡터( basic vector ) 를 구한다.
		//eye point( 카메라 위치 ) 와 look-at point( 보는 위치 ) 의 차를 구하면
		//이것이 시선의 방향이 된다.
		MYD3DXVec3Subtract(&ZAxis, pAt, pEye);

		//z 기저 벡터를 정규화
		MYD3DXVec3Normalize(&ZAxis, &ZAxis);

		//z 기저 벡터와 vUp 벡터를 외적(Cross) 하여 X 기저 벡터를 구한다.
		MYD3DXVec3Cross(&XAxis, pUp, &ZAxis);

		//X 기저 벡터를 정규화
		MYD3DXVec3Normalize(&XAxis, &XAxis);

		//Z기저 벡터와 X 기저 벡터를 외적(Corss) 하여 Y 기저 벡터를 구한다.
		MYD3DXVec3Cross(&YAxis, &ZAxis, &XAxis);

		//행렬 만들기
		// XAxis.x				YAxis.x				ZAxis.x				0.0f
		// XAxis.y				YAxis.y				ZAxis.y				0.0f
		// XAxis.z				YAxis.z				ZAxis.z				0.0f
		// -( XAxis Dot pEye )	-( YAxis Dot Eye )	-( ZAxis Dot Eye )	1.0f

		// 위의3 행은 카메라 뷰의 시점 회전에 사용되는 기저 벡터를 가지며
		// 네번째 행은 이동변화 값을 가진다.

		pOut->_11 = XAxis.x;
		pOut->_21 = XAxis.y;
		pOut->_31 = XAxis.z;
		pOut->_41 = -MYD3DXVec3Dot(&XAxis, pEye);

		pOut->_12 = YAxis.x;
		pOut->_22 = YAxis.y;
		pOut->_32 = YAxis.z;
		pOut->_42 = -MYD3DXVec3Dot(&YAxis, pEye);

		pOut->_13 = ZAxis.x;
		pOut->_23 = ZAxis.y;
		pOut->_33 = ZAxis.z;
		pOut->_43 = -MYD3DXVec3Dot(&ZAxis, pEye);

		pOut->_14 = 0.0f;
		pOut->_24 = 0.0f;
		pOut->_34 = 0.0f;
		pOut->_44 = 1.0f;

		return pOut;
	}

	// 직교 투영 행렬 얻기
	// FLOAT w		뷰의 가로 크기
	// FLOAT h		뷰의 세로 크기
	// FLOAT sz		근거리 클리핑
	// FLOAT sf		원거리 클리핑
	MYD3DXMATRIX* MYD3DXMatrixOrthoLH(MYD3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf)
	{
		//직교 투영 행렬 공식
		// 2 / w		0,				0,					0
		// 0,			2 / h,			0,					0
		// 0,			0,				1 / ( zf - zn ),	0
		// 0,			0,				zn / ( zn - zf ),	1

		pOut->_11 = 2 / w;
		pOut->_21 = 0;
		pOut->_31 = 0;
		pOut->_41 = 0;

		pOut->_12 = 0;
		pOut->_22 = 2 / h;
		pOut->_32 = 0;
		pOut->_42 = 0;

		pOut->_13 = 0;
		pOut->_23 = 0;
		pOut->_33 = 1 / (zf - zn);
		pOut->_43 = zn / (zn - zf);

		pOut->_14 = 0;
		pOut->_24 = 0;
		pOut->_34 = 0;
		pOut->_44 = 1;

		return pOut;
	}

	// 원근 투영 행렬 얻기
	// FLOAT fovy	화각
	// FLOAT aspect	뷰의 종횡비
	// FLOAT sz		근거리 클리핑
	// FLOAT sf		원거리 클리핑
	MYD3DXMATRIX* MYD3DXMatrixPerspectiveFovLH(MYD3DXMATRIX* pOut, FLOAT fovy, FLOAT aspect, FLOAT zn, FLOAT zf)
	{
		//d 는 시점 위치로 부터 수직 폭이 2.0 이 되는 거리이다.


		// 프로젝션 행렬 삼각뿔 수직 단면에서 화각을 2로 나눈 직각삼각형의 면
		//
		//      P
		//    / |
		//   /  |
		//  /	|		<- E 의 Theta 는 fovy / 2.0f;
		// E----D		<- 프로젝션의 수직 단변의 삼각영을 2등분 하게 되면 D 부터 P 까지의 거리는 1 이다.
		//
		// 따라서 E 부터 D 까지의 거리 d 를 구하는 공식은 1 / tan( fovy / 2.0f ) 이된다.

		// d = 시점 위치로 부터 수직 폭이 2.0 가 되는 거리
		// a = 종횡비
		// zn = 근거리 클리핑 거리
		// zf = 원거리 클리핑 거리

		// 프로 젝션 행렬
		// d / a,			0,				0,					0
		// 0,				d,				0,					0
		// 0,				0,				zf/( zf-zn )		1
		// 0,				0,				-(zn*zf/(zf-zn))	0

		float d = 1.0f / tanf(fovy / 2.0f);

		pOut->_11 = d / aspect;	pOut->_12 = 0;			pOut->_13 = 0;						pOut->_14 = 0;
		pOut->_21 = 0;			pOut->_22 = d;			pOut->_23 = 0;						pOut->_24 = 0;
		pOut->_31 = 0;			pOut->_32 = 0;			pOut->_33 = zf / (zf - zn);			pOut->_34 = 1;
		pOut->_41 = 0;			pOut->_42 = 0;			pOut->_43 = -(zn*zf / (zf - zn));	pOut->_44 = 0;

		return pOut;
	}





}