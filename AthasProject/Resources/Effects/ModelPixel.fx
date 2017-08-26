

//Diffuse Texture
texture Diffuse_Tex;
sampler2D DiffuseSampler = sampler_state
{
	Texture = (Diffuse_Tex);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};


//Normal Texture
texture Normal_Tex;
sampler2D NormalSampler = sampler_state
{
	Texture = (Normal_Tex);
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
};


//Specular Texture
texture Specular_Tex;
sampler2D SpecularSampler = sampler_state
{
	Texture = (Specular_Tex);
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
};


//Emission Texture
texture Emission_Tex;
sampler2D EmissionSampler = sampler_state
{
	Texture = (Emission_Tex);
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
};

//Reflection Texture
texture Reflect_Tex;
sampler2D ReflectSampler = sampler_state
{
	Texture = (Reflect_Tex);
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
};


struct PS_INPUT{
	float2 Texcoord : TEXCOORD0;		//이건 픽셀셰이더로 전달된다.
	float3 ViewDir : TEXCOORD1;		//이건 픽셀셰이더로 전달된다.
	float3 Tangent : TEXCOORD2;
	float3 Binormal : TEXCOORD3;
	float3 Normal : TEXCOORD4;

};



//반사될 환경 큐브 맵
textureCUBE Environment_Tex;
samplerCUBE EnvironmentSampler = sampler_state
{
	Texture = (Environment_Tex);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};


//출력 컬러 설정
struct PS_OUTPUT{
	float4 stage0 : COLOR0;		//0 번스테이지 출력 컬러
	float4 stage1 : COLOR1;			//1 번스테이지 출력 컬러
	float4 stage2 : COLOR2;		//2 번스테이지 출력 컬러
};


//
// 픽셀 셰이더 엔트리 함수
//
// 필셀셰이더의 의무는 최종적으로 출력될 색상이 어떻게 출력되는지를 담당하여
// 출력될 색상을 리턴한다.
PS_OUTPUT ps_main(PS_INPUT Input) 
{

	PS_OUTPUT Output = (PS_OUTPUT)0;

	//노말 Texture 의 컬러를 가지고 온다.
	float3 norTexColor = tex2D(NormalSampler, Input.Texcoord).rgb;

	//탄젠트 Space 기준의 노말 0 ~ 1 사이의 컬러범위를 -1 에 1 사이로
	//셰이더 에서는 벡터에 스칼라 빼기를 하면 각 성분을 알아서 빼준다.
	float3 tanNormal = norTexColor * 2.0f - 1.0f;

	//위에서 얻은 Tangent Space 기준의 벡터를 월드 벡터로 변환.
	float3x3 TBN = float3x3(
	normalize(Input.Tangent),
	normalize(Input.Binormal),
	normalize(Input.Normal));
	
	//월드 노말
	float3 worldNormal = mul(tanNormal, TBN);

	float4 diffuseColor = tex2D(DiffuseSampler, Input.Texcoord);

	Output.stage0 = diffuseColor;
	Output.stage1 = float4(worldNormal, 1 );

	return Output;

}


PS_OUTPUT ps_tree(PS_INPUT Input)
{
	PS_OUTPUT  Output = (PS_OUTPUT)0;


	//노말 Texture 의 컬러를 가지고 온다.
	float3 norTexColor = tex2D(NormalSampler, Input.Texcoord).rgb;

	//탄젠트 Space 기준의 노말 0 ~ 1 사이의 컬러범위를 -1 에 1 사이로
	//셰이더 에서는 벡터에 스칼라 빼기를 하면 각 성분을 알아서 빼준다.
	float3 tanNormal = norTexColor * 2.0f - 1.0f;

	//위에서 얻은 Tangent Space 기준의 벡터를 월드 벡터로 변환.
	float3x3 TBN = float3x3(
	normalize(Input.Tangent),
	normalize(Input.Binormal),
	normalize(Input.Normal));

	//월드 노말
	float3 worldNormal = mul(tanNormal, TBN);


	float4 diffuseColor = tex2D(DiffuseSampler, Input.Texcoord);


	//clip( 스칼라값 )		// 입력되는 스칼라값이 음수면 픽섹자체를 안그린다.
	clip(diffuseColor.a - 0.5f);



	Output.stage0 = diffuseColor;
	Output.stage1 = float4(worldNormal, 1);

	return Output;
}
