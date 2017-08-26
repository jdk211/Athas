#pragma once

//
// 3D sound ��ü
//
class cSound3D
{
public:
	cTransform				Transform;

private:
	FMOD::System*			m_pSystem;			//����� ���� System �� ���� ���´�.
	FMOD::Sound*			m_pSound;			//3D Sound ��ü
	FMOD::Channel*			m_pChanel;			//3D Sound ä�� ( NULL �̸� �÷������� �ƴϴ� )

public:
	cSound3D();
	~cSound3D();

	void Init(FMOD::System* pSystem, FMOD::Sound* pSound);
	void UpdateSound();		//��ġ�� �ٲ�� ��ġ�� �ٲﰪ�� ������Ʈ�ؾ� �Ǳ� ������ �ž�����Ʈ���� �����ش�.
	void Play(bool queued = true);
	void Stop();


};

