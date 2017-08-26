#pragma once

#include "cCamera.h"
#include "cUIButton.h"

class cRenderTexture;
class cTexQuad;
class cUIObject;

class cScene : public iButtonDelegate
{
private:
	static LPD3DXEFFECT pEnvironmentEffect;	//ȯ��Effect
	static LPD3DXMESH pEnvironmentSphere;	//ȯ�汸

public:
	cCamera		mainCamera;		//���� �Ҵ�� ����ī�޶�
	LPDIRECT3DCUBETEXTURE9 pEnvironmentCubeTex;	//ȯ�� ť���

	cRenderTexture*		pRenderTexture0;		//���⿡�� diffuse ���� ������.
	cRenderTexture*		pRenderTexture1;		//���⿡�� specular ���� ������.
	cRenderTexture*		pRenderTexture2;		//���⿡�� normal ���� ������.
	cRenderTexture*		pRenderTexture3;		//���⿡�� depth ���� ������.
	cRenderTexture*		pRenderDiffuseLight;	//����� Diffuse ������ ó�� �̹����� �׷�����.
	cRenderTexture*		pRenderSpecularLight;	//����� Specular ������ ó�� �̹����� �׷�����.
	cRenderTexture*		pRenderFinalLight;		//������ ó����
	cRenderTexture*		pRenderColorLevel;		//�÷����� ó����


	cRenderTexture*		pRenderBlurX;				//�� X ���
	cRenderTexture*		pRenderBlurY;				//�� Y ���
	cRenderTexture*		pRenderBlur;				//�� ó���Ϸ�

	cRenderTexture*		pRenderOutLine;				//�ܰ���

	cRenderTexture*		pScreenTex;					//��ũ�� ����ȭ��...

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


	//4�ѻ��Լ��� �߻��Լ���......
	virtual HRESULT Scene_Init() = 0;
	virtual void Scene_Release() = 0;
	virtual void Scene_Update( float timeDelta ) = 0;
	
	virtual void Scene_Render() = 0;			//���� ����
	virtual void Scene_ShadowRender(){};		//������ ����
	virtual void Scene_LightRender() = 0;		//������ ����
	virtual void Scene_ParticleRender(){};		//������ ����
	virtual void Scene_GizmoRender(){};			//Gizmo ����

	virtual void Scene_RenderSprite(){}
	void SetEnvironmentTexture(string path);
	void RenderEnvironment();

	virtual void OnClick(cUIButton* pSender) = 0;

private:
	void Blur(cRenderTexture* source, cRenderTexture* target, int iter, float blurScale);

};

