#pragma once

//
// 3D sound 객체
//
class cSound3D
{
public:
	cTransform				Transform;

private:
	FMOD::System*			m_pSystem;			//재생을 위해 System 을 물어 놓는다.
	FMOD::Sound*			m_pSound;			//3D Sound 객체
	FMOD::Channel*			m_pChanel;			//3D Sound 채널 ( NULL 이면 플레이중이 아니다 )

public:
	cSound3D();
	~cSound3D();

	void Init(FMOD::System* pSystem, FMOD::Sound* pSound);
	void UpdateSound();		//위치가 바뀌면 위치가 바뀐값을 업데이트해야 되기 때문에 매업데이트마다 물어준다.
	void Play(bool queued = true);
	void Stop();


};

