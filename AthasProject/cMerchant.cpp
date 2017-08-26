#include "stdafx.h"
#include "cMerchant.h"
#include "cParticleEmitter.h"
#include "cArthas.h"


cMerchant::cMerchant()
{
}


cMerchant::~cMerchant()
{
}

HRESULT cMerchant::Init(float x, float y, float z)
{
	m_pTransForm.SetWorldPosition(x, y, z);
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, 0.05f, 0.05f, 0.05f);
	D3DXMATRIXA16 matRotation;
	D3DXMatrixRotationY(&matRotation, -((D3DX_PI / 180) * 270));
	D3DXMATRIXA16 matTrans;

	//D3DXMatrixTranslation(&matTrans, x, y, z);
	mat = matScale * matRotation; //* matTrans;
	m_pSkinnedMesh = RESOURCE_XMESHSKINNED->GetResource(
		"./Resources/XMeshSkinned/kelthuzard/kelthuzard.X", mat);
	this->m_pSkinnedAnim = new cSkinnedAnimation();
	this->m_pSkinnedAnim->Init(m_pSkinnedMesh);

	m_pTransForm.AddChild(&m_BoundTrans0);
	this->m_BoundBox0 = new cBoundBox();
	this->m_BoundBox0->SetBound(&m_pTransForm.GetWorldPosition(), &D3DXVECTOR3(1.2, 4, 1.2));

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f));
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(30);

	//파티클 랜더러 셋팅
	this->pParticleEmitter = new cParticleEmitter();
	this->pParticleEmitter->Init(
		200,
		100.0f,
		1.4f,
		3.4f,
		D3DXVECTOR3(0, 5, 0.0f),
		D3DXVECTOR3(0, 0, 0.0f),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 0),
		colors,
		scales,
		0.1F,
		0.1F,
		RESOURCE_TEXTURE->GetResource("./Resources/Textures/particle_0.png"),
		true,
		PARTICLE_VELOCITY_LOCAL | PARTICLE_ACCELATE_WORLD,
		false
	);

	this->pParticleEmitter->StartEmissionPoint();
	this->pParticleEmitter->Transform.SetWorldPosition(D3DXVECTOR3(x,y + 2, z));
	//m_pTransForm.AddChild(&pParticleEmitter->Transform);
	return S_OK;
}

void cMerchant::Release()
{
	m_pSkinnedAnim->Release();
	SAFE_DELETE(m_BoundBox0);
}

void cMerchant::Update(float timeDelta, cArthas* user)
{
	distence = MYUTIL::Vector3Distance(&user->GetTransform()->GetWorldPosition(), &m_pTransForm.GetWorldPosition());
	this->m_pSkinnedAnim->Update(timeDelta);
	this->pParticleEmitter->Update(timeDelta);
}

void cMerchant::Render(cTransform * pTrans, cCamera * pCam)
{
	if (pTrans != NULL)
		this->m_pSkinnedAnim->Render(pTrans, pCam);
	else
	{
		this->m_pSkinnedAnim->Render(&m_pTransForm, pCam);
	}
}

void cMerchant::particleRender(cArthas* user)
{
	if(distence < 10.0f)
		this->pParticleEmitter->Render();
}

