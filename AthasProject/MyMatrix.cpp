#include "StdAfx.h"
#include "MyVector3.h"
#include "MyMatrix.h"
#include "MyMatrixFunction.h"


namespace MYD3D
{
	MYD3DXMATRIX& MYD3DXMATRIX::operator *= (CONST MYD3DXMATRIX& mat)
	{
		MYD3DXMATRIX Temp;
		ZeroMemory(&Temp, sizeof(MYD3DXMATRIX));

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					Temp.m[i][j] += m[i][k] * mat.m[k][j];
				}
			}
		}

		memcpy(this, &Temp, sizeof(MYD3DXMATRIX));

		return *this;

	}

	MYD3DXMATRIX& MYD3DXMATRIX::operator += (CONST MYD3DXMATRIX& mat)
	{
		_11 += mat._11;	_12 += mat._12;	_13 += mat._13;	_14 += mat._14;
		_21 += mat._21;	_22 += mat._22;	_23 += mat._23;	_24 += mat._24;
		_31 += mat._31;	_32 += mat._32;	_33 += mat._33;	_34 += mat._34;
		_41 += mat._41;	_42 += mat._42;	_43 += mat._43;	_44 += mat._44;

		return *this;
	}

	MYD3DXMATRIX& MYD3DXMATRIX::operator -= (CONST MYD3DXMATRIX& mat)
	{
		_11 -= mat._11;	_12 -= mat._12;	_13 -= mat._13;	_14 -= mat._14;
		_21 -= mat._21;	_22 -= mat._22;	_23 -= mat._23;	_24 -= mat._24;
		_31 -= mat._31;	_32 -= mat._32;	_33 -= mat._33;	_34 -= mat._34;
		_41 -= mat._41;	_42 -= mat._42;	_43 -= mat._43;	_44 -= mat._44;

		return *this;
	}

	MYD3DXMATRIX& MYD3DXMATRIX::operator *= (FLOAT fValue)
	{
		_11 *= fValue;	_12 *= fValue; _13 *= fValue;	_14 *= fValue;
		_21 *= fValue;	_22 *= fValue; _23 *= fValue;	_24 *= fValue;
		_31 *= fValue;	_32 *= fValue; _33 *= fValue;	_34 *= fValue;
		_41 *= fValue;	_42 *= fValue; _43 *= fValue;	_44 *= fValue;

		return *this;
	}

	MYD3DXMATRIX& MYD3DXMATRIX::operator /= (FLOAT fValue)
	{
		FLOAT inv = 1.0f / fValue;

		_11 *= inv;	_12 *= inv; _13 *= inv;	_14 *= inv;
		_21 *= inv;	_22 *= inv; _23 *= inv;	_24 *= inv;
		_31 *= inv;	_32 *= inv; _33 *= inv;	_34 *= inv;
		_41 *= inv;	_42 *= inv; _43 *= inv;	_44 *= inv;

		return *this;

	}

	MYD3DXMATRIX MYD3DXMATRIX::operator + () const
	{
		return *this;
	}

	MYD3DXMATRIX MYD3DXMATRIX::operator - () const
	{
		return MYD3DXMATRIX(
			-_11, -_12, -_13, -_14,
			-_21, -_22, -_23, -_24,
			-_31, -_32, -_33, -_34,
			-_41, -_42, -_43, -_44
			);
	}

	MYD3DXMATRIX MYD3DXMATRIX::operator * (CONST MYD3DXMATRIX& mat) const
	{
		MYD3DXMATRIX matT;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				matT.m[i][j] = 0.0f;

				for (int k = 0; k < 4; k++)
				{
					matT.m[i][j] += m[i][k] * mat.m[k][j];

				}
			}
		}

		return matT;
	}

	MYD3DXMATRIX MYD3DXMATRIX::operator + (CONST MYD3DXMATRIX& mat) const
	{
		return MYD3DXMATRIX(
			_11 + mat._11, _12 + mat._12, _13 + mat._13, _14 + mat._14,
			_21 + mat._21, _22 + mat._22, _23 + mat._23, _24 + mat._24,
			_31 + mat._31, _32 + mat._32, _33 + mat._33, _34 + mat._34,
			_41 + mat._41, _42 + mat._42, _43 + mat._43, _44 + mat._44
			);
	}

	MYD3DXMATRIX MYD3DXMATRIX::operator - (CONST MYD3DXMATRIX& mat) const
	{
		return MYD3DXMATRIX(
			_11 - mat._11, _12 - mat._12, _13 - mat._13, _14 - mat._14,
			_21 - mat._21, _22 - mat._22, _23 - mat._23, _24 - mat._24,
			_31 - mat._31, _32 - mat._32, _33 - mat._33, _34 - mat._34,
			_41 - mat._41, _42 - mat._42, _43 - mat._43, _44 - mat._44
			);
	}

	MYD3DXMATRIX MYD3DXMATRIX::operator * (FLOAT fValue) const
	{
		return MYD3DXMATRIX(
			_11 * fValue, _12 * fValue, _13 * fValue, _14 * fValue,
			_21 * fValue, _22 * fValue, _23 * fValue, _24 * fValue,
			_31 * fValue, _32 * fValue, _33 * fValue, _34 * fValue,
			_41 * fValue, _42 * fValue, _43 * fValue, _44 * fValue
			);
	}

	MYD3DXMATRIX MYD3DXMATRIX::operator / (FLOAT fValue) const
	{
		FLOAT inv = 1.0f / fValue;

		return MYD3DXMATRIX(
			_11 * inv, _12 * inv, _13 * inv, _14 * inv,
			_21 * inv, _22 * inv, _23 * inv, _24 * inv,
			_31 * inv, _32 * inv, _33 * inv, _34 * inv,
			_41 * inv, _42 * inv, _43 * inv, _44 * inv
			);
	}

	MYD3DXMATRIX operator * (FLOAT fValue, CONST MYD3DXMATRIX& mat)
	{
		return MYD3DXMATRIX(
			mat._11 * fValue, mat._12 * fValue, mat._13 * fValue, mat._14 * fValue,
			mat._21 * fValue, mat._22 * fValue, mat._23 * fValue, mat._24 * fValue,
			mat._31 * fValue, mat._32 * fValue, mat._33 * fValue, mat._34 * fValue,
			mat._41 * fValue, mat._42 * fValue, mat._43 * fValue, mat._44 * fValue
			);
	}

	BOOL MYD3DXMATRIX::operator == (CONST MYD3DXMATRIX& mat) const
	{
		return 0 == memcmp(this, &mat, sizeof(MYD3DXMATRIX));
	}

	BOOL MYD3DXMATRIX::operator != (CONST MYD3DXMATRIX& mat) const
	{
		return 0 != memcmp(this, &mat, sizeof(MYD3DXMATRIX));
	}

}