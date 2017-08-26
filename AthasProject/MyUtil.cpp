#include "stdafx.h"
#include "MyUtil.h"
#include "cMainGame.h"


namespace MYUTIL{


	DWORD checkNum;

	//디바이스
	LPDIRECT3DDEVICE9		Device;
	//IDirect3DDevice9*		Device;
	
	DWORD maxAnisotropy;


	cMainGame* pMainGame;

	float maxPSize;


	//
	// Initialize Device
	//
	HRESULT InitDeivce()
	{

		//
		// Direct3D 초기화
		//

		//
		// 1. Device 를 생성할 IDirect3D9 객체를 얻는다.
		//

		LPDIRECT3D9 d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
		if (d3d9 == NULL) {
			//IDirect3D9 객체를 얻기에 실패했다면..
			LOG_MGR->AddLog("IDirect3D9 객체를 얻기에 실패");
			return E_FAIL;
		}

		//
		// 2. 하드웨어 ( 비디오카드 ) 정보를 가지고 와서 자신의 정점 프로세스 타입을 정하자
		//

		D3DCAPS9	caps;		//하드웨어 정보를 가지고 있다.
		

		d3d9->GetDeviceCaps(
			D3DADAPTER_DEFAULT,		//주 디스플레이 그래픽 카드 그냥 D3DADAPTER_DEFAULT
			D3DDEVTYPE_HAL,			//디바이스타입 설정 그냥 D3DDEVTYPE_HAL
			&caps					//디바이스 정보를 받아올 D3DCAPS9 포인터
			);

		//이방성 필터링 최대 레벨
		maxAnisotropy = caps.MaxAnisotropy;

		//정점 포인트 크기 최대 사이즈
		maxPSize = caps.MaxPointSize;


		//정점계산 처리방식을 지정할 플레그 값
		DWORD vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;

		//정점 처리와 정점 광원 처리를 하드웨어에서 지원하는가?
		if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else{
			vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			LOG_MGR->AddLog("야이새끼야 컴바꿔....");
		}


		//
		// 3. D3DPRESENT_PARAMETERS 구조체 정보를 생성
		// 내가 이러한 Device 를 만들겟다라는 정보
		//
		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8; //뎁스 버퍼와 스테실 버퍼 크기 뎁스 24bit 스텐실 버퍼 8 비트
		d3dpp.BackBufferCount = 1;					 //백버퍼는 한개
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;					//백버퍼 색상 포맷 알파8비트,레드8비트,그린8비트,블루8비트
		d3dpp.BackBufferHeight = WINSIZE_Y;							//백버퍼 픽셀 높이 크기
		d3dpp.BackBufferWidth = WINSIZE_X;							//백버퍼 픽셀 가로 크기
		d3dpp.EnableAutoDepthStencil = true;						//자동 깊이버퍼 사용 여부 ( 그냥 true )
		d3dpp.Flags = 0;											//일단 0
		d3dpp.hDeviceWindow = g_hWnd;								//Device 가 출력될 윈도우 핸들
		d3dpp.MultiSampleQuality = 0;								//멀티 샘플링 질 
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;				//멀티 샘플링 타입 ( D3DMULTISAMPLE_NONE 안티알리아싱 사용안함 ) 
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	//화면 주사율 ( 그냥 D3DPRESENT_RATE_DEFAULT 모니터 주사율과 동일시 )
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;		//화면 전송 간격 ( 그냥 D3DPRESENT_INTERVAL_ONE 모니터 주사율과 동일시 )
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					//화면 스왑 체인 방식 ( 그냥 D3DSWAPEFFECT_DISCARD )
		d3dpp.Windowed = true;

		//
		// 4. Device 생성
		//
		if (FAILED(d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,			//주 디스플레이 그래픽 카드 그냥 D3DADAPTER_DEFAULT
			D3DDEVTYPE_HAL,				//디바이스타입 설정 그냥 D3DDEVTYPE_HAL
			g_hWnd,						//디바이스를 사용할 윈도우 핸들
			vertexProcessing | D3DCREATE_MULTITHREADED,			//정점 처리 방식에 대한 플레그, D3DCREATE_MULTITHREADED 이플래그를 추가 해야 멀티 스레드 에 대응되는 Device 를 만들게 된다.
			&d3dpp,						//앞에서 정의한 D3DPRESENT_PARAMETERS 구조체 포인터
			&Device					//얻어올 디바이스 더블포인터
			)))
		{
			LOG_MGR->AddLog("Device 생성실패");
			return E_FAIL;
		}


		//Deivce 가 나타났으니 너는 필요없다
		d3d9->Release();

		return S_OK;
	}




	//
	// Extra Homework
	//

	float clamp(float value, float min, float max){
		if (max < min)
		{
			float temp = max;
			max = min;
			min = max;
		}


		if (value > max)
			return max;
		else if (value < min)
			return min;

		return value;
	}


	//min 값과 max 값을 t 의 진행도 만큼 보간값을 리턴...
	//t 는 0 과 1 사이의 실수값이 들어간다...
	//min = 5.0f;
	//max = 10.0f; 일때...
	//t = 0.5f;
	//return = 7.5f;
	float lerp(float min, float max, float t){
		float delta = max - min;
		return min + (delta * clamp(t, 0.0f, 1.0f));
	}


	//start 값과 end 값 사이에 value 가 어떻게 위치하는지 0 ~ 1 사이의 값으로...
	//start = 5.0f;
	//end = 10.0f; 일때...
	//value = 7.5f;
	//return = 0.5f;
	float inverseLerp(float start, float end, float value)
	{
		float result = abs( value - start ) / abs(end - start);
		
		return clamp(result, 0.0f, 1.0f);
	}




	//배열 보간...
	float arrayLerp(float* pArr, int arrSize, float t){

		//0 ~ 1 로 막는다.
		t = clamp(t, 0.0f, 1.0f);

		//간격
		float interval = 1.0f / (arrSize - 1);

		//포지션
		//int startPosition = static_cast<int>( t / interval );
		float position = t / interval;
		int startIndex = static_cast<int>(position);

		//보간량
		float t2 = position - static_cast<float>(startIndex);

		return lerp(pArr[startIndex], pArr[startIndex + 1], t2);
	}


	//t 는 0 ~ 1 사이의 값이 온다.
	//이것에 대한 값위치 범위를 -1 에서 1 로 고쳐라...
	//0 => -1
	//0.5 => 0
	//1 => 1
	float ChangeRangeMinusOneToOne(float t){
		return (t * 2.0f) - 1.0f;
	}

	//t 는 -1 ~ 1 사이의 값이 온다.
	//이것에 대한 값위치 범위를 0 에서 1 로 고쳐라...
	//-1 => 0
	//0 => 0.5
	//1 => 1
	float ChangeRangeZeroToOne(float t){

		return (t + 1.0f) * 0.5f;
	}




	POINT GetMousePos()
	{
		POINT ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse); 
		return ptMouse;
	}

	//랜덤을 얻는다.
	float GetRandom(float min, float max)
	{
		//0 ~ 32767
		float randVal = static_cast<float>(rand()) / 32767.0f;

		//randVal 0 ~ 1.0f;

		return (max - min) * randVal + min;


	}

	//초당 timePerMove 값만큼 nowValue 가 targetValue 를 따라간다.
	float GetTargetValue(
		float targetValue,		//타겟 값
		float nowValue,			//현제 값
		float timeDelta,		//타임 델타
		float timePerMove		//초당 변위량
		)
	{
		//현제 값과 타겟값의 절대적이 차이량
		float dist = abs(targetValue - nowValue);

		//거의 차이가 없다면 넘겨
		if (FLOAT_EQUAL(dist, 0.0f))
			return targetValue;


		//현제 변위 가능량
		float moveDelta = timePerMove * timeDelta;

		float t = moveDelta / dist;

		//타겟벨류까지 충분히 값이 변할수 있다.
		if (t >= 1.0f)
		{
			return targetValue;
		}

		else
		{
			return lerp(nowValue, targetValue, t);
		}
		





	}


	//float 값을 비트 손실없이 DWORD 형으로 형변환
	DWORD FloatToDWORD(float f)
	{
		DWORD* pdw = (DWORD*)&f;
		return (*pdw);
	}



	//정점의 노말을 계산해주는 함수
	void ComputeNormal(
		D3DXVECTOR3* pNorams,			//결과로 받을 노말백터 배열 포인터 ( 노말벡터의 배열의 수는 정점 배열의 수와 같다 )
		const D3DXVECTOR3* pVertices,	//계산에 사용될 정점위치 배열 포인터
		int verticesNum,				//정점 갯수
		const DWORD* pIndices,			//인덱스 배열포인터 ( 일단 인덱스는 DWORD 로 대동단결 )
		int indicesNum					//인덱스 배열포인터 갯수
		)
	{
		//노말 결과 털고 시작
		ZeroMemory(pNorams, sizeof(D3DXVECTOR3) * verticesNum);

		//삼각형 갯수
		DWORD triNum = indicesNum / 3;

		for (DWORD i = 0; i < triNum; i++)
		{
			//해당 삼각형의 정점 인덱스
			DWORD i0 = pIndices[(i * 3) + 0];
			DWORD i1 = pIndices[(i * 3) + 1];
			DWORD i2 = pIndices[(i * 3) + 2];

			//정점 3개
			D3DXVECTOR3 v0 = pVertices[i0];
			D3DXVECTOR3 v1 = pVertices[i1];
			D3DXVECTOR3 v2 = pVertices[i2];

			//Edge
			D3DXVECTOR3 edge1 = v1 - v0;
			D3DXVECTOR3 edge2 = v2 - v0;

			//Cross
			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross, &edge1, &edge2);

			//Normal
			D3DXVECTOR3 normal;
			D3DXVec3Normalize(&normal, &cross);


			//증가 시킨다.
			pNorams[i0] += normal;
			pNorams[i1] += normal;
			pNorams[i2] += normal;
		}


		//최종 적으로 Normalvector 정규화한다.
		for (DWORD i = 0; i < verticesNum; i++)
		{
			D3DXVec3Normalize(&pNorams[i], &pNorams[i]);
		}

	}


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
		)
	{
		//탄젠트 바이노말 턴다
		ZeroMemory(outTangets, sizeof(D3DXVECTOR3)* NumVertices);
		ZeroMemory(outBinormals, sizeof(D3DXVECTOR3)* NumVertices);

		//임시 Tangent Binormal 배열 ( 직교화 되지않는 탄젠트와 바이노말이 저장된다 )
		D3DXVECTOR3* pTangents = new D3DXVECTOR3[NumVertices];
		D3DXVECTOR3* pBinormals = new D3DXVECTOR3[NumVertices];

		//임시 Tangent Binormal 배열 턴다
		ZeroMemory(pTangents, sizeof(D3DXVECTOR3)* NumVertices);
		ZeroMemory(pBinormals, sizeof(D3DXVECTOR3)* NumVertices);

		//일단 삼각형 수대로....
		for (DWORD a = 0; a < NumTris; a++)
		{

			//해당 삼각형의 인덱스
			DWORD i0 = pIndices[a * 3 + 0];
			DWORD i1 = pIndices[a * 3 + 1];
			DWORD i2 = pIndices[a * 3 + 2];

			//해당 삼각형의 정점위치
			D3DXVECTOR3 p0 = pPositions[i0];
			D3DXVECTOR3 p1 = pPositions[i1];
			D3DXVECTOR3 p2 = pPositions[i2];

			//해당 삼각형의 UV
			D3DXVECTOR2 uv0 = pUVs[i0];
			D3DXVECTOR2 uv1 = pUVs[i1];
			D3DXVECTOR2 uv2 = pUVs[i2];

			//각변의 Edge
			D3DXVECTOR3 edge1 = p1 - p0;
			D3DXVECTOR3 edge2 = p2 - p0;

			//UV Edge
			D3DXVECTOR2 uvEdge1 = uv1 - uv0;
			D3DXVECTOR2 uvEdge2 = uv2 - uv0;

			// 위의 정보로 다음과 같은 공식이 성립
			// edge1 = ( uvEdge1.x ) * Tangent + ( uvEdge1.y ) * Binormal;
			// edge2 = ( uvEdge2.x ) * Tangent + ( uvEdge2.y ) * Binormal;

			// 다음과 같이 치환
			// E1 = edge1;
			// E2 = edge2;
			// U1 = uvEdge1.x;
			// V1 = uvEdge1.y;
			// U2 = uvEdge2.x;
			// V2 = uvEdge2.y;
			// T = Tangent;
			// B = Binormal;

			// E1 = U1 * T + V1 * B;
			// E2 = U2 * T + V2 * B;

			// | E1 |   | U1 , V1 | | T |
			// |    | = |         | |   |
			// | E2 |   | U2 , V2 | | B |

			// | U1 , V1 |				       1        |  V2 , -V1 |
			// |         | => 역행렬 =>  --------------- |           |
			// | U2 , V2 |				 U1*V2 - V1*U2  | -U2 ,  U1 |

			//          1        |  V2 , -V1 |   | E1 |    | T |
			//   --------------- |           | * |    | =  |   |
			//	  U1*V2 - V1*U2  | -U2 ,  U1 |   | E2 |    | B |

			// | T |          1        |  V2 , -V1 | | E1 |
			// |   | = --------------- |           | |    |
			// | B |    U1*V2 - V1*U2  | -U2 ,  U1 | | E2 |


			// R = 1 / U1*V2 - V1*U2;
			// T = ( ( E1 * V2 ) + ( E2 * -V1 ) ) * R
			// B = ( ( E1 * -U2 ) + ( E2 * U1 ) ) * R


			float r =
				1.0f / ((uvEdge1.x * uvEdge2.y) - (uvEdge1.y * uvEdge2.x));


			//Tangent ( 어차피 정규화 하기 때문에 r 곱셈은 생략가능 )
			D3DXVECTOR3 t = ((edge1 * uvEdge2.y) + (edge2 * -uvEdge1.y)) * r;
			D3DXVec3Normalize(&t, &t);

			//Binormal ( 어차피 정규화 하기 때문에 r 곱셈은 생략가능 )
			D3DXVECTOR3 b = ((edge1 * -uvEdge2.x) + (edge2 * uvEdge1.x)) * r;
			D3DXVec3Normalize(&b, &b);


			//탄젠트 바이노말 임시 배열에 추가
			pTangents[i0] += t;
			pTangents[i1] += t;
			pTangents[i2] += t;
			pBinormals[i0] += b;
			pBinormals[i1] += b;
			pBinormals[i2] += b;


		}


		//Binormal Tangent 노말에 직교화
		for (int i = 0; i < NumVertices; i++){

			D3DXVECTOR3 n = pNormals[i];

			//Tangent 그람슈미트 직교
			D3DXVECTOR3 t = pTangents[i] -
				(D3DXVec3Dot(&pTangents[i], &n) * n);
			D3DXVec3Normalize(&t, &t);

			//노말과 직교화된 T 와 외적하여 B
			D3DXVECTOR3 b;
			D3DXVec3Cross(&b, &n, &t);
			D3DXVec3Normalize(&b, &b);

			outTangets[i] = t;
			outBinormals[i] = b;
		}

		//임시 배열 삭제
		SAFE_DELETE_ARR(pTangents);
		SAFE_DELETE_ARR(pBinormals);



	}


	float Vector3Distance(D3DXVECTOR3* p0, D3DXVECTOR3* p1)
	{
		D3DXVECTOR3 dir = *p1 - *p0;

		return D3DXVec3Length(&dir);

	}
	float Vector3DistanceSq(D3DXVECTOR3* p0, D3DXVECTOR3* p1)
	{

		D3DXVECTOR3 dir = *p1 - *p0;

		return D3DXVec3LengthSq(&dir);
	}

	float GetDegree(D3DXVECTOR3 fromPosition, D3DXVECTOR3 toPosition)
	{
		return atan2(fromPosition.z - toPosition.z, fromPosition.x - toPosition.x) * 180 / D3DX_PI;
	}

}