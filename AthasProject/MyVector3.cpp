#include "StdAfx.h"
#include "MyVector3.h"

namespace MYD3D
{
#ifdef __cplusplus

	//FLOAT 배열로 생성
	MYD3DXVECTOR3::MYD3DXVECTOR3( CONST FLOAT * pv )
	{
		x = pv[0];
		y = pv[1];
		z = pv[2];
	}
	//인자 있는 생성자	
	MYD3DXVECTOR3::MYD3DXVECTOR3( FLOAT x, FLOAT y, FLOAT z )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	//복사 생성자
	MYD3DXVECTOR3::MYD3DXVECTOR3( CONST MYD3DXVECTOR3& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	//캐스팅
	MYD3DXVECTOR3::operator FLOAT* ()
	{
		return static_cast<FLOAT*>( &x );
	}

	MYD3DXVECTOR3::operator CONST FLOAT* () const
	{
		return static_cast<CONST FLOAT*>( &x );
	}

	//할당 연산자
	MYD3DXVECTOR3& MYD3DXVECTOR3::operator += ( CONST MYD3DXVECTOR3& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}
	MYD3DXVECTOR3& MYD3DXVECTOR3::operator -= ( CONST MYD3DXVECTOR3& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		
		return *this;
	}
	MYD3DXVECTOR3& MYD3DXVECTOR3::operator *= ( FLOAT fValue )
	{
		x *= fValue;
		y *= fValue;
		z *= fValue;

		return *this;
	}
	MYD3DXVECTOR3& MYD3DXVECTOR3::operator /= ( FLOAT fValue )
	{
		//x /= fValue;
		//y /= fValue;
		//z /= fValue;

		float inv = 1.0f / fValue;
		x *= inv;
		y *= inv;
		z *= inv;

		return *this;
	}

	//단항 연산자 ( 부호 )
	MYD3DXVECTOR3 MYD3DXVECTOR3::operator + () const
	{
		return *this;
	}
	MYD3DXVECTOR3 MYD3DXVECTOR3::operator - () const
	{
		return MYD3DXVECTOR3( -x, -y, -z );
	}

	//이항연산자 
	MYD3DXVECTOR3 MYD3DXVECTOR3::operator + ( CONST MYD3DXVECTOR3& v ) const
	{
		return MYD3DXVECTOR3( x + v.x, y + v.y, z + v.z );
	}

	MYD3DXVECTOR3 MYD3DXVECTOR3::operator - ( CONST MYD3DXVECTOR3& v ) const
	{
		return MYD3DXVECTOR3(x - v.x, y - v.y, z - v.z);
	}

	MYD3DXVECTOR3 MYD3DXVECTOR3::operator * ( FLOAT fValue ) const
	{
		return MYD3DXVECTOR3(x * fValue, y * fValue, z * fValue);
	}

	MYD3DXVECTOR3 MYD3DXVECTOR3::operator / ( FLOAT fValue ) const
	{
		float inv = 1.0f / fValue;
		return MYD3DXVECTOR3(x * inv, y * inv, z * inv);
	}


	//상수에 대한 곱하기 연산을 처리하기 위한 함수
	MYD3DXVECTOR3 operator * ( FLOAT fValue, CONST MYD3DXVECTOR3& v )
	{
		return MYD3DXVECTOR3( fValue * v.x, fValue * v.y, fValue * v.z );
	}

	//논리 연산
	BOOL MYD3DXVECTOR3::operator == ( CONST MYD3DXVECTOR3& v ) const
	{
		return ( x == v.x && y == v.y && z == v.z );
	}
	BOOL MYD3DXVECTOR3::operator != ( CONST MYD3DXVECTOR3& v ) const
	{
		return ( x != v.x || y != v.y || z != v.z );
	}

#endif //__cplusplus

}