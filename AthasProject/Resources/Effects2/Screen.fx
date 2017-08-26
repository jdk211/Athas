

//Diffuse Texture 변수 ( 외부 셋팅 )
texture DiffuseTex;
sampler2D DiffuseSampler = sampler_state{
	Texture = DiffuseTex;		
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;

	ADDRESSU = CLAMP;
	ADDRESSV = CLAMP;
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
	float4 diffuseColor = tex2D(DiffuseSampler, Texcoord);

	//Alpha 에  1.0 일수록 스스로 빛을 발산한다.
	float emissive = ( diffuseColor.a );

	//표면에서 받는 Diffuse 광원량
	float3 diffuseLight = tex2D(DiffuseLightSampler, Texcoord).rgb;

	//표면 Specular 반사량
	float3 specularColor = tex2D(SpecularSampler, Texcoord).rgb;

	//Specular Color
	float3 specularLight = tex2D(SpecularLightSampler, Texcoord).rgb;

	float3 diff = lerp(diffuseLight, float3(1, 1, 1), emissive);

	float3 spec = lerp(specularLight, float3(0, 0, 0), emissive);
	//최종 컬러
	float3 finalColor = (diffuseColor * diff) + (specularColor * spec);

	return float4(finalColor, 1);
}


float ToonGyungGaeSu = 3.0f;

// ToonGyungGaeSu 경계를 나눈 명암을 준다.
float3 toonGyungGea(float3 val)
{
	//위에서 계산 된 val 값에 계단 값을 주자.
	float interval = 1.0 / ToonGyungGaeSu;

	//floor 소수점 재껴준다.
	float3 toon = floor(val / interval) * interval;

	return saturate( toon + interval );
}

// ToonGyungGaeSu 경계를 나눈 명암을 준다.
float3 toonGyungGeaSpec(float3 val)
{
	//위에서 계산 된 val 값에 계단 값을 주자.
	float interval = 1.0 / 2.0f;

	//floor 소수점 재껴준다.
	float3 toon = floor(val / interval) * interval;

	return toon;
}



//
// 픽셀 셰이더 엔트리 함수
//
float4 ps_toon(float2 Texcoord : TEXCOORD0) : COLOR
{
	//단순 표면 컬러...
	float4 diffuseColor = tex2D(DiffuseSampler, Texcoord);

	//Alpha 에  1.0 일수록 스스로 빛을 발산한다.
	float emissive = (diffuseColor.a);

	//표면에서 받는 Diffuse 광원량
	float3 diffuseLight = tex2D(DiffuseLightSampler, Texcoord).rgb;

	diffuseLight = toonGyungGea(diffuseLight);

	//표면 Specular 반사량
	float3 specularColor = tex2D(SpecularSampler, Texcoord).rgb;

	specularColor = toonGyungGeaSpec(specularColor);


	//Specular Color
	float3 specularLight = tex2D(SpecularLightSampler, Texcoord).rgb;

	float3 diff = lerp(diffuseLight, float3(1, 1, 1), emissive);

	float3 spec = lerp(specularLight, float3(0, 0, 0), emissive);

	//최종 컬러
	float3 finalColor = (diffuseColor * diff);

	return float4(finalColor, 1);
}


//
// 단순 출력
//
float4 ps_color(float2 Texcoord : TEXCOORD0) : COLOR
{
	//단순 표면 컬러...
	float4 diffuseColor = tex2D(DiffuseSampler, Texcoord);
	return diffuseColor;
}













//회색으로...
float4 ps_gray(float2 Texcoord : TEXCOORD0) : COLOR
{
	//단순 표면 컬러...
	float4 diffuseColor = tex2D(DiffuseSampler, Texcoord);


	//그레이비율은 다음과 같다.
	//Red 0.299f
	//Green 0.587f
	//Blue 0.114f


	//회색으로 바꾸기...
	float grayScale =
		diffuseColor.r * 0.299f +
		diffuseColor.g * 0.587f +
		diffuseColor.b * 0.114f;

	return float4(grayScale, grayScale, grayScale, 1);
}


/////////////////////////////////////////////////////////////


//Additive Texture 변수 ( 외부 셋팅 )
texture AdditiveTex;
sampler2D AdditiveSampler = sampler_state{
	Texture = AdditiveTex;
	MIPFILTER = LINEAR;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;

	ADDRESSU = CLAMP;
	ADDRESSV = CLAMP;
};

float addAmount = 1.0f;

float4 ps_addtive(float2 Texcoord : TEXCOORD0) : COLOR
{
	//단순 표면 컬러...
	float4 diffuseColor = tex2D(DiffuseSampler, Texcoord);
	float4 addColor = tex2D(AdditiveSampler, Texcoord) * addAmount;

	return diffuseColor + addColor;
}



/////////////////////////////////////////

float InBlack = 0.0f;		//컬러를 0 ~ 255 로 볼때 이값 미만이면 0 으로 처리한다.
float Gamma = 1.0f;			//컬러를 0 ~ 255 로 볼때 이값 이상이면 1 으로 처리한다.
float InWhite = 255.0f;		//증폭 
float OutBlack = 0.0f;		//감마 처리 이후 얻어진 값을 0 ~ 255 로 볼때 이값 미만이면 0 으로 처리한다.
float OutWhite = 255.0f;	//감마 처리 이후 얻어진 값을 0 ~ 255 로 볼때 이값 이상이면 1 으로 처리한다.

float level(float color)
{
	// 0 ~ 1 사이의 범위 의 컬러 값을 0 ~ 255 범위로...
	float c = color * 255.0f;

	//0 밑으로 안내려 가게....
	c = max(0, c - InBlack);

	//컬러 값 InBlack 에서 InWhite 에 대한 범위의 Factor 값 
	//이과정에서 c 는 0 ~ 1 사이의 값이 된다.
	c = saturate( c / (InWhite - InBlack));

	//감마 처리 
	c = pow(c, Gamma);

	//컬러 Out 처리 	( 여기서 다시 0 ~ 255 의 컬러 범위 값이 된다 )	
	//여기서 처리한는 것이 
	//지금까지 계산된 c 의 0 ~ 1 값범위를
	//outBlack 과 outWhite 로본다.
	//c = c * (outWhite - outBlack) + outBlack;
	c = lerp(OutBlack, OutWhite, c);

	//최종 컬러 
	c = saturate(c / 255.0f);

	return c;
}

//회색으로...
float4 ps_colorlevel(float2 Texcoord : TEXCOORD0) : COLOR
{
	//단순 표면 컬러...
	float4 diffuseColor = tex2D(DiffuseSampler, Texcoord);

	float r = level(diffuseColor.r);
	float g = level(diffuseColor.g);
	float b = level(diffuseColor.b);


	return float4(r, g, b, 1);
}




// Blur 관련
/////////////////////////////////////////


float BlurWeights[13] =
{
	0.002216,		// -6
	0.008764,       // -5
	0.026995,       // -4
	0.064759,       // -3
	0.120985,       // -2
	0.176033,       // -1
	0.199471,		//Center
	0.176033,		//	1
	0.120985,		//	2
	0.064759,		//	3
	0.026995,		//	4
	0.008764,		//	5
	0.002216		//	6
};

float bulrPos[13] =
{
	-6,
	-5,
	-4,
	-3,
	-2,
	-1,
	0,
	1,
	2,
	3,
	4,
	5,
	6
};


float blurScale = 1.0f;			//블러스케일


float4 ps_BlurX(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float4 finalColor = float4(0, 0, 0, 1);
	
	for (int i = 0; i < 13; i++)
	{
		float2 uv = Texcoord + float2(
			bulrPos[i] * onePixelX, 0) * blurScale;

		finalColor += tex2D(DiffuseSampler, uv) * BlurWeights[i];

	}

	return finalColor;
}



float4 ps_BlurY(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float4 finalColor = float4(0, 0, 0, 1);

	for (int i = 0; i < 13; i++)
	{
		float2 uv = Texcoord + float2(
			0, bulrPos[i] * onePixelX) * blurScale;

		finalColor += tex2D(DiffuseSampler, uv) * BlurWeights[i];

	}

	return finalColor;
}

// OutLine
//////////////////////////////////////////////////////////


texture NormalTex;
sampler2D NormalSampler = sampler_state{
	Texture = NormalTex;
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};



float3 GetWorldNormal(float2 uv)
{

	//압축된 노말
	float2 pNor = tex2D(NormalSampler, uv).xy;	//X Y 값만 가지고 온다.

		//압축된 노말을 풀어 월드 노말로 복구
		float num = 2 / (1 + pNor.x * pNor.x + pNor.y * pNor.y);
	float x = pNor.x * num;
	float y = pNor.y * num;
	float z = num - 1;
	float3 worldNormal = float3(x, y, z);

	return worldNormal;

}



float mask[9] = {
	-1, -1, -1,
	-1,  8, -1,
	-1, -1, -1 };		//라플라스 필터

float coordX[3] = { -1, 0, 1 };
float coordY[3] = { -1, 0, 1 };

float3 outlineColor = float3(0, 0, 0);

float4 ps_outline(float2 Texcoord : TEXCOORD0) : COLOR0
{

	float outlineStrength = 0.0f;

	//자신을 포함한 주변 픽셀정보 9개 
	for (int i = 0; i < 9; i++)
	{
		float2 uv = float2(coordX[i % 3], coordY[i / 3]);
		uv.x *= onePixelX;
		uv.y *= onePixelY;
		uv = Texcoord + uv;


		float3 normalColor = GetWorldNormal(uv) * mask[i];

		outlineStrength += normalColor.x;
		outlineStrength += normalColor.y;
		outlineStrength += normalColor.z;
	}

	float value = saturate(abs(outlineStrength));
	value = pow(value, 2.0f);


	return float4(outlineColor, value);
}


// Fog
///////////////////////////////////////////////

texture DepthTex;
sampler2D DepthSampler = sampler_state{
	Texture = DepthTex;
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};

float CamNear = 0.0f;
float CamFar = 0.0f;
float fogStart = 0.4f;	//포그가 시작되는 depth
float fogEnd = 1.0f;	//포그종료 depth
float3 FogColor = float3(1, 1, 1);

float4 ps_AddFog(float2 Texcoord : TEXCOORD0) : COLOR0
{
	//단순 표면 컬러...
	float4 diffuseColor = tex2D(DiffuseSampler, Texcoord);


	float depth = tex2D(DepthSampler, Texcoord).x;
	
	//위의 depth 값을 선형으로 펴준다
	float z = depth;
	float a = CamFar / (CamFar - CamNear);
	float b = -CamNear / (CamFar - CamNear);
	depth = b / (z - a);



	float delta = fogEnd - fogStart;
	float fog = saturate(depth - fogStart) / delta;



	float3 finalColor = lerp(diffuseColor.rgb, FogColor, fog);

	return float4(finalColor, 1);
}




//////////////////////////////////////////////////////////

technique ComputeLight
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.
		PixelShader = compile ps_3_0 ps_main();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.
	}
};

technique ComputeToonLight
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.
		PixelShader = compile ps_3_0 ps_toon();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.
	}
};



technique GrayScale
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.
		PixelShader = compile ps_3_0 ps_gray();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.
	}
};

technique ColorLevel
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.
		PixelShader = compile ps_3_0 ps_colorlevel();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.
	}
};

technique BlurX
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.
		PixelShader = compile ps_3_0 ps_BlurX();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.
	}
};


technique BlurY
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.
		PixelShader = compile ps_3_0 ps_BlurY();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.
	}
};

technique AddFog
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.
		PixelShader = compile ps_3_0 ps_AddFog();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.
	}


};


technique Outline
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.
		PixelShader = compile ps_3_0 ps_outline();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.
	}

	
};

technique Additive
{
	pass p0
	{
		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.
		PixelShader = compile ps_3_0 ps_addtive();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.
	}

	pass p1
	{
		ALPHABLENDENABLE = true;
		SRCBLEND = SRCALPHA;
		DESTBLEND = INVSRCALPHA;


		VertexShader = compile vs_3_0 vs_main();		//이예기는 정점셰이더 함수는 vs_main 이고 셰이더 버전 3.0 으로 돌리겠다.
		PixelShader = compile ps_3_0 ps_color();			//이예기는 픽셀셰이더 함수는 ps_main 이고 셰이더 버전 3.0 으로 돌리겠다.
	}
};
