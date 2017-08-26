#include "StdAfx.h"
#include "cScene.h"
#include "cTexQuad.h" 
#include "cRenderTexture.h"
#include "cLight.h"
#include "cUIObject.h"

//ȯ��Effect
LPD3DXEFFECT cScene::pEnvironmentEffect = NULL;

//ȯ�汸
LPD3DXMESH cScene::pEnvironmentSphere = NULL;


cScene::cScene(void)
: pEnvironmentCubeTex(NULL)
{
}


cScene::~cScene(void)
{
	SAFE_RELEASE(this->pEnvironmentCubeTex);
	SAFE_RELEASE(pEnvironmentSphere);

}


HRESULT cScene::Init()
{
	//�����Ǵ� Texture �� �ػ�
	int width = WINSIZE_X;
	int height = WINSIZE_Y;

	//���� Texture ����
	this->pRenderTexture0 = new cRenderTexture();
	this->pRenderTexture0->Init(width, height, D3DFMT_A8R8G8B8);
	this->pRenderTexture1 = new cRenderTexture();
	this->pRenderTexture1->Init(width, height, D3DFMT_A8R8G8B8);
	this->pRenderTexture2 = new cRenderTexture();
	this->pRenderTexture2->Init(width, height, D3DFMT_G16R16F);		//R 16 ��Ʈ G 16 ��Ʈ
	this->pRenderTexture3 = new cRenderTexture();
	this->pRenderTexture3->Init(width, height, D3DFMT_R32F);		//Depth Red �� 32 ��Ʈ ���ʹ�...

	this->pRenderDiffuseLight = new cRenderTexture();
	this->pRenderDiffuseLight->Init(width, height, D3DFMT_A16B16G16R16F);

	this->pRenderSpecularLight = new cRenderTexture();
	this->pRenderSpecularLight->Init(width, height, D3DFMT_A16B16G16R16F);

	this->pRenderFinalLight = new cRenderTexture();
	this->pRenderFinalLight->Init(width, height, D3DFMT_A8R8G8B8);

	this->pRenderColorLevel = new cRenderTexture();
	this->pRenderColorLevel->Init(width, height, D3DFMT_A8R8G8B8);

	this->pRenderOutLine = new cRenderTexture();
	this->pRenderOutLine->Init(width, height, D3DFMT_A8R8G8B8);


	this->pRenderBlurX = new cRenderTexture();
	this->pRenderBlurX->Init(width / 4, height / 4, D3DFMT_A8R8G8B8);

	this->pRenderBlurY = new cRenderTexture();
	this->pRenderBlurY->Init(width / 4, height / 4, D3DFMT_A8R8G8B8);

	this->pRenderBlur = new cRenderTexture();
	this->pRenderBlur->Init(width, height, D3DFMT_A8R8G8B8);

	this->pScreenTex = new cRenderTexture();
	this->pScreenTex->Init(width, height, D3DFMT_A8R8G8B8);




	//ScreenQuad ����
	this->pScreenQuad = new cTexQuad();

	//ScreenEffect �ε�
	this->pScreenEffect = RESOURCE_EFFECT->GetResource("./Resources/Effects2/Screen.fx");

	bBBoSha = true;
	bToon = false;
	bOutline = false;
	bFog = true;

	return this->Scene_Init();
}

void cScene::Release()
{
	pRenderTexture0->Release();
	pRenderTexture1->Release();
	pRenderTexture2->Release();
	pRenderTexture3->Release();
	pRenderDiffuseLight->Release();
	pRenderSpecularLight->Release();
	pRenderFinalLight->Release();
	pRenderColorLevel->Release();
	pRenderBlurX->Release();
	pRenderBlurY->Release();
	pRenderBlur->Release();
	pRenderOutLine->Release();

	pScreenTex->Release();

	SAFE_DELETE(this->pRenderTexture0);
	SAFE_DELETE(this->pRenderTexture1);
	SAFE_DELETE(this->pRenderTexture2);
	SAFE_DELETE(this->pRenderTexture3);
	SAFE_DELETE(this->pRenderDiffuseLight);
	SAFE_DELETE(this->pRenderSpecularLight);
	SAFE_DELETE(this->pRenderFinalLight);
	SAFE_DELETE(this->pRenderColorLevel);
	SAFE_DELETE(this->pRenderBlurX);
	SAFE_DELETE(this->pRenderBlurY);
	SAFE_DELETE(this->pRenderBlur);
	SAFE_DELETE(this->pRenderOutLine);
	SAFE_DELETE(pScreenQuad);
	SAFE_DELETE(this->pScreenTex);


	this->Scene_Release();
}

void cScene::Update(float timeDelta)
{
	this->Scene_Update(timeDelta);
}

void cScene::Render()
{

	//
	// Geometry Buffer Render
	//

	//RenderTexture �� �׷���..
	//DirectX9 ������ ��Ƽ RenderTexture �� �ִ� 4����� ������ ����..
	this->pRenderTexture0->BeginRender(0, 0x00000000);		//Diffuse Color
	this->pRenderTexture1->BeginRender(1, 0x00000000);		//Normal
	this->pRenderTexture2->BeginRender(2, 0x00000000);		//WorldPosition
	this->pRenderTexture3->BeginRender(3, 0xFFFFFFFF);		//Depth ���� �հɷ� �ʱ�ȭ �ؾ��ϴ�  ȭ��Ʈ�� �ʱ�ȭ....


	//���� ���� ȯ����� �׸��� �����Ѵ�.
	this->RenderEnvironment();

	this->Scene_Render();


	this->pRenderTexture0->EndRender();
	this->pRenderTexture1->EndRender();
	this->pRenderTexture2->EndRender();
	this->pRenderTexture3->EndRender();


	//
	// Shadow Render
	//
	this->Scene_ShadowRender();

	//
	// Lighting 
	//

	//������ �׸���... ��������� �÷����۸������.
	this->pRenderDiffuseLight->BeginRender(0, 0x00000000, D3DCLEAR_TARGET);
	this->pRenderSpecularLight->BeginRender(1, 0x00000000, D3DCLEAR_TARGET);


	cLight::SetRenderTexture(
		this->pRenderTexture2->GetTexture(),
		this->pRenderTexture3->GetTexture());
	cLight::SetCamera(&this->mainCamera);


	this->Scene_LightRender();


	this->pRenderDiffuseLight->EndRender();
	this->pRenderSpecularLight->EndRender();


	//
	// ScreenQuad �׸���...
	//
	this->pRenderFinalLight->BeginRender(0, 0xFFFFFFFF, D3DCLEAR_TARGET );
	//Pixel �ϳ������� UV ������
	float onePixelX = 1.0f / WINSIZE_X;
	float onePixelY = 1.0f / WINSIZE_Y;
	float halfPixelX = onePixelX * 0.5f;
	float halfPixelY = onePixelY * 0.5f;

	Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	this->pScreenEffect->SetFloat("onePixelX", onePixelX);
	this->pScreenEffect->SetFloat("onePixelY", onePixelY);
	this->pScreenEffect->SetFloat("halfPixelX", halfPixelX);
	this->pScreenEffect->SetFloat("halfPixelY", halfPixelY);

	if ( this->bToon )
		this->pScreenEffect->SetTechnique("ComputeToonLight");
	else
		this->pScreenEffect->SetTechnique("ComputeLight");


	this->pScreenEffect->SetTexture("DiffuseTex", pRenderTexture0->GetTexture());
	this->pScreenEffect->SetTexture("DiffuseLightTex", pRenderDiffuseLight->GetTexture());
	this->pScreenEffect->SetTexture("SpecularTex", pRenderTexture1->GetTexture());
	this->pScreenEffect->SetTexture("SpecularLightTex", pRenderSpecularLight->GetTexture());

	UINT pass;
	this->pScreenEffect->Begin(&pass, 0);
	this->pScreenEffect->BeginPass(0);
	pScreenQuad->Render();
	this->pScreenEffect->EndPass();
	this->pScreenEffect->End();

	Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	//Alpha �� �� ��ƼŬ�� �׸���.
	Scene_ParticleRender();

	//ȭ�� ��� �����Ѵ��� Gizmo �׸���.
	this->Scene_GizmoRender();

	this->pRenderFinalLight->EndRender();



	//�Ȱ�
	/*if (bFog){

		this->pRenderFinalLight->BeginRender(0, 0xFFFFFFFF, 0);

		this->pScreenEffect->SetTechnique("AddFog");
		this->pScreenEffect->SetTexture("DepthTex", pRenderTexture3->GetTexture());
		this->pScreenEffect->SetTexture("DiffuseTex", pRenderFinalLight->GetTexture());
		this->pScreenEffect->SetFloat("CamNear", this->mainCamera.Near);
		this->pScreenEffect->SetFloat("CamFar", this->mainCamera.Far);

		this->pScreenEffect->Begin(&pass, 0);
		this->pScreenEffect->BeginPass(0);
		pScreenQuad->Render();
		this->pScreenEffect->EndPass();
		this->pScreenEffect->End();

		this->pRenderFinalLight->EndRender();
	}
*/

	//ȭ�鿡 Quad �� �׸���.
	//ȸ���׸���
	//this->pScreenEffect->SetTechnique("GrayScale");
	
	
	//�÷�����
	this->pRenderColorLevel->BeginRender(0, 0x00000000);
	
	/*if (bBBoSha){

		this->pScreenEffect->SetTechnique("ColorLevel");
		this->pScreenEffect->SetFloat("InBlack", 131);
		this->pScreenEffect->SetFloat("Gamma", 0.79);
		this->pScreenEffect->SetFloat("InWhite", 230);
		this->pScreenEffect->SetFloat("OutBlack", 0);
		this->pScreenEffect->SetFloat("OutWhite", 255);
		this->pScreenEffect->SetTexture("DiffuseTex", pRenderFinalLight->GetTexture());

		this->pScreenEffect->Begin(&pass, 0);
		this->pScreenEffect->BeginPass(0);
		pScreenQuad->Render();
		this->pScreenEffect->EndPass();
		this->pScreenEffect->End();
	}*/
	

	this->pRenderColorLevel->EndRender();
	
	//if (this->bBBoSha){
	//	//ColorLevel Blur
	//	Blur(this->pRenderColorLevel, this->pRenderBlur, 5, 1.5f);
	//}

	if (bOutline){

		this->pRenderOutLine->BeginRender(0, 0x00000000);
		this->pScreenEffect->SetTechnique("Outline");
		this->pScreenEffect->SetTexture("NormalTex", pRenderTexture2->GetTexture());
		this->pScreenEffect->Begin(&pass, 0);
		this->pScreenEffect->BeginPass(0);
		pScreenQuad->Render();
		this->pScreenEffect->EndPass();
		this->pScreenEffect->End();
		this->pRenderOutLine->EndRender();

	}

	//
	// ������
	//
	this->pScreenEffect->SetTechnique("Additive");
	this->pScreenEffect->SetTexture("DiffuseTex", pRenderFinalLight->GetTexture());
	this->pScreenEffect->SetTexture("AdditiveTex", pRenderBlur->GetTexture());

	this->pScreenEffect->Begin(&pass, 0);

	//ù��° �н�
	this->pScreenEffect->BeginPass(0);
	pScreenQuad->Render();
	this->pScreenEffect->EndPass();

	//�ܰ���
	if (this->bOutline)
	{

		this->pScreenEffect->SetTexture("DiffuseTex", pRenderOutLine->GetTexture());
		this->pScreenEffect->CommitChanges();

	
		this->pScreenEffect->BeginPass(1);
		pScreenQuad->Render();
		this->pScreenEffect->EndPass();

	}


	this->pScreenEffect->End();



}



void cScene::SetEnvironmentTexture(string path)
{
	if (pEnvironmentEffect == NULL)
		pEnvironmentEffect = RESOURCE_EFFECT->GetResource("./Resources/Effects2/EnvironmentCube.fx");

	//������ ���õǾ��ִ°��� ������ �𸣴� ��������.
	SAFE_RELEASE(this->pEnvironmentCubeTex);

	//�ε�
	D3DXCreateCubeTextureFromFile(
		Device,
		path.c_str(),
		&this->pEnvironmentCubeTex);

}


void cScene::RenderEnvironment()
{
	if (this->pEnvironmentCubeTex == NULL)
		return;


	//ȯ�� �� �޽ð� ���õǾ����� �ʴٸ� 
	if (pEnvironmentSphere == NULL){

		//���Ǿ�
		D3DXCreateSphere(
			Device,
			3.0f,
			50,
			50,
			&pEnvironmentSphere,
			NULL);
	}

	//����ī�޶� ��ġ��.
	D3DXVECTOR3 pos = this->mainCamera.Transform.GetWorldPosition();

	D3DXMATRIXA16 matSphereWorld;
	D3DXMatrixTranslation(
		&matSphereWorld,
		pos.x, pos.y, pos.z);

	pEnvironmentEffect->SetMatrix("matWorld", &matSphereWorld);
	pEnvironmentEffect->SetMatrix("matViewProjection", &this->mainCamera.GetViewProjectionMatrix());

	//Cube Texture ����
	pEnvironmentEffect->SetTexture("Cube_Tex", this->pEnvironmentCubeTex);



	UINT pass;
	pEnvironmentEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pEnvironmentEffect->BeginPass(p);

		pEnvironmentSphere->DrawSubset(0);

		pEnvironmentEffect->EndPass();

	}


	pEnvironmentEffect->End();

}

void cScene::Blur(cRenderTexture* source, cRenderTexture* target, int iter, float blurScale)
{
	//���� onePixel �� halfPixel ���
	float remonePixelX = 0.0f;
	float remonePixelY = 0.0f;
	float remhalfPixelX = 0.0f;
	float remhalfPixelY = 0.0f;
	this->pScreenEffect->GetFloat("onePixelX", &remonePixelX);
	this->pScreenEffect->GetFloat("onePixelY", &remonePixelY);
	this->pScreenEffect->GetFloat("halfPixelX", &remhalfPixelX);
	this->pScreenEffect->GetFloat("halfPixelY", &remhalfPixelY);

	float onePixelX = 0.0f;
	float onePixelY = 0.0f;
	float halfPixelX = 0.0f;
	float halfPixelY = 0.0f;



	this->pScreenEffect->SetFloat("blurScale", blurScale);

	UINT pass;
	for (int i = 0; i < iter; i++)
	{
		//
		// BlurX ó��
		//
		this->pRenderBlurX->BeginRender(0, 0x00000000);
		this->pScreenEffect->SetTechnique("BlurX");

		if (i == 0){
			this->pScreenEffect->SetTexture("DiffuseTex", source->GetTexture());
			
			onePixelX = 1.0f / source->GetWidth();
			onePixelY = 1.0f / source->GetHeight();
			halfPixelX = onePixelX * 0.5f;
			halfPixelY = onePixelY * 0.5f;
			this->pScreenEffect->SetFloat("onePixelX", onePixelX);
			this->pScreenEffect->SetFloat("onePixelY", onePixelY);
			this->pScreenEffect->SetFloat("halfPixelX", halfPixelX);
			this->pScreenEffect->SetFloat("halfPixelY", halfPixelY);

		}
		else{
			this->pScreenEffect->SetTexture("DiffuseTex", pRenderBlurY->GetTexture());

			onePixelX = 1.0f / pRenderBlurY->GetWidth();
			onePixelY = 1.0f / pRenderBlurY->GetHeight();
			halfPixelX = onePixelX * 0.5f;
			halfPixelY = onePixelY * 0.5f;
			this->pScreenEffect->SetFloat("onePixelX", onePixelX);
			this->pScreenEffect->SetFloat("onePixelY", onePixelY);
			this->pScreenEffect->SetFloat("halfPixelX", halfPixelX);
			this->pScreenEffect->SetFloat("halfPixelY", halfPixelY);
		}

		this->pScreenEffect->Begin(&pass, 0);
		this->pScreenEffect->BeginPass(0);
		pScreenQuad->Render();
		this->pScreenEffect->EndPass();
		this->pScreenEffect->End();
		
		this->pRenderBlurX->EndRender();


		//
		// BlurY ó��
		//
		//�������̶��...
		if (i == iter - 1){
			if (target != NULL)
				target->BeginRender(0, 0x00000000);
		}
		else{
			this->pRenderBlurY->BeginRender(0, 0x00000000);
		}
		
		this->pScreenEffect->SetTechnique("BlurY");
		this->pScreenEffect->SetTexture("DiffuseTex", pRenderBlurX->GetTexture());
		
		onePixelX = 1.0f / pRenderBlurX->GetWidth();
		onePixelY = 1.0f / pRenderBlurX->GetHeight();
		halfPixelX = onePixelX * 0.5f;
		halfPixelY = onePixelY * 0.5f;
		this->pScreenEffect->SetFloat("onePixelX", onePixelX);
		this->pScreenEffect->SetFloat("onePixelY", onePixelY);
		this->pScreenEffect->SetFloat("halfPixelX", halfPixelX);
		this->pScreenEffect->SetFloat("halfPixelY", halfPixelY);


		this->pScreenEffect->Begin(&pass, 0);
		this->pScreenEffect->BeginPass(0);
		pScreenQuad->Render();
		this->pScreenEffect->EndPass();
		this->pScreenEffect->End();
		
		//�������̶��...
		if (i == iter - 1){
			if (target != NULL)
				target->EndRender();
		}
		else{
			this->pRenderBlurY->EndRender();
		}

	}

	//onePixel �� halfPixel ���󺹱�
	this->pScreenEffect->SetFloat("onePixelX", remonePixelX);
	this->pScreenEffect->SetFloat("onePixelY", remonePixelY);
	this->pScreenEffect->SetFloat("halfPixelX", remhalfPixelX);
	this->pScreenEffect->SetFloat("halfPixelY", remhalfPixelY);

}