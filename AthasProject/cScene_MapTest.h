#pragma once
#include "cScene.h"
#include "cBoundBox.h"

class cLight_Directional;
class cTerrain;

class cScene_MapTest : public cScene
{
public:
	typedef struct ST_BUILD
	{
		cXMesh_Static*	skin;
		char*			skinPath;
		cTransform		trans;
		D3DXVECTOR3		position;
		cBoundBox*		box;
	}STBUILDE, *LPSTBUILD;

private:
	cTerrain*				pTerrain;
	cLight_Directional*		m_pDirectionalLight;
	vector<LPSTBUILD>		m_vecSetBuild;

public:
	cScene_MapTest();
	~cScene_MapTest();

	virtual HRESULT Scene_Init();
	virtual void Scene_Release();
	virtual void Scene_Update(float timeDelta);
	virtual void Scene_Render();
	virtual void Scene_LightRender();

	void CreateBuild();

	virtual void OnClick(cUIButton* pSender) override;
};
