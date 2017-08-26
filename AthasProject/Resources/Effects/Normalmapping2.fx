
//
// 셰이더 전역 변수
//
float4x4	matWorld;		//월드 행렬
float4x4	matView;		//뷰 행렬
float4x4	matProjection;	//투영 행렬

float3 lightDir;			//월드 광원 방향
float3 camPos;				//시점의 위치

//스펙큘라 파워
float specPower = 30;				//정반사의 날카로움

//빛의 색상
float3 lightColor = float3(1, 1, 1);

//Texture 변수 ( 외부 셋팅 )
texture DiffuseTex;

//Sampling 변수 ( Pixel Shader 에서 가져다 쓴다 )
sampler2D DiffuseSampler = sampler_state{
	Texture = DiffuseTex;		//이 sampler2D 는 MainTex 를 Sampling 한다.

	//FilterMode 를 설정
	MIPFILTER = ANISOTROPIC;
	MAGFILTER = ANISOTROPIC;
	MINFILTER = ANISOTROPIC;
	MAXANISOTROPY = 16;

};


//Texture 변수 ( 외부 셋팅 )
texture NormalTex;

//Sampling 변수 ( Pixel Shader 에서 가져다 쓴다 )
sampler2D NormalSampler = sampler_state{
	Texture = NormalTex;		//이 sampler2D 는 MainTex 를 Sampling 한다.

	//FilterMode 를 설정
	MIPFILTER = ANISOTROPIC;
	MAGFILTER = ANISOTROPIC;
	MINFILTER = ANISOTROPIC;
	MAXANISOTROPY = 16;

};



//
// 정점 입력 구조체 ( 그리는 물체가 가지고있는 정점정보 )
//
struct VS_INPUT{
	float4 Position : POSITION;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	float3 Normal : NORMAL;
	float2 Texcoord : TEXCOORD0;
};

//
// 정점 출력 구조체
//
struct VS_OUTPUT{
	float4 Position : POSITION;		//동차 좌표까지 변환이 완료된 정점의 위치
	float2 Texcoord : TEXCOORD0;		//이건 픽셀셰이더로 전달된다.
	float3 ViewDir : TEXCOORD1;		//이건 픽셀셰이더로 전달된다.
	float3 LightDir : TEXCOORD2;
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
	float4 worldPos = mul(Input.Position, matWorld);

	//정점의 뷰 변환
	Output.Position = mul(worldPos, matView);

	//투영 변환
	Output.Position = mul(Output.Position, matProjection);

	//뷰 방향 넘긴다.
	Output.ViewDir = camPos - worldPos.xyz;

	//Texture 
	Output.Texcoord = Input.Texcoord;

	//Tangent, Binormal, Normal 월드 변형하여 행렬을 준비한다.
	float3 tangent = mul(Input.Tangent, (float3x3)matWorld);
	float3 binormal = mul(Input.Binormal, (float3x3)matWorld);
	float3 normal = mul(Input.Normal, (float3x3)matWorld);

	//행렬 만든다.
	float3x3 TBN = float3x3(
		normalize(tangent),
		normalize(binormal),
		normalize(normal));

	//역행렬 ( 직교회전행렬의 역행렬은 전치 행렬과 같다 )
	float3x3 InvTBN = transpose(TBN);

	Output.LightDir = mul(lightDir, InvTBN);
	Output.ViewDir = mul( normalize(Output.ViewDir), InvTBN);



	return Output;

}


struct PS_INPUT{
	float2 Texcoord : TEXCOORD0;		//이건 픽셀셰이더로 전달된다.
	float3 ViewDir : TEXCOORD1;		//이건 픽셀셰이더로 전달된다.
	float3 LightDir : TEXCOORD2;
};

//
// 픽셀 셰이더 엔트리 함수
//
// 필셀셰이더의 의무는 최종적으로 출력될 색상이 어떻게 출력되는지를 담당하여
// 출력될 색상을 리턴한다.
float4 ps_main(PS_INPUT Input) : COLOR
{
	//노말 Texture 의 컬러를 가지고 온다.
	float3 norTexColor = tex2D(NormalSampler, Input.Texcoord).rgb;

	//탄젠트 Space 기준의 노말 0 ~ 1 사이의 컬러범위를 -1 에 1 사이로
	//셰이더 에서는 벡터에 스칼라 빼기를 하면 각 성분을 알아서 빼준다.
	float3 tanNormal = norTexColor * 2.0f - 1.0f;
	//tanNormal.x = norTexColor.x * 2.0f - 1.0f;
	//tanNormal.y = norTexColor.y * 2.0f - 1.0f;
	//tanNormal.z = norTexColor.z * 2.0f - 1.0f;

	//월드 노말 
	float3 worldNormal = tanNormal;


	//시야 방향 정규화
	float3 viewDir = normalize(Input.ViewDir);

	//라이트 정규화
	float3 lightInTanDir = normalize(Input.LightDir);

	//Texture 샘플링 컬러
	float4 diffColor = tex2D(DiffuseSampler, Input.Texcoord);

	//
	// Diffuse 확산광 계산
	//

	//광원의 근원지와 월드 노말과 내적
	float d = dot(worldNormal, -lightInTanDir);

	//0~1 사이의 범위로 짤라
	float diff = saturate(d);

	//발산되는 최종 확산광의 색상
	float3 diffuseColor = diffColor.rgb * lightColor * diff;

	//
	// Specular 정반사광 계산
	//

	//광원이 월드 노말에 정반사된 방향벡터
	float3 refl = lightInTanDir + (d * worldNormal * 2.0f);
	refl = normalize(refl);

	//정반사 량
	float spec = dot(refl, viewDir);
	spec = saturate(spec);

	//Power 적용
	spec = pow(spec, specPower);

	//발산되는 최종 정반사의 색상
	float3 specularColor = lightColor * spec * diff;		//정반사가 확산광의 영향을 받는경우

	//최종 색상은 더해
	float3 finalColor = diffuseColor + specularColor;

	return float4(finalColor, 1);
	

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