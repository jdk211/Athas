#include "stdafx.h"
#include "cScene_MapTool.h"
#include "cLight_Directional.h"
#include "cQuadTree.h"
#include "cUIImageView.h"
#include "cUITextView.h"
#include "cUIObject.h"

cScene_MapTool::cScene_MapTool()
{
}

cScene_MapTool::~cScene_MapTool()
{
}

HRESULT cScene_MapTool::Scene_Init()
{
	m_isBuildPick = true;

	m_fGapSize = 1 / 100.f;
	m_nHeightSize = 1;
	m_pVertex = NULL;
	m_pQuadTree = NULL;
	m_nBrushRadius = 10;

	m_nSelectBuildNum = -1;
	m_stModeState = MODE_HEIGHT;
	m_strMode = "Height";

	m_pDirectionalLight = new cLight_Directional();
	m_pDirectionalLight->lightColor = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1);
	m_pDirectionalLight->InitShadowMap(4096, 100);
	m_pDirectionalLight->Transform.SetRotateWorld(
		90.0f * ONE_RAD, 0, 0);

	//맵 정보를 받아온다.
	m_cpMapName = MAP_NAME_TOWN;

	BMPFileOpen();

	//이미지 사이즈 정보
	m_nMapSize = hInfo.biWidth;               // ex) 257
	m_nTileNum = m_nMapSize - 1;            // ex) 257 - 1
	m_nTotalVertex = m_nMapSize * m_nMapSize;   // ex) 257 * 257

	m_pVertex = new PNTVERTEX[m_nTotalVertex];

	CreateGround();

	U = 0.f;
	V = 0.f;
	Dist = 0.f;

	m_ClickRay.origin = D3DXVECTOR3(0, 0, 0);
	m_ClickRay.direction = D3DXVECTOR3(0, 0, 1);

	m_pQuadTree = new cQuadTree;
	m_pQuadTree->Init(m_pVertex, m_nMapSize);

	D3DXMATRIXA16 mat;
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale, 0.01f, 0.01f, 0.01f);

	//회전 180
	D3DXMATRIXA16 matRotate;
	D3DXMatrixRotationY(&matRotate, 1.0f * ONE_RAD);

	mat = matScale * matRotate;

	m_pTestHome = RESOURCE_XMESHSTATIC->GetResource(
		"./Resources/XMeshSkinned/OBJ/House2/house2.X", &mat);
	m_strSkinPath = "./Resources/XMeshSkinned/OBJ/House2/house2.X";

	//UI추가
	m_pUIRoot = NULL;
	m_pSprite = NULL;

	D3DXCreateSprite(Device, &m_pSprite);

	cUIImageView* ImgView = new cUIImageView;
	ImgView->SetTexture("./Resources/UI/build_panel.png");
	ImgView->SetPosition(D3DXVECTOR3(0, 0, 0));

	cUIButton* house = new cUIButton;
	house->SetTexture("./Resources/UI/house_click.png", "./Resources/UI/house_over.png", "./Resources/UI/house_idle.png");
	house->SetPosition(D3DXVECTOR3(10, 10, 0));
	house->SetDelegate(this);
	house->SetTag(1);

	cUIButton* house2 = new cUIButton;
	house2->SetTexture("./Resources/UI/house2_click.png", "./Resources/UI/house2_over.png", "./Resources/UI/house2_idle.png");
	house2->SetPosition(D3DXVECTOR3(10, 100, 0));
	house2->SetDelegate(this);
	house2->SetTag(2);

	cUIButton* house3 = new cUIButton;
	house3->SetTexture("./Resources/UI/house3_click.png", "./Resources/UI/house3_over.png", "./Resources/UI/house3_idle.png");
	house3->SetPosition(D3DXVECTOR3(10, 190, 0));
	house3->SetDelegate(this);
	house3->SetTag(3);

	cUIButton* tower = new cUIButton;
	tower->SetTexture("./Resources/UI/tower_click.png", "./Resources/UI/tower_over.png", "./Resources/UI/tower_idle.png");
	tower->SetPosition(D3DXVECTOR3(10, 280, 0));
	tower->SetDelegate(this);
	tower->SetTag(4);

	m_pUIRoot = new cUIObject;
	m_pUIRoot->SetPosition(D3DXVECTOR3(WINSIZE_X - 130, 0, 0));
	m_pUIRoot->SetSize(ST_SIZEN(100, 800));

	m_pUIRoot->AddChild(ImgView);
	m_pUIRoot->AddChild(house);
	m_pUIRoot->AddChild(house2);
	m_pUIRoot->AddChild(house3);
	m_pUIRoot->AddChild(tower);

	this->mainCamera.SetCameraFar(1000.f);
	this->mainCamera.Transform.SetWorldPosition(D3DXVECTOR3(0, 200, 0));
	this->mainCamera.Transform.SetCameraMoveSpd(10.f);

	return S_OK;
}

void cScene_MapTool::Scene_Release()
{
	m_vecIndex.clear();
	m_vecSetBuild.clear();
	SAFE_DELETE_ARR(m_pVertex);
}

void cScene_MapTool::Scene_Update(float timeDelta)
{
	ptMouse = GetMousePos();

	m_BTestTrans.SetWorldPosition(m_vCircleCenter);

	ModeSelect();
	ModeUpdate();
	PickGround();
	PickisPossible();

	if (m_pUIRoot)m_pUIRoot->Update();

	this->mainCamera.Transform.MouseDefaultControl(timeDelta);
}

void cScene_MapTool::Scene_Render()
{
	Device->Clear(NULL,
		NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(47, 121, 112),
		1.0f, 0);

	//상태에 따른 원, 건물(설치 할 것)
	if (m_stModeState == MODE_HEIGHT) BrushCircleDraw();
	else if (m_stModeState == MODE_BUILD)
	{
		m_pTestHome->Render(&m_BTestTrans, &mainCamera);
		if (m_pUIRoot)m_pUIRoot->Render(m_pSprite);
	}

	//Build
	for each(auto p in m_vecSetBuild)
	{
		p->skin->Render(&p->trans, &mainCamera);
		p->box->RenderGizmos(&p->trans);
	}


	//현재 상태, 키 출력
	PrintText();
	//Ground
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pMesh->DrawSubset(0);
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}


void cScene_MapTool::Scene_LightRender()
{
	this->m_pDirectionalLight->Render();
}

void cScene_MapTool::CreateGround()
{
	//점을 정의
	for (int i = 0; i < m_nTotalVertex; ++i)
	{
		PNTVERTEX v;

		v.p = D3DXVECTOR3(i % m_nMapSize - (m_nMapSize / 2), 0, i / m_nMapSize - (m_nMapSize / 2));
		v.n = D3DXVECTOR3(0, 1, 0);
		v.t = D3DXVECTOR2((i % m_nMapSize) / (float)m_nMapSize, (i / m_nMapSize) / (float)m_nMapSize);

		m_pVertex[i] = v;
	}

	for (int z = 1; z < m_nTileNum; ++z)
	{
		for (int x = 1; x < m_nTileNum; ++x)
		{
			int left = (z + 0) * m_nMapSize + x - 1;
			int right = (z + 0) * m_nMapSize + x + 1;
			int up = (z + 1) * m_nMapSize + x + 0;
			int down = (z - 1) * m_nMapSize + x + 0;

			D3DXVECTOR3 leftToRight = m_pVertex[right].p - m_pVertex[left].p;
			D3DXVECTOR3 downToUp = m_pVertex[up].p - m_pVertex[down].p;
			D3DXVECTOR3 normal;
			D3DXVec3Cross(&normal, &downToUp, &leftToRight);
			D3DXVec3Normalize(&normal, &normal);

			int nIndex = z * m_nMapSize + x;
			m_pVertex[nIndex].n = normal;
		}
	}

	//점에 순서를 정해준다
	for (int z = 0; z < m_nTileNum; ++z)
	{
		for (int x = 0; x < m_nTileNum; ++x)
		{
			int _0 = (z + 0) * m_nMapSize + x;
			int _1 = (z + 1) * m_nMapSize + x;
			int _2 = (z + 0) * m_nMapSize + x + 1;
			int _3 = (z + 1) * m_nMapSize + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);

			m_vecIndex.push_back(_3);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
		}
	}

	//메시를 설정
	D3DXCreateMeshFVF(
		m_vecIndex.size() / 3,
		m_nTotalVertex,
		D3DXMESH_MANAGED | D3DXMESH_32BIT,
		PNT_VERTEX::FVF,
		Device,
		&m_pMesh
	);

	//메시에 vertex, index 저장
	ResetMesh();

	DWORD* pAttr = NULL;
	m_pMesh->LockAttributeBuffer(0, &pAttr);
	ZeroMemory(pAttr, (m_vecIndex.size() / 3) * sizeof(DWORD));
	m_pMesh->UnlockAttributeBuffer();

	//메시 최적화
	vector<DWORD> vecAdj(m_vecIndex.size());
	m_pMesh->GenerateAdjacency(0.0f, &vecAdj[0]);
	m_pMesh->OptimizeInplace(
		D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
		&vecAdj[0],
		NULL,
		NULL,
		NULL
	);
}

void cScene_MapTool::DeleteBuild()
{
	if (m_vecSetBuild.size() == NULL) return;

	int buildIndex = -1;

	if (KEY_MGR->IsOnceDown(VK_LBUTTON)) buildIndex = SelectBuildNum();

	if (buildIndex > -1) m_vecSetBuild.erase(m_vecSetBuild.begin() + buildIndex);
}

void cScene_MapTool::EditBuild()
{
	if (m_vecSetBuild.size() == NULL) return;

	if (KEY_MGR->IsOnceDown(VK_LBUTTON)) m_nSelectBuildNum = SelectBuildNum();

	if (m_nSelectBuildNum > -1)
	{
		if (KEY_MGR->IsOnceDown('U'))
			m_vecSetBuild[m_nSelectBuildNum]->trans.Scaling(D3DXVECTOR3(0.1f, 0.1f, 0.1f));
		if (KEY_MGR->IsOnceDown('J'))
			m_vecSetBuild[m_nSelectBuildNum]->trans.Scaling(D3DXVECTOR3(-0.1f, -0.1f, -0.1f));
		//X
		if (KEY_MGR->IsOnceDown('R'))
			m_vecSetBuild[m_nSelectBuildNum]->trans.RotateSelf(0.1f, 0.0f, 0.0f);
		if (KEY_MGR->IsOnceDown('F'))
			m_vecSetBuild[m_nSelectBuildNum]->trans.RotateSelf(-0.1f, 0.0f, 0.0f);
		//Y
		if (KEY_MGR->IsOnceDown('T'))
			m_vecSetBuild[m_nSelectBuildNum]->trans.RotateSelf(0.0f, 0.1f, 0.0f);
		if (KEY_MGR->IsOnceDown('G'))
			m_vecSetBuild[m_nSelectBuildNum]->trans.RotateSelf(0.0f, -0.1f, 0.0f);
		//Z
		if (KEY_MGR->IsOnceDown('Y'))
			m_vecSetBuild[m_nSelectBuildNum]->trans.RotateSelf(0.0f, 0.0f, 0.1f);
		if (KEY_MGR->IsOnceDown('H'))
			m_vecSetBuild[m_nSelectBuildNum]->trans.RotateSelf(0.0f, 0.0f, -0.1f);
	}
}

void cScene_MapTool::FixGroundHeight()
{
	if (KEY_MGR->IsStayDown(VK_LBUTTON))
	{
		POINT ptMouse = GetMousePos();
		D3DXVECTOR3 curVertex(0, 0, 0);   //브러쉬 안에 정점
		D3DXVECTOR3 tempHit;

		this->mainCamera.GetWorldRay(&this->m_ClickRay, ptMouse);

		if (IsIntersectRay(&tempHit, &this->m_ClickRay))
		{
			float fX = ((m_nMapSize / 2) - (int)tempHit.x);
			float fZ = ((m_nMapSize / 2) - (int)tempHit.z) * m_nMapSize;

			int PickIndex = m_nTotalVertex - (fZ + fX);

			//w * i + i / (w * 6)
			for (int z = -m_nBrushRadius; z < m_nBrushRadius; ++z)
			{
				for (int x = -m_nBrushRadius; x < m_nBrushRadius; ++x)
				{
					//((z * m_nMapSize) + PickIndex) + x; 네모

					int index = ((z * m_nMapSize) + PickIndex) + x;

					//예외처리 추가
					if (index < 0 || index > m_nTotalVertex) continue;

					//원 검사
					curVertex = m_pVertex[index].p;

					m_vPickingPos = m_pVertex[index].p;

					float len = (curVertex.x - tempHit.x)*(curVertex.x - tempHit.x) + (curVertex.z - tempHit.z)*(curVertex.z - tempHit.z);

					// 차후 다른 알고리즘을 적용해서 자연스럽게 증가되게 만든다.
					// 중심점으로 부터 거리가 멀수록 증가값이 작아지게
					//float y = 1 - 1.0f / m_nBrushRadius * len * /*m_fSlopeRate*/ + m_nBrushRadius;

					//float y = 0.01f - (sqrt(len) / (m_nBrushRadius * m_fGapSize));
					float y = cosf(sqrt(len) / m_nBrushRadius) * m_nHeightSize;

					if (len < ((m_nBrushRadius) * (m_nBrushRadius)))  // 정점이 원안에 있다면
					{
						if (y == 0) m_pVertex[index].p.y = y;
						else m_pVertex[index].p.y += y;
					}
				}
			}

			ResetMesh();

		}
	}
}

void cScene_MapTool::BrushCircleDraw()
{
	int count = 50;
	float radian = D3DX_PI * 2 / count;               // 회전각 라디안
	int brushOutSize = m_nBrushRadius;         // 브러쉬 크기 반지름 

	BRUSHVERTEX brushOutLine[2];               // 두점을 D3DTS_LINELIST를 통해 그려나간다.
	D3DXCOLOR m_dwBrushOutColor(255, 0, 0, 0);
	D3DXVECTOR3 curPos(1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 newPos;
	D3DXMATRIX mat;


	Device->SetRenderState(D3DRS_LIGHTING, false);
	Device->SetTexture(0, NULL);
	Device->SetFVF(D3DFVF_BRUSHVERTEX);

	brushOutLine[1].p = curPos * brushOutSize + m_vCircleCenter;
	//brushOutLine[1].p.y = GetHeightY(brushOutLine[1].p.x, brushOutLine[1].p.z);
	brushOutLine[1].color = m_dwBrushOutColor;

	for (int i = 1; i < count + 1; ++i)
	{
		D3DXMatrixRotationY(&mat, radian*i);
		D3DXVec3TransformCoord(&newPos, &curPos, &mat);
		D3DXVec3Normalize(&newPos, &newPos);

		brushOutLine[0] = brushOutLine[1];
		brushOutLine[1].p = newPos * brushOutSize + m_vCircleCenter;
		//brushOutLine[1].p.y = GetHeightY(brushOutLine[1].p.x, brushOutLine[1].p.z);
		brushOutLine[1].color = m_dwBrushOutColor;
		Device->DrawPrimitiveUP(D3DPT_LINELIST, 1, brushOutLine, sizeof(BRUSHVERTEX));
	}
}

bool cScene_MapTool::BMPFileOpen()
{
	m_cpMapPath = "./Resources/MapTool/TownMap.bmp";

	fopen_s(&infile, m_cpMapPath, "rb");
	if (infile == NULL) { printf("No Image File"); return 0; }

	fread(&hf, sizeof(BITMAPFILEHEADER), 1, infile);

	if (hf.bfType != 0x4D42) exit(1);

	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, infile);

	if (hInfo.biBitCount != 24) { printf("Bad File Format!!"); return 0; }

	fclose(infile);
}

void cScene_MapTool::ResetMesh()
{
	PNT_VERTEX* pVertex = NULL;
	m_pMesh->LockVertexBuffer(NULL, (LPVOID*)&pVertex);
	memcpy(pVertex, m_pVertex, sizeof(PNT_VERTEX) * m_nTotalVertex);
	m_pMesh->UnlockVertexBuffer();

	DWORD* pIndex = NULL;
	m_pMesh->LockIndexBuffer(NULL, (LPVOID*)&pIndex);
	memcpy(pIndex, &m_vecIndex[0], sizeof(DWORD) * m_vecIndex.size());
	m_pMesh->UnlockIndexBuffer();
}

void cScene_MapTool::SaveMap()
{
	FILE* outFile;

	fopen_s(&outFile, "./Resources/Textures/HeightValue.txt", "w");

	for (int i = 0; i < m_nTotalVertex; i++)
	{
		fprintf_s(outFile, "%f ", m_pVertex[i].p.y);
	}

	fclose(outFile);

	SaveBuild();
}

void cScene_MapTool::SaveBuild()
{
	FILE* outFile;

	fopen_s(&outFile, "./Resources/Textures/Build.txt", "w");

	if (m_vecSetBuild.size() == NULL)
	{
		fprintf_s(outFile, "noBuild");
	}
	else
	{
		for (int i = 0; i < m_vecSetBuild.size(); i++)
		{
			fprintf_s(outFile, "K %s\n", m_vecSetBuild[i]->skinPath);
			fprintf_s(outFile, "P %f %f\n", m_vecSetBuild[i]->position.x, m_vecSetBuild[i]->position.z); //포지션 저장
			fprintf_s(outFile, "S %f %f %f\n", m_vecSetBuild[i]->trans.GetScale().x, m_vecSetBuild[i]->trans.GetScale().y, m_vecSetBuild[i]->trans.GetScale().z);
			fprintf_s(outFile, "1 %f %f %f\n", m_vecSetBuild[i]->trans.GetWorldRotateMatrix()._11,
				m_vecSetBuild[i]->trans.GetWorldRotateMatrix()._12,
				m_vecSetBuild[i]->trans.GetWorldRotateMatrix()._13);
			fprintf_s(outFile, "2 %f %f %f\n", m_vecSetBuild[i]->trans.GetWorldRotateMatrix()._21,
				m_vecSetBuild[i]->trans.GetWorldRotateMatrix()._22,
				m_vecSetBuild[i]->trans.GetWorldRotateMatrix()._23);
			fprintf_s(outFile, "3 %f %f %f\n", m_vecSetBuild[i]->trans.GetWorldRotateMatrix()._31,
				m_vecSetBuild[i]->trans.GetWorldRotateMatrix()._32,
				m_vecSetBuild[i]->trans.GetWorldRotateMatrix()._33);
			fprintf_s(outFile, "B \n");
		}
	}

	fclose(outFile);

}

int cScene_MapTool::SelectBuildNum()
{
	this->mainCamera.GetWorldRay(&this->m_ClickRay, ptMouse);

	for (int i = 0; i < m_vecSetBuild.size(); i++)
	{
		if (PHYSICS_MGR->IsRayHitBoundBox(&this->m_ClickRay, m_vecSetBuild[i]->box, &m_vecSetBuild[i]->trans, false, NULL, NULL))
		{
			return i;
		}
	}
	return -1;
}

void cScene_MapTool::FixBrushSize()
{
	if (KEY_MGR->IsOnceDown('9'))
	{
		m_nBrushRadius -= 1.0f;
		if (m_nBrushRadius < 1.0f)
		{
			m_nBrushRadius = 1.0f;
		}
	}
	else if (KEY_MGR->IsOnceDown('0'))
	{
		m_nBrushRadius += 1.0f;
	}

	if (KEY_MGR->IsOnceDown('O'))
	{
		m_nHeightSize -= 1.0f;
	}
	else if (KEY_MGR->IsOnceDown('P'))
	{
		m_nHeightSize += 1.0f;
	}
	else if (KEY_MGR->IsOnceDown('L'))
	{
		m_nHeightSize = 0;
	}

}

void cScene_MapTool::PickBulid()
{
	if (KEY_MGR->IsOnceDown(VK_LBUTTON))
	{
		LPSTBUILD tempBuild;

		tempBuild = new STBUILDE;
		tempBuild->box = new cBoundBox();
		tempBuild->skin = m_pTestHome;
		tempBuild->skinPath = m_strSkinPath;
		tempBuild->trans = m_BTestTrans;
		tempBuild->position = m_vCircleCenter;
		tempBuild->box = tempBuild->skin->GetDefaultBound();

		m_vecSetBuild.push_back(tempBuild);
	}
}

void cScene_MapTool::ModeSelect()
{
	if (KEY_MGR->IsOnceDown('1'))
	{
		m_stModeState = MODE_HEIGHT;
		m_strMode = "Height";
	}
	else if (KEY_MGR->IsOnceDown('2'))
	{
		m_stModeState = MODE_BUILD;
		m_strMode = "Build";
	}
	else if (KEY_MGR->IsOnceDown('3'))
	{
		m_nSelectBuildNum = -1;
		m_stModeState = MODE_EDIT;
		m_strMode = "Edit";
	}
	else if (KEY_MGR->IsOnceDown('4'))
	{
		m_stModeState = MODE_DELETE;
		m_strMode = "Delete";
	}
	else if (KEY_MGR->IsOnceDown('8'))
	{
		SaveMap();
		SCENE_MGR->ChangeSceneWithLoading("타이틀씬", "로딩씬");
	}
}

void cScene_MapTool::ModeUpdate()
{
	switch (m_stModeState)
	{
	case cScene_MapTool::MODE_HEIGHT:
		FixBrushSize();
		FixGroundHeight();
		break;
	case cScene_MapTool::MODE_BUILD:
	{
		D3DXMATRIXA16 mat;
		D3DXMATRIXA16 matScale;
		D3DXMatrixScaling(&matScale, 0.01f, 0.01f, 0.01f);

		//회전 180
		D3DXMATRIXA16 matRotate;
		D3DXMatrixRotationY(&matRotate, 1.0f * ONE_RAD);

		mat = matScale * matRotate;

		m_pTestHome = RESOURCE_XMESHSTATIC->GetResource(m_strSkinPath, &mat);

		if (m_isBuildPick) PickBulid();
	}
	break;
	case cScene_MapTool::MODE_EDIT:
		EditBuild();
		break;
	case cScene_MapTool::MODE_DELETE:
		DeleteBuild();
		break;
	default:
		break;
	}
}

void cScene_MapTool::PickGround()
{
	POINT ptMouse = GetMousePos();
	D3DXVECTOR3 curVertex(0, 0, 0);   //브러쉬 안에 정점
	D3DXVECTOR3 tempHit;

	this->mainCamera.GetWorldRay(&this->m_ClickRay, ptMouse);

	if (IsIntersectRay(&tempHit, &this->m_ClickRay))
	{
		float fX = ((m_nMapSize / 2) - (int)tempHit.x);
		float fZ = ((m_nMapSize / 2) - (int)tempHit.z) * m_nMapSize;

		int PickIndex = m_nTotalVertex - (fZ + fX);

		//w * i + i / (w * 6)
		for (int z = -m_nBrushRadius; z < m_nBrushRadius; ++z)
		{
			for (int x = -m_nBrushRadius; x < m_nBrushRadius; ++x)
			{
				//((z * m_nMapSize) + PickIndex) + x; 네모

				int index = ((z * m_nMapSize) + PickIndex) + x;

				//예외처리 추가
				if (index < 0 || index > m_nTotalVertex) continue;

				m_vCircleCenter = tempHit;
			}
		}
	}
}

void cScene_MapTool::PickisPossible()
{
	RECT rc;
	SetRect(&rc, m_pUIRoot->GetPosition().x, m_pUIRoot->GetPosition().y,
		m_pUIRoot->GetPosition().x + m_pUIRoot->GetSize().nWidth,
		m_pUIRoot->GetPosition().y + m_pUIRoot->GetSize().nHeight);

	if (PtInRect(&rc, ptMouse))
	{
		m_isBuildPick = false;
	}
	else
	{
		m_isBuildPick = true;
	}
}

void cScene_MapTool::PrintText()
{
	DXFONT_MGR->PrintText("1,Heigth 2,Build 3,Edit(Build) 4,Delete 8,Save", 700, 10, D3DXCOLOR(0, 0, 0, 1));
	DXFONT_MGR->PrintText("Mode : " + m_strMode, 10, 10, D3DXCOLOR(0, 0, 0, 1));
	if (m_stModeState == MODE_HEIGHT)
	{
		string tmpBrushSz = std::to_string(m_nBrushRadius);
		string tmpHeigthSz = std::to_string(m_nHeightSize);

		DXFONT_MGR->PrintText("Brush  Size " + tmpBrushSz + ": -(9) +(0)", 10, 50, D3DXCOLOR(0, 0, 0, 1));
		DXFONT_MGR->PrintText("Height Size " + tmpHeigthSz + ": -(O) +(P)", 10, 70, D3DXCOLOR(0, 0, 0, 1));
		DXFONT_MGR->PrintText("Height Zero : L", 10, 90, D3DXCOLOR(0, 0, 0, 1));
	}
	else if (m_stModeState == MODE_EDIT)
	{
		DXFONT_MGR->PrintText("Scale   : U, J", 10, 50, D3DXCOLOR(0, 0, 0, 1));
		DXFONT_MGR->PrintText("Rotate X : R, F", 10, 70, D3DXCOLOR(0, 0, 0, 1));
		DXFONT_MGR->PrintText("Rotate Y : T, G", 10, 90, D3DXCOLOR(0, 0, 0, 1));
		DXFONT_MGR->PrintText("Rotate Z : Y, H", 10, 110, D3DXCOLOR(0, 0, 0, 1));
	}
}

float cScene_MapTool::GetHeightY(float x, float z)
{
	float height = 0.0f;

	// 지형의 시작점을 원점으로 이동하는 변환
	x = ((float)m_nMapSize) + x;
	z = ((float)m_nMapSize) - z;            // z값이 아래로 증가 하게 만든다.
	x = (float)(x);
	z = (float)(z);

	// 마우스 위치가 맵을 넘어가면 0.0f로 셋팅
	if (0.0f >= x || x >= (float)(m_pVertex[m_nTotalVertex].p.x) ||
		0.0f >= z || z >= (float)(m_pVertex[m_nTotalVertex].p.z))
	{
		return 0.0f;
	}

	long int nx = (long int)::floorf(x);                  // floorf는 소숫점을 없애버린다.
	long int nz = (long int)::floorf(z);

	//  A   B
	//  *---*
	//  | / |
	//  *---*
	//  C   D (1,1)
	float A = m_pVertex[nx + nz * m_nTotalVertex].p.y;   // nx, nz가 포함된 위치의 셀 정점의 Y값을 얻어온다.
	float B = m_pVertex[nx + 1 + nz * m_nTotalVertex].p.y;
	float C = m_pVertex[nx + (nz + 1) * m_nTotalVertex].p.y;
	float D = m_pVertex[nx + 1 + (nz + 1) * m_nTotalVertex].p.y;

	// nx, nz가 포함된 셀이 0,0 원점의 위치가 되도록 가상으로 설정한다.(0~1사이 값이된다)
	float dx = x - nx;
	float dz = z - nz;

	// 벡터의 합의 기하학적 의미와 선형보간을 통해 x,z위치의 y값을 얻어온다.
	// 셀은 가로 세로 1의 정사각형이다. 즉 정사각형의 대각선은 y=x이고, dx와 dz의 비교를 통해서 삼각형의 위치를 판별.
	if (dz + dx < 1.0f)  // 위쪽 삼각형
	{
		float uy = B - A;
		float vy = C - A;
		height = A + _Lerp(0.0f, uy, dx) + _Lerp(0.0f, vy, dz);
	}
	else         // 아래쪽 삼각형
	{
		float uy = C - D;
		float vy = B - D;
		height = D + _Lerp(0.0f, uy, 1.0f - dx) + _Lerp(0.0f, vy, 1.0f - dz);
	}
	return height;
}

bool cScene_MapTool::IsIntersectRay(OUT D3DXVECTOR3 * pVector, IN const LPRay pRay)
{
	//결과 값을 받아올 벡터 준비
	static vector<D3DXVECTOR3> hits;
	hits.clear();

	//쿼드트리 를 통해 레이 체크...
	this->m_pQuadTree->GetMapToolRayHits(
		&hits, pRay);

	if (hits.size() == 0)
		return false;

	//충돌되었다... ( origin 에서 가장 가까운 놈 으로 hit 위치 )

	//첫번째 충돌지점
	D3DXVECTOR3 hitPos = hits[0];

	//Ray와의 거리
	D3DXVECTOR3 dirToHit = hitPos - pRay->origin;
	float hitDistanceSQ = D3DXVec3LengthSq(&dirToHit);

	for (int i = 1; i < hits.size(); i++)
	{
		D3DXVECTOR3 newHitPos = hits[i];
		dirToHit = newHitPos - pRay->origin;

		float newHitDistanceSQ = D3DXVec3LengthSq(&dirToHit);

		//기존에 저장된 놈보다 더 작다면..
		if (newHitDistanceSQ < hitDistanceSQ) {
			hitDistanceSQ = newHitDistanceSQ;
			hitPos = hits[i];
		}
	}

	*pVector = hitPos;

	return true;
}

void cScene_MapTool::OnClick(cUIButton * pSender)
{
	switch (pSender->GetTag())
	{
	case 1:
		m_strSkinPath = "./Resources/XMeshSkinned/OBJ/House/house.X";
		break;
	case 2:
		m_strSkinPath = "./Resources/XMeshSkinned/OBJ/House2/house2.X";
		break;
	case 3:
		m_strSkinPath = "./Resources/XMeshSkinned/OBJ/House3/house3.X";
		break;
	case 4:
		m_strSkinPath = "./Resources/XMeshSkinned/OBJ/Tower/tower.X";
		break;
	default:
		break;
	}
}