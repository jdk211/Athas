

//Diffuse Texture 변수 ( 외부 셋팅 )
texture DiffuseTex;
sampler2D DiffuseSampler = sampler_state{
	Texture = DiffuseTex;		
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};

//Diffuse Light Texture 변수
texture DiffuseLightTex;
sampler2D DiffuseLightSampler = sampler_state{
	Texture = DiffuseLightTex;		
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};


//Specular Texture 변수 ( 외부 셋팅 )
texture SpecularTex;
sampler2D SpecularSampler = sampler_state{
	Texture = SpecularTex;
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};


//Specular Light Texture 변수
texture SpecularLightTex;
sampler2D SpecularLightSampler = sampler_state{
	Texture = SpecularLightTex;
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};





float onePixelX = 0.0f;
float onePixelY = 0.0f;
float halfPixelX = 0.0f;
float halfPixelY = 0.0f;

//
// 정점 입력 구조체 ( 그리는 물체가 가지고있는 정점정보 )
//
struct VS_INPUT{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
};

//
// 정점 출력 구조체
//
struct VS_OUTPUT{
	float4 Position : POSITION;		//동차 좌표까지 변환이 완료된 정점의 위치
	float2 Texcoord : TEXCOORD0;		//이건 픽셀셰이더로 전달된다.
};


VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output;

	//들어오는정점위치를 그대로 동차좌표로 쓴다.
	Output.Position = Input.Position;

	//텍스쳐 좌표 넘긴다.
	//Output.Texcoord = Input.Texcoord;
	Output.Texcoord.x = Input.Texcoord.x + halfPixelX;
	Output.Texcoord.y = Input.Texcoord.y + halfPixelY;

	return Output;
}



//
// 픽셀 셰이더 엔트리 함수
//
float4 ps_main(float2 Texcoord : TEXCOORD0) : COLOR
{
	//단순 표면 컬러...
	float3 diffuseColor = tex2D(DiffuseSampler, Texcoord).rgb;

	//표면에서 받는 Diffuse 광원량
	float3 diffuseLight = tex2D(DiffuseLightSampler, Texcoord).rgb;


	//표면 Specular 반사량
	float3 specularColor = tex2D(SpecularSampler, Texcoord).rgb;

	//Specular Color
	float3 specularLight = tex2D(SpecularLightSampler, Texcoord).rgb;


	//최종 컬러
	float3 finalColor = (diffuseColor * diffuseLight) + ( specularColor * specularLight );

	return float4(finalColor, 1);
}


//
// 테크닉 설정
//
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