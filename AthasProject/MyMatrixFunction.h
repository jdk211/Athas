#pragma once


namespace MYD3D
{
	// 행렬을 포인트에 반영하는 함수 ( D3DXVec3TransformCoord)
	MYD3DXVECTOR3* MYD3DXVec3TransformCoord(MYD3DXVECTOR3* pOut, CONST MYD3DXVECTOR3* pV, CONST MYD3DXMATRIX* pM );

	// 행렬을 벡터에 반영하는 함수 ( D3DXVec3TranformNormal)
	MYD3DXVECTOR3* MYD3DXVec3TransformNormal(MYD3DXVECTOR3* pOut, CONST MYD3DXVECTOR3* pV, CONST MYD3DXMATRIX* pM );

	// 행렬 곱
	MYD3DXMATRIX* MYD3DXMatrixMultiply( MYD3DXMATRIX *pOut, CONST MYD3DXMATRIX *pM1, CONST MYD3DXMATRIX *pM2);

	//항등 행렬
	MYD3DXMATRIX* MYD3DXMatrixIdentity( MYD3DXMATRIX *pOut );

	// 전치 행렬
	MYD3DXMATRIX* MYD3DXMatrixTranspose( MYD3DXMATRIX *pOut, CONST MYD3DXMATRIX *pM);

	// 이동 행렬
	MYD3DXMATRIX* MYD3DXMatrixTranslation( MYD3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z);

	// 회전 행렬
	MYD3DXMATRIX* MYD3DXMatrixRotationX( MYD3DXMATRIX *pOut, FLOAT Angle );
	MYD3DXMATRIX* MYD3DXMatrixRotationY( MYD3DXMATRIX *pOut, FLOAT Angle );
	MYD3DXMATRIX* MYD3DXMatrixRotationZ( MYD3DXMATRIX *pOut, FLOAT Angle );

	// 축 회전
	MYD3DXMATRIX* MYD3DXMatrixRotationAxis( MYD3DXMATRIX* pOut, MYD3DXVECTOR3* vAxis, FLOAT fAngle);

	// 크기 변형 행렬
	MYD3DXMATRIX* MYD3DXMatrixScaling( MYD3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz);

	//역행렬
	//http://cafe.naver.com/dreamem/431 (역행렬 공식 주소)
	MYD3DXMATRIX* MYD3DXMatrixInverse( MYD3DXMATRIX *pOut, FLOAT* pDerterminant, CONST MYD3DXMATRIX* pM);

	// 카메라 좌표계로 변환
	MYD3DXMATRIX* MYD3DXMatrixLookAtLH( MYD3DXMATRIX *pOut, CONST MYD3DXVECTOR3 *pEye, CONST MYD3DXVECTOR3 *pAt, CONST MYD3DXVECTOR3 *pUp );

	// 직교 투영 행렬 얻기
	MYD3DXMATRIX* MYD3DXMatrixOrthoLH( MYD3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf ); 

	// 원근 투영 행렬 얻기
	MYD3DXMATRIX* MYD3DXMatrixPerspectiveFovLH(MYD3DXMATRIX* pOut, FLOAT fovy, FLOAT aspect, FLOAT zn, FLOAT zf);

}