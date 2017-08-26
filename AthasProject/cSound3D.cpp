#include "stdafx.h"
#include "cSound3D.h"


cSound3D::cSound3D()
: m_pChanel( NULL)
{
}


cSound3D::~cSound3D()
{
}

//��ġ�� �ٲ�� ��ġ�� �ٲﰪ�� ������Ʈ�ؾ� �Ǳ� ������ �ž�����Ʈ���� �����ش�.
void cSound3D::Init(FMOD::System* pSystem, FMOD::Sound* pSound)
{
	m_pSystem = pSystem;

	//Sound ������
	m_pSound = pSound;
}
void cSound3D::UpdateSound()
{
	//ä���� �÷������δ�?
	bool isPlaying = false;
	if (this->m_pChanel != NULL){
		this->m_pChanel->isPlaying(&isPlaying);
	}

	//�÷��� ���� �ȴϸ� ���ʿ� ����...
	if (isPlaying == false) return;

	//������� �´ٸ� ������̶� ���� �ִ�.
	D3DXVECTOR3 pos = Transform.GetWorldPosition();

	//�߿��Ѱ� set3DAttributes �����Ҷ� �Ͻ����� �س��� �������ؾ��Ѵ�.
	m_pChanel->setPaused(true);

	m_pChanel->set3DAttributes(
		(FMOD_VECTOR*)&pos,			//���� ��ġ
		NULL);

	//resume
	m_pChanel->setPaused(false);

}
void cSound3D::Play(bool queued)
{
	//ä���� �÷������δ�?
	bool isPlaying = false;
	if (this->m_pChanel != NULL){
		this->m_pChanel->isPlaying(&isPlaying);
	}

	//ä�ο� ���� �� ����ǰ� �ִٸ� �ش� ä���� ���带 ����
	if (isPlaying && queued == false)
		m_pChanel->stop();

	//���� �����ġ
	D3DXVECTOR3 pos = Transform.GetWorldPosition();

	//�ϴ� �÷��̸� ��Ű�� ������ ���·�,,,
	//m_pSystem->playSound(
	//	m_pSound,
	//	NULL,
	//	true,				//���� �Ͻ������� ���·� �÷��̰� �ȴ�.
	//	&m_pChanel			//���� �÷��� ��ų ä��
	//	);


	//3d ������ ��ġ�� �����Ѵ�. 
	//( ��ġ������ ���尡 ������̰� �Ͻ� �����϶� �����ϴ�( �� �Ͻ��������µ� �ϴ��� �÷��� ������ �νĵ� )

	//m_pChanel->set3DAttributes(
	//	(FMOD_VECTOR*)&pos,			//���� ��ġ
	//	NULL,						//���� �̵��� ���� ���ν�Ƽ ( NULL ���� ���ν�Ƽ������ �Ҹ� ������ ���� )
	//	NULL);

	//�Ͻ������� ����
	//m_pChanel->setPaused(false);
	//m_pChanel->setPitch(1.5f);
	//3dSound ä�κ��� MinMax ������ ����
	//m_pChanel->set3DMinMaxDistance( )
}
void cSound3D::Stop()
{
	//ä���� �÷������δ�?
	bool isPlaying = false;
	if (this->m_pChanel != NULL){
		this->m_pChanel->isPlaying(&isPlaying);
	}

	if (isPlaying)
		this->m_pChanel->stop();
}


