#include "stdafx.h"
#include "cScene_Intro.h"
#include "cLight_Directional.h"


cScene_Intro::cScene_Intro()
{
}


cScene_Intro::~cScene_Intro()
{
}

HRESULT cScene_Intro::Scene_Init()
{
	//���⼺���� �غ�
	pDirectionalLight = new cLight_Directional();
	pDirectionalLight->lightColor = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1);
	pDirectionalLight->InitShadowMap(4096, 100);
	pDirectionalLight->Transform.SetRotateWorld(
		90.0f * ONE_RAD, 0, 0);

	//����=========����=============����===============
	_introVideo = new video;
	_introVideo->play("./Resources/RichKing.wmv", 0, 0, WINSIZE_X, WINSIZE_Y);
	_introVideo->setVolume(200);
	//����=========����=============����===============
	_initComplete = false;
	_startBackbuffer = false;

	return S_OK;
}

void cScene_Intro::Scene_Release()
{
	SAFE_DELETE(_introVideo);
	SAFE_DELETE(this->pDirectionalLight);
}

void cScene_Intro::Scene_Update(float timeDelta)
{
	if (!_introVideo->videoState())
	{
		SCENE_MGR->ChangeSceneWithLoading("Ÿ��Ʋ��", "�ε���");
	}

	if (KEY_MGR->IsOnceDown(VK_RETURN))
	{
		_introVideo->stop();
	}
	else if(KEY_MGR->IsOnceDown(VK_LBUTTON))
	{
		_introVideo->stop();
	}
}

void cScene_Intro::Scene_Render()
{
	this->pDirectionalLight->Render();
}

void cScene_Intro::Scene_LightRender()
{
}

void cScene_Intro::OnClick(cUIButton * pSender)
{
}
