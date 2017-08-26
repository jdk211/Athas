#pragma once

#include "cSingletonBase.h"
#include <string>
#include <map> 
#include "cScene.h"

class cSceneManager : public cSingletonBase<cSceneManager>
{
private:
	typedef std::map<std::string, cScene*>		MAP_SCENE;

	//�ε������� �Լ��� ��ģ��
	friend DWORD CALLBACK LoadingThread(LPVOID lpParam);


public:
	float			fLoadingProgress;			//�ε� ���൵ 0 ~ 1

private:
	cScene*			m_pNowScene;			//���� ���� �ִ¾�
	cScene*			m_pReleaseScene;		//���� ��
	MAP_SCENE		m_Scenes;				//���� ������ �ִ¾�
	MAP_SCENE		m_LoadingScenes;		//�ε�����

public:
	cSceneManager();
	~cSceneManager();

	HRESULT Init();
	void Release();
	void Update(float timeDelta);
	void Render();


	//���ӿ� ���Ǵ� ������ Init �� �ȵȴ�.
	void AddScene(std::string sceneName, cScene* pScene);

	//���ӿ� ���Ǵ� �ε��� �߰� ������ ��� Init �Ȼ��°� �ȴ�.
	void AddLoadingScene(std::string sceneName, cScene* pScene);

	//������
	void ChangeScene(std::string sceneName);

	//���� �����ϴµ� �ε����� �߰��Ͽ� ����
	void ChangeSceneWithLoading(
		std::string sceneName,
		std::string loadingSceneName);

	bool IsScenePlay(std::string sceneName);
};


#define SCENE_MGR cSceneManager::GetInstance()