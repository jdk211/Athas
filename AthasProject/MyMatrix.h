#pragma once

namespace MYD3D
{
	typedef struct _MYD3DMATRIX{
		union{
			struct{
				FLOAT _11, _12, _13, _14;
				FLOAT _21, _22, _23, _24;
				FLOAT _31, _32, _33, _34;
				FLOAT _41, _42, _43, _44;
			};
			FLOAT m[4][4];
		};
	}MYD3DMATRIX;


	typedef struct MYD3DXMATRIX : public MYD3DMATRIX
	{
	public:
		MYD3DXMATRIX(){};
		MYD3DXMATRIX(CONST FLOAT *pf )
		{
			memmove(&_11, pf, sizeof(MYD3DXMATRIX) );
		}
		MYD3DXMATRIX( CONST MYD3DMATRIX& mat )
		{
			memmove(&_11, &mat, sizeof(MYD3DXMATRIX) );
		}

		MYD3DXMATRIX(
			FLOAT f11, FLOAT f12, FLOAT f13, FLOAT f14,
			FLOAT f21, FLOAT f22, FLOAT f23, FLOAT f24,
			FLOAT f31, FLOAT f32, FLOAT f33, FLOAT f34,
			FLOAT f41, FLOAT f42, FLOAT f43, FLOAT f44 )
		{
			_11 = f11;	_12 = f12;	_13 = f13;	_14 = f14;
			_21 = f21;	_22 = f22;	_23 = f23;	_24 = f24;
			_31 = f31;	_32 = f32;	_33 = f33;	_34 = f34;
			_41 = f41;	_42 = f42;	_43 = f43;	_44 = f44;
		}

		//일반 객체의 사용시 콜~
		inline FLOAT& operator() (UINT iRow, UINT iCol)
		{
			return m[iRow][iCol];
		}

		//상수 객체 사용시 콜~
		inline FLOAT operator()( UINT iRow, UINT iCol ) const
		{
			return m[iRow][iCol];
		}

		inline operator FLOAT* ()
		{
			return static_cast<FLOAT*>( &_11 );
		}

		inline operator CONST FLOAT*() const
		{
			return static_cast<CONST FLOAT*>( &_11 );
		}

		MYD3DXMATRIX& operator *= ( CONST MYD3DXMATRIX& );
		MYD3DXMATRIX& operator += ( CONST MYD3DXMATRIX& );
		MYD3DXMATRIX& operator -= ( CONST MYD3DXMATRIX& );
		MYD3DXMATRIX& operator *= ( FLOAT );
		MYD3DXMATRIX& operator /= ( FLOAT );

		MYD3DXMATRIX operator + () const;
		MYD3DXMATRIX operator - () const;

		MYD3DXMATRIX operator * ( CONST MYD3DXMATRIX& ) const;
		MYD3DXMATRIX operator + ( CONST MYD3DXMATRIX& ) const;
		MYD3DXMATRIX operator - ( CONST MYD3DXMATRIX& ) const;
		MYD3DXMATRIX operator * ( FLOAT ) CONST;
		MYD3DXMATRIX operator / ( FLOAT ) CONST;

		friend MYD3DXMATRIX operator * ( FLOAT, CONST MYD3DXMATRIX& );

		BOOL operator == ( CONST MYD3DXMATRIX& ) const;
		BOOL operator != ( CONST MYD3DXMATRIX& ) const;

	} MYD3DXMATRIX, *LPMYD3DXMATRIX;
}