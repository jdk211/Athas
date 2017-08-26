#pragma once
class cMainGame;

namespace MYUTIL{

	extern DWORD checkNum;

	//����̽�
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


	//min ���� max ���� t �� ���൵ ��ŭ �������� ����...
	//t �� 0 �� 1 ������ �Ǽ����� ����...
	//min = 5.0f;
	//max = 10.0f; �϶�...
	//t = 0.5f;
	//return = 7.5f;
	float lerp(float min, float max, float t);

	//start ���� end �� ���̿� value �� ��� ��ġ�ϴ��� 0 ~ 1 ������ ������...
	//start = 5.0f;
	//end = 10.0f; �϶�...
	//value = 7.5f;
	//return = 0.5f;
	float inverseLerp(float start, float end, float value);


	//�迭 ����...
	float arrayLerp(float* pArr, int arrSize, float t);


	//t �� 0 ~ 1 ������ ���� �´�.
	//�̰Ϳ� ���� ����ġ ������ -1 ���� 1 �� ���Ķ�...
	//0 => -1
	//0.5 => 0
	//1 => 1
	float ChangeRangeMinusOneToOne(float t);

	//t �� -1 ~ 1 ������ ���� �´�.
	//�̰Ϳ� ���� ����ġ ������ 0 ���� 1 �� ���Ķ�...
	//-1 => 0
	//0 => 0.5
	//1 => 1
	float ChangeRangeZeroToOne(float t);


	//���콺 ��ġ�� ��´�.
	POINT GetMousePos();

	//������ ��´�.
	float GetRandom(float min, float max);

	//�ʴ� timePerMove ����ŭ nowValue �� targetValue �� ���󰣴�.
	float GetTargetValue(
		float targetValue,		//Ÿ�� ��
		float nowValue,			//���� ��
		float timeDelta,		//Ÿ�� ��Ÿ
		float timePerMove		//�ʴ� ������
		);

	//float ���� ��Ʈ �սǾ��� DWORD ������ ����ȯ
	DWORD FloatToDWORD(float f);



	//������ �븻�� ������ִ� �Լ�
	void ComputeNormal(
		D3DXVECTOR3* pNorams,			//����� ���� �븻���� �迭 ������ ( �븻������ �迭�� ���� ���� �迭�� ���� ���� )
		const D3DXVECTOR3* pVertices,	//��꿡 ���� ������ġ �迭 ������
		int verticesNum,				//���� ����
		const DWORD* pIndices,			//�ε��� �迭������ ( �ϴ� �ε����� DWORD �� �뵿�ܰ� )
		int indicesNum					//�ε��� �迭������ ����
		);



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
		);

	float Vector3Distance(D3DXVECTOR3* p0, D3DXVECTOR3* p1);
	float Vector3DistanceSq(D3DXVECTOR3* p0, D3DXVECTOR3* p1);
	float GetDegree(D3DXVECTOR3 fromPosition, D3DXVECTOR3 toPosition);


}