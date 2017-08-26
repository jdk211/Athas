#pragma once
#include "cSingletonBase.h"
#include "../AthasProject/inc/fmod.hpp"		
#pragma comment(lib, "../AthasProject/lib/fmodex_vc.lib")	//lib 링크
using namespace FMOD;		//FMOD 네임스페이스 사용

//채널버퍼, 사운드버퍼
#define EXTRACHANNERBUFFER 5
#define SOUNDBUFFER 20
//총사운 버퍼설정: 버퍼가 작아서 씹히는 문제를 해결해주기 위함
#define TOTALSOUNDBUFFER SOUNDBUFFER + EXTRACHANNERBUFFER

class soundManager : public cSingletonBase <soundManager>
{
private:
	typedef map<string, Sound**> arrSounds;
	typedef map<string, Sound**>::iterator arrSoundIter;

private:
	System* _system;		//fmod의 시스템 클래스
	Sound** _sound;			//fmod의 사운드 클래스
	Channel** _channel;		//fmod의 채널 클래스

	arrSounds _mTotalSounds;//맵에 담긴 사운드 목록

public:
	HRESULT Init(void);
	void release(void);
	void update(void);
	void render(void);

	//사운드 추가(키값, 파일이름, BGM? 루프냐?)
	void addSound(string keyName, string soundName, bool bgm = FALSE, bool loop = FALSE);

	//사운드 플레이(키값, 불륨) 0.0f ~ 1.0f
	void play(string keyName, float volume = 1.0f);
	//사운드 정지
	void stop(string keyName);
	//사운드 일시정지
	void pause(string keyName);
	//사운드 다시 재생
	void resume(string ketName);

	//플레이 중이냐?
	bool isPlaySound(string keyName);
	//일시정지 중이냐?
	bool isPauseSound(string KeyName);

	soundManager();
	~soundManager();
};

#define SOUND_MGR soundManager::GetInstance()