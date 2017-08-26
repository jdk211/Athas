#pragma once

#include "cCamera.h"
#include "cUIButton.h"

class cRenderTexture;
class cTexQuad;
class cUIObject;

class cScene : public iButtonDelegate
{
private:
	static LPD3DXEFFECT pEnvironmentEffect;	//환경Effect
	static LPD3DXMESH pEnvironmentSphere;	//환경구

public:
	cCamera		mainCamera;		//씬에 할당된 메인카메라
	LPDIRECT3DCUBETEXTURE9 pEnvironmentCubeTex;	//환경 큐브맵

	cRenderTexture*		pRenderTexture0;		//여기에는 diffuse 값이 써진다.
	cRenderTexture*		pRenderTexture1;		//여기에는 specular 값이 써진다.
	cRenderTexture*		pRenderTexture2;		//여기에는 normal 값이 써진다.
	cRenderTexture*		pRenderTexture3;		//여기에는 depth 값이 써진다.
	cRenderTexture*		pRenderDiffuseLight;	//여긴는 Diffuse 라이팅 처리 이미지가 그려진다.
	cRenderTexture*		pRenderSpecularLight;	//여긴는 Specular 라이팅 처리 이미지가 그려진다.
	cRenderTexture*		pRenderFinalLight;		//라이팅 처리끝
	cRenderTexture*		pRenderColorLevel;		//컬러레벨 처리끝


	cRenderTexture*		pRenderBlurX;				//블러 X 결과
	cRenderTexture*		pRenderBlurY;				//블러 Y 결과
	cRenderTexture*		pRenderBlur;				//블러 처리완료

	cRenderTexture*		pRenderOutLine;				//외각선

	cRenderTexture*		pScreenTex;					//스크린 최종화면...

	cTexQuad*			pScreenQuad;
	LPD3DXEFFECT		pScreenEffect;

	bool				bBBoSha;
	bool				bToon;
	bool				bOutline;
	bool				bFog;

public:
	cScene(void);
	~cScene(void);

	HRESULT Init();
	void Release();
	void Update(float timeDelta);
	void Render();


	//4총사함수를 추상함수로......
	virtual HRESULT Scene_Init() = 0;
	virtual void Scene_Release() = 0;
	virtual void Scene_Update( float timeDelta ) = 0;
	
	virtual void Scene_Render() = 0;			//기하 랜더
	virtual void Scene_ShadowRender(){};		//쉐도우 랜더
	virtual void Scene_LightRender() = 0;		//라이팅 랜더
	virtual void Scene_ParticleRender(){};		//라이팅 랜더
	virtual void Scene_GizmoRender(){};			//Gizmo 랜더

	virtual void Scene_RenderSprite(){}
	void SetEnvironmentTexture(string path);
	void RenderEnvironment();

	virtual void OnClick(cUIButton* pSender) = 0;

private:
	void Blur(cRenderTexture* source, cRenderTexture* target, int iter, float blurScale);

};

