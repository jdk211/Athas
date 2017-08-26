#include "stdafx.h"
#include "cSceneManager.h"



//�ε��������� �����Լ�
DWORD CALLBACK LoadingThread(LPVOID lpParam) // LPVOID �� void* �� ������ void* �����ʹ� ��� �ּҸ� �� ������ ������ ���� �ּҸ� ����ȯ�Ͽ� ����Ѵ�.
{

	//�Ű������� ������ ����ȯ
	cScene* pScene = (cScene*)lpParam;


	//�Ű������� ���� �� �ʱ�ȭ
	pScene->Init();

	//������� �Դٸ� pScene �� Init �� ��������

	//SCENE_MGR �� ���� ���� ��ü ������ ��ȯ
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

	//�����ִ� �ε��� ����
	for (iter = this->m_LoadingScenes.begin(); iter != this->m_LoadingScenes.end(); ++iter) {


		//�ε����� init �� �� ���¿��� �������ֱ� ������
		//���⼭ �����ϰ� �����Ѵ�
		iter->second->Release();

		if (iter->second == m_pNowScene)
		{
			m_pNowScene = NULL;
		}

		SAFE_DELETE(iter->second);
	}


	//�����ִ� ���� �ִٸ�..
	if (this->m_pNowScene != NULL) {
		this->m_pNowScene->Release();
	}

	//�����ִ� �� ����
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

		//���� �� ������ ���
		GIZMO_MGR->RenderGrid();

		SPRITE_MGR->BeginSpriteRender();
		DXFONT_MGR->SetSprite(SPRITE_MGR->GetSprite());

		m_pNowScene->Scene_RenderSprite();

		DXFONT_MGR->SetSprite(NULL);
		SPRITE_MGR->EndSpriteRender();
	}
}


//���ӿ� ���Ǵ� ������ Init �� �ȵȴ�.
void cSceneManager::AddScene(std::string sceneName, cScene* pScene)
{
	MAP_SCENE::iterator  iter = m_Scenes.find(sceneName);

	//�ߺ�����
	if (iter == m_Scenes.end()) {
		m_Scenes.insert(std::make_pair(sceneName, pScene));
	}
}

//���ӿ� ���Ǵ� �ε��� �߰� ������ ��� Init �Ȼ��°� �ȴ�.
void cSceneManager::AddLoadingScene(std::string sceneName, cScene* pScene)
{

	MAP_SCENE::iterator  iter = m_LoadingScenes.find(sceneName);

	//�ߺ�����
	if (iter == m_LoadingScenes.end()) {

		//Init �� �ϰ� �´�.
		pScene->Init();
		m_LoadingScenes.insert(std::make_pair(sceneName, pScene));
	}
}

//������
void cSceneManager::ChangeScene(std::string sceneName)
{

	//���� ã�´�.
	MAP_SCENE::iterator  iter = m_Scenes.find(sceneName);

	if (iter == m_Scenes.end())
		return;


	//�������� ����
	//if (m_pNowScene != NULL)
	//	m_pNowScene->Release();
	this->m_pReleaseScene = m_pNowScene;

	//�ٲ�� �� �ʱ�ȭ
	iter->second->Init();

	//���� ������ ����
	this->m_pNowScene = iter->second;

}

//���� �����ϴµ� �ε����� �߰��Ͽ� ����
void cSceneManager::ChangeSceneWithLoading(
	std::string sceneName,
	std::string loadingSceneName)
{
	//�ε� ���� ã�´�.
	MAP_SCENE::iterator
		iter = m_LoadingScenes.find(loadingSceneName);

	//��ã�Ҵ�.
	if (iter == m_LoadingScenes.end()) {
		return;
	}

	//�ε������
	cScene* pLoadingScene = iter->second;


	//����� ���� ã�´�.
	iter = m_Scenes.find(sceneName);

	//��ã�Ҵ�.
	if (iter == m_Scenes.end()) {
		return;
	}

	//����ɾ� ���
	cScene* pNextScene = iter->second;


	//����� 0.0f;

	fLoadingProgress = 0.0f;
	//
	// ��Ƽ ������ �ε� ����
	//
	//�ٸ� ������� ���� ���� Init �Ѵ�.
	DWORD dwThreadID;
	HANDLE hThread;

	//�ϲ� ����
	hThread = CreateThread(
		NULL,					//������ �Ӽ� �ϴ� NULL				
		0,						//�������� ���� ������ (0 �����ϸ� MainThread �� ���� 1MB �� �ȴ� )
		LoadingThread,			//���ӰԻ����Ǵ� �������� �����Լ� CALLBACK �Լ�������
		pNextScene,				//���ӰԻ����Ǵ� �������� �����Լ��� �Ű����� ������ ( LoadingThread �� �Ű������� ���� )
		0,						//������ �÷��� ( 0 ���� �ָ� �ٷ� LoadingThread ������ )
		&dwThreadID				//������� ������ ID 
	);

	//���̻� �������ʿ�� ������ �ڵ��� ������ ����
	CloseHandle(hThread);




	//�������� ���������� ���
	m_pReleaseScene = m_pNowScene;

	//���ο���� �ε������� 
	m_pNowScene = pLoadingScene;

}

bool cSceneManager::IsScenePlay(std::string sceneName)
{
	MAP_SCENE::iterator find = m_Scenes.find(sceneName);
	return (find->second == m_pNowScene);
}

