#include "stdafx.h"
#include "cMonsterZone.h"
#include "cTest.h"
#include "cArthas.h"

cMonsterZone::cMonsterZone()
	:isUse(false),
	 m_vMonster(NULL),
	 isCollision(false)
{
}


cMonsterZone::~cMonsterZone()
{
}

HRESULT cMonsterZone::Init(cTerrain* map, D3DXVECTOR3 v_pos)
{
	D3DXMATRIXA16 mat;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRotation;
	D3DXMATRIXA16 matTranslation;
	D3DXMatrixScaling(&matScale, 0.015f, 0.015f, 0.015f);
	D3DXMatrixRotationX(&matRotation, ((D3DX_PI / 180) * 2));
	D3DXMatrixTranslation(&matTranslation, 5.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&mat);
	mat = matScale; //* matRotation;// * matTranslation;
	m_pSkinnedStatic = RESOURCE_XMESHSTATIC->GetResource(
		"./Resources/XMeshStatic/monsterZone/monsterZone2.X", &mat);


	m_pTransForm.SetWorldPosition(v_pos.x, v_pos.y, v_pos.z);
	pos = SetHeightPos(map);
	m_pTransForm.SetWorldPosition(pos.x, pos.y + 0.2f, pos.z);

	return S_OK;
}

void cMonsterZone::Release()
{
	SAFE_RELEASE(m_pSkinnedStatic);
}

void cMonsterZone::Update(float timeDelta, cArthas * user, cCamera * pCam)
{
	eraseMonster();
	if (MYUTIL::Vector3Distance(&user->GetTransform()->GetWorldPosition(), &m_pTransForm.GetWorldPosition()) <= 14.f)	
		isCollision = true;
	else      
		isCollision = false;

	MonsterOBB();
	ZenMonster(user);

	for each(auto p in m_vMonster)
	{
		p->Update(timeDelta);
		p->MoveRay(user, pCam);
	}

}

void cMonsterZone::Render(cTransform * pTrans, cCamera * pCam)
{
	this->m_pSkinnedStatic->Render(&m_pTransForm, pCam);
	for each(auto p in m_vMonster)
	{
		p->Render(NULL, pCam);
	}
	SetMonsterPosition();
}

void cMonsterZone::CreateMonster(int level)
{
	for (int i = 0; i < 3; i++)
	{
		if (g_MonsterLevel == 1)
		{
			cTest* monster = new cTest;
			monster->Init("./Resources/XMeshSkinned/human/human.X", D3DXVECTOR3(0.04f, 0.04f, 0.04f), D3DXVECTOR3(1.05f, 1.05f, 1.05f), "./Resources/XMeshSkinned/garrosh/goreHowl.X", "hum_garrison_a_guard_bone_52");
			monster->SetDam(4);
			monster->SetHp(200);
			monster->GetTransform()->SetWorldPosition(pos.x, pos.y, pos.z);
			m_vMonster.push_back(monster);
		}
		if (g_MonsterLevel == 2)
		{
			cTest* monster = new cTest;
			monster->Init("./Resources/XMeshSkinned/human/human.X", D3DXVECTOR3(0.04f, 0.04f, 0.04f), D3DXVECTOR3(1.05f, 1.05f, 1.05f), "./Resources/XMeshSkinned/garrosh/goreHowl.X", "hum_garrison_a_guard_bone_52");
			monster->SetDam(12);
			monster->SetHp(600);
			monster->GetTransform()->SetWorldPosition(pos.x, pos.y, pos.z);
			m_vMonster.push_back(monster);
		}
		if (g_MonsterLevel == 3)
		{
			cTest* monster = new cTest;
			monster->Init("./Resources/XMeshSkinned/human/human.X", D3DXVECTOR3(0.04f, 0.04f, 0.04f), D3DXVECTOR3(1.05f, 1.05f, 1.05f), "./Resources/XMeshSkinned/garrosh/goreHowl.X", "hum_garrison_a_guard_bone_52");
			monster->SetDam(4);
			monster->SetHp(1200);
			monster->GetTransform()->SetWorldPosition(pos.x, pos.y, pos.z);
			m_vMonster.push_back(monster);
		}
		
	}
}

void cMonsterZone::ZenMonster(cArthas * user)
{
	if (!isUse)
	{
		if (isCollision)
		{
			isUse = true;
			CreateMonster(g_MonsterLevel);
		}
	}

	if (isUse)
	{
		if (!isCollision)
		{
			if (m_vMonster.size() <= 0)
			isUse = false;
		}
	}
	
}

void cMonsterZone::MonsterOBB()
{
	if (m_vMonster.size() > 1)
	{
		for (int i = 0; i < m_vMonster.size(); i++)
		{
			for (int j = 0; j < m_vMonster.size(); j++)
			{
				if (i == j) continue;
				PHYSICS_MGR->IsBlockingBox(
					m_vMonster[i]->GetBoundTrans0(),
					m_vMonster[i]->GetBoundBox0(),
					m_vMonster[j]->GetBoundTrans0(),
					m_vMonster[j]->GetBoundBox0(), 1.0f);

				PHYSICS_MGR->IsOverlapBox(
					m_vMonster[i]->GetBoundTrans1(),
					m_vMonster[i]->GetBoundBox1(),
					m_vMonster[j]->GetBoundTrans0(),
					m_vMonster[j]->GetBoundBox0());
			}

		}
	}
}

void cMonsterZone::eraseMonster()
{
	if (m_vMonster.size() > 0)
	{
		int j = m_vMonster.size();
		for (int i = 0; i < j; i++)
		{
			if (m_vMonster[i]->GetStatus().hp <= 0)
			{
				//int a = m_vMonster[i]->GetStatus().hp;
				//int b = 0;
				if (m_vMonster[i]->GetDeathCount() >= 2.0f)
				{
					if (g_MonsterLevel == 1)
					{
						Gold += (int)GetRandom(100, 300);
					}
					if (g_MonsterLevel == 2)
					{
						Gold += (int)GetRandom(500, 1000);
					}
					if (g_MonsterLevel == 3)
					{
						Gold += (int)GetRandom(1000, 1500);
					}
					m_vMonster.erase(m_vMonster.begin() + i);
				}
				break;
			}
		}
	}
}

void cMonsterZone::SetMonsterPosition()
{
	for each(auto p in m_vMonster)
	{
		if (MYUTIL::Vector3Distance(&p->GetTransform()->GetWorldPosition(), &pos) >= 12.f)
		{
			p->GetTransform()->SetWorldPosition(pos);
		}
	}
}

