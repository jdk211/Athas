//#include "ModelPixel_HalfLambert.fx"
#include "ModelPixel.fx"


#ifndef MATRIX_PALETTE_SIZE_DEFAULT
#define MATRIX_PALETTE_SIZE_DEFAULT 35
#endif



const int MATRIX_PALETTE_SIZE = MATRIX_PALETTE_SIZE_DEFAULT;	//최대 행렬 크기
float4x3 amPalette[MATRIX_PALETTE_SIZE_DEFAULT];				//최대 행렬 크기에 따른 행렬배열(여기에 적용되는 본의 행렬값들이 들어온다)


//스키닝 입력 구조체
struct VS_SKIN_INPUT
{
	float4      Position;
	float3      BlendWeights;
	int4        BlendIndices;
	float3		Normal;
	float3		Binormal;
	float3		Tangent;
};

//스키닝 출력 구조체
struct VS_SKIN_OUTPUT
{
	float4		Position;			//월드 변환 끝 정점
	float3		Normal;				//노말 변환 끝
	float3		Binormal;			//바이노말 변환 끝
	float3		Tangent;			//탄젠트 변환 끝
};




//스키닝 처리 함수 ( VS_SKIN_INPUT 받아 VS_SKIN_OUTPUT 리턴시킨다 )
VS_SKIN_OUTPUT VS_Skin(VS_SKIN_INPUT Input, int iNumBones)
{

	//VS_SKIN_OUTPUT Output 값 초기화.
	VS_SKIN_OUTPUT Output = (VS_SKIN_OUTPUT)0;		//ZeroMemory

	float fLastWeight = 1.0;
	float fWeight = 0.0f;
	float afBlendWeights[3] = (float[3])Input.BlendWeights;		//해당 정점에서 적용되는 본의 가중치
	int aiIndices[4] = (int[4])Input.BlendIndices;				//해당 정점에서 적용되는 본의 인덱스

	//본의 스키닝 처리.... ( 본에서 적용받는 갯수대로 ( 최대 3번만돈다 )
	for (int iBone = 0; (iBone < 3) && (iBone < iNumBones - 1); ++iBone)
	{
		fWeight = afBlendWeights[iBone];
		fLastWeight -= fWeight;		//적용되는 가중치 만큼 (가중치 뺀다 )
		Output.Position.xyz += mul(Input.Position, amPalette[aiIndices[iBone]]) * fWeight;

		Output.Normal += mul(Input.Normal, (float3x3)amPalette[aiIndices[iBone]]) * fWeight;
		Output.Binormal += mul(Input.Binormal, (float3x3)amPalette[aiIndices[iBone]]) * fWeight;
		Output.Tangent += mul(Input.Tangent, (float3x3)amPalette[aiIndices[iBone]]) * fWeight;
	}

	//여기까지 오면 마지막 하나 남 가중치 값은 fLastWeight  에 들어가있다.

	//마지막 하나 처리....
	Output.Position.xyz += mul(Input.Position, amPalette[aiIndices[iNumBones - 1]]) * fLastWeight;
	Output.Normal += mul(Input.Normal, (float3x3)amPalette[aiIndices[iNumBones - 1]]) * fLastWeight;
	Output.Binormal += mul(Input.Binormal, (float3x3)amPalette[aiIndices[iNumBones - 1]]) * fLastWeight;
	Output.Tangent += mul(Input.Tangent, (float3x3)amPalette[aiIndices[iNumBones - 1]]) * fLastWeight;

	return Output;
}




//------------------------------------------------------------------------------------
// SkinnedMesh Base 
//------------------------------------------------------------------------------------

//
// 셰이더 전역 변수
//
float4x4	matWorld;				//월드 행렬
float4x4	matViewProjection;		//뷰 * 프로젝션 행렬
float3		viewPos;				//시점의 위치


//
// 정점 입력 구조체 ( 그리는 물체가 가지고있는 정점정보 )
//
struct VS_INPUT
{
	float4  Position        : POSITION;			//정점위치
	float3  BlendWeights    : BLENDWEIGHT;		//정점가중치 (정보3개)
	float4  BlendIndices    : BLENDINDICES;		//적용되는 정점의 본인덱스 4개
	float3  Normal          : NORMAL;			//노말 벡터
	float3  Binormal        : BINORMAL;			//바이노말 벡터
	float3  Tangent         : TANGENT;			//탄젠트 
	float2  Texcoord        : TEXCOORD0;		//UV
};

//
// 정점 출력 구조체
//
struct VS_OUTPUT{
	float4 Position : POSITION;		//동차 좌표까지 변환이 완료된 정점의 위치
	float2 Texcoord : TEXCOORD0;	//이건 픽셀셰이더로 전달된다.
	float3 ViewDir : TEXCOORD1;		//이건 픽셀셰이더로 전달된다.
	float3 Tangent : TEXCOORD2;
	float3 Binormal : TEXCOORD3;
	float3 Normal : TEXCOORD4;
};

//
// 정점 셰이더 엔트리 함수
//
//매개변수로 들어오는 VS_INPUT 구조체는 정점의 정보를 가지고 있다.
//정점 셰이더 메인함수에서는 정점의 변환을 담당하게 된다.
//변환된 정점 정보를 지닌 VS_OUTPUT 을 리턴할 의무가 있다.
VS_OUTPUT vs_main(VS_INPUT Input, uniform int iNumBones )
{
	VS_OUTPUT Output;

	//스키닝 입력 구조체 준비
	VS_SKIN_INPUT vsi = { 
		Input.Position, 
		Input.BlendWeights, 
		Input.BlendIndices, 
		Input.Normal, 
		Input.Binormal, 
		Input.Tangent };

	//VS_Skin 함수에 스키닝 입력 구조체 를 넣고
	//적용되는 본의 수를 넣은후 결과값을 VS_SKIN_OUTPUT 으로 받는다.
	VS_SKIN_OUTPUT vso = VS_Skin(vsi, iNumBones);

	//vsfloat4 worldPos = float4( vso.Position.xyz, 1.0f );o 안의 Position 은 월드 변환이 끝난 정점의 위치이다.
	float4 worldPos = float4(vso.Position.xyz, 1.0f);

	//여기서 부터 원래 하던대로.... 단 노말과 바이노말 탄젠트도 월드 변환이 끝난 상태이다.

	//정점의 뷰 투영변환
	Output.Position = mul(worldPos, matViewProjection);


	//뷰 방향 넘긴다.
	Output.ViewDir = viewPos - worldPos.xyz;

	//Texture 
	Output.Texcoord = Input.Texcoord;

	//Tangent, Binormal, Normal 월드 변형하여 넘긴다.
	//Output.Tangent = mul(Input.Tangent, (float3x3)matWorld);
	//Output.Binormal = mul(Input.Binormal, (float3x3)matWorld);
	//Output.Normal = mul(Input.Normal, (float3x3)matWorld);

	//VS_Skin 함수에서 월드 변환이 된것이기 때문에 대입만하면 된다.
	Output.Normal = vso.Normal;
	Output.Binormal = vso.Binormal;
	Output.Tangent = vso.Tangent;


	return Output;

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int CurNumBones = 1;

VertexShader vsArray20[4] = { compile vs_3_0 vs_main(1),
							  compile vs_3_0 vs_main(2),
							  compile vs_3_0 vs_main(3),
							  compile vs_3_0 vs_main(4) };


//
// 테크닉 설정
//
// Pass 설정을 한다. 
technique ColorShader
{


	//Pass 설정
	//해당 Pass 의 정점 셰이더를 셰이더 얼마버전으로 돌릴건지 또한 엔트리함수는 어떻게 되는지 결정
	//그리고 픽셀 셰이더를 셰이더 얼마버전으로 돌릴건지 또한 엔트리함수는 어떻게 되는지 결정
	pass p0
	{
		//정점 셰이더 설정
		VertexShader = (vsArray20[CurNumBones]);		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.

		//픽셀 셰이더 설정
		PixelShader = compile ps_3_0 ps_main();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.

	}


};