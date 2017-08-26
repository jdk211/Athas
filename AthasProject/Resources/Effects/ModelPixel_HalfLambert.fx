
float3 lightDir;			//월드 광원 방향

//빛의 색상
float3 lightColor = float3(1, 1, 1);

//스펙큘라 파워
float specPower = 30;				//정반사의 날카로움


//Ambient
float ambient = 0.1f;				//환경광 량


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


struct PS_INPUT{
	float2 Texcoord : TEXCOORD0;		//이건 픽셀셰이더로 전달된다.
	float3 ViewDir : TEXCOORD1;		//이건 픽셀셰이더로 전달된다.
	float3 Tangent : TEXCOORD2;
	float3 Binormal : TEXCOORD3;
	float3 Normal : TEXCOORD4;

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

	//위에서 얻은 Tangent Space 기준의 벡터를 월드 벡터로 변환.
	float3x3 TBN = float3x3(
	normalize(Input.Tangent),
	normalize(Input.Binormal),
	normalize(Input.Normal));
	//월드 노말
	float3 worldNormal = mul(tanNormal, TBN);



	//시야 방향 정규화
	float3 viewDir = normalize(Input.ViewDir);


	//
	// Diffuse 확산광 계산
	//

	//광원의 근원지와 월드 노말과 내적
	float d = dot(worldNormal, -lightDir);

	//위에서 계산된 -1 ~ 1 사이의 램버트 연산 결과를
	//0 ~ 1 의 범위로 땡긴다 ( 이게 하프램버트 )
	float diff = d * 0.5f + 0.5f;


	//최종 확산광의 생상은
	float3 diffuseLightColor = lightColor * diff;


	//
	// Specular 정반사광 계산
	//

	//광원이 월드 노말에 정반사된 방향벡터
	float3 refl = lightDir + (d * worldNormal * 2.0f);
	refl = normalize(refl);

	//정반사 량
	float spec = dot(refl, viewDir);
	spec = saturate(spec);

	//Power 적용
	spec = pow(spec, specPower);

	//발산되는 최종 정반사의 색상
	float3 specularLightColor = lightColor * spec * diff;		//정반사가 확산광의 영향을 받는경우


	//
	// 최종 Diffuse 색상
	//

	float4 diffuseTex = tex2D(DiffuseSampler, Input.Texcoord);
	float3 diffuseColor = diffuseTex.rgb * diffuseLightColor;


	//
	// 최종 Specular 색상
	//
	float3 specularTex = tex2D(SpecularSampler, Input.Texcoord);
	float3 specularColor = specularTex * specularLightColor;


	//
	// 최종 Emission 색상
	//
	float3 emissionColor = tex2D(EmissionSampler, Input.Texcoord);


	//최종 색상은 더해
	float3 finalColor = diffuseColor + specularColor + emissionColor;

	return float4(finalColor, 1);

}
