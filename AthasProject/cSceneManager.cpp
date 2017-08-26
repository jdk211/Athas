#include "stdafx.h"
#include "cSceneManager.h"



//로딩스레드의 메인함수
DWORD CALLBACK LoadingThread(LPVOID lpParam) // LPVOID 는 void* 형 포인터 void* 포인터는 모든 주소를 다 받을수 있으며 받은 주소를 형변환하여 사용한다.
{

	//매개변수로 받은씬 형변환
	cScene* pScene = (cScene*)lpParam;


	//매개변수로 받은 씬 초기화
	pScene->Init();

	//여기까지 왔다면 pScene 은 Init 가 끝난것임

	//SCENE_MGR 의 현재 씬을 교체 씬으로 전환
	SCENE_MGR->m_pNowScene = pScene;

	return 0;
}



cSceneManager::cSceneManager()
	:m_pNowScene(NULL), m_pReleaseScene(NULL)
{
}


cSceneManager::~cSceneManager()
{
}


HRESULT cSceneManager::Init()
{

	return S_OK;
}
void cSceneManager::Release()
{
	MAP_SCENE::iterator iter;

	//물려있는 로딩씬 삭제
	for (iter = this->m_LoadingScenes.begin(); iter != this->m_LoadingScenes.end(); ++iter) {


		//로딩씬은 init 가 된 상태에서 가지고있기 때문에
		//여기서 해재하고 삭제한다
		iter->second->Release();

		if (iter->second == m_pNowScene)
		{
			m_pNowScene = NULL;
		}

		SAFE_DELETE(iter->second);
	}


	//물려있는 씬이 있다면..
	if (this->m_pNowScene != NULL) {
		this->m_pNowScene->Release();
	}

	//물려있는 씬 삭제
	for (iter = this->m_Scenes.begin(); iter != this->m_Scenes.end(); ++iter)
		SAFE_DELETE(iter->second);



}
void cSceneManager::Update(float timeDelta)
{
	if (m_pReleaseScene != NULL) {

		m_pReleaseScene->Release();
		m_pReleaseScene = NULL;
	}


	if (m_pNowScene != NULL)
		m_pNowScene->Update(timeDelta);
}
void cSceneManager::Render()
{
	if (m_pNowScene != NULL) {

		m_pNowScene->mainCamera.BeginRender(0);

		m_pNowScene->Render();

		//여기 가 랜더링 명령
		GIZMO_MGR->RenderGrid();

		SPRITE_MGR->BeginSpriteRender();
		DXFONT_MGR->SetSprite(SPRITE_MGR->GetSprite());

		m_pNowScene->Scene_RenderSprite();

		DXFONT_MGR->SetSprite(NULL);
		SPRITE_MGR->EndSpriteRender();
	}
}


//게임에 사용되는 씬들은 Init 가 안된다.
void cSceneManager::AddScene(std::string sceneName, cScene* pScene)
{
	MAP_SCENE::iterator  iter = m_Scenes.find(sceneName);

	//중복방지
	if (iter == m_Scenes.end()) {
		m_Scenes.insert(std::make_pair(sceneName, pScene));
	}
}

//게임에 사용되는 로딩씬 추가 씬들은 모두 Init 된상태가 된다.
void cSceneManager::AddLoadingScene(std::string sceneName, cScene* pScene)
{

	MAP_SCENE::iterator  iter = m_LoadingScenes.find(sceneName);

	//중복방지
	if (iter == m_LoadingScenes.end()) {

		//Init 를 하고 는다.
		pScene->Init();
		m_LoadingScenes.insert(std::make_pair(sceneName, pScene));
	}
}

//씬변경
void cSceneManager::ChangeScene(std::string sceneName)
{

	//씬을 찾는다.
	MAP_SCENE::iterator  iter = m_Scenes.find(sceneName);

	if (iter == m_Scenes.end())
		return;


	//기존씬은 해재
	//if (m_pNowScene != NULL)
	//	m_pNowScene->Release();
	this->m_pReleaseScene = m_pNowScene;

	//바뀌는 씬 초기화
	iter->second->Init();

	//현재 씬으로 물려
	this->m_pNowScene = iter->second;

}

//씬을 변경하는데 로딩씬을 추가하여 변경
void cSceneManager::ChangeSceneWithLoading(
	std::string sceneName,
	std::string loadingSceneName)
{
	//로딩 씬을 찾는다.
	MAP_SCENE::iterator
		iter = m_LoadingScenes.find(loadingSceneName);

	//못찾았다.
	if (iter == m_LoadingScenes.end()) {
		return;
	}

	//로딩씬기억
	cScene* pLoadingScene = iter->second;


	//변경될 씬을 찾는다.
	iter = m_Scenes.find(sceneName);

	//못찾았다.
	if (iter == m_Scenes.end()) {
		return;
	}

	//변경될씬 기억
	cScene* pNextScene = iter->second;


	//진행률 0.0f;

	fLoadingProgress = 0.0f;
	//
	// 멀티 쓰레드 로딩 시작
	//
	//다른 쓰레드로 다음 씬을 Init 한다.
	DWORD dwThreadID;
	HANDLE hThread;

	//일꾼 생성
	hThread = CreateThread(
		NULL,					//쓰레드 속성 일단 NULL				
		0,						//쓰레드의 스텍 사이즈 (0 으로하면 MainThread 와 같은 1MB 가 된다 )
		LoadingThread,			//새롭게생성되는 쓰레드의 메인함수 CALLBACK 함수포인터
		pNextScene,				//새롭게생성되는 쓰레드의 메인함수의 매개변수 포인터 ( LoadingThread 의 매개변수로 들어간다 )
		0,						//쓰레드 플레그 ( 0 으로 주면 바로 LoadingThread 가실행 )
		&dwThreadID				//만들어진 쓰레드 ID 
	);

	//더이상 조작할필요는 없느니 핸들은 날려도 무방
	CloseHandle(hThread);




	//이전씬은 해제씬으로 등록
	m_pReleaseScene = m_pNowScene;

	//새로운씬을 로딩씬으로 
	m_pNowScene = pLoadingScene;

}

bool cSceneManager::IsScenePlay(std::string sceneName)
{
	MAP_SCENE::iterator find = m_Scenes.find(sceneName);
	return (find->second == m_pNowScene);
}

