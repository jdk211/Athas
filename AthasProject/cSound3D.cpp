#include "stdafx.h"
#include "cSound3D.h"


cSound3D::cSound3D()
: m_pChanel( NULL)
{
}


cSound3D::~cSound3D()
{
}

//위치가 바뀌면 위치가 바뀐값을 업데이트해야 되기 때문에 매업데이트마다 물어준다.
void cSound3D::Init(FMOD::System* pSystem, FMOD::Sound* pSound)
{
	m_pSystem = pSystem;

	//Sound 물린다
	m_pSound = pSound;
}
void cSound3D::UpdateSound()
{
	//채널이 플레이중인니?
	bool isPlaying = false;
	if (this->m_pChanel != NULL){
		this->m_pChanel->isPlaying(&isPlaying);
	}

	//플레이 중이 안니면 할필요 없음...
	if (isPlaying == false) return;

	//여기까지 온다면 재생중이라 볼수 있다.
	D3DXVECTOR3 pos = Transform.GetWorldPosition();

	//중요한건 set3DAttributes 설정할때 일시정지 해놓고 셋팅을해야한다.
	m_pChanel->setPaused(true);

	m_pChanel->set3DAttributes(
		(FMOD_VECTOR*)&pos,			//사운드 위치
		NULL);

	//resume
	m_pChanel->setPaused(false);

}
void cSound3D::Play(bool queued)
{
	//채널이 플레이중인니?
	bool isPlaying = false;
	if (this->m_pChanel != NULL){
		this->m_pChanel->isPlaying(&isPlaying);
	}

	//채널에 사운드 가 재생되고 있다면 해당 채널의 사운드를 정지
	if (isPlaying && queued == false)
		m_pChanel->stop();

	//사운드 재생위치
	D3DXVECTOR3 pos = Transform.GetWorldPosition();

	//일단 플레이를 시키되 정지된 상태로,,,
	//m_pSystem->playSound(
	//	m_pSound,
	//	NULL,
	//	true,				//예로 일시정지된 상태로 플레이가 된다.
	//	&m_pChanel			//사운드 플레이 시킬 채널
	//	);


	//3d 사운드의 위치를 설정한다. 
	//( 위치설정은 사운드가 재생중이고 일시 정지일때 가능하다( 단 일시정지상태도 일단은 플레이 중으로 인식됨 )

	//m_pChanel->set3DAttributes(
	//	(FMOD_VECTOR*)&pos,			//사운드 위치
	//	NULL,						//사운드 이동에 의한 벨로시티 ( NULL 쓰면 벨로시티에대한 소리 웨곡은 없다 )
	//	NULL);

	//일시정지를 해재
	//m_pChanel->setPaused(false);
	//m_pChanel->setPitch(1.5f);
	//3dSound 채널별로 MinMax 설정이 가능
	//m_pChanel->set3DMinMaxDistance( )
}
void cSound3D::Stop()
{
	//채널이 플레이중인니?
	bool isPlaying = false;
	if (this->m_pChanel != NULL){
		this->m_pChanel->isPlaying(&isPlaying);
	}

	if (isPlaying)
		this->m_pChanel->stop();
}


