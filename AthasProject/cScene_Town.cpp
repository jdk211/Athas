#include "stdafx.h"

#include "cScene_Town.h"
#include "cModelCube.h"
#include "cSkinnedAnimation.h"
#include "cLight_Directional.h"
#include "cLight_Point.h"
#include "cArthas.h"
#include "cMerchant.h"
#include "cTerrain.h"
#include "cTest.h"
#include "cUIImageView.h"
#include "cUITextView.h"
#include "cUIObject.h"
#include "cParticleEmitter.h"
#include "cMonsterZone.h"
#include "cBossZone.h"

cScene_Town::cScene_Town()
{
	posZ = posY = 0.0f;
}

cScene_Town::~cScene_Town()
{
}

HRESULT cScene_Town::Scene_Init()
{
	this->SetEnvironmentTexture("./Resources/CubeTextures/BloodSeaCubeMap.dds");


	D3DXMATRIXA16 mat;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matTranslation;
	D3DXMatrixScaling(&matScale, 3.5, 3.5, 3.5);
	D3DXMatrixTranslation(&matTranslation, 5.0f, 0.0f, 0.0f);
	mat = matScale * matTranslation;

	this->mainCamera.Far = 300.0f;

	//방향성광원 준비
	pDirectionalLight = new cLight_Directional();
	pDirectionalLight->lightColor = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1);
	pDirectionalLight->InitShadowMap(4096, 100);
	pDirectionalLight->Transform.SetRotateWorld(
		90.0f * ONE_RAD, 0, 0);
	angle = 0.0f;

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

	//
	// 파티클 이미터
	//
	this->mainCamera.Far = 100.0f;

	m_pUser = new cArthas;
	m_pUser->Init();

	pTerrain = new cTerrain;
	pTerrain->Init(
		"./Resources/Textures/MyHeight2.bmp",
		"./Resources/Textures/rock1.png",
		"./Resources/Textures/rock2.png",
		"./Resources/Textures/rock3.jpg",
		"./Resources/Textures/Snow.jpg",
		"./Resources/Textures/MySplatMap.png",
		"./Resources/Textures/TownHeightValue.txt",
		1.0f,
		20.0f,
		2,
		1
	);

	m_pMerchant = new cMerchant;
	m_pMerchant->Init(0, 0, 30);
	m_pMerchant->SetHeightPos(pTerrain);

	mainCamera.Transform.SetLocalPosition(trans.GetLocalPosition().x, trans.GetLocalPosition().y + 30, trans.GetLocalPosition().z - 15);
	mainCamera.Transform.SetRotateWorld(((D3DX_PI / 180) * 50), 0.0f, 0.0f);

	//MonsterZone
	m_pM_Zone = new cMonsterZone;
	m_pM_Zone->Init(pTerrain, D3DXVECTOR3(25,0,-40));
	m_pB_Zone = new cBossZone;
	m_pB_Zone->Init(pTerrain, D3DXVECTOR3(-30, 0, -50));
	//UI
	cUIImageView* ImgView = new cUIImageView;
	ImgView->SetTexture("./Resources/UI/menuBar.png");
	ImgView->SetPosition(D3DXVECTOR3(0, 720, 0));

	cUIImageView* userInfo = new cUIImageView;
	userInfo->SetTexture("./Resources/UI/userInfo.png");
	userInfo->SetPosition(D3DXVECTOR3(0, 10, 0));

	m_pSpell = new cSpell;
	m_pSpell->init();

	m_pSpell2 = new cSpell2;
	m_pSpell2->init();

	CreateBuild();

	this->mainCamera.SetCameraFar(1000.f);

	return S_OK;
}


void cScene_Town::Scene_Release()
{
	m_pUser->Release();
	pTerrain->Release();
	m_pM_Zone->Release();
	m_pB_Zone->Release();
	m_pMerchant->Release();

	for each(auto p in m_vecMonster)
	{
		p->Release();
	}

	SAFE_DELETE(pTerrain);
	m_pSpell->Release();
	m_pSpell2->Release();

}

void cScene_Town::Scene_Update(float timeDelta)
{
	//:<<
	m_pM_Zone->Update(timeDelta, m_pUser, &mainCamera);
	m_pB_Zone->Update(timeDelta, m_pUser, &mainCamera);
	D3DXVECTOR3 userPos;
	userPos = m_pUser->SetHeightPos(pTerrain);

	m_pUser->GetTransform()->SetLocalPosition(userPos.x, userPos.y, userPos.z);
	//pParticleEmitter->Transform.AttachTo(m_pUser->GetTransform());
	//mainCamera.Transform.SetWorldPosition(m_pUser->GetTransform()->GetWorldPosition().x, m_pUser->GetTransform()->GetWorldPosition().y + 30, m_pUser->GetTransform()->GetWorldPosition().z - 25);
	mainCamera.Transform.AttachTo(m_pUser->GetTransform());
	//mainCamera.Transform.MouseDefaultControl(timeDelta);

	m_pMerchant->Update(timeDelta, m_pUser);

	mainCamera.Transform.SetLocalPosition(trans.GetLocalPosition().x, trans.GetLocalPosition().y + 30 + posY, trans.GetLocalPosition().z - 15 - posZ);
	//D3DXVECTOR3 place = D3DXVECTOR3(0,0,1);
	//this->pParticleEmitter->Transform.SetWorldPosition(this->pParticleEmitter->Transform.GetWorldPosition() + place * 0.2f);

	if (KEY_MGR->IsStayDown('Z'))
	{
		posY += (10.0f / 100.f);
		posZ += (5.f / 100.f);
	}

	if (KEY_MGR->IsStayDown('X'))
	{
		posY -= (30.f / 100.f);
		posZ -= (20.f / 100.f);
	}
	//mainCamera.Transform.MouseDefaultControl(timeDelta);

	//this->mainCamera.Transform.SetWorldPosition(D3DXVECTOR3(m_pUser->GetTransform()->GetWorldPosition().x, m_pUser->GetTransform()->GetWorldPosition().y + (30 - posY), m_pUser->GetTransform()->GetWorldPosition().z - (20 - posZ)));

	//mainCamera.Transform.SetWorldPosition(m_pUser->GetTransform()->GetWorldPosition().x, m_pUser->GetTransform()->GetWorldPosition().y + 30, m_pUser->GetTransform()->GetWorldPosition().z - 25);
	/*D3DXMATRIXA16 matW;
	D3DXMatrixLookAtLH(&matW,
		&D3DXVECTOR3(0, 0 , 0),
		&m_pUser->GetTransform()->GetWorldPosition(),
		&D3DXVECTOR3(0, 1, 0));
	D3DXMatrixTranspose(&matW, &matW);
	mainCamera.Transform.SetWorldMatrix(&matW);*/

	//m_pUser->GetTransform()->SetLocalPosition(x, y, z);
	//mainCamera.Transform.AttachTo(m_pUser->GetTransform());
	//m_pUser->GetTransform()->AddChild(&mainCamera.Transform);
	//:>>
	//trans.GetLocalPosition().x, trans.GetLocalPosition().y + 30, trans.GetLocalPosition().z - 20

	//this->mainCamera.Transform.SetWorldPosition(D3DXVECTOR3(m_pUser->GetTransform()->GetWorldPosition().x, m_pUser->GetTransform()->GetWorldPosition().y + 30, m_pUser->GetTransform()->GetWorldPosition().z - 20));

	//this->mainCamera.Transform.MouseDefaultControl(timeDelta);
	//mainCamera.Transform.MouseDefaultControl(timeDelta);
	if (KEY_MGR->IsStayDown('A'))
	{
		angle += 0.01;
	}
	if (KEY_MGR->IsStayDown('D'))
	{
		angle -= 0.01;
	}

	m_pUser->Update(timeDelta);
	m_pUser->MoveRay(pTerrain, &mainCamera); 
	//m_pUser->MoveRay(pTerrain, &mainCamera);

	for each(auto p in m_vecMonster)
	{
		PHYSICS_MGR->IsBlockingBox(
			this->m_pUser->GetBoundTrans0(),
			this->m_pUser->GetBoundBox0(),
			p->GetBoundTrans0(),
			p->GetBoundBox0(), 1.0f);

		this->hit = PHYSICS_MGR->IsOverlapBox(
			this->m_pUser->GetBoundTrans1(),
			this->m_pUser->GetBoundBox1(),
			p->GetBoundTrans0(),
			p->GetBoundBox0());
	}

	for (int i = 0; i < m_vecMonster.size(); i++)
	{
		for (int j = 0; j < m_vecMonster.size(); j++)
		{
			if (i == j) continue;
			PHYSICS_MGR->IsBlockingBox(
				m_vecMonster[i]->GetBoundTrans0(),
				m_vecMonster[i]->GetBoundBox0(),
				m_vecMonster[j]->GetBoundTrans0(),
				m_vecMonster[j]->GetBoundBox0(), 1.0f);

			PHYSICS_MGR->IsOverlapBox(
				m_vecMonster[i]->GetBoundTrans1(),
				m_vecMonster[i]->GetBoundBox1(),
				m_vecMonster[j]->GetBoundTrans0(),
				m_vecMonster[j]->GetBoundBox0());
		}
	}

	//건물 충돌
	for each(auto p in m_vecSetBuild)
	{
		PHYSICS_MGR->IsBlockingBox(
			this->m_pUser->GetBoundTrans0(),
			this->m_pUser->GetBoundBox0(),
			&p->trans,
			p->box, 1.0f);
	}
	m_pUser->hitMonster(m_pM_Zone->GetVecMonster());
	m_pUser->hitMonster(m_pB_Zone->GetVecMonster());

	//npc 충돌
	PHYSICS_MGR->IsBlockingBox(
		this->m_pUser->GetBoundTrans0(),
		this->m_pUser->GetBoundBox0(),
		m_pMerchant->GetBoundTrans0(),
		m_pMerchant->GetBoundBox0(), 1.0f);

	if (hit && m_pUser->GetStateAttack() == ATTACK)
	{
		this->m_pUser->GetBoundBox1()->SetRenderColor(D3DCOLOR_XRGB(255, 0, 0));
	}
	else
	{
		this->m_pUser->GetBoundBox1()->SetRenderColor(D3DCOLOR_XRGB(255, 255, 0));
	}

	m_pSpell->Update(timeDelta, &mainCamera, m_pM_Zone->GetVecMonster(), m_pUser, m_pB_Zone->GetVecMonster());
	m_pSpell2->Update(timeDelta, &mainCamera, m_pM_Zone->GetVecMonster(), m_pUser, m_pB_Zone->GetVecMonster());
	//this->pSkinned->SetPlaySpeed(0.1f);
#pragma endregion

	//UI
	if (KEY_MGR->IsStayDown(VK_LCONTROL))
	{
		//this->shipTransform.DefaultControl(timeDelta);
		//this->pParticleEmitter->Transform.DefaultControl(timeDelta);
	}
	if (KEY_MGR->IsStayDown(VK_LSHIFT))
	{
		//방향성 광원 컨트롤...
		this->pDirectionalLight->Transform.DefaultControl(timeDelta);
	}

	//this->pParticleEmitter->Update(timeDelta);
}


void cScene_Town::Scene_Render()
{
	cXMesh_Skinned::SetCamera(&this->mainCamera);
	cXMesh_Skinned::SetLight(&this->lightTrans, D3DXCOLOR(1, 1, 1, 1), 0.3f);
	cXMesh_Skinned::SetEnvironment(this->pEnvironmentCubeTex);
	cXMesh_Static::SetCamera(&this->mainCamera);
	cXMesh_Static::SetLight(&this->lightTrans, D3DXCOLOR(1, 1, 1, 1), 0.3f);
	cXMesh_Static::SetEnvironment(this->pEnvironmentCubeTex);
	m_pUser->Render(NULL, &mainCamera);
	m_pM_Zone->Render(NULL, &mainCamera);
	m_pB_Zone->Render(NULL, &mainCamera);

	for each(auto p in m_vecMonster)
	{
		p->Render(NULL, &mainCamera);
	}
	pTerrain->Render(&this->mainCamera);

	pTerrain->Render(&this->mainCamera);
	m_pSpell->Render();
	m_pSpell2->Render();
	for each(auto p in m_vecSetBuild)
	{
		p->skin->Render(&p->trans, &mainCamera);
		p->skin->GetDefaultBound()->RenderGizmos(&p->trans);
	}
	m_pMerchant->Render(NULL, &mainCamera);
	//if (m_pUIRoot)m_pUIRoot->Render(m_pSprite);
}

void cScene_Town::Scene_ShadowRender()
{
	//방향성 광원에 대한 그림자 맵을 준비한다.
	this->pDirectionalLight->BeginShadowRender(&this->mainCamera);

	//무기 그림자 아서스
	this->m_pUser->GetSkinnedStatic()->RenderNoiseCutoutShadow(
		m_pUser->GetItemTransform(),
		this->pDirectionalLight->GetLightCamera(),
		RESOURCE_TEXTURE->GetResource("./Resources/Textures/GrayNoise2.png"),
		0.0f);

	////무기 그림자 몬스터
	//for each(auto p in m_vecMonster)
	//{
	//	p->GetSkinnedStatic()->RenderNoiseCutoutShadow(
	//		p->GetItemTransform(),
	//		this->pDirectionalLight->GetLightCamera(),
	//		NULL,
	//		0.0f);
	//}

	/*this->m_pMonster->GetSkinnedStatic()->RenderNoiseCutoutShadow(
		m_pMonster->GetItemTransform(),
		this->pDirectionalLight->GetLightCamera(),
		NULL,
		0.0f);*/

	//Terrain 쉐도우 맵 그린다.
	this->pTerrain->RenderShadow(
		this->pDirectionalLight->GetLightCamera());

	this->m_pUser->GetSkinnedMeshAnimation()->RenderShadow(
		m_pUser->GetTransform(),
		this->pDirectionalLight->GetLightCamera());

	//방향성 광원에 대한 그림자 맵 랜더 종료
	this->pDirectionalLight->EndShdowRender();
}

void cScene_Town::Scene_LightRender()
{
	this->pDirectionalLight->Render();
	for (int i = 0; i < LIGHT_NUM; i++) {
		this->pPointLights[i]->Render();
	}
}

void cScene_Town::Scene_ParticleRender()
{
	//this->pParticleEmitter->Render();
	m_pSpell->Scene_ParticleRender();
	m_pSpell2->Scene_ParticleRender();
	m_pMerchant->particleRender(m_pUser);
}

void cScene_Town::OnClick(cUIButton * pSender)
{
}

void cScene_Town::CreateBuild()
{
	FILE* openBuild;

	fopen_s(&openBuild, "./Resources/Textures/TownBuild.txt", "r");

	while (!feof(openBuild))
	{
		LPSTBUILD	tmpBuild = new STBUILDE;

		D3DXMATRIXA16 mat;
		D3DXMatrixIdentity(&mat);

		D3DXMATRIXA16 matScale;

		D3DXMATRIXA16 matRotate;
		D3DXMatrixRotationY(&matRotate, 180.0f * ONE_RAD);

		D3DXMATRIXA16 matTrans;

		char szBuf[1024] = { '\0', };
		fgets(szBuf, 1024, openBuild);
		if (strlen(szBuf) == 0) continue;

		if (szBuf[0] == 'K')
		{
			char szMtlPath[1024];
			sscanf(szBuf, "%*s %s", szMtlPath);
			tmpBuild->skinPath = szMtlPath;
		}
		fgets(szBuf, 1024, openBuild);
		if (szBuf[0] == 'P')
		{
			char tmpX[1024], tmpZ[1024];
			sscanf(szBuf, "%*s %s %s", tmpX, tmpZ);
			tmpBuild->position.x = atof(tmpX);
			tmpBuild->position.z = atof(tmpZ);
			tmpBuild->position.y = pTerrain->GetHeight(tmpBuild->position.x, tmpBuild->position.z);

			D3DXMatrixTranslation(&matTrans, tmpBuild->position.x, tmpBuild->position.y, tmpBuild->position.z);
		}
		fgets(szBuf, 1024, openBuild);
		if (szBuf[0] == 'S')
		{
			char _41[1024], _42[1024], _43[1024];
			sscanf(szBuf, "%*s %s %s %s", _41, _42, _43);
			D3DXMatrixScaling(&matScale, atof(_41) * 0.01f, atof(_42) * 0.01f, atof(_43) * 0.01f);
		}
		fgets(szBuf, 1024, openBuild);
		if (szBuf[0] == '1')
		{
			char _11[1024], _12[1024], _13[1024];
			sscanf(szBuf, "%*s %s %s %s", _11, _12, _13);
			matRotate._11 = atof(_11);
			matRotate._12 = atof(_12);
			matRotate._13 = atof(_13);
		}
		fgets(szBuf, 1024, openBuild);
		if (szBuf[0] == '2')
		{
			char _21[1024], _22[1024], _23[1024];
			sscanf(szBuf, "%*s %s %s %s", _21, _22, _23);
			int a = 0;
			matRotate._21 = atof(_21);
			matRotate._22 = atof(_22);
			matRotate._23 = atof(_23);
		}
		fgets(szBuf, 1024, openBuild);
		if (szBuf[0] == '3')
		{
			char _31[1024], _32[1024], _33[1024];
			sscanf(szBuf, "%*s %s %s %s", _31, _32, _33);
			matRotate._31 = atof(_31);
			matRotate._32 = atof(_32);
			matRotate._33 = atof(_33);
		}
		fgets(szBuf, 1024, openBuild);
		if (szBuf[0] == 'B')
		{
			mat = matScale * matRotate;
			tmpBuild->skin = RESOURCE_XMESHSTATIC->GetResource(tmpBuild->skinPath, &mat);
			tmpBuild->trans.SetWorldPosition(tmpBuild->position.x, tmpBuild->position.y, tmpBuild->position.z);
			
			tmpBuild->box = new cBoundBox;
			tmpBuild->box = tmpBuild->skin->GetDefaultBound();

			m_vecSetBuild.push_back(tmpBuild);
		}
	}

	fclose(openBuild);
}
