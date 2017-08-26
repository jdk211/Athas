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

//셋팅
HRESULT cMainGame::Init(void)
{
	pMainGame = this;

	srand(GetTickCount());
	Gold = 100000;
	//Device 초기화
	InitDeivce();

	//기본랜더 셋팅
	//나는 정점 라이팅을 사용하지않겠다.
	Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	//매니져 셋팅
	TIME_MGR->Init();
	LOG_MGR->Init( LOG_WINDOW | LOG_FILE, g_hWnd, "Dump" );
	GIZMO_MGR->Init();
	GIZMO_MGR->InitGrid(10, 1.0f);
	DXFONT_MGR->Init();
	SPRITE_MGR->Init();
	GRAPH_MGR->Init(); 
	SOUND_MGR->Init();
	SCENE_MGR->Init();

	//게임에 사용되는 씬추가
	SCENE_MGR->AddScene("타이틀씬", new cScene_Title());
	SCENE_MGR->AddScene("마을씬", new cScene_Town());
	SCENE_MGR->AddScene("맵툴씬", new cScene_MapTool());
	SCENE_MGR->AddScene("테스트씬", new cScene_MapTest());
	SCENE_MGR->AddScene("인트로씬", new cScene_Intro());
	//게임에 사용되는 로딩씬 추가
	SCENE_MGR->AddLoadingScene("로딩씬", new cScene_Loading());

	SOUND_MGR->addSound("브금1", "./Resources/Sounds/dzd.mp3");
	SOUND_MGR->addSound("브금2", "./Resources/Sounds/BGM2.mp3");
	SOUND_MGR->addSound("총소리", "./Resources/Sounds/NailGunSound.ogg");
	SOUND_MGR->addSound("공격", "./Resources/Sounds/HumanMaleAttackA.ogg");
	SOUND_MGR->addSound("발소리", "./Resources/Sounds/footstep.wav", false, true);
	SOUND_MGR->addSound("구매", "./Resources/Sounds/buy.mp3");
	SOUND_MGR->addSound("상인", "./Resources/Sounds/store.mp3");

	//시작씬은 첫번째로
	SCENE_MGR->ChangeScene("마을씬");

	return S_OK;
}	

//해제
void cMainGame::Release()
{
	//매니져 해제
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


	//리소스 해제
	RESOURCE_TEXTURE->ClearResource();
	cResourceMgr_Texture::ReleaseInstance();
	RESOURCE_EFFECT->ClearResource();
	cResourceMgr_Effect::ReleaseInstance();
	RESOURCE_XMESHSTATIC->ClearResource();
	cResourceMgr_XMeshStatic::ReleaseInstance();
	RESOURCE_XMESHSKINNED->ClearResource();
	cResourceMgr_XMeshSkinned::ReleaseInstance();


	//디바이스 해제
	//Device->Release();
	SAFE_RELEASE(Device);
}

//업데이트
void cMainGame::Update()
{
	//사운드 매니져 업데이트
	SOUND_MGR->update();

	//타임매니져 업데이트
	TIME_MGR->UpdateTime( 60.0f );

	//한프레임 갱신 시간
	double timeDelta = TIME_MGR->GetFrameDeltaSec();

	//씬업데이트
	SCENE_MGR->Update( static_cast<float>( timeDelta ));
}

//드로우
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

	if (!SCENE_MGR->IsScenePlay("인트로씬"))
	{
		//버퍼 청소
		if (SUCCEEDED(Device->Clear(
			0,				//청소할 영역의 D3DRECT 배열 갯수		( 전체 클리어 0 )
			NULL,			//청소할 영역의 D3DRECT 배열 포인터		( 전체 클리어 NULL )
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,		//청소될 버퍼 플레그 ( D3DCLEAR_TARGET 컬러버퍼, D3DCLEAR_ZBUFFER 깊이버퍼, D3DCLEAR_STENCIL 스텐실버퍼
			0xFF000000,			//컬러버퍼를 청소하고 채워질 색상( 0xAARRGGBB )
			1.0f,				//깊이버퍼를 청소할값 ( 0 ~ 1, 0 이 카메라에서 제일가까운 1 이 카메라에서 제일 먼 )
			0					//스텐실 버퍼를 채울값
		)))
		{
			//버퍼 청소가 성공했다면...
			Device->BeginScene();	//랜더링 시작

									//씬랜더
			SCENE_MGR->Render();


			Device->EndScene();	//랜더링 종료

								//랜더링된이미지를 화면에 전송
			Device->Present(0, 0, 0, 0);

		}
	}
}


//////////////////////////////////////////////////////////////////////////

//메인 프로시져 함수
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

	case WM_DESTROY:			//윈도우가 파괴된다면..
		PostQuitMessage( 0 );	//프로그램 종료 요청 ( 메시지 루프를 빠져나가게 된다 )
		break;
	}
	return DefWindowProc( hWnd, iMessage, wParam, lParam );
}

