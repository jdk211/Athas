
//
// 셰이더 전역 변수
//
float4x4	matWorld;		//월드 행렬
float4x4	matView;		//뷰 행렬
float4x4	matProjection;	//투영 행렬

float3 lightDir;			//월드 광원 방향

float4 mainColor = float4(1, 1, 0, 1); // default 가 노란색




//
// 정점 입력 구조체 ( 그리는 물체가 가지고있는 정점정보 )
//
struct VS_INPUT{
	float4 Position : POSITION;		//포지션값은 무조건 float4 받는다.
	float3 Normal : NORMAL;
};

//
// 정점 출력 구조체
//
struct VS_OUTPUT{
	float4 Position : POSITION;		//동차 좌표까지 변환이 완료된 정점의 위치
	float4 Color : TEXCOORD0;		//이건 픽셀셰이더로 전달된다.
};


//
// 정점 셰이더 엔트리 함수
//
//매개변수로 들어오는 VS_INPUT 구조체는 정점의 정보를 가지고 있다.
//정점 셰이더 메인함수에서는 정점의 변환을 담당하게 된다.
//변환된 정점 정보를 지닌 VS_OUTPUT 을 리턴할 의무가 있다.
VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output;

	//정점의 월드 변환
	Output.Position = mul(Input.Position, matWorld);

	//정점의 뷰 변환
	Output.Position = mul(Output.Position, matView);

	//투영 변환
	Output.Position = mul(Output.Position, matProjection);

	//입력된 노말 값을 월드 단위로 변경하자.
	float3 worldNormal = mul(Input.Normal, (float3x3)matWorld);
	//스케일을 먹었을지도 모르니깐 정규화
	worldNormal = normalize( worldNormal );


	//램버트 광원 연산으로 광원량을 구하자.
	float diff = dot(worldNormal, -lightDir);
	if (diff < 0.0f)
		diff = 0.0f;



	//컬러를 노말 벡터값을 써보자.
	//Output.Color.x = Input.Normal.x;
	//Output.Color.y = Input.Normal.y;
	//Output.Color.z = Input.Normal.z;

	//벡터를 볼때 요소의 이름을 다음과 같이 쓸수있다.
	//x == r, y == g, b == z, w == a
	//Output.Color.r = worldNormal.r;
	//Output.Color.g = worldNormal.g;
	//Output.Color.b = worldNormal.b;
	//Output.Color.a = 1.0f;

	//Output.Color.rgb = worldNormal;
	//Output.Color.xyz = worldNormal;
	
	//Output.Color.r = diff;
	//Output.Color.g = diff;
	//Output.Color.b = diff;
	//Output.Color.rgb = diff.rrr;  //스칼라값을 동일하게 x 값으로 모두 vector3 요소로 보겠다.
	
	Output.Color.rgb = mainColor.rgb * diff;
	Output.Color.a = 1.0f;


	return Output;
}


//
// 픽셀 셰이더 엔트리 함수
//
// 필셀셰이더의 의무는 최종적으로 출력될 색상이 어떻게 출력되는지를 담당하여
// 출력될 색상을 리턴한다.
float4 ps_main(float4 Color : TEXCOORD0) : COLOR
{
	return Color;
}

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
		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.

		//픽셀 셰이더 설정
		PixelShader = compile ps_3_0 ps_main();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.

	}


};