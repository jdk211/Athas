#include "stdafx.h"
#include "cTest.h"
#include "cArthas.h"

cTest::cTest()
{
	atkCount = 0.0f;
	deathCount = 0.0f;
}

cTest::~cTest()
{
}

HRESULT cTest::Init()
{
	m_UserStatus.pDam = 5;
	m_UserStatus.hp = 50;
	m_UserStatus.maxHp = 50;
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

	m_pUser = new cArthas;

	//m_pSkinnedStatic
	D3DXMATRIXA16 matTranslation;
	D3DXMatrixScaling(&matScale, 3.05f, 3.05f, 3.05f);
	D3DXMatrixTranslation(&matTranslation, 5.0f, 0.0f, 0.0f);
	mat = matScale * matTranslation;
	m_pSkinnedStatic = RESOURCE_XMESHSTATIC->GetResource(
		"./Resources/XMeshSkinned/arthas/sword.X", &mat);


	this->m_pSkinnedAnim->AddBoneTransform("arthaslichking_unarmed_bone_50", &this->m_ItemTrans);

	m_pTransForm.AddChild(&m_BoundTrans0);
	m_ItemTrans.AddChild(&m_BoundTrans1);

	this->m_BoundBox0 = new cBoundBox();
	this->m_BoundBox0->SetBound(&m_pTransForm.GetWorldPosition(), &D3DXVECTOR3(1.2, 4, 1.2));
	this->m_BoundBox1 = new cBoundBox();
	this->m_BoundBox1->SetBound(&D3DXVECTOR3(m_ItemTrans.GetWorldPosition().x + 35, m_ItemTrans.GetWorldPosition().y, m_ItemTrans.GetWorldPosition().z), &D3DXVECTOR3(50.5f, 8, 8.0f));


	m_UserState = STAND;
	m_UserStateAttack = NON;
	atkCount = 0.0f;
	countAttack = false;
	isDeath = false;


	HPPROGRESS = new cUI_Billboard;
	HPPROGRESS->Init("./Resources/UI/hpBar.png", 2.0f, 0.2f);

	return S_OK;
}

HRESULT cTest::Init(string fName, D3DXVECTOR3 uScale, D3DXVECTOR3 sScale, string sFName, string bName)
{
	D3DXMATRIXA16 mat;
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, uScale.x, uScale.y, uScale.z);
	D3DXMATRIXA16 matRotation;
	D3DXMatrixRotationY(&matRotation, -((D3DX_PI / 180) * 85));
	mat = matScale * matRotation;
	m_pSkinnedMesh = RESOURCE_XMESHSKINNED->GetResource(
		fName, mat);
	this->m_pSkinnedAnim = new cSkinnedAnimation();
	this->m_pSkinnedAnim->Init(m_pSkinnedMesh);

	m_pUser = new cArthas;

	//m_pSkinnedStatic
	D3DXMATRIXA16 matTranslation;
	D3DXMatrixScaling(&matScale, sScale.x, sScale.y, sScale.z);
	D3DXMatrixTranslation(&matTranslation, 5.0f, 0.0f, 0.0f);
	mat = matScale * matTranslation;
	m_pSkinnedStatic = RESOURCE_XMESHSTATIC->GetResource(
		sFName, &mat);


	this->m_pSkinnedAnim->AddBoneTransform(bName, &this->m_ItemTrans);

	m_pTransForm.AddChild(&m_BoundTrans0);
	m_ItemTrans.AddChild(&m_BoundTrans1);

	this->m_BoundBox0 = new cBoundBox();
	this->m_BoundBox0->SetBound(&m_pTransForm.GetWorldPosition(), &D3DXVECTOR3(0.8f, 4, 0.8f));
	this->m_BoundBox1 = new cBoundBox();
	this->m_BoundBox1->SetBound(&D3DXVECTOR3(m_ItemTrans.GetWorldPosition().x + 35, m_ItemTrans.GetWorldPosition().y, m_ItemTrans.GetWorldPosition().z), &D3DXVECTOR3(50.5f, 8, 8.0f));


	m_UserState = STAND;
	m_UserStateAttack = NON;
	atkCount = 0.0f;
	countAttack = false;
	isDeath = false;


	//UI
	HPPROGRESS = new cUI_Billboard;
	HPPROGRESS->Init("./Resources/UI/hpBar.png", 2.0f, 0.2f);

	return S_OK;
}

void cTest::Release()
{
	SAFE_DELETE(m_BoundBox0);
	SAFE_DELETE(m_BoundBox1);
	SAFE_DELETE(m_pUser);
}

void cTest::Update(float timeDelta)
{
	MonsterDeath();
	AniControl();
	this->m_pSkinnedAnim->Update(timeDelta);


	D3DXVECTOR3 pos = m_pTransForm.GetWorldPosition();
	HPPROGRESS->Update(D3DXVECTOR3(pos.x, pos.y + 5, pos.z));///////////////////////////////
	HPPROGRESS->ProgressBar(m_UserStatus.hp, m_UserStatus.maxHp);
}

void cTest::Render(cTransform * pTrans, cCamera * pCam)
{
	if (pTrans != NULL)
		this->m_pSkinnedAnim->Render(pTrans, pCam);
	else
	{
		this->m_pSkinnedAnim->Render(&m_pTransForm, pCam);
	}

	//this->m_BoundBox0->RenderGizmos(&this->m_BoundTrans0);
	//this->m_BoundBox1->RenderGizmos(&this->m_BoundTrans1);
	this->m_pSkinnedStatic->Render(&m_ItemTrans, pCam);
	HPPROGRESS->Render();
}

void cTest::AniControl()
{
}

void cTest::MoveRay(cArthas * user, cCamera * camera)
{
	if (m_UserState != DEATH)
	{
		bool hit = PHYSICS_MGR->IsOverlapBox(
			&m_BoundTrans1,
			m_BoundBox1,
			user->GetBoundTrans0(),
			user->GetBoundBox0());

		if (hit)
		{
			if (m_UserStateAttack == NON)
			{
				m_UserStateAttack = ATTACK;
				m_pSkinnedAnim->Play("attack1", 0.2f);
			}
		}



		if (atkCount >= 1.0f && !countAttack)
		{
			countAttack = true;
			user->SetHp(user->GetStatus().hp - m_UserStatus.pDam);
		}

		if (atkCount >= 2.0f)
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
			atkCount = 0.0f;
			countAttack = false;
		}

		if (m_UserStateAttack == ATTACK)
		{
			atkCount += TIME_MGR->GetFrameDeltaSec();
		}

		else if (m_UserStateAttack == NON)
		{
			if (MYUTIL::Vector3Distance(&user->GetTransform()->GetWorldPosition(), &m_pTransForm.GetWorldPosition()) <= 18.f)
			{
				if (MYUTIL::Vector3Distance(&user->GetTransform()->GetWorldPosition(), &m_pTransForm.GetWorldPosition()) >= 3.3f)
				{
					if (m_UserState == STAND)
					{
						m_UserState = RUN;
						m_pSkinnedAnim->Play("run", 0.2f);
					}

					D3DXVECTOR3 vecMove;

					float a = MYUTIL::Vector3Distance(&user->GetTransform()->GetWorldPosition(), &m_pTransForm.GetWorldPosition());

					vecMove = user->GetTransform()->GetWorldPosition() - m_pTransForm.GetWorldPosition();
					D3DXVec3Normalize(&vecMove, &vecMove);

					D3DXMATRIXA16 tempRot;

					D3DXMatrixLookAtLH(&tempRot,
						&D3DXVECTOR3(0, 0, 0),
						&vecMove,
						&D3DXVECTOR3(0, 1, 0));
					D3DXMatrixTranspose(&tempRot, &tempRot);
					m_pTransForm.SetRotateWorld(tempRot);

					m_pTransForm.SetWorldPosition(m_pTransForm.GetWorldPosition() + vecMove * 0.05f);
				}
				else
				{
					if (m_UserState == RUN)
					{
						m_UserState = STAND;
						m_pSkinnedAnim->Play("stand", 0.2f);
					}
				}
			}
			else
			{
				if (m_UserState == RUN)
				{
					m_UserState = STAND;
					m_pSkinnedAnim->Play("stand", 0.2f);
				}
			}
		}
	}
}

void cTest::MonsterDeath()
{
	if (m_UserState == DEATH)
	{
		deathCount += TIME_MGR->GetFrameDeltaSec();
	}

	if (!isDeath)
	{
		if (m_UserStatus.hp <= 0 && m_UserState != DEATH)
		{
			m_UserState = DEATH;
			m_UserStateAttack = NON;
			countAttack = 0.0f;
			m_pSkinnedAnim->Play("death", 0.2f);
		}
	}


}
