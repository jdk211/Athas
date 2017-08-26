#pragma once
#include "cScene.h"
#include "video.h"

class cLight_Directional;

class cScene_Intro : public cScene
{
private:
	cLight_Directional*	pDirectionalLight;

	video* _introVideo;
	bool _startBackbuffer;
	bool _initComplete;

public:
	cScene_Intro();
	~cScene_Intro();

	virtual HRESULT Scene_Init();
	virtual void Scene_Release();
	virtual void Scene_Update(float timeDelta);
	virtual void Scene_Render();
	virtual void Scene_LightRender();

	virtual void OnClick(cUIButton* pSender) override;

	bool getStartBackBuffer() { return _startBackbuffer; }
};

