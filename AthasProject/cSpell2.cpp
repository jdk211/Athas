#include "stdafx.h"
#include "cSpell2.h"
#include "cTest.h"
#include "cArthas.h"


cSpell2::cSpell2()
{
}


cSpell2::~cSpell2()
{
}

HRESULT cSpell2::init()
{
	//파티클 랜더러 셋팅
	this->pParticleEmitter = NULL;
	this->m_BoundBox0 = NULL;
	isFront = false;
	m_SpellCount = 0.0f;
	isAttack = false;;
	m_attackCount = 0.0f;
	return S_OK;
}

void cSpell2::Release()
{
}

void cSpell2::Update(float timeDelta, cCamera * camera, vector<cTest*> monster, cArthas * user, vector<cTest*> monster2)
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

	if (isAttack)
	{
		m_attackCount += TIME_MGR->GetFrameDeltaSec();
	}


	if (pParticleEmitter)   this->pParticleEmitter->Update(timeDelta);

	if (pParticleEmitter == NULL)
	{
		float distence = 100000.0f;
		if ((monster.size() > 0 || monster2.size() > 0) && !isFront)
		{
			if (user->GetStatus().mp > 50)
			{
				if (KEY_MGR->IsOnceDown('4'))
				{
					user->SetMp(user->GetStatus().mp - 50);
					isAttack = true;

					VEC_COLOR colors;
					colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.0f));
					colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.5f));
					colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.5f));
					colors.push_back(D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.5f));
					colors.push_back(D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.0f));



					VEC_SCALE scales;
					scales.push_back(2.0F);
					scales.push_back(4.0F);
					scales.push_back(20.0F);
					scales.push_back(40.0F);
					scales.push_back(100.0F);

					this->pParticleEmitter = new cParticleEmitter();
					this->pParticleEmitter->Init(
						15000,                                                      //총 파티클 량
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
					if (!mon)
					{
						if (monster2.size() > 0)
						{
							for each(auto p in monster2)
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
					}


					//this->pParticleEmitter->Transform.AttachTo();
					this->pParticleEmitter->StartEmissionPoint();
					D3DXVECTOR3 pos = mon->GetTransform()->GetWorldPosition();
					this->pParticleEmitter->Transform.SetWorldPosition(pos.x, pos.y + 30, pos.z);
					this->pParticleEmitter->Transform.AddChild(&m_BoundTrans0);

					this->m_BoundBox0 = new cBoundBox();
					this->m_BoundBox0->SetBound(&this->pParticleEmitter->Transform.GetWorldPosition(), &D3DXVECTOR3(5, 5, 5));
				}
			}
		}
	}
	if (this->pParticleEmitter) MoveRay(monster, camera, user);
	if (this->pParticleEmitter) MoveRay(monster2, camera, user);
}

void cSpell2::Render()
{
	if (m_BoundBox0)
	{
		//this->m_BoundBox0->RenderGizmos(&this->m_BoundTrans0);
	}
}

void cSpell2::Scene_ParticleRender()
{
	if (this->pParticleEmitter)   this->pParticleEmitter->Render();
}

void cSpell2::MoveRay(vector<cTest*> monster, cCamera * camera, cArthas * user)
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


		if (m_attackCount >= 4.9f)
		{
			for each(auto p in monster)
			{
				bool hit = PHYSICS_MGR->IsOverlapBox(
					&m_BoundTrans0,
					m_BoundBox0,
					p->GetBoundTrans0(),
					p->GetBoundBox0());

				if (hit)
				{
					p->SetCurrentHp(mon->GetStatus().hp - (user->GetStatus().mDam));
				}
			}
		}

		if (m_attackCount >= 5.0f)
		{
			

			pParticleEmitter = NULL;
			m_BoundBox0 = NULL;
			cTransform tempTrans;
			m_BoundTrans0 = tempTrans;
			isAttack = false;
			m_attackCount = 0.0f;
		}
	}
}
