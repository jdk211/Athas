#include "StdAfx.h"
#include "cScene.h"
#include "cTexQuad.h" 
#include "cRenderTexture.h"
#include "cLight.h"
#include "cUIObject.h"

//환경Effect
LPD3DXEFFECT cScene::pEnvironmentEffect = NULL;

//환경구
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
	//생성되는 Texture 의 해상도
	int width = WINSIZE_X;
	int height = WINSIZE_Y;

	//랜더 Texture 생성
	this->pRenderTexture0 = new cRenderTexture();
	this->pRenderTexture0->Init(width, height, D3DFMT_A8R8G8B8);
	this->pRenderTexture1 = new cRenderTexture();
	this->pRenderTexture1->Init(width, height, D3DFMT_A8R8G8B8);
	this->pRenderTexture2 = new cRenderTexture();
	this->pRenderTexture2->Init(width, height, D3DFMT_G16R16F);		//R 16 비트 G 16 비트
	this->pRenderTexture3 = new cRenderTexture();
	this->pRenderTexture3->Init(width, height, D3DFMT_R32F);		//Depth Red 만 32 비트 쓰것다...

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




	//ScreenQuad 생성
	this->pScreenQuad = new cTexQuad();

	//ScreenEffect 로딩
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

	//RenderTexture 에 그려라..
	//DirectX9 에서는 멀티 RenderTexture 가 최대 4장까지 세팅이 가능..
	this->pRenderTexture0->BeginRender(0, 0x00000000);		//Diffuse Color
	this->pRenderTexture1->BeginRender(1, 0x00000000);		//Normal
	this->pRenderTexture2->BeginRender(2, 0x00000000);		//WorldPosition
	this->pRenderTexture3->BeginRender(3, 0xFFFFFFFF);		//Depth 가장 먼걸로 초기화 해야하니  화이트로 초기화....


	//현제 씬의 환경부터 그리고 시작한다.
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

	//라이팅 그릴때... 뎁스남기고 컬러버퍼만지운다.
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
	// ScreenQuad 그리자...
	//
	this->pRenderFinalLight->BeginRender(0, 0xFFFFFFFF, D3DCLEAR_TARGET );
	//Pixel 하나에대한 UV 변위량
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

	//Alpha 가 들어간 파티클을 그리자.
	Scene_ParticleRender();

	//화면 모두 구성한다음 Gizmo 그리자.
	this->Scene_GizmoRender();

	this->pRenderFinalLight->EndRender();



	//안개
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

	//화면에 Quad 를 그린다.
	//회색그리기
	//this->pScreenEffect->SetTechnique("GrayScale");
	
	
	//컬러레벨
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
	// 마지막
	//
	this->pScreenEffect->SetTechnique("Additive");
	this->pScreenEffect->SetTexture("DiffuseTex", pRenderFinalLight->GetTexture());
	this->pScreenEffect->SetTexture("AdditiveTex", pRenderBlur->GetTexture());

	this->pScreenEffect->Begin(&pass, 0);

	//첫번째 패스
	this->pScreenEffect->BeginPass(0);
	pScreenQuad->Render();
	this->pScreenEffect->EndPass();

	//외각선
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

	//기존에 셋팅되어있는것이 있을지 모르니 안전해제.
	SAFE_RELEASE(this->pEnvironmentCubeTex);

	//로딩
	D3DXCreateCubeTextureFromFile(
		Device,
		path.c_str(),
		&this->pEnvironmentCubeTex);

}


void cScene::RenderEnvironment()
{
	if (this->pEnvironmentCubeTex == NULL)
		return;


	//환경 구 메시가 셋팅되어있지 않다면 
	if (pEnvironmentSphere == NULL){

		//스피어
		D3DXCreateSphere(
			Device,
			3.0f,
			50,
			50,
			&pEnvironmentSphere,
			NULL);
	}

	//메인카메라 위치에.
	D3DXVECTOR3 pos = this->mainCamera.Transform.GetWorldPosition();

	D3DXMATRIXA16 matSphereWorld;
	D3DXMatrixTranslation(
		&matSphereWorld,
		pos.x, pos.y, pos.z);

	pEnvironmentEffect->SetMatrix("matWorld", &matSphereWorld);
	pEnvironmentEffect->SetMatrix("matViewProjection", &this->mainCamera.GetViewProjectionMatrix());

	//Cube Texture 셋팅
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
	//이전 onePixel 과 halfPixel 기억
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
		// BlurX 처리
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
		// BlurY 처리
		//
		//마지막이라면...
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
		
		//마지막이라면...
		if (i == iter - 1){
			if (target != NULL)
				target->EndRender();
		}
		else{
			this->pRenderBlurY->EndRender();
		}

	}

	//onePixel 과 halfPixel 원상복귀
	this->pScreenEffect->SetFloat("onePixelX", remonePixelX);
	this->pScreenEffect->SetFloat("onePixelY", remonePixelY);
	this->pScreenEffect->SetFloat("halfPixelX", remhalfPixelX);
	this->pScreenEffect->SetFloat("halfPixelY", remhalfPixelY);

}