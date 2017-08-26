//
// 셰이더 전역 변수
//
float4x4	matWorld;				//월드 행렬
float4x4	matViewProjection;		//뷰 * 프로젝션 행렬
float4x4	matInvViewProjection;	//뷰 * 프로젝션 역행렬

//방향성 광원의 광원 방향
float3 lightDir = float3(1, 0, 0);

//점광원의 광원 위치
float3 lightPos = float3(0, 0, 0);

//점광원의 영향력
float maxRange = 10;
float minRange = 1;
float fallOff = 1;


//광원의 색상
float3 lightColor = float3(1, 1, 1);

//광원의 세기
float intensity = 0.0f;

//시점자의 위치
float3 camPos = float3(0, 0, 0);


float halfPixelSizeX;
float halfPixelSizeY;



//WorldNormal
texture NormalTex;
sampler2D NormalSampler = sampler_state{

	Texture = NormalTex;		
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};

//WorldPosition
texture DepthTex;
sampler2D DepthSampler = sampler_state{
	Texture = DepthTex;
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};


//ShadowTex
texture ShadowTex;
sampler2D ShadowSampler = sampler_state{
	Texture = ShadowTex;
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
	ADDRESSU = BORDER;
	ADDRESSV = BORDER;
	BORDERCOLOR = 0xFFFFFFFF;



};

//DirectionLightViewProjection
float4x4 matDirectionLightViewProj;

float ambient = 0.2f;


struct VS_INPUT{
	float4 Position : POSITION;
};

struct VS_OUTPUT{
	float4 Position : POSITION;		//동차 좌표까지 변환이 완료된 정점의 위치
	float4 ClipPosition : TEXCOORD0;	//필셀셰이더로 전달되는 동차 좌표
};

struct PS_OUTPUT{
	float4 diffuseLight : COLOR0;
	float4 specularLight : COLOR1;
};


//
// Direction Light
//


//방향성광원의 버텍스 셰이더
VS_OUTPUT vs_Direction(VS_INPUT Input)
{
	VS_OUTPUT Output;

	//들어오는정점위치를 그대로 동차좌표로 쓴다.
	Output.Position = Input.Position;
	Output.ClipPosition = Output.Position;	//동차를 PixelShader 로 넘긴다.

	return Output;
}

//방향성광원의 픽셀 셰이더
PS_OUTPUT ps_Direction(float4 ClipPosition : TEXCOORD0)
{
	float2 clip = ClipPosition.xy / ClipPosition.w;
	float2 uv = clip;


	//-1 ~ 1 범위를 0 ~ 1 범위로..
	//uv.x = (uv.x + 1.0f) * 0.5f;
	//uv.y = (uv.y + 1.0f) * 0.5f;
	uv = (uv + 1.0f) * 0.5f;		//vector 에 스칼라 덧셈 뺄셈가능 ( 요소를 다계산함 )
	uv.y = 1.0f - uv.y;				//Y 값 반전...

	//반 픽셀 밀어
	uv.x += halfPixelSizeX;
	uv.y += halfPixelSizeY;

	//월드 노말
	//float3 worldNormal = tex2D(NormalSampler, uv).xyz;	//노말 가지고와

	//압축된 노말
	float2 pNor = tex2D(NormalSampler, uv).xy;	//X Y 값만 가지고 온다.
	
	//압축된 노말을 풀어 월드 노말로 복구
	float num = 2 / (1 + pNor.x * pNor.x + pNor.y * pNor.y);
	float x = pNor.x * num;
	float y = pNor.y * num;
	float z = num - 1;
	float3 worldNormal = float3(x, y, z);


	//뎁스값 얻는다.
	float depth = tex2D(DepthSampler, uv).r;
	//동차 좌표
	float4 clipPos = float4(clip.x, clip.y, depth, 1);
	//월드 위치
	float4 worldPos = mul(clipPos, matInvViewProjection);
	worldPos.xyz /= worldPos.w;


	//라이트 ViewProjection 의 동차 좌표
	float4 lightCamClip = mul(float4(worldPos.xyz, 1), 
		matDirectionLightViewProj);
	//ShadowMap 의 UV 를 얻는다.
	float2 shadowUV = (lightCamClip.xy + 1.0f) * 0.5f;
	//y 반전
	shadowUV.y = 1.0f - shadowUV.y;

	//쉐도우 맵에 써진 뎁스 
	float shadowDepth = tex2D(ShadowSampler, shadowUV);
	float myDepthToLight = lightCamClip.z;



	//diffuse 량 계산
	float d = dot(worldNormal, -lightDir);
	float diff = d;

	float shadowBias = 0.002f;
	//그림자 져진다. 빛의 량은 없다.
	if (shadowDepth + shadowBias < myDepthToLight && 
		lightCamClip.z <= 1.0f ){
		diff = 0.0f;
	}
	//Ambient 적용
	diff = max(ambient, diff);


	//float amount = diff * intensity * (1.0f - shadowIntensity );
	
	float amount = diff * intensity;
	float3 diffuseColor = lightColor * amount;


	//
	// 정반사 계산
	//
	//해당 픽셀위치의 월드위치
	//위치에서부터 시점자방향벡터
	float3 viewDir = camPos - worldPos.xyz;
	viewDir = normalize(viewDir);

	//광원이 노말에 정반사되어 나가는 벡터.
	//float3 reflectLight = reflect(lightDir, worldNormal); 
	float3 reflectLight = lightDir + d * worldNormal * 2.0f;
	reflectLight = normalize(reflectLight); 

	//정반사량
	float spec = dot(reflectLight, viewDir);
	spec = pow(spec, 20) * amount;

	//최종 정반사 컬러....
	float3 specularColor = float3(spec, spec, spec);

	PS_OUTPUT Output;
	Output.diffuseLight = float4(diffuseColor, 1);
	Output.specularLight = float4(specularColor, 1);

	return Output;
}



//
// Point Light
//


//점 광원의 버텍스 셰이더
VS_OUTPUT vs_Point(VS_INPUT Input)
{
	VS_OUTPUT Output;

	//월드 위치를 화면 동차로 랜더링한다.
	Output.Position = mul(Input.Position, matWorld);
	Output.Position = mul(Output.Position, matViewProjection);
	
	Output.ClipPosition = Output.Position;	//동차를 PixelShader 로 넘긴다.

	return Output;
}

//점광원의 픽셀 셰이더
PS_OUTPUT ps_Point(float4 ClipPosition : TEXCOORD0)
{
	float2 clip = ClipPosition.xy / ClipPosition.w;
	float2 uv = clip;

	//-1 ~ 1 범위를 0 ~ 1 범위로..
	//uv.x = (uv.x + 1.0f) * 0.5f;
	//uv.y = (uv.y + 1.0f) * 0.5f;
	uv = (uv + 1.0f) * 0.5f;		//vector 에 스칼라 덧셈 뺄셈가능 ( 요소를 다계산함 )
	uv.y = 1.0f - uv.y;				//Y 값 반전...

	//반 픽셀 밀어
	uv.x += halfPixelSizeX;
	uv.y += halfPixelSizeY;




	//뎁스값 얻는다.
	float depth = tex2D(DepthSampler, uv).r;
	//동차 좌표
	float4 clipPos = float4(clip.x, clip.y, depth, 1);
	//월드 위치
	float4 worldPos = mul(clipPos, matInvViewProjection);
	worldPos.xyz /= worldPos.w;

	//월드 노말
	//float3 worldNormal = tex2D(NormalSampler, uv).xyz;	//노말 가지고와

	//압축된 노말
	float2 pNor = tex2D(NormalSampler, uv).xy;	//X Y 값만 가지고 온다.

	//압축된 노말을 풀어 월드 노말로 복구
	float num = 2 / (1 + pNor.x * pNor.x + pNor.y * pNor.y);
	float x = pNor.x * num;
	float y = pNor.y * num;
	float z = num - 1;
	float3 worldNormal = float3(x, y, z);



	//빛의 방향
	float3 toLight = lightPos - worldPos.xyz;
	
	//광원과의 거리
	float distToLight = length(toLight);	//벡터 길이
	//toLight = normalize(toLight);
	toLight *= 1.0f / distToLight;			//정규화



	//Max 와 Min 의 차이
	float delta = maxRange - minRange;		
	float t = saturate( (distToLight - minRange) / delta );
	t = 1.0f - t;

	//fallOff 적용
	t = pow(t, fallOff);


	//diffuse 량 계산
	float d = dot(worldNormal, toLight);
	float diff = saturate(d);

	float amount = diff * intensity * t;

	float3 diffuseLight = lightColor * amount;


	//
	// 정반사 계산
	//

	//위치에서부터 시점자방향벡터
	float3 viewDir = camPos - worldPos.xyz;
	viewDir = normalize(viewDir);

	//광원이 노말에 정반사되어 나가는 벡터.
	//float3 reflectLight = reflect(lightDir, worldNormal); 
	float3 reflectLight = -toLight + d * worldNormal * 2.0f;
	reflectLight = normalize(reflectLight);

	//정반사량
	float spec = dot(reflectLight, viewDir);
	spec = pow(spec, 20) * amount;

	//최종 정반사 컬러....
	float3 specularColor = float3(spec, spec, spec);

	PS_OUTPUT Output;
	Output.diffuseLight = float4(diffuseLight, amount);
	Output.specularLight = float4(specularColor, amount);

	return Output;
}


//방향성 광원 technique
technique DirectionLight
{
	pass p0
	{
		ZWRITEENABLE = FALSE;		//절대로 깊이를 쓰지마...
		VertexShader = compile vs_3_0 vs_Direction();
		PixelShader = compile ps_3_0 ps_Direction();
	}
};


//점 광원 
technique PointLight
{
	//스텐실을 쓰기 위한 Pass
	pass StencilPass
	{
		ZWRITEENABLE = FALSE;		//절대로 깊이를 쓰지마...
		CULLMODE = NONE;
		COLORWRITEENABLE = 0x0;
		ZENABLE = TRUE;			//뎁스 테스트 반드시 한다

		STENCILENABLE = TRUE;	//스텐실 버사용을 활성화
		STENCILFUNC = ALWAYS;	//랜더링되는 영역에서 스텐실은 항상통과된다.
		STENCILPASS = KEEP;
		STENCILFAIL = KEEP;
		STENCILZFAIL = INCR;	//값증가

		VertexShader = compile vs_3_0 vs_Point();
		PixelShader = NULL;			//어차피 컬러안그림....
	}

	pass RenderPass
	{
		ZWRITEENABLE = FALSE;		//절대로 깊이를 쓰지마...

		COLORWRITEENABLE = 0xFFFFFFFF;
		ZENABLE = FALSE;
		CULLMODE = CW;		//이거 CW 로 해야함 NONE 으로 하면 같은 픽셀에 2 번랜더링 될수 있음
		
		STENCILREF = 0x01;
		STENCILFUNC = LESSEQUAL;	//랜더링되는 영역에서 스텐실은 항상통과된다.
		STENCILPASS = KEEP;
		STENCILFAIL = KEEP;
		STENCILZFAIL = KEEP;	//값증가


		//Alpha 블렌드 설정
		ALPHABLENDENABLE = TRUE;
		SRCBLEND = SRCALPHA;
		DESTBLEND = ONE;

		VertexShader = compile vs_3_0 vs_Point();
		PixelShader = compile ps_3_0 ps_Point();
	}
};







