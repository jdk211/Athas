#pragma once

namespace MYD3D
{
	//���� X Y Z
	typedef struct _MYD3DVECTOR3
	{
		FLOAT x;
		FLOAT y;
		FLOAT z;
	}MYD3DVECTOR3;


#ifdef __cplusplus

	//���� Ŭ����
	typedef struct MYD3DXVECTOR3 : public MYD3DVECTOR3
	{
	public:
		MYD3DXVECTOR3(){};							//�⺻ ������
		MYD3DXVECTOR3( CONST FLOAT * );				//�����ִ� ������
		MYD3DXVECTOR3( FLOAT x, FLOAT y, FLOAT z );	//���� �ִ� ������		
		MYD3DXVECTOR3( CONST MYD3DXVECTOR3& v );	//���� ������

		//ĳ����
		operator FLOAT* ();
		operator CONST FLOAT* () const;

		//�Ҵ� ������
		MYD3DXVECTOR3& operator += ( CONST MYD3DXVECTOR3& v );	
		MYD3DXVECTOR3& operator -= ( CONST MYD3DXVECTOR3& v );	
		MYD3DXVECTOR3& operator *= ( FLOAT fValue );	
		MYD3DXVECTOR3& operator /= ( FLOAT fValue );	

		//���� ������ ( ��ȣ )
		MYD3DXVECTOR3 operator + () const;
		MYD3DXVECTOR3 operator - () const;

		//���׿�����
		MYD3DXVECTOR3 operator + ( CONST MYD3DXVECTOR3& v ) const;
		MYD3DXVECTOR3 operator - ( CONST MYD3DXVECTOR3& v ) const;	//(����)
		MYD3DXVECTOR3 operator * ( FLOAT fValue ) const;	//(����)
		MYD3DXVECTOR3 operator / ( FLOAT fValue ) const;	//(����)

		//����� ���� ���ϱ� ������ ó���ϱ� ���� �Լ� friecd ����
		friend MYD3DXVECTOR3 operator * ( FLOAT fValue, CONST MYD3DXVECTOR3& v );

		//�� ����
		BOOL operator == ( CONST MYD3DXVECTOR3& v ) const;
		BOOL operator != ( CONST MYD3DXVECTOR3& v ) const;
	}MYD3DXVECTOR3, *LPMYD3DXVECTOR3;

#endif //__cplusplus
}