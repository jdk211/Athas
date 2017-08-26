#pragma once

#include "cScene.h"
#include "cUI_Billboard.h"
#include "cParticleEmitter.h"

class cSkinnedAnimation;
class cXMesh_Skinned;
class cTransform;
class cLight_Directional;
class cLight_Point;

#define LIGHT_NUM 1

class cScene_Title : public cScene
{
private:
	cXMesh_Skinned*      m_pSkinnedMesh;
	cSkinnedAnimation*   m_pSkinnedAnim;
	cTransform         m_pTransForm;

	cLight_Directional*   pDirectionalLight;
	cLight_Point*      pPointLights[LIGHT_NUM];

	cUIObject*         m_pUIRoot;
	LPD3DXSPRITE      m_pSprite;

	cParticleEmitter*   pParticleEmitter;
	cParticleEmitter*   pParticleEmitter2;

public:
	cScene_Title();
	~cScene_Title();

	virtual HRESULT Scene_Init();
	virtual void Scene_Release();
	virtual void Scene_Update(float timeDelta);
	virtual void Scene_Render();
	virtual void Scene_LightRender();

	virtual void OnClick(cUIButton* pSender) override;
};
