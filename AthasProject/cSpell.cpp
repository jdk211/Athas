#include "stdafx.h"
#include "cSpell.h"
#include "cTest.h"
#include "cArthas.h"


cSpell::cSpell()
{
}


cSpell::~cSpell()
{
}

HRESULT cSpell::init()
{
	//파티클 랜더러 셋팅
	this->pParticleEmitter = NULL;
	this->m_BoundBox0 = NULL;
	isFront = false;
	m_SpellCount = 0.0f;
	return S_OK;
}

void cSpell::Release()
{
}

void cSpell::Update(float timeDelta, cCamera* camera, vector<cTest*> monster, cArthas* user, vector<cTest*> monster2)
{
	if (isFront)
	{
		m_SpellCount += TIME_MGR->GetFrameDeltaSec();
		if (m_SpellCount > 6.0f)
		{
			isFront = false;
			m_SpellCount = 0.0f;
		}
	}



	if (pParticleEmitter)   this->pParticleEmitter->Update(timeDelta);

	if (pParticleEmitter == NULL)
	{
		if ((monster.size() > 0  || monster2.size() > 0)&& !isFront)
		{
			if (user->GetStatus().mp > 30)
			{
				if (KEY_MGR->IsOnceDown('3'))
				{
					user->SetMp(user->GetStatus().mp - 30);
					VEC_COLOR colors;
					colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));
					colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f));
					colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f));
					colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));
					colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));



					VEC_SCALE scales;
					//scales.push_back(1.0F);
					//scales.push_back(2.0F);
					scales.push_back(10.0F);
					scales.push_back(20.0F);
					scales.push_back(50.0F);

					this->pParticleEmitter = new cParticleEmitter();
					this->pParticleEmitter->Init(
						70,                                                      //총 파티클 량
						100.0f,                                                      //초당 발생량
						1.4f,                                                      //파티클하나의 시간
						3.4f,
						D3DXVECTOR3(0, 0, 6),                                       //파티클 시작 속도
						D3DXVECTOR3(0, 0, 5.0f),
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
					//this->pParticleEmitter->Transform.AttachTo();
					this->pParticleEmitter->StartEmissionPoint();
					D3DXVECTOR3 pos = user->GetTransform()->GetWorldPosition();
					this->pParticleEmitter->Transform.SetWorldPosition(pos.x, pos.y + 1, pos.z);
					this->pParticleEmitter->Transform.AddChild(&m_BoundTrans0);

					this->m_BoundBox0 = new cBoundBox();
					this->m_BoundBox0->SetBound(&this->pParticleEmitter->Transform.GetWorldPosition(), &D3DXVECTOR3(1, 1, 1));
				}
			}
		}
			
	}
	if (this->pParticleEmitter) MoveRay(monster, camera, user);
	if (this->pParticleEmitter) MoveRay(monster2, camera, user);
}

void cSpell::Render()
{
	if (m_BoundBox0)
	{
	//	this->m_BoundBox0->RenderGizmos(&this->m_BoundTrans0);
	}
}

void cSpell::Scene_ParticleRender()
{
	if (this->pParticleEmitter)   this->pParticleEmitter->Render();
}

void cSpell::MoveRay(vector<cTest*> monster, cCamera * camera, cArthas* user)
{
	float distence = 100000.0f;
	cTest* mon = NULL;
	if (monster.size() > 0)
	{
		for each(auto p in monster)
		{
			float tempDistence = 0.0f;
			tempDistence = MYUTIL::Vector3Distance(&pParticleEmitter->Transform.GetWorldPosition(), &p->GetTransform()->GetWorldPosition());
			if (tempDistence < distence)
			{
				distence = tempDistence;
				mon = p;
			}
		}
	}

	if (mon)
	{
		if (!isFront)
		{
			user->MoveRay(mon, camera);
			isFront = true;
		}
		

		D3DXVECTOR3 vecMove;
		D3DXVECTOR3 monPos = mon->GetTransform()->GetWorldPosition();
		monPos.y = mon->GetTransform()->GetWorldPosition().y + 2.0f;
		vecMove = monPos - pParticleEmitter->Transform.GetWorldPosition();
		D3DXVec3Normalize(&vecMove, &vecMove);

		D3DXMATRIXA16 tempRot;

		D3DXMatrixLookAtLH(&tempRot,
			&D3DXVECTOR3(0, 0, 0),
			&vecMove,
			&D3DXVECTOR3(0, 1, 0));
		D3DXMatrixTranspose(&tempRot, &tempRot);
		pParticleEmitter->Transform.SetRotateWorld(tempRot);

		pParticleEmitter->Transform.SetWorldPosition(pParticleEmitter->Transform.GetWorldPosition() + vecMove * 0.15f);
		

		bool hit = PHYSICS_MGR->IsOverlapBox(
			&m_BoundTrans0,
			m_BoundBox0,
			mon->GetBoundTrans0(),
			mon->GetBoundBox0());

		if (hit)
		{
			pParticleEmitter = NULL;
			mon->SetCurrentHp(mon->GetStatus().hp - (user->GetStatus().mDam) * 5);
			m_BoundBox0 = NULL;
			cTransform tempTrans;
			m_BoundTrans0 = tempTrans; 
			//isFront = false;
		}
	}

	/*else if (mon == NULL)
	{
		D3DXVECTOR3 vecMove;
		vecMove = D3DXVECTOR3(0, 0, 1);
		D3DXVec3Normalize(&vecMove, &vecMove);

		D3DXMATRIXA16 tempRot;

		D3DXMatrixLookAtLH(&tempRot,
			&D3DXVECTOR3(0, 0, 0),
			&vecMove,
			&D3DXVECTOR3(0, 1, 0));
		D3DXMatrixTranspose(&tempRot, &tempRot);
		pParticleEmitter->Transform.SetRotateWorld(tempRot);

		pParticleEmitter->Transform.SetWorldPosition(pParticleEmitter->Transform.GetWorldPosition() + vecMove * 0.15f);
	}*/


}