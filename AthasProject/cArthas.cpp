#include "stdafx.h"
#include "cArthas.h"
#include "cTest.h"
#include "cUserInfoUI.h"
#include "Shop.h"

cArthas::cArthas()
	:RayTrans(NULL),
	 m_fDegree(0.0f),
	 m_bfootwWark(false)
{
}


cArthas::~cArthas()
{
}

HRESULT cArthas::Init()
{
	//status 설정
	m_UserStatus.level = 1;
	m_UserStatus.str = 5;
	m_UserStatus.con = 5;
	m_UserStatus.wis = 5;
	m_UserStatus.hp = m_UserStatus.con * 50;
	m_UserStatus.maxHp = m_UserStatus.con * 50;
	m_UserStatus.mp = m_UserStatus.wis * 50;
	m_UserStatus.maxMp = m_UserStatus.wis * 50;
	m_UserStatus.mDam = m_UserStatus.wis * 3; //마법데미지
	m_UserStatus.pDam = m_UserStatus.str * 3; //물리데미지


	pRay.origin = D3DXVECTOR3(0, 0, 0);
	pRay.direction = D3DXVECTOR3(0, 0, 1);

	D3DXMATRIXA16 mat;
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, 0.04f, 0.04f, 0.04f);
	D3DXMATRIXA16 matRotation;
	D3DXMatrixRotationY(&matRotation, -((D3DX_PI / 180) * 85));
	mat = matScale * matRotation;
	m_pSkinnedMesh = RESOURCE_XMESHSKINNED->GetResource(
		"./Resources/XMeshSkinned/arthas/arthasStand.X", mat);
	this->m_pSkinnedAnim = new cSkinnedAnimation();
	this->m_pSkinnedAnim->Init(m_pSkinnedMesh);
	
	//m_pSkinnedStatic
	D3DXMATRIXA16 matTranslation;
	D3DXMatrixScaling(&matScale, 3.05f, 3.05f, 3.05f);
	D3DXMatrixTranslation(&matTranslation, 5.0f, 0.0f, 0.0f);
	mat = matScale * matTranslation;
	m_pSkinnedStatic = RESOURCE_XMESHSTATIC->GetResource(
		"./Resources/XMeshSkinned/arthas/sword.X", &mat);

	//"./Resources/XMeshSkinned/OBJ/home/home/home.X", &mat);


	this->m_pSkinnedAnim->AddBoneTransform("arthaslichking_unarmed_bone_50", &this->m_ItemTrans);

	m_pTransForm.AddChild(&m_BoundTrans0);
	m_ItemTrans.AddChild(&m_BoundTrans1);
	//RayTrans->SetWorldPosition(m_pTransForm.GetWorldPosition());
	this->m_BoundBox0 = new cBoundBox();
	this->m_BoundBox0->SetBound(&m_pTransForm.GetWorldPosition(), &D3DXVECTOR3(1.2, 4, 1.2));
	this->m_BoundBox1 = new cBoundBox();
	this->m_BoundBox1->SetBound(&D3DXVECTOR3(m_ItemTrans.GetWorldPosition().x + 35, m_ItemTrans.GetWorldPosition().y, m_ItemTrans.GetWorldPosition().z), &D3DXVECTOR3(50.5f, 8, 8.0f));

	m_SphereBox.SetBound(&m_pTransForm.GetWorldPosition(), &D3DXVECTOR3(10, 10, 10));

	m_UserState = STAND;
	m_UserStateAttack = NON;
	atkCount = 0.0f;

	m_pUI = new cUserInfoUI;
	m_pUI->Init();
	m_pUI->setUser(this);
	countAttack = false;
	return S_OK;
}

void cArthas::Release()
{
	m_pSkinnedAnim->Release();
	m_pSkinnedStatic->Release();
	SAFE_DELETE(m_BoundBox0);
	SAFE_DELETE(m_BoundBox1);
	m_pUI->Release();
}

void cArthas::Update(float timeDelta)
{
	m_UserStatus.maxMp = m_UserStatus.wis * 50;
	m_UserStatus.maxHp = m_UserStatus.con * 50;
	m_UserStatus.mDam = m_UserStatus.wis * 3; //마법데미지
	m_UserStatus.pDam = m_UserStatus.str * 3; //물리데미지

	if (m_UserStatus.hp > m_UserStatus.maxHp)
	{
		m_UserStatus.hp = m_UserStatus.maxHp;
	}
	if (m_UserStatus.mp > m_UserStatus.maxMp)
	{
		m_UserStatus.mp = m_UserStatus.maxMp;
	}
	if (KEY_MGR->IsOnceDown('0'))
	{
		Gold += 100000;
	}
	m_pUI->Update();
	m_pUI->SetHpBar(m_UserStatus.hp, m_UserStatus.maxHp);
	m_pUI->SetMpBar(m_UserStatus.mp, m_UserStatus.maxMp);

	AniControl();
	this->m_pSkinnedAnim->Update(timeDelta);
	this->m_pTransForm.DefaultControl(timeDelta);
}

void cArthas::Render(cTransform* pTrans, cCamera* pCam)
{
	if(pTrans != NULL)
		this->m_pSkinnedAnim->Render(pTrans, pCam);
	else
	{
		this->m_pSkinnedAnim->Render(&m_pTransForm, pCam);
	}

	//this->m_BoundBox0->RenderGizmos(&this->m_BoundTrans0);
	//this->m_BoundBox1->RenderGizmos(&this->m_BoundTrans1);
	//this->m_SphereBox.RenderGizmos(&this->m_BoundTrans0);
	this->m_pSkinnedStatic->Render(&m_ItemTrans, pCam);

	if (RayTrans != NULL)
	{
		GIZMO_MGR->DrawWireSphere(
			&RayTrans->GetWorldPosition(),
			0.5f,
			0xFFFF00FF);
	}
	m_pUI->Render();
}

void cArthas::AniControl()
{
	if (m_UserStateAttack == ATTACK || m_UserStateAttack == ATTACK2)
	{
		atkCount += TIME_MGR->GetFrameDeltaSec();
		//int a = 0;
	}


	if (KEY_MGR->IsOnceDown('1'))
	if (m_UserStateAttack == NON)
	{
		m_pSkinnedAnim->Play("attack1", 0.2f);
		m_UserStateAttack = ATTACK;

		SOUND_MGR->play("공격", 0.1f);
	}

	if (KEY_MGR->IsOnceDown('2'))
	{
		if (m_UserStateAttack == NON)
		{
			m_pSkinnedAnim->Play("attack2", 0.2f);
			m_UserStateAttack = ATTACK2;
		}
	}


	if (KEY_MGR->IsOnceDown('W') || KEY_MGR->IsOnceDown('S'))
	{
		if (m_UserStateAttack == NON)	m_pSkinnedAnim->Play("run", 0.2f);
		m_UserState = RUN; 
		RayTrans = NULL;
		m_bfootwWark = true;
		if (m_bfootwWark == true)
		{
			SOUND_MGR->play("발소리", 0.2f);
		}
	}

	if(KEY_MGR->IsOnceUp('W') || KEY_MGR->IsOnceUp('S'))
	{
		if (m_UserStateAttack == NON)	m_pSkinnedAnim->Play("stand", 0.2f);
		m_UserState = STAND;
		m_bfootwWark = false;
		if (m_bfootwWark == false)
		{
			SOUND_MGR->stop("발소리");
		}
	}

	if (m_UserStateAttack == ATTACK)
	{
		if (atkCount > 2.0f)
		{
			if (m_UserState == STAND)
			{
				m_pSkinnedAnim->Play("stand", 0.2f);
			}
			if (m_UserState == RUN)
			{
				m_pSkinnedAnim->Play("run", 0.2f);
			}
			m_UserStateAttack = NON;
			atkCount = 0;
			countAttack = false;
		}
	}

	if (m_UserStateAttack == ATTACK2)
	{
		if (atkCount > 3.0f)
		{
			if (m_UserState == STAND)
			{
				m_pSkinnedAnim->Play("stand", 0.2f);
			}
			if (m_UserState == RUN)
			{
				m_pSkinnedAnim->Play("run", 0.2f);
			}
			m_UserStateAttack = NON;
			atkCount = 0;
			countAttack = false;
		}
	}
	
	if (m_UserStatus.hp <= 0.0f)
	{
		if (m_UserState == STAND)
		{
			m_pSkinnedAnim->Play("death", 0.2f);
			m_UserState = DEATH;
			m_UserStateAttack = NON;
		}
	}
}

void cArthas::MoveRay(cTerrain * terrain, cCamera* camera)
{
	if (KEY_MGR->IsOnceDown(VK_SPACE)) {

		POINT ptMouse = GetMousePos();
		//클릭한지점 월드 레이
		camera->GetWorldRay(&this->pRay, ptMouse);


		D3DXVECTOR3 hitPos;
		//터레인 히트 위치 구함
		RayTrans = new cTransform;
		if (terrain->IsIntersectRay(
			&hitPos, &this->pRay))
		{
			this->RayTrans->SetWorldPosition(hitPos);
			m_pSkinnedAnim->Play("run", 0.2f); // 마우스가 클릭됬을때 움직이는 모션추가
		}
	}

	if (RayTrans == NULL) return;

	if (MYUTIL::Vector3Distance(&RayTrans->GetWorldPosition(), &m_pTransForm.GetWorldPosition()) >= 3.5f)
	{
		D3DXVECTOR3 vecMove;

		float a = MYUTIL::Vector3Distance(&RayTrans->GetWorldPosition(), &m_pTransForm.GetWorldPosition());

		vecMove = RayTrans->GetWorldPosition() - m_pTransForm.GetWorldPosition(); // from에서 to 를빼주면 방향 백터
		D3DXVec3Normalize(&vecMove, &vecMove);

		D3DXMATRIXA16 tempRot;

		D3DXMatrixLookAtLH(&tempRot,
			&D3DXVECTOR3(0, 0, 0),
			&vecMove,
			&D3DXVECTOR3(0, 1, 0));
		D3DXMatrixTranspose(&tempRot, &tempRot);
		m_pTransForm.SetRotateWorld(tempRot);

		m_pTransForm.SetWorldPosition(m_pTransForm.GetWorldPosition() + vecMove * 0.1f);
	}
	
	else
	{
		D3DXVECTOR3 vecMove;

		float a = MYUTIL::Vector3Distance(&RayTrans->GetWorldPosition(), &m_pTransForm.GetWorldPosition());

		vecMove = RayTrans->GetWorldPosition() - m_pTransForm.GetWorldPosition(); // from에서 to 를빼주면 방향 백터
		D3DXVec3Normalize(&vecMove, &vecMove);

		D3DXMATRIXA16 tempRot;

		D3DXMatrixLookAtLH(&tempRot,
			&D3DXVECTOR3(0, 0, 0),
			&vecMove,
			&D3DXVECTOR3(0, 1, 0));
		D3DXMatrixTranspose(&tempRot, &tempRot);
		m_pTransForm.SetRotateWorld(tempRot);
	}

}
//20170223 추가
void cArthas::MoveRay(cTest * mon, cCamera * camera)
{
	D3DXVECTOR3 vecMove;

	vecMove = mon->GetTransform()->GetWorldPosition() - m_pTransForm.GetWorldPosition(); // from에서 to 를빼주면 방향 백터
	D3DXVec3Normalize(&vecMove, &vecMove);

	D3DXMATRIXA16 tempRot;

	D3DXMatrixLookAtLH(&tempRot,
		&D3DXVECTOR3(0, 0, 0),
		&vecMove,
		&D3DXVECTOR3(0, 1, 0));
	D3DXMatrixTranspose(&tempRot, &tempRot);
	m_pTransForm.SetRotateWorld(tempRot);
}

void cArthas::hitMonster(cTest* monster)
{
	;
}

void cArthas::hitMonster(vector<cTest*> monster)
{
	bool hit;

	if (monster.size() > 0)
	{
		for each(auto p in monster)
		{
			PHYSICS_MGR->IsBlockingBox(
				&m_BoundTrans0,
				m_BoundBox0,
				p->GetBoundTrans0(),
				p->GetBoundBox0(), 0.0f);

			hit = PHYSICS_MGR->IsOverlapBox(
				&m_BoundTrans1,
				m_BoundBox1,
				p->GetBoundTrans0(),
				p->GetBoundBox0());


			if (hit)
			{
				switch (m_UserStateAttack)
				{
				case ATTACK :
					if (atkCount >= 0.7f && !countAttack)
					{
						countAttack = true;
						p->SetCurrentHp(p->GetStatus().hp - m_UserStatus.pDam);
						int b = p->GetStatus().hp;
						int a = 0;
					}
					break;
				case ATTACK2 :
					countAttack = true;
					p->SetCurrentHp(p->GetStatus().hp - m_UserStatus.pDam / 4);
					int b = p->GetStatus().hp;
					int a = 0;
					break;
				}
			}

		}
	}

}
