#pragma once

namespace MYD3D
{
	//벡터 X Y Z
	typedef struct _MYD3DVECTOR3
	{
		FLOAT x;
		FLOAT y;
		FLOAT z;
	}MYD3DVECTOR3;


#ifdef __cplusplus

	//벡터 클레스
	typedef struct MYD3DXVECTOR3 : public MYD3DVECTOR3
	{
	public:
		MYD3DXVECTOR3(){};							//기본 생성자
		MYD3DXVECTOR3( CONST FLOAT * );				//인자있는 생성자
		MYD3DXVECTOR3( FLOAT x, FLOAT y, FLOAT z );	//인자 있는 생성자		
		MYD3DXVECTOR3( CONST MYD3DXVECTOR3& v );	//복사 생성자

		//캐스팅
		operator FLOAT* ();
		operator CONST FLOAT* () const;

		//할당 연산자
		MYD3DXVECTOR3& operator += ( CONST MYD3DXVECTOR3& v );	
		MYD3DXVECTOR3& operator -= ( CONST MYD3DXVECTOR3& v );	
		MYD3DXVECTOR3& operator *= ( FLOAT fValue );	
		MYD3DXVECTOR3& operator /= ( FLOAT fValue );	

		//단항 연산자 ( 부호 )
		MYD3DXVECTOR3 operator + () const;
		MYD3DXVECTOR3 operator - () const;

		//이항연산자
		MYD3DXVECTOR3 operator + ( CONST MYD3DXVECTOR3& v ) const;
		MYD3DXVECTOR3 operator - ( CONST MYD3DXVECTOR3& v ) const;	//(숙제)
		MYD3DXVECTOR3 operator * ( FLOAT fValue ) const;	//(숙제)
		MYD3DXVECTOR3 operator / ( FLOAT fValue ) const;	//(숙제)

		//상수에 대한 곱하기 연산을 처리하기 위한 함수 friecd 선언
		friend MYD3DXVECTOR3 operator * ( FLOAT fValue, CONST MYD3DXVECTOR3& v );

		//논리 연산
		BOOL operator == ( CONST MYD3DXVECTOR3& v ) const;
		BOOL operator != ( CONST MYD3DXVECTOR3& v ) const;
	}MYD3DXVECTOR3, *LPMYD3DXVECTOR3;

#endif //__cplusplus
}