#include "StdAfx.h"
#include "MyVector3.h"
#include "MyVector3Function.h"
#include "MyMatrix.h"
#include "MyMatrixFunction.h"

namespace MYD3D
{
	// ����� ����Ʈ�� �ݿ��ϴ� �Լ� ( D3DXVec3TransformCoord)
	MYD3DXVECTOR3* MYD3DXVec3TransformCoord(MYD3DXVECTOR3* pOut, CONST MYD3DXVECTOR3* pV, CONST MYD3DXMATRIX* pM)
	{
		//�̵� ���б��� ������ �ȴ�.
		float x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + 1 * pM->_41;
		float y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + 1 * pM->_42;
		float z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + 1 * pM->_43;
		
		//��ȯ����ġ
		float w = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + 1 * pM->_44;

		//��ȯ����ġ�� ��� xyz �� ������ ���Ͻ�Ų�� ( ���� ��ȯ���� ������ ����ϸ� ���ص� ���������, ������ĺ�ȯ���� �����Ѵٸ� �� �ؾߵȴ� )
		pOut->x = x / w;
		pOut->y = y / w;
		pOut->z = z / w;

		return pOut;
	}

	// ����� ���Ϳ� �ݿ��ϴ� �Լ� ( D3DXVec3TranformNormal)
	MYD3DXVECTOR3* MYD3DXVec3TransformNormal(MYD3DXVECTOR3* pOut, CONST MYD3DXVECTOR3* pV, CONST MYD3DXMATRIX* pM)
	{
		//�̵� ������ ������� �ʴ´�.
		FLOAT x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31;
		FLOAT y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32;
		FLOAT z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33;

		pOut->x = x;
		pOut->y = y;
		pOut->z = z;

		return pOut;

	}

	// ��� ��
	MYD3DXMATRIX* MYD3DXMatrixMultiply(MYD3DXMATRIX *pOut, CONST MYD3DXMATRIX *pM1, CONST MYD3DXMATRIX *pM2)
	{
		//�ʱ�ȭ
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

	//�׵� ���
	MYD3DXMATRIX* MYD3DXMatrixIdentity(MYD3DXMATRIX *pOut)
	{
		ZeroMemory(pOut, sizeof(MYD3DXMATRIX));
		for (int i = 0; i < 4; i++)
			pOut->m[i][i] = 1.0f;

		return pOut;
	}

	// ��ġ ��� : ��� ���� ��ȯ�Ѵ�.
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

	// �̵� ���
	MYD3DXMATRIX* MYD3DXMatrixTranslation(MYD3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z)
	{
		pOut->_11 = 1;		pOut->_12 = 0;		pOut->_13 = 0;		pOut->_14 = 0;
		pOut->_21 = 0;		pOut->_22 = 1;		pOut->_23 = 0;		pOut->_24 = 0;
		pOut->_31 = 0;		pOut->_32 = 0;		pOut->_33 = 1;		pOut->_34 = 0;
		pOut->_41 = x;		pOut->_42 = y;		pOut->_43 = z;		pOut->_44 = 1;

		return pOut;
	}

	///< X ȸ�� ��� : _22 _23 _32 _33 / cos sin -sin cos
	MYD3DXMATRIX* MYD3DXMatrixRotationX(MYD3DXMATRIX *pOut, FLOAT Angle)
	{
		///< �׵� ��ķ� �ʱ�ȭ
		MYD3DXMatrixIdentity(pOut);

		pOut->_22 = cosf(Angle);
		pOut->_23 = sinf(Angle);
		pOut->_32 = -sinf(Angle);
		pOut->_33 = cosf(Angle);

		return pOut;
	}

	// Y ȸ�� ��� : _11 , _13 , _31 , _33 / cos -sin sin cos
	MYD3DXMATRIX* MYD3DXMatrixRotationY(MYD3DXMATRIX *pOut, FLOAT Angle)
	{
		// �׵� ��ķ� �ʱ�ȭ
		MYD3DXMatrixIdentity(pOut);

		pOut->_11 = cosf(Angle);
		pOut->_13 = -sinf(Angle);
		pOut->_31 = sinf(Angle);
		pOut->_33 = cosf(Angle);

		return pOut;
	}
	// Z ȸ�� ���
	MYD3DXMATRIX* MYD3DXMatrixRotationZ(MYD3DXMATRIX *pOut, FLOAT Angle)
	{
		// �׵� ��ķ� �ʱ�ȭ
		MYD3DXMatrixIdentity(pOut);

		pOut->_11 = cosf(Angle);
		pOut->_12 = sinf(Angle);
		pOut->_21 = -sinf(Angle);
		pOut->_22 = cosf(Angle);

		return pOut;
	}

	// ��ȸ�� 
	// MYD3DXMATRIX* pOut : ��ȯ
	// MYD3DXVECTOR3* vAxis : ȸ�� ���� ��
	// FLOAT fAngle : ȸ�� ��( Rad )
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

	// ũ�� ���� ���
	MYD3DXMATRIX* MYD3DXMatrixScaling(MYD3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz)
	{
		ZeroMemory(pOut, sizeof(MYD3DMATRIX));

		pOut->_11 = sx;
		pOut->_22 = sy;
		pOut->_33 = sz;
		pOut->_44 = 1;

		return pOut;
	}

	//�����
	MYD3DXMATRIX* MYD3DXMatrixInverse(MYD3DXMATRIX *pOut, FLOAT* pDerterminant, CONST MYD3DXMATRIX* pM)
	{
		///< ��Ľ� : �Ǻ��� 
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

		if (detpM == 0)	//�Ǻ��� �Ǻ� ����
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

	// ī�޶� ��ǥ��� ��ȯ
	MYD3DXMATRIX* MYD3DXMatrixLookAtLH(MYD3DXMATRIX *pOut,
		CONST MYD3DXVECTOR3 *pEye, CONST MYD3DXVECTOR3 *pAt, CONST MYD3DXVECTOR3 *pUp)
	{
		MYD3DXVECTOR3 XAxis, YAxis, ZAxis;

		//���� ���� ����Ű�� Z ��������( basic vector ) �� ���Ѵ�.
		//eye point( ī�޶� ��ġ ) �� look-at point( ���� ��ġ ) �� ���� ���ϸ�
		//�̰��� �ü��� ������ �ȴ�.
		MYD3DXVec3Subtract(&ZAxis, pAt, pEye);

		//z ���� ���͸� ����ȭ
		MYD3DXVec3Normalize(&ZAxis, &ZAxis);

		//z ���� ���Ϳ� vUp ���͸� ����(Cross) �Ͽ� X ���� ���͸� ���Ѵ�.
		MYD3DXVec3Cross(&XAxis, pUp, &ZAxis);

		//X ���� ���͸� ����ȭ
		MYD3DXVec3Normalize(&XAxis, &XAxis);

		//Z���� ���Ϳ� X ���� ���͸� ����(Corss) �Ͽ� Y ���� ���͸� ���Ѵ�.
		MYD3DXVec3Cross(&YAxis, &ZAxis, &XAxis);

		//��� �����
		// XAxis.x				YAxis.x				ZAxis.x				0.0f
		// XAxis.y				YAxis.y				ZAxis.y				0.0f
		// XAxis.z				YAxis.z				ZAxis.z				0.0f
		// -( XAxis Dot pEye )	-( YAxis Dot Eye )	-( ZAxis Dot Eye )	1.0f

		// ����3 ���� ī�޶� ���� ���� ȸ���� ���Ǵ� ���� ���͸� ������
		// �׹�° ���� �̵���ȭ ���� ������.

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

	// ���� ���� ��� ���
	// FLOAT w		���� ���� ũ��
	// FLOAT h		���� ���� ũ��
	// FLOAT sz		�ٰŸ� Ŭ����
	// FLOAT sf		���Ÿ� Ŭ����
	MYD3DXMATRIX* MYD3DXMatrixOrthoLH(MYD3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf)
	{
		//���� ���� ��� ����
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

	// ���� ���� ��� ���
	// FLOAT fovy	ȭ��
	// FLOAT aspect	���� ��Ⱦ��
	// FLOAT sz		�ٰŸ� Ŭ����
	// FLOAT sf		���Ÿ� Ŭ����
	MYD3DXMATRIX* MYD3DXMatrixPerspectiveFovLH(MYD3DXMATRIX* pOut, FLOAT fovy, FLOAT aspect, FLOAT zn, FLOAT zf)
	{
		//d �� ���� ��ġ�� ���� ���� ���� 2.0 �� �Ǵ� �Ÿ��̴�.


		// �������� ��� �ﰢ�� ���� �ܸ鿡�� ȭ���� 2�� ���� �����ﰢ���� ��
		//
		//      P
		//    / |
		//   /  |
		//  /	|		<- E �� Theta �� fovy / 2.0f;
		// E----D		<- ���������� ���� �ܺ��� �ﰢ���� 2��� �ϰ� �Ǹ� D ���� P ������ �Ÿ��� 1 �̴�.
		//
		// ���� E ���� D ������ �Ÿ� d �� ���ϴ� ������ 1 / tan( fovy / 2.0f ) �̵ȴ�.

		// d = ���� ��ġ�� ���� ���� ���� 2.0 �� �Ǵ� �Ÿ�
		// a = ��Ⱦ��
		// zn = �ٰŸ� Ŭ���� �Ÿ�
		// zf = ���Ÿ� Ŭ���� �Ÿ�

		// ���� ���� ���
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