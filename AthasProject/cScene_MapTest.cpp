#include "stdafx.h"
#include "cScene_MapTest.h"
#include "cTerrain.h"
#include "cLight_Directional.h"
#include "cQuadTree.h"

cScene_MapTest::cScene_MapTest()
{
}


cScene_MapTest::~cScene_MapTest()
{
}

HRESULT cScene_MapTest::Scene_Init()
{
	pTerrain = new cTerrain;
	pTerrain->Init(
		"./Resources/Textures/MyHeight2.bmp",
		"./Resources/Textures/rock1.png",
		"./Resources/Textures/rock2.png",
		"./Resources/Textures/rock3.jpg",
		"./Resources/Textures/Snow.jpg",
		"./Resources/Textures/MySplatMap.png",
		"./Resources/Textures/HeightValue.txt",
		1.0f,
		20.0f,
		4,
		1
	);

	m_pDirectionalLight = new cLight_Directional();
	m_pDirectionalLight->lightColor = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1);
	m_pDirectionalLight->InitShadowMap(4096, 100);
	m_pDirectionalLight->Transform.SetRotateWorld(
		90.0f * ONE_RAD, 0, 0);

	CreateBuild();

	this->mainCamera.SetCameraFar(1000.f);
	this->mainCamera.Transform.SetWorldPosition(D3DXVECTOR3(0, 200, 0));
	this->mainCamera.Transform.SetCameraMoveSpd(30.f);

	return S_OK;
}

void cScene_MapTest::Scene_Release()
{
	m_vecSetBuild.clear();
	pTerrain->Release();
	SAFE_DELETE(pTerrain);
}

void cScene_MapTest::Scene_Update(float timeDelta)
{
	this->mainCamera.Transform.MouseDefaultControl(timeDelta);

	if (KEY_MGR->IsOnceDown('8'))
	{
		SCENE_MGR->ChangeSceneWithLoading("Å¸ÀÌÆ²¾À", "·Îµù¾À");
	}
}

void cScene_MapTest::Scene_Render()
{
	Device->Clear(NULL,
		NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(47, 121, 112),
		1.0f, 0);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pTerrain->Render(&this->mainCamera);

	for each(auto p in m_vecSetBuild)
	{
		p->skin->Render(&p->trans, &mainCamera);
		p->box->RenderGizmos(&p->trans);
	}
}

void cScene_MapTest::Scene_LightRender()
{
	this->m_pDirectionalLight->Render();
}

void cScene_MapTest::CreateBuild()
{
	FILE* openBuild;

	fopen_s(&openBuild, "./Resources/Textures/Build.txt", "r");

	while (!feof(openBuild))
	{
		LPSTBUILD   tmpBuild = new STBUILDE;

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

			tmpBuild->trans.SetRotateWorld(matRotate);
			tmpBuild->box = new cBoundBox;
			tmpBuild->box = tmpBuild->skin->GetDefaultBound();

			m_vecSetBuild.push_back(tmpBuild);
		}
	}

	fclose(openBuild);
}

void cScene_MapTest::OnClick(cUIButton * pSender)
{
}