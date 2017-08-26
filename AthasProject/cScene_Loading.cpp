#include "StdAfx.h"

#include "cScene_Loading.h"
#include "cFrustum.h"
#include "cLight_Directional.h"
#include "cLight_Point.h"
#include "cUIImageView.h"
#include "cUITextView.h"
#include "cUIObject.h"


cScene_Loading::cScene_Loading(void)
{
}


cScene_Loading::~cScene_Loading(void)
{
}

HRESULT cScene_Loading::Scene_Init()
{
	m_pUIRoot = NULL;
	m_pSprite = NULL;

	this->mainCamera.Far = 300.0f;

	//방향성광원 준비
	pDirectionalLight = new cLight_Directional();
	pDirectionalLight->lightColor = D3DXCOLOR(1, 1, 1, 1);
	pDirectionalLight->InitShadowMap(4096, 100);
	pDirectionalLight->Transform.SetRotateWorld(
		90.0f * ONE_RAD, 0, 0);

	//스케일 0.01
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, 0.005f, 0.005f, 0.005f);


	this->mainCamera.Far = 100.0f;

	this->bFog = true;

	D3DXCreateSprite(Device, &m_pSprite);

	cUIImageView* ImgView = new cUIImageView;
	ImgView->SetTexture("./Resources/Loading.jpg");
	ImgView->SetPosition(D3DXVECTOR3(0, 0, 1));

	m_pUIRoot = new cUIObject;
	m_pUIRoot->SetPosition(D3DXVECTOR3(0, 0, 0));
	m_pUIRoot->SetSize(ST_SIZEN(256, 50));

	m_pUIRoot->AddChild(ImgView);

	return S_OK;
}


void cScene_Loading::Scene_Release()
{
	SAFE_DELETE(this->pDirectionalLight);
}

void cScene_Loading::Scene_Update(float timeDelta)
{
	if (m_pUIRoot)	m_pUIRoot->Update();
}


void cScene_Loading::Scene_Render()
{
	if (m_pUIRoot)m_pUIRoot->Render(m_pSprite);
}

void cScene_Loading::Scene_ShadowRender()
{
	//방향성 광원에 대한 그림자 맵을 준비한다.
	this->pDirectionalLight->BeginShadowRender(&this->mainCamera);


	//방향성 광원에 대한 그림자 맵 랜더 종료
	this->pDirectionalLight->EndShdowRender();


}

void cScene_Loading::Scene_LightRender()
{
	this->pDirectionalLight->Render();
}


void cScene_Loading::Scene_ParticleRender()
{
}

void cScene_Loading::Scene_GizmoRender()
{
}

void cScene_Loading::Scene_RenderSprite()
{
	char szTemp[128];
	sprintf_s(szTemp, "Loading...", SCENE_MGR->fLoadingProgress);
	DXFONT_MGR->PrintTextShadow(szTemp, WINSIZE_X / 2 - 50, WINSIZE_Y - 100, 0xFFFF0000);
}

void cScene_Loading::OnClick(cUIButton * pSender)
{
}
