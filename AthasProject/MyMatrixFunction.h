#pragma once


namespace MYD3D
{
	// ����� ����Ʈ�� �ݿ��ϴ� �Լ� ( D3DXVec3TransformCoord)
	MYD3DXVECTOR3* MYD3DXVec3TransformCoord(MYD3DXVECTOR3* pOut, CONST MYD3DXVECTOR3* pV, CONST MYD3DXMATRIX* pM );

	// ����� ���Ϳ� �ݿ��ϴ� �Լ� ( D3DXVec3TranformNormal)
	MYD3DXVECTOR3* MYD3DXVec3TransformNormal(MYD3DXVECTOR3* pOut, CONST MYD3DXVECTOR3* pV, CONST MYD3DXMATRIX* pM );

	// ��� ��
	MYD3DXMATRIX* MYD3DXMatrixMultiply( MYD3DXMATRIX *pOut, CONST MYD3DXMATRIX *pM1, CONST MYD3DXMATRIX *pM2);

	//�׵� ���
	MYD3DXMATRIX* MYD3DXMatrixIdentity( MYD3DXMATRIX *pOut );

	// ��ġ ���
	MYD3DXMATRIX* MYD3DXMatrixTranspose( MYD3DXMATRIX *pOut, CONST MYD3DXMATRIX *pM);

	// �̵� ���
	MYD3DXMATRIX* MYD3DXMatrixTranslation( MYD3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z);

	// ȸ�� ���
	MYD3DXMATRIX* MYD3DXMatrixRotationX( MYD3DXMATRIX *pOut, FLOAT Angle );
	MYD3DXMATRIX* MYD3DXMatrixRotationY( MYD3DXMATRIX *pOut, FLOAT Angle );
	MYD3DXMATRIX* MYD3DXMatrixRotationZ( MYD3DXMATRIX *pOut, FLOAT Angle );

	// �� ȸ��
	MYD3DXMATRIX* MYD3DXMatrixRotationAxis( MYD3DXMATRIX* pOut, MYD3DXVECTOR3* vAxis, FLOAT fAngle);

	// ũ�� ���� ���
	MYD3DXMATRIX* MYD3DXMatrixScaling( MYD3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz);

	//�����
	//http://cafe.naver.com/dreamem/431 (����� ���� �ּ�)
	MYD3DXMATRIX* MYD3DXMatrixInverse( MYD3DXMATRIX *pOut, FLOAT* pDerterminant, CONST MYD3DXMATRIX* pM);

	// ī�޶� ��ǥ��� ��ȯ
	MYD3DXMATRIX* MYD3DXMatrixLookAtLH( MYD3DXMATRIX *pOut, CONST MYD3DXVECTOR3 *pEye, CONST MYD3DXVECTOR3 *pAt, CONST MYD3DXVECTOR3 *pUp );

	// ���� ���� ��� ���
	MYD3DXMATRIX* MYD3DXMatrixOrthoLH( MYD3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf ); 

	// ���� ���� ��� ���
	MYD3DXMATRIX* MYD3DXMatrixPerspectiveFovLH(MYD3DXMATRIX* pOut, FLOAT fovy, FLOAT aspect, FLOAT zn, FLOAT zf);

}