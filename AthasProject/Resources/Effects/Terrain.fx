

//
// 셰이더 전역 변수
//
float4x4	matViewProjection;	//뷰투영 행렬

//월드 광원 방향
float3 lightDir = float3( 0, -1, 0);			

//빛의 색상
float3 lightColor = float3(1, 1, 1);

//스펙큘라 파워
float specPower = 30;				//정반사의 날카로움

//Ambient
float ambient = 0.1f;				//환경광 량



texture Splat_Tex;
sampler2D Splat_Sampler = sampler_state
{
	Texture = (Splat_Tex);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};



//Diffuse Texture
texture Tile_Diffuse_0;
sampler2D Tile_Sampler_0 = sampler_state{
	Texture = (Tile_Diffuse_0);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture Tile_Diffuse_1;
sampler2D Tile_Sampler_1 = sampler_state
{
	Texture = (Tile_Diffuse_1);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};


texture Tile_Diffuse_2;
sampler2D Tile_Sampler_2 = sampler_state
{
	Texture = (Tile_Diffuse_2);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture Tile_Diffuse_3;
sampler2D Tile_Sampler_3 = sampler_state
{
	Texture = (Tile_Diffuse_3);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};


//Normal Texture
texture Tile_Normal_0;
sampler2D Tile_NormalSampler_0 = sampler_state{
	Texture = (Tile_Normal_0);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture Tile_Normal_1;
sampler2D Tile_NormalSampler_1 = sampler_state{
	Texture = (Tile_Normal_1);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture Tile_Normal_2;
sampler2D Tile_NormalSampler_2 = sampler_state{
	Texture = (Tile_Normal_2);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture Tile_Normal_3;
sampler2D Tile_NormalSampler_3 = sampler_state{
	Texture = (Tile_Normal_3);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};


//Secular Texture
texture Tile_Specular_0;
sampler2D Tile_SpecularSampler_0 = sampler_state{
	Texture = (Tile_Specular_0);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};

texture Tile_Specular_1;
sampler2D Tile_SpecularSampler_1 = sampler_state{
	Texture = (Tile_Specular_1);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};


texture Tile_Specular_2;
sampler2D Tile_SpecularSampler_2 = sampler_state{
	Texture = (Tile_Specular_2);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};


texture Tile_Specular_3;
sampler2D Tile_SpecularSampler_3 = sampler_state{
	Texture = (Tile_Specular_3);
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
};






//
// 정점 입력 구조체 ( 그리는 물체가 가지고있는 정점정보 )
//
struct VS_INPUT{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	float3 Binormal : BINORMAL;
	float3 Tangent : TANGENT;
	float2 BaseUV : TEXCOORD0;			//0~1 UV
	float2 TileUV : TEXCOORD1;			//타일 UV 
};

//
// 정점 출력 구조체
//
struct VS_OUTPUT{
	float4 Position : POSITION;	
	float2 BaseUV : TEXCOORD0;			//0~1 UV
	float2 TileUV : TEXCOORD1;			//타일 UV 
	float3 Normal : TEXCOORD2;
	float3 Binormal : TEXCOORD3;
	float3 Tangent : TEXCOORD4;
	float3 WorldPos : TEXCOORD5;
};




//
// 정점 셰이더 엔트리 함수
//
VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output;

	//터레인은 정점위치가 바로 월드이다.
	float4 worldPos = Input.Position;

	//정점의 뷰 투영 변환
	Output.Position = mul(worldPos, matViewProjection);

	Output.BaseUV = Input.BaseUV;
	Output.TileUV = Input.TileUV;

	//TBN 넘겨
	Output.Normal = Input.Normal;
	Output.Binormal = Input.Binormal;
	Output.Tangent = Input.Tangent;

	//월드 위치 넘겨 
	Output.WorldPos = worldPos.xyz;

	return Output;
}



//
// 픽셀 입력 구조체
//
struct PS_INPUT{
	float2 BaseUV : TEXCOORD0;			//0~1 UV
	float2 TileUV : TEXCOORD1;			//타일 UV 
	float3 Normal : TEXCOORD2;
	float3 Binormal : TEXCOORD3;
	float3 Tangent : TEXCOORD4;
	float3 WorldPos : TEXCOORD5;
};



//출력 컬러 설정
struct PS_OUTPUT{
	float4 stage0 : COLOR0;		//0 번스테이지 출력 컬러
	float4 stage1 : COLOR1;			//1 번스테이지 출력 컬러
	float4 stage2 : COLOR2;		//2 번스테이지 출력 컬러
	float4 stage3 : COLOR3;		//3 번스테이지 출력 컬러
};


//
// 픽셀 셰이더 엔트리 함수
//
PS_OUTPUT ps_main(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0;


	float4 splatColor = tex2D(Splat_Sampler, Input.BaseUV);

	//
	// Diffuse Tex
	//
	float4 diffuseTex0 = tex2D(Tile_Sampler_0, Input.TileUV);
	float4 diffuseTex1 = tex2D(Tile_Sampler_1, Input.TileUV);
	float4 diffuseTex2 = tex2D(Tile_Sampler_2, Input.TileUV);
	float4 diffuseTex3 = tex2D(Tile_Sampler_3, Input.TileUV);

	float black = 1.0f - saturate(splatColor.r + splatColor.g + splatColor.b);

	float4 diffuseColor =
		diffuseTex0 * splatColor.r +
		diffuseTex1 * splatColor.g +
		diffuseTex2 * splatColor.b +
		diffuseTex3 * black;

	//
	// Normal Tex
	//
	float4 normalTex0 = tex2D(Tile_NormalSampler_0, Input.TileUV);
	float4 normalTex1 = tex2D(Tile_NormalSampler_1, Input.TileUV);
	float4 normalTex2 = tex2D(Tile_NormalSampler_2, Input.TileUV);
	float4 normalTex3 = tex2D(Tile_NormalSampler_3, Input.TileUV);

	float3 normalTex =
		normalTex0.rgb * splatColor.r +
		normalTex1.rgb * splatColor.g +
		normalTex2.rgb * splatColor.b +
		normalTex3.rgb * black;

	//
	// specular Tex
	//
	float4 specularTex0 = tex2D(Tile_SpecularSampler_0, Input.TileUV);
	float4 specularTex1 = tex2D(Tile_SpecularSampler_1, Input.TileUV);
	float4 specularTex2 = tex2D(Tile_SpecularSampler_2, Input.TileUV);
	float4 specularTex3 = tex2D(Tile_SpecularSampler_3, Input.TileUV);

	float3 specularTex =
	specularTex0.rgb * splatColor.r +
	specularTex1.rgb * splatColor.g +
	specularTex2.rgb * splatColor.b +
	specularTex3.rgb * black;


	//노말 Texture 의 컬러를 가지고 온다.
	float3 norTexColor = normalTex;

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

	Output.stage0 = diffuseColor;
	Output.stage1 = float4(worldNormal, 1);
	Output.stage2 = float4(Input.WorldPos, 1);
	Output.stage3 = float4(specularTex, 1);

	return Output;
}

//
// 테크닉 설정
//
// Pass 설정을 한다. 
technique Terrain
{
	pass p0
	{
		//정점 셰이더 설정
		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.

		//픽셀 셰이더 설정
		PixelShader = compile ps_3_0 ps_main();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.

	}
};