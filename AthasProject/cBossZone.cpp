
#include "stdafx.h"
#include "cBossZone.h"
#include "cTest.h"
#include "cArthas.h"


cBossZone::cBossZone()
{
}


cBossZone::~cBossZone()
{
}

HRESULT cBossZone::Init(cTerrain * map, D3DXVECTOR3 v_pos)
{
	isUse = false;
	isCollision = false;
	D3DXMATRIXA16 mat;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRotation;
	D3DXMATRIXA16 matTranslation;
	D3DXMatrixScaling(&matScale, 0.014f, 0.014f, 0.014f);
	D3DXMatrixRotationY(&matRotation, ((D3DX_PI / 180) * 270));
	D3DXMatrixTranslation(&matTranslation, 5.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&mat);
	mat = matScale * matRotation;// * matTranslation;
	m_pSkinnedStatic = RESOURCE_XMESHSTATIC->GetResource(
		"./Resources/XMeshStatic/bossZone/bossZone.X", &mat);


	m_pTransForm.SetWorldPosition(v_pos.x, v_pos.y, v_pos.z); // 50,0,30
	pos = SetHeightPos(map);
	m_pTransForm.SetWorldPosition(pos.x, pos.y, pos.z);

	return S_OK;
}

void cBossZone::Release()
{
	SAFE_RELEASE(m_pSkinnedStatic);
}

void cBossZone::Update(float timeDelta, cArthas * user, cCamera * pCam)
{
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

	eraseMonster();
	SetMonsterPosition();
}

void cBossZone::Render(cTransform * pTrans, cCamera * pCam)
{
	this->m_pSkinnedStatic->Render(&m_pTransForm, pCam);
	for each(auto p in m_vMonster)
	{
		p->Render(NULL, pCam);
	}
}

void cBossZone::CreateMonster(int level)
{
	for (int i = 0; i < 1; i++)
	{
		if (level == 1)
		{
			cTest* monster = new cTest;
			monster->Init("./Resources/XMeshSkinned/garrosh/garrosh_stand.X", D3DXVECTOR3(0.04f, 0.04f, 0.04f), D3DXVECTOR3(1.05f, 1.05f, 1.05f), "./Resources/XMeshSkinned/garrosh/goreHowl.X", "garrosh2_bone_20");
			monster->SetDam(50);
			monster->SetHp(1000);
			monster->GetTransform()->SetWorldPosition(pos.x, pos.y, pos.z);
			m_vMonster.push_back(monster);
		}
		if (level == 2)
		{
			cTest* monster = new cTest;
			monster->Init("./Resources/XMeshSkinned/garrosh/garrosh_stand.X", D3DXVECTOR3(0.04f, 0.04f, 0.04f), D3DXVECTOR3(1.05f, 1.05f, 1.05f), "./Resources/XMeshSkinned/\garrosh/goreHowl.X", "garrosh2_bone_20");
			monster->SetDam(100);
			monster->SetHp(2000);
			monster->GetTransform()->SetWorldPosition(pos.x, pos.y, pos.z);
			m_vMonster.push_back(monster);
		}
		if (level == 3)
		{
			cTest* monster = new cTest;
			monster->Init("./Resources/XMeshSkinned/thrall/thrall.X", D3DXVECTOR3(0.04f, 0.04f, 0.04f), D3DXVECTOR3(1.05f, 1.05f, 1.05f), "./Resources/XMeshSkinned/thrall/thrall_weapon.X", "shaman_thrall_bone_173");
			monster->SetDam(300);
			monster->SetHp(6000);
			monster->GetTransform()->SetWorldPosition(pos.x, pos.y, pos.z);
			m_vMonster.push_back(monster);
		}
	}
}

void cBossZone::ZenMonster(cArthas * user)
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

void cBossZone::MonsterOBB()
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

void cBossZone::eraseMonster()
{
	if (m_vMonster.size() > 0)
	{
		int j = m_vMonster.size();
		for (int i = 0; i < j; i++)
		{
			if (m_vMonster[i]->GetStatus().hp <= 0)
			{
				if (m_vMonster[i]->GetDeathCount() >= 2.0f)
				{
					if (g_MonsterLevel == 1)
					{
						Gold += 50000;
					}
					if (g_MonsterLevel == 2)
					{
						Gold += 100000;
					}
					if (g_MonsterLevel == 3)
					{
						Gold += 500000;
					}
					m_vMonster.erase(m_vMonster.begin() + i);
					g_MonsterLevel += 1;
				}
				break;
			}
		}
	}



}

void cBossZone::SetMonsterPosition()
{
	for each(auto p in m_vMonster)
	{
		if (MYUTIL::Vector3Distance(&p->GetTransform()->GetWorldPosition(), &pos) >= 14.f)
		{
			p->GetTransform()->SetWorldPosition(pos);
		}
	}
}
