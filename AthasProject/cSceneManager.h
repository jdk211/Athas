#pragma once

#include "cSingletonBase.h"
#include <string>
#include <map> 
#include "cScene.h"

class cSceneManager : public cSingletonBase<cSceneManager>
{
private:
	typedef std::map<std::string, cScene*>		MAP_SCENE;

	//로딩쓰래드 함수는 내친구
	friend DWORD CALLBACK LoadingThread(LPVOID lpParam);


public:
	float			fLoadingProgress;			//로딩 진행도 0 ~ 1

private:
	cScene*			m_pNowScene;			//현재 물려 있는씬
	cScene*			m_pReleaseScene;		//날릴 씬
	MAP_SCENE		m_Scenes;				//현재 가지고 있는씬
	MAP_SCENE		m_LoadingScenes;		//로딩씬들

public:
	cSceneManager();
	~cSceneManager();

	HRESULT Init();
	void Release();
	void Update(float timeDelta);
	void Render();


	//게임에 사용되는 씬들은 Init 가 안된다.
	void AddScene(std::string sceneName, cScene* pScene);

	//게임에 사용되는 로딩씬 추가 씬들은 모두 Init 된상태가 된다.
	void AddLoadingScene(std::string sceneName, cScene* pScene);

	//씬변경
	void ChangeScene(std::string sceneName);

	//씬을 변경하는데 로딩씬을 추가하여 변경
	void ChangeSceneWithLoading(
		std::string sceneName,
		std::string loadingSceneName);

	bool IsScenePlay(std::string sceneName);
};


#define SCENE_MGR cSceneManager::GetInstance()