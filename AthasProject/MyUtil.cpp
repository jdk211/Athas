#include "stdafx.h"
#include "MyUtil.h"
#include "cMainGame.h"


namespace MYUTIL{


	DWORD checkNum;

	//����̽�
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
		// Direct3D �ʱ�ȭ
		//

		//
		// 1. Device �� ������ IDirect3D9 ��ü�� ��´�.
		//

		LPDIRECT3D9 d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
		if (d3d9 == NULL) {
			//IDirect3D9 ��ü�� ��⿡ �����ߴٸ�..
			LOG_MGR->AddLog("IDirect3D9 ��ü�� ��⿡ ����");
			return E_FAIL;
		}

		//
		// 2. �ϵ���� ( ����ī�� ) ������ ������ �ͼ� �ڽ��� ���� ���μ��� Ÿ���� ������
		//

		D3DCAPS9	caps;		//�ϵ���� ������ ������ �ִ�.
		

		d3d9->GetDeviceCaps(
			D3DADAPTER_DEFAULT,		//�� ���÷��� �׷��� ī�� �׳� D3DADAPTER_DEFAULT
			D3DDEVTYPE_HAL,			//����̽�Ÿ�� ���� �׳� D3DDEVTYPE_HAL
			&caps					//����̽� ������ �޾ƿ� D3DCAPS9 ������
			);

		//�̹漺 ���͸� �ִ� ����
		maxAnisotropy = caps.MaxAnisotropy;

		//���� ����Ʈ ũ�� �ִ� ������
		maxPSize = caps.MaxPointSize;


		//������� ó������� ������ �÷��� ��
		DWORD vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;

		//���� ó���� ���� ���� ó���� �ϵ����� �����ϴ°�?
		if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else{
			vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			LOG_MGR->AddLog("���̻����� �Ĺٲ�....");
		}


		//
		// 3. D3DPRESENT_PARAMETERS ����ü ������ ����
		// ���� �̷��� Device �� ����ٴٶ�� ����
		//
		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8; //���� ���ۿ� ���׽� ���� ũ�� ���� 24bit ���ٽ� ���� 8 ��Ʈ
		d3dpp.BackBufferCount = 1;					 //����۴� �Ѱ�
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;					//����� ���� ���� ����8��Ʈ,����8��Ʈ,�׸�8��Ʈ,���8��Ʈ
		d3dpp.BackBufferHeight = WINSIZE_Y;							//����� �ȼ� ���� ũ��
		d3dpp.BackBufferWidth = WINSIZE_X;							//����� �ȼ� ���� ũ��
		d3dpp.EnableAutoDepthStencil = true;						//�ڵ� ���̹��� ��� ���� ( �׳� true )
		d3dpp.Flags = 0;											//�ϴ� 0
		d3dpp.hDeviceWindow = g_hWnd;								//Device �� ��µ� ������ �ڵ�
		d3dpp.MultiSampleQuality = 0;								//��Ƽ ���ø� �� 
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;				//��Ƽ ���ø� Ÿ�� ( D3DMULTISAMPLE_NONE ��Ƽ�˸��ƽ� ������ ) 
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	//ȭ�� �ֻ��� ( �׳� D3DPRESENT_RATE_DEFAULT ����� �ֻ����� ���Ͻ� )
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;		//ȭ�� ���� ���� ( �׳� D3DPRESENT_INTERVAL_ONE ����� �ֻ����� ���Ͻ� )
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					//ȭ�� ���� ü�� ��� ( �׳� D3DSWAPEFFECT_DISCARD )
		d3dpp.Windowed = true;

		//
		// 4. Device ����
		//
		if (FAILED(d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,			//�� ���÷��� �׷��� ī�� �׳� D3DADAPTER_DEFAULT
			D3DDEVTYPE_HAL,				//����̽�Ÿ�� ���� �׳� D3DDEVTYPE_HAL
			g_hWnd,						//����̽��� ����� ������ �ڵ�
			vertexProcessing | D3DCREATE_MULTITHREADED,			//���� ó�� ��Ŀ� ���� �÷���, D3DCREATE_MULTITHREADED ���÷��׸� �߰� �ؾ� ��Ƽ ������ �� �����Ǵ� Device �� ����� �ȴ�.
			&d3dpp,						//�տ��� ������ D3DPRESENT_PARAMETERS ����ü ������
			&Device					//���� ����̽� ����������
			)))
		{
			LOG_MGR->AddLog("Device ��������");
			return E_FAIL;
		}


		//Deivce �� ��Ÿ������ �ʴ� �ʿ����
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


	//min ���� max ���� t �� ���൵ ��ŭ �������� ����...
	//t �� 0 �� 1 ������ �Ǽ����� ����...
	//min = 5.0f;
	//max = 10.0f; �϶�...
	//t = 0.5f;
	//return = 7.5f;
	float lerp(float min, float max, float t){
		float delta = max - min;
		return min + (delta * clamp(t, 0.0f, 1.0f));
	}


	//start ���� end �� ���̿� value �� ��� ��ġ�ϴ��� 0 ~ 1 ������ ������...
	//start = 5.0f;
	//end = 10.0f; �϶�...
	//value = 7.5f;
	//return = 0.5f;
	float inverseLerp(float start, float end, float value)
	{
		float result = abs( value - start ) / abs(end - start);
		
		return clamp(result, 0.0f, 1.0f);
	}




	//�迭 ����...
	float arrayLerp(float* pArr, int arrSize, float t){

		//0 ~ 1 �� ���´�.
		t = clamp(t, 0.0f, 1.0f);

		//����
		float interval = 1.0f / (arrSize - 1);

		//������
		//int startPosition = static_cast<int>( t / interval );
		float position = t / interval;
		int startIndex = static_cast<int>(position);

		//������
		float t2 = position - static_cast<float>(startIndex);

		return lerp(pArr[startIndex], pArr[startIndex + 1], t2);
	}


	//t �� 0 ~ 1 ������ ���� �´�.
	//�̰Ϳ� ���� ����ġ ������ -1 ���� 1 �� ���Ķ�...
	//0 => -1
	//0.5 => 0
	//1 => 1
	float ChangeRangeMinusOneToOne(float t){
		return (t * 2.0f) - 1.0f;
	}

	//t �� -1 ~ 1 ������ ���� �´�.
	//�̰Ϳ� ���� ����ġ ������ 0 ���� 1 �� ���Ķ�...
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

	//������ ��´�.
	float GetRandom(float min, float max)
	{
		//0 ~ 32767
		float randVal = static_cast<float>(rand()) / 32767.0f;

		//randVal 0 ~ 1.0f;

		return (max - min) * randVal + min;


	}

	//�ʴ� timePerMove ����ŭ nowValue �� targetValue �� ���󰣴�.
	float GetTargetValue(
		float targetValue,		//Ÿ�� ��
		float nowValue,			//���� ��
		float timeDelta,		//Ÿ�� ��Ÿ
		float timePerMove		//�ʴ� ������
		)
	{
		//���� ���� Ÿ�ٰ��� �������� ���̷�
		float dist = abs(targetValue - nowValue);

		//���� ���̰� ���ٸ� �Ѱ�
		if (FLOAT_EQUAL(dist, 0.0f))
			return targetValue;


		//���� ���� ���ɷ�
		float moveDelta = timePerMove * timeDelta;

		float t = moveDelta / dist;

		//Ÿ�ٺ������� ����� ���� ���Ҽ� �ִ�.
		if (t >= 1.0f)
		{
			return targetValue;
		}

		else
		{
			return lerp(nowValue, targetValue, t);
		}
		





	}


	//float ���� ��Ʈ �սǾ��� DWORD ������ ����ȯ
	DWORD FloatToDWORD(float f)
	{
		DWORD* pdw = (DWORD*)&f;
		return (*pdw);
	}



	//������ �븻�� ������ִ� �Լ�
	void ComputeNormal(
		D3DXVECTOR3* pNorams,			//����� ���� �븻���� �迭 ������ ( �븻������ �迭�� ���� ���� �迭�� ���� ���� )
		const D3DXVECTOR3* pVertices,	//��꿡 ���� ������ġ �迭 ������
		int verticesNum,				//���� ����
		const DWORD* pIndices,			//�ε��� �迭������ ( �ϴ� �ε����� DWORD �� �뵿�ܰ� )
		int indicesNum					//�ε��� �迭������ ����
		)
	{
		//�븻 ��� �а� ����
		ZeroMemory(pNorams, sizeof(D3DXVECTOR3) * verticesNum);

		//�ﰢ�� ����
		DWORD triNum = indicesNum / 3;

		for (DWORD i = 0; i < triNum; i++)
		{
			//�ش� �ﰢ���� ���� �ε���
			DWORD i0 = pIndices[(i * 3) + 0];
			DWORD i1 = pIndices[(i * 3) + 1];
			DWORD i2 = pIndices[(i * 3) + 2];

			//���� 3��
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


			//���� ��Ų��.
			pNorams[i0] += normal;
			pNorams[i1] += normal;
			pNorams[i2] += normal;
		}


		//���� ������ Normalvector ����ȭ�Ѵ�.
		for (DWORD i = 0; i < verticesNum; i++)
		{
			D3DXVec3Normalize(&pNorams[i], &pNorams[i]);
		}

	}


	//ź��Ʈ�� ���̳븻 ���
	void ComputeTangentAngBinormal(
		D3DXVECTOR3* outTangets,			//��� Tangent �迭
		D3DXVECTOR3* outBinormals,			//��� Binormal �迭
		const D3DXVECTOR3* pPositions,		//���� ��ġ �迭
		const D3DXVECTOR3* pNormals,		//���� �븻 �迭
		const D3DXVECTOR2* pUVs,			//������ UV �迭
		const DWORD* pIndices,				//�ε��� �迭
		DWORD NumTris,						//�ﰢ�� ����		
		DWORD NumVertices 					//���� ����
		)
	{
		//ź��Ʈ ���̳븻 �ϴ�
		ZeroMemory(outTangets, sizeof(D3DXVECTOR3)* NumVertices);
		ZeroMemory(outBinormals, sizeof(D3DXVECTOR3)* NumVertices);

		//�ӽ� Tangent Binormal �迭 ( ����ȭ �����ʴ� ź��Ʈ�� ���̳븻�� ����ȴ� )
		D3DXVECTOR3* pTangents = new D3DXVECTOR3[NumVertices];
		D3DXVECTOR3* pBinormals = new D3DXVECTOR3[NumVertices];

		//�ӽ� Tangent Binormal �迭 �ϴ�
		ZeroMemory(pTangents, sizeof(D3DXVECTOR3)* NumVertices);
		ZeroMemory(pBinormals, sizeof(D3DXVECTOR3)* NumVertices);

		//�ϴ� �ﰢ�� �����....
		for (DWORD a = 0; a < NumTris; a++)
		{

			//�ش� �ﰢ���� �ε���
			DWORD i0 = pIndices[a * 3 + 0];
			DWORD i1 = pIndices[a * 3 + 1];
			DWORD i2 = pIndices[a * 3 + 2];

			//�ش� �ﰢ���� ������ġ
			D3DXVECTOR3 p0 = pPositions[i0];
			D3DXVECTOR3 p1 = pPositions[i1];
			D3DXVECTOR3 p2 = pPositions[i2];

			//�ش� �ﰢ���� UV
			D3DXVECTOR2 uv0 = pUVs[i0];
			D3DXVECTOR2 uv1 = pUVs[i1];
			D3DXVECTOR2 uv2 = pUVs[i2];

			//������ Edge
			D3DXVECTOR3 edge1 = p1 - p0;
			D3DXVECTOR3 edge2 = p2 - p0;

			//UV Edge
			D3DXVECTOR2 uvEdge1 = uv1 - uv0;
			D3DXVECTOR2 uvEdge2 = uv2 - uv0;

			// ���� ������ ������ ���� ������ ����
			// edge1 = ( uvEdge1.x ) * Tangent + ( uvEdge1.y ) * Binormal;
			// edge2 = ( uvEdge2.x ) * Tangent + ( uvEdge2.y ) * Binormal;

			// ������ ���� ġȯ
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
			// |         | => ����� =>  --------------- |           |
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


			//Tangent ( ������ ����ȭ �ϱ� ������ r ������ �������� )
			D3DXVECTOR3 t = ((edge1 * uvEdge2.y) + (edge2 * -uvEdge1.y)) * r;
			D3DXVec3Normalize(&t, &t);

			//Binormal ( ������ ����ȭ �ϱ� ������ r ������ �������� )
			D3DXVECTOR3 b = ((edge1 * -uvEdge2.x) + (edge2 * uvEdge1.x)) * r;
			D3DXVec3Normalize(&b, &b);


			//ź��Ʈ ���̳븻 �ӽ� �迭�� �߰�
			pTangents[i0] += t;
			pTangents[i1] += t;
			pTangents[i2] += t;
			pBinormals[i0] += b;
			pBinormals[i1] += b;
			pBinormals[i2] += b;


		}


		//Binormal Tangent �븻�� ����ȭ
		for (int i = 0; i < NumVertices; i++){

			D3DXVECTOR3 n = pNormals[i];

			//Tangent �׶�����Ʈ ����
			D3DXVECTOR3 t = pTangents[i] -
				(D3DXVec3Dot(&pTangents[i], &n) * n);
			D3DXVec3Normalize(&t, &t);

			//�븻�� ����ȭ�� T �� �����Ͽ� B
			D3DXVECTOR3 b;
			D3DXVec3Cross(&b, &n, &t);
			D3DXVec3Normalize(&b, &b);

			outTangets[i] = t;
			outBinormals[i] = b;
		}

		//�ӽ� �迭 ����
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