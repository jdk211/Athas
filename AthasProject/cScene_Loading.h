#pragma once


#include "cScene.h"

class cFrustum;
class cLight_Directional;
class cLight_Point;

class cScene_Loading : public cScene
{
private:
	cFrustum* pDirectionalLightFrustum;

	//방향성 광원 준비	
	cLight_Directional*	pDirectionalLight;

	cUIObject*			m_pUIRoot;
	LPD3DXSPRITE		m_pSprite;

public:
	cScene_Loading(void);
	~cScene_Loading(void);

	virtual HRESULT Scene_Init();
	virtual void Scene_Release();
	virtual void Scene_Update(float timeDelta);
	virtual void Scene_Render();

	virtual void Scene_ShadowRender() override;
	virtual void Scene_LightRender() override;
	virtual void Scene_ParticleRender() override;
	virtual void Scene_GizmoRender() override;
	virtual void Scene_RenderSprite() override;

	virtual void OnClick(cUIButton* pSender) override;
};

