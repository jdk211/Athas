#include "stdafx.h"
#include "cScene_Title.h"
#include "cSkinnedAnimation.h"
#include "cXMesh_Skinned.h"
#include "cTransform.h"
#include "cLight_Directional.h"
#include "cLight_Point.h"
#include "cUIImageView.h"
#include "cUITextView.h"
#include "cUIObject.h"

cScene_Title::cScene_Title()
{
}


cScene_Title::~cScene_Title()
{
}

HRESULT cScene_Title::Scene_Init()
{
	m_pUIRoot = NULL;
	m_pSprite = NULL;

	this->SetEnvironmentTexture("./Resources/CubeTextures/Black.dds");

	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matRotate;
	D3DXMATRIXA16 matScalling;

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&matScalling, 0.2f, 0.2f, 0.2f);
	D3DXMatrixRotationY(&matRotate, (D3DX_PI / 180) * 70);
	matWorld = matScalling * matRotate;

	m_pSkinnedMesh = RESOURCE_XMESHSKINNED->GetResource(
		"./Resources/XMeshSkinned/arthas/titleArthas.X", matWorld);
	this->m_pSkinnedAnim = new cSkinnedAnimation();
	this->m_pSkinnedAnim->Init(m_pSkinnedMesh);

	//방향성광원 준비
	pDirectionalLight = new cLight_Directional();
	pDirectionalLight->lightColor = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1);
	pDirectionalLight->InitShadowMap(4096, 100);
	pDirectionalLight->Transform.SetRotateWorld(
		90.0f * ONE_RAD, 0, 0);

	//점광원 준비
	for (int i = 0; i < LIGHT_NUM; i++)
	{

		cLight_Point* pPointLight = new cLight_Point();
		pPointLight->Transform.SetWorldPosition(D3DXVECTOR3(0, 1000, 0));

		pPointLight->maxRange = 50.0f;

		this->pPointLights[i] = pPointLight;

		//광원 색 랜덤
		D3DXCOLOR lightColor;
		lightColor.a = 1;
		lightColor.r = GetRandom(0.0f, 1.0f) * 3;
		lightColor.g = GetRandom(0.0f, 1.0f) * 3;
		lightColor.b = GetRandom(0.0f, 1.0f) * 3;

		this->pPointLights[i]->lightColor = lightColor;
	}

	mainCamera.Transform.SetWorldPosition(m_pTransForm.GetWorldPosition().x + 4,
		m_pTransForm.GetWorldPosition().y + 16,
		m_pTransForm.GetWorldPosition().z - 10);

	D3DXCreateSprite(Device, &m_pSprite);

	cUIImageView* ImgView = new cUIImageView;
	ImgView->SetTexture("./Resources/TitleUI/title_background.jpg");
	ImgView->SetPosition(D3DXVECTOR3(0, 0, 1));

	cUIButton* titleFront = new cUIButton;
	titleFront->SetTexture("./Resources/TitleUI/title_front1.png", "./Resources/TitleUI/title_front1.png", "./Resources/TitleUI/title_front1.png");
	titleFront->SetPosition(D3DXVECTOR3(WINSIZE_X - 530, 30, 0));
	titleFront->SetDelegate(this);
	titleFront->SetTag(0);

	cUIButton* newLoad = new cUIButton;
	newLoad->SetSize(ST_SIZEN(510, 250));
	newLoad->SetTexture("./Resources/TitleUI/gamestart_idle.png", "./Resources/TitleUI/gamestart_over.png", "./Resources/TitleUI/gamestart_click.png");
	newLoad->SetPosition(D3DXVECTOR3(WINSIZE_X - 400, 300, 0));
	newLoad->SetDelegate(this);
	newLoad->SetTag(1);

	cUIButton* saveLoad = new cUIButton;
	saveLoad->SetSize(ST_SIZEN(510, 250));
	saveLoad->SetTexture("./Resources/TitleUI/mapload_idle.png", "./Resources/TitleUI/mapload_over.png", "./Resources/TitleUI/mapload_click.png");
	saveLoad->SetPosition(D3DXVECTOR3(WINSIZE_X - 400, 380, 0));
	saveLoad->SetDelegate(this);
	saveLoad->SetTag(2);

	cUIButton* mapToolLoad = new cUIButton;
	mapToolLoad->SetSize(ST_SIZEN(510, 250));
	mapToolLoad->SetTexture("./Resources/TitleUI/maptool_idle.png", "./Resources/TitleUI/maptool_over.png", "./Resources/TitleUI/maptool_ciick.png");
	mapToolLoad->SetPosition(D3DXVECTOR3(WINSIZE_X - 400, 460, 0));
	mapToolLoad->SetDelegate(this);
	mapToolLoad->SetTag(3);

	m_pUIRoot = new cUIObject;
	m_pUIRoot->SetPosition(D3DXVECTOR3(0, 0, 0));
	m_pUIRoot->SetSize(ST_SIZEN(256, 50));

	m_pUIRoot->AddChild(ImgView);
	m_pUIRoot->AddChild(titleFront);
	m_pUIRoot->AddChild(newLoad);
	m_pUIRoot->AddChild(saveLoad);
	m_pUIRoot->AddChild(mapToolLoad);

	this->pParticleEmitter = NULL;
	this->pParticleEmitter2 = NULL;

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.0f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.5f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.5f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.5f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.0f));

	VEC_SCALE scales;
	//scales.push_back(1.0F);
	//scales.push_back(2.0F);
	scales.push_back(100.0F);
	scales.push_back(200.0F);
	scales.push_back(300.0F);

	this->pParticleEmitter = new cParticleEmitter();
	this->pParticleEmitter->Init(
		1000,                                                      //총 파티클 량
		100.0f,                                                      //초당 발생량
		1.4f,                                                      //파티클하나의 시간
		3.4f,
		D3DXVECTOR3(0, 0, 6),                                       //파티클 시작 속도
		D3DXVECTOR3(0, 10, 0),
		D3DXVECTOR3(0, 0, 0),                                          //파티클 가속
		D3DXVECTOR3(0, 0, 0),
		colors,                                                      //파티클 컬러 벡터배열
		scales,                                                      //파티클 스케일 컬러배열
		0.1F,                                                      //파티클 스케일
		0.1F,
		RESOURCE_TEXTURE->GetResource("./Resources/Textures/particle_1.png"),      //파티클 Texture
		true,                                                      //색태우니?
		PARTICLE_VELOCITY_LOCAL | PARTICLE_ACCELATE_WORLD,
		false
	);

	this->pParticleEmitter->StartEmissionPoint();
	this->pParticleEmitter->Transform.SetWorldPosition(m_pTransForm.GetWorldPosition().x + 0.8f,
		m_pTransForm.GetWorldPosition().y + 13.5,
		m_pTransForm.GetWorldPosition().z - 2.25);

	this->pParticleEmitter2 = new cParticleEmitter();
	this->pParticleEmitter2->Init(
		1000,                                                      //총 파티클 량
		100.0f,                                                      //초당 발생량
		1.4f,                                                      //파티클하나의 시간
		3.4f,
		D3DXVECTOR3(0, 0, 6),                                       //파티클 시작 속도
		D3DXVECTOR3(0, 10, 0),
		D3DXVECTOR3(0, 0, 0),                                          //파티클 가속
		D3DXVECTOR3(0, 0, 0),
		colors,                                                      //파티클 컬러 벡터배열
		scales,                                                      //파티클 스케일 컬러배열
		0.1F,                                                      //파티클 스케일
		0.1F,
		RESOURCE_TEXTURE->GetResource("./Resources/Textures/particle_1.png"),      //파티클 Texture
		true,                                                      //색태우니?
		PARTICLE_VELOCITY_LOCAL | PARTICLE_ACCELATE_WORLD,
		false
	);

	this->pParticleEmitter2->StartEmissionPoint();
	this->pParticleEmitter2->Transform.SetWorldPosition(m_pTransForm.GetWorldPosition().x - 1.0f,
		m_pTransForm.GetWorldPosition().y + 13.5,
		m_pTransForm.GetWorldPosition().z - 3.25);

	return S_OK;
}

void cScene_Title::Scene_Release()
{
	//SAFE_RELEASE(m_pSprite);
	//SAFE_RELEASE(m_pSkinnedMesh);
	//SAFE_RELEASE(m_pSkinnedAnim);

	//if (m_pUIRoot)
	//{
	//   m_pUIRoot->Destroy();
	//}
}

void cScene_Title::Scene_Update(float timeDelta)
{
	this->m_pSkinnedAnim->Update(timeDelta);

	if (m_pUIRoot)   m_pUIRoot->Update();

	if (pParticleEmitter)   this->pParticleEmitter->Update(timeDelta);
	if (pParticleEmitter2)   this->pParticleEmitter2->Update(timeDelta);

	mainCamera.Transform.MouseDefaultControl(timeDelta);
}

void cScene_Title::Scene_Render()
{
	this->m_pSkinnedAnim->Render(&m_pTransForm, &mainCamera);
	if (m_pUIRoot)m_pUIRoot->Render(m_pSprite);
	if (this->pParticleEmitter)   this->pParticleEmitter->Render();
	if (this->pParticleEmitter2)   this->pParticleEmitter2->Render();
}

void cScene_Title::Scene_LightRender()
{
	this->pDirectionalLight->Render();
	for (int i = 0; i < LIGHT_NUM; i++) {
		this->pPointLights[i]->Render();
	}
}

void cScene_Title::OnClick(cUIButton * pSender)
{
	switch (pSender->GetTag())
	{
	case 1:
		SCENE_MGR->ChangeScene("마을씬");
		SOUND_MGR->play("브금1", 0.05f);
		break;
	case 2:
		SCENE_MGR->ChangeScene("테스트씬");
		break;
	case 3:
		SCENE_MGR->ChangeScene("맵툴씬");
	default:
		break;
	}
}