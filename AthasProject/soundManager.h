#pragma once
#include "cSingletonBase.h"
#include "../AthasProject/inc/fmod.hpp"		
#pragma comment(lib, "../AthasProject/lib/fmodex_vc.lib")	//lib ��ũ
using namespace FMOD;		//FMOD ���ӽ����̽� ���

//ä�ι���, �������
#define EXTRACHANNERBUFFER 5
#define SOUNDBUFFER 20
//�ѻ�� ���ۼ���: ���۰� �۾Ƽ� ������ ������ �ذ����ֱ� ����
#define TOTALSOUNDBUFFER SOUNDBUFFER + EXTRACHANNERBUFFER

class soundManager : public cSingletonBase <soundManager>
{
private:
	typedef map<string, Sound**> arrSounds;
	typedef map<string, Sound**>::iterator arrSoundIter;

private:
	System* _system;		//fmod�� �ý��� Ŭ����
	Sound** _sound;			//fmod�� ���� Ŭ����
	Channel** _channel;		//fmod�� ä�� Ŭ����

	arrSounds _mTotalSounds;//�ʿ� ��� ���� ���

public:
	HRESULT Init(void);
	void release(void);
	void update(void);
	void render(void);

	//���� �߰�(Ű��, �����̸�, BGM? ������?)
	void addSound(string keyName, string soundName, bool bgm = FALSE, bool loop = FALSE);

	//���� �÷���(Ű��, �ҷ�) 0.0f ~ 1.0f
	void play(string keyName, float volume = 1.0f);
	//���� ����
	void stop(string keyName);
	//���� �Ͻ�����
	void pause(string keyName);
	//���� �ٽ� ���
	void resume(string ketName);

	//�÷��� ���̳�?
	bool isPlaySound(string keyName);
	//�Ͻ����� ���̳�?
	bool isPauseSound(string KeyName);

	soundManager();
	~soundManager();
};

#define SOUND_MGR soundManager::GetInstance()