

//Diffuse Texture ���� ( �ܺ� ���� )
texture DiffuseTex;
sampler2D DiffuseSampler = sampler_state{
	Texture = DiffuseTex;		
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};

//Diffuse Light Texture ����
texture DiffuseLightTex;
sampler2D DiffuseLightSampler = sampler_state{
	Texture = DiffuseLightTex;		
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};


//Specular Texture ���� ( �ܺ� ���� )
texture SpecularTex;
sampler2D SpecularSampler = sampler_state{
	Texture = SpecularTex;
	MIPFILTER = POINT;
	MAGFILTER = POINT;
	MINFILTER = POINT;
};


//Specular Light Texture ����
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
// ���� �Է� ����ü ( �׸��� ��ü�� �������ִ� �������� )
//
struct VS_INPUT{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
};

//
// ���� ��� ����ü
//
struct VS_OUTPUT{
	float4 Position : POSITION;		//���� ��ǥ���� ��ȯ�� �Ϸ�� ������ ��ġ
	float2 Texcoord : TEXCOORD0;		//�̰� �ȼ����̴��� ���޵ȴ�.
};


VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output;

	//������������ġ�� �״�� ������ǥ�� ����.
	Output.Position = Input.Position;

	//�ؽ��� ��ǥ �ѱ��.
	//Output.Texcoord = Input.Texcoord;
	Output.Texcoord.x = Input.Texcoord.x + halfPixelX;
	Output.Texcoord.y = Input.Texcoord.y + halfPixelY;

	return Output;
}



//
// �ȼ� ���̴� ��Ʈ�� �Լ�
//
float4 ps_main(float2 Texcoord : TEXCOORD0) : COLOR
{
	//�ܼ� ǥ�� �÷�...
	float3 diffuseColor = tex2D(DiffuseSampler, Texcoord).rgb;

	//ǥ�鿡�� �޴� Diffuse ������
	float3 diffuseLight = tex2D(DiffuseLightSampler, Texcoord).rgb;


	//ǥ�� Specular �ݻ緮
	float3 specularColor = tex2D(SpecularSampler, Texcoord).rgb;

	//Specular Color
	float3 specularLight = tex2D(SpecularLightSampler, Texcoord).rgb;


	//���� �÷�
	float3 finalColor = (diffuseColor * diffuseLight) + ( specularColor * specularLight );

	return float4(finalColor, 1);
}


//
// ��ũ�� ����
//
technique ColorShader
{


	//Pass ����
	//�ش� Pass �� ���� ���̴��� ���̴� �󸶹������� �������� ���� ��Ʈ���Լ��� ��� �Ǵ��� ����
	//�׸��� �ȼ� ���̴��� ���̴� �󸶹������� �������� ���� ��Ʈ���Լ��� ��� �Ǵ��� ����
	pass p0
	{
		//���� ���̴� ����
		VertexShader = compile vs_3_0 vs_main();		//�̿���� �������̴� �Լ��� vs_main �̰� ���̴� ���� 3.0 ���� �����ڴ�.

		//�ȼ� ���̴� ����
		PixelShader = compile ps_3_0 ps_main();			//�̿���� �ȼ����̴� �Լ��� ps_main �̰� ���̴� ���� 3.0 ���� �����ڴ�.

	}


};