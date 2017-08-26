#include "StdAfx.h"
#include "cMainGame.h"
#include "cImage.h"
#include "cScene_Town.h"
#include "cScene_Title.h"
#include "cScene_Loading.h"
#include "cScene_MapTool.h"
#include "cScene_MapTest.h"
#include "cScene_Intro.h"

cMainGame::cMainGame(void)
{
}

cMainGame::~cMainGame(void)
{
}

//����
HRESULT cMainGame::Init(void)
{
	pMainGame = this;

	srand(GetTickCount());
	Gold = 100000;
	//Device �ʱ�ȭ
	InitDeivce();

	//�⺻���� ����
	//���� ���� �������� ��������ʰڴ�.
	Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	//�Ŵ��� ����
	TIME_MGR->Init();
	LOG_MGR->Init( LOG_WINDOW | LOG_FILE, g_hWnd, "Dump" );
	GIZMO_MGR->Init();
	GIZMO_MGR->InitGrid(10, 1.0f);
	DXFONT_MGR->Init();
	SPRITE_MGR->Init();
	GRAPH_MGR->Init(); 
	SOUND_MGR->Init();
	SCENE_MGR->Init();

	//���ӿ� ���Ǵ� ���߰�
	SCENE_MGR->AddScene("Ÿ��Ʋ��", new cScene_Title());
	SCENE_MGR->AddScene("������", new cScene_Town());
	SCENE_MGR->AddScene("������", new cScene_MapTool());
	SCENE_MGR->AddScene("�׽�Ʈ��", new cScene_MapTest());
	SCENE_MGR->AddScene("��Ʈ�ξ�", new cScene_Intro());
	//���ӿ� ���Ǵ� �ε��� �߰�
	SCENE_MGR->AddLoadingScene("�ε���", new cScene_Loading());

	SOUND_MGR->addSound("���1", "./Resources/Sounds/dzd.mp3");
	SOUND_MGR->addSound("���2", "./Resources/Sounds/BGM2.mp3");
	SOUND_MGR->addSound("�ѼҸ�", "./Resources/Sounds/NailGunSound.ogg");
	SOUND_MGR->addSound("����", "./Resources/Sounds/HumanMaleAttackA.ogg");
	SOUND_MGR->addSound("�߼Ҹ�", "./Resources/Sounds/footstep.wav", false, true);
	SOUND_MGR->addSound("����", "./Resources/Sounds/buy.mp3");
	SOUND_MGR->addSound("����", "./Resources/Sounds/store.mp3");

	//���۾��� ù��°��
	SCENE_MGR->ChangeScene("������");

	return S_OK;
}	

//����
void cMainGame::Release()
{
	//�Ŵ��� ����
	TIME_MGR->Release();
	cTimeMgr::ReleaseInstance();
	KEY_MGR->Release();
	cKeyMgr::ReleaseInstance();
	LOG_MGR->Release();
	cLogMgr::ReleaseInstance();
	GIZMO_MGR->Release();
	cGizmoManager::ReleaseInstance();
	DXFONT_MGR->Release();
	cDXFontManager::ReleaseInstance();
	SPRITE_MGR->Release();
	cSpriteManager::ReleaseInstance();
	cPhysicsManager::ReleaseInstance();
	GRAPH_MGR->Release();
	cGraphManager::ReleaseInstance();
	SOUND_MGR->release();
	soundManager::ReleaseInstance();
	SCENE_MGR->Release();
	cSceneManager::ReleaseInstance();


	//���ҽ� ����
	RESOURCE_TEXTURE->ClearResource();
	cResourceMgr_Texture::ReleaseInstance();
	RESOURCE_EFFECT->ClearResource();
	cResourceMgr_Effect::ReleaseInstance();
	RESOURCE_XMESHSTATIC->ClearResource();
	cResourceMgr_XMeshStatic::ReleaseInstance();
	RESOURCE_XMESHSKINNED->ClearResource();
	cResourceMgr_XMeshSkinned::ReleaseInstance();


	//����̽� ����
	//Device->Release();
	SAFE_RELEASE(Device);
}

//������Ʈ
void cMainGame::Update()
{
	//���� �Ŵ��� ������Ʈ
	SOUND_MGR->update();

	//Ÿ�ӸŴ��� ������Ʈ
	TIME_MGR->UpdateTime( 60.0f );

	//�������� ���� �ð�
	double timeDelta = TIME_MGR->GetFrameDeltaSec();

	//��������Ʈ
	SCENE_MGR->Update( static_cast<float>( timeDelta ));
}

//��ο�
void cMainGame::Draw()
{
	//0x00000000
	//0xFFFF0000		: Red
	//0xFFFFFF00		: Yellow
	//0xFF00FF00		: Green
	//0xFF00FFFF		: Cyan
	//0xFF0000FF		: Blue
	//0xFFFF00FF		: Magenta
	//0xFFFFFFFF		: White
	//0xFF000000		: Black

	if (!SCENE_MGR->IsScenePlay("��Ʈ�ξ�"))
	{
		//���� û��
		if (SUCCEEDED(Device->Clear(
			0,				//û���� ������ D3DRECT �迭 ����		( ��ü Ŭ���� 0 )
			NULL,			//û���� ������ D3DRECT �迭 ������		( ��ü Ŭ���� NULL )
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,		//û�ҵ� ���� �÷��� ( D3DCLEAR_TARGET �÷�����, D3DCLEAR_ZBUFFER ���̹���, D3DCLEAR_STENCIL ���ٽǹ���
			0xFF000000,			//�÷����۸� û���ϰ� ä���� ����( 0xAARRGGBB )
			1.0f,				//���̹��۸� û���Ұ� ( 0 ~ 1, 0 �� ī�޶󿡼� ���ϰ���� 1 �� ī�޶󿡼� ���� �� )
			0					//���ٽ� ���۸� ä�ﰪ
		)))
		{
			//���� û�Ұ� �����ߴٸ�...
			Device->BeginScene();	//������ ����

									//������
			SCENE_MGR->Render();


			Device->EndScene();	//������ ����

								//���������̹����� ȭ�鿡 ����
			Device->Present(0, 0, 0, 0);

		}
	}
}


//////////////////////////////////////////////////////////////////////////

//���� ���ν��� �Լ�
LRESULT cMainGame::MainProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch( iMessage )
	{	
	case WM_TIMER:
		Update();
		break;

	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_ESCAPE:
			DestroyWindow( hWnd );
			break;
		}
		break;

	case WM_DESTROY:			//�����찡 �ı��ȴٸ�..
		PostQuitMessage( 0 );	//���α׷� ���� ��û ( �޽��� ������ ���������� �ȴ� )
		break;
	}
	return DefWindowProc( hWnd, iMessage, wParam, lParam );
}

