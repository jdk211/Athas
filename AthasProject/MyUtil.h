#pragma once
class cMainGame;

namespace MYUTIL{

	extern DWORD checkNum;

	//디바이스
	extern LPDIRECT3DDEVICE9 Device;
	extern DWORD maxAnisotropy;

	extern cMainGame* pMainGame;

	extern float maxPSize;

	//
	// Initialize Device
	//
	HRESULT InitDeivce();




	//
	// Extra Homework
	//
	float clamp(float value, float min, float max);


	//min 값과 max 값을 t 의 진행도 만큼 보간값을 리턴...
	//t 는 0 과 1 사이의 실수값이 들어간다...
	//min = 5.0f;
	//max = 10.0f; 일때...
	//t = 0.5f;
	//return = 7.5f;
	float lerp(float min, float max, float t);

	//start 값과 end 값 사이에 value 가 어떻게 위치하는지 0 ~ 1 사이의 값으로...
	//start = 5.0f;
	//end = 10.0f; 일때...
	//value = 7.5f;
	//return = 0.5f;
	float inverseLerp(float start, float end, float value);


	//배열 보간...
	float arrayLerp(float* pArr, int arrSize, float t);


	//t 는 0 ~ 1 사이의 값이 온다.
	//이것에 대한 값위치 범위를 -1 에서 1 로 고쳐라...
	//0 => -1
	//0.5 => 0
	//1 => 1
	float ChangeRangeMinusOneToOne(float t);

	//t 는 -1 ~ 1 사이의 값이 온다.
	//이것에 대한 값위치 범위를 0 에서 1 로 고쳐라...
	//-1 => 0
	//0 => 0.5
	//1 => 1
	float ChangeRangeZeroToOne(float t);


	//마우스 위치를 얻는다.
	POINT GetMousePos();

	//랜덤을 얻는다.
	float GetRandom(float min, float max);

	//초당 timePerMove 값만큼 nowValue 가 targetValue 를 따라간다.
	float GetTargetValue(
		float targetValue,		//타겟 값
		float nowValue,			//현제 값
		float timeDelta,		//타임 델타
		float timePerMove		//초당 변위량
		);

	//float 값을 비트 손실없이 DWORD 형으로 형변환
	DWORD FloatToDWORD(float f);



	//정점의 노말을 계산해주는 함수
	void ComputeNormal(
		D3DXVECTOR3* pNorams,			//결과로 받을 노말백터 배열 포인터 ( 노말벡터의 배열의 수는 정점 배열의 수와 같다 )
		const D3DXVECTOR3* pVertices,	//계산에 사용될 정점위치 배열 포인터
		int verticesNum,				//정점 갯수
		const DWORD* pIndices,			//인덱스 배열포인터 ( 일단 인덱스는 DWORD 로 대동단결 )
		int indicesNum					//인덱스 배열포인터 갯수
		);



	//탄젠트와 바이노말 계산
	void ComputeTangentAngBinormal(
		D3DXVECTOR3* outTangets,			//결과 Tangent 배열
		D3DXVECTOR3* outBinormals,			//결과 Binormal 배열
		const D3DXVECTOR3* pPositions,		//정점 위치 배열
		const D3DXVECTOR3* pNormals,		//정점 노말 배열
		const D3DXVECTOR2* pUVs,			//정점의 UV 배열
		const DWORD* pIndices,				//인덱스 배열
		DWORD NumTris,						//삼각형 갯수		
		DWORD NumVertices 					//정점 갯수
		);

	float Vector3Distance(D3DXVECTOR3* p0, D3DXVECTOR3* p1);
	float Vector3DistanceSq(D3DXVECTOR3* p0, D3DXVECTOR3* p1);
	float GetDegree(D3DXVECTOR3 fromPosition, D3DXVECTOR3 toPosition);


}