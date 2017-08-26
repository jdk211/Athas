#include "stdafx.h"
#include "cQuadParticleEmitter.h"


cQuadParticleEmitter::cQuadParticleEmitter()
{
}


cQuadParticleEmitter::~cQuadParticleEmitter()
{
}



HRESULT cQuadParticleEmitter::Init(
	DWORD partcleNum,					//총 파티클 량
	float emission,						//초당 발생량
	float liveTimeMin,
	float liveTimeMax,
	const D3DXVECTOR3& velocityMin,
	const D3DXVECTOR3& velocityMax,
	const D3DXVECTOR3& accelMin,
	const D3DXVECTOR3& accelMax,
	const D3DXVECTOR3& startRotateMin,		//시작할때 회전 최소값 
	const D3DXVECTOR3& startRotateMax,		//시작할때 회전 최대값
	const D3DXVECTOR3& rotateMin,			//움직임에대한 회전 최소량
	const D3DXVECTOR3& rotateMax,			//움직임에대한 회전 최대량
	const D3DXVECTOR3& rotateAccelMin,		//움직임 가속에대한 회전 최소량
	const D3DXVECTOR3& rotateAccelMax,		//움직임 가속에대한 회전 최대량
	const VEC_COLOR& colors,
	const VEC_SCALE& scales,
	float scaleMin,
	float scaleMax,
	LPDIRECT3DTEXTURE9 pTex,
	bool bLocal
	)
{

	//해당 파티클 랜더의 총 파티클 수
	m_PaticleNum = partcleNum;

	//총파티클 수 * 4 만큼 버텍스를 만든다
	m_ParticleVerticles = new PARTICLEQUAD_VERTEX[m_PaticleNum * 4];

	//총파티클 수 * 6 만큼 인덱스를 만든다.
	m_ParticleIndex = new DWORD[m_PaticleNum * 6];

	//파티클 객체 생성
	m_pPaticles = new cQuadParticle[m_PaticleNum];

	//파티클 소트용 Array
	m_pSortArray = new cQuadParticle*[m_PaticleNum];

	//초당 생성량
	m_fEmissionPerSec = emission;

	//초당 생성량 따른 발생 간격
	m_fEmisionIntervalTime = 1.0f / m_fEmissionPerSec;

	//지난 시간도 0
	m_fEmisionDeltaTime = 0.0f;

	//발생 여부 false
	m_bEmission = false;

	//컬러 대입
	m_Colors.clear();
	for (int i = 0; i < colors.size(); i++)
		m_Colors.push_back(colors[i]);

	//사이즈 대입
	m_Scales.clear();
	for (int i = 0; i < scales.size(); i++)
		m_Scales.push_back(scales[i]);


	//시작 라이브 타임 대입
	m_fStartLiveTimeMin = liveTimeMin;
	m_fStartLiveTimeMax = liveTimeMax;

	//시작 속도 대입
	m_StartVelocityMin = velocityMin;
	m_StartVelocityMax = velocityMax;

	//시작 가속 대입
	m_StartAccelateMin = accelMin;
	m_StartAccelateMax = accelMax;

	//시작 스케일 대입
	m_fStartScaleMin = scaleMin;
	m_fStartScaleMax = scaleMax;

	//시작시 회전방향 셋팅 함수
	m_StartSetRotateMax = startRotateMax;
	m_StartSetRotateMin = startRotateMin;

	//시작회전 값 대입
	m_StartRotateMin = rotateMin;
	m_StartRotateMax = rotateMax;

	//회전 가속 값 대입
	m_StartRotateAccelateMin = rotateAccelMin;
	m_StartRotateAccelateMax = rotateAccelMax;

	//시작순번 초기화
	m_dwParticleCount = 0;

	//파티클 텍스쳐
	m_pTex = pTex;

	m_bLocal = bLocal;


	this->m_bCameraSort = false;
	this->m_pCameraTransform = NULL;

	return S_OK;
}

void cQuadParticleEmitter::Release()
{
	SAFE_DELETE_ARR(m_pSortArray);
	SAFE_DELETE_ARR(m_pPaticles);
	SAFE_DELETE_ARR(m_ParticleVerticles);
	SAFE_DELETE_ARR(m_ParticleIndex);
	SAFE_RELEASE(m_pTex);
}


void cQuadParticleEmitter::Update(float timeDelta)
{
	//모든 파티클 업데이트
	for (int i = 0; i < m_PaticleNum; i++){
		m_pPaticles[i].Update(timeDelta);
	}


	//너가 지금 발생 상태니?
	if (m_bEmission){
		//하나 발생하고 지난시간
		m_fEmisionDeltaTime += timeDelta;

		while (m_fEmisionDeltaTime > m_fEmisionIntervalTime){
			m_fEmisionDeltaTime -= m_fEmisionIntervalTime;

			//파티클 하나 발사
			StartOneParticle();
		}
	}
}



void cQuadParticleEmitter::Render(bool additiveBlend)
{
	
}

void cQuadParticleEmitter::StartEmission()
{
	this->m_bEmission = true;
}

void cQuadParticleEmitter::StopEmission()
{
	this->m_bEmission = false;
}


//카메라 소팅을 할꺼니?
void cQuadParticleEmitter::SetCameraSort(cTransform* pCameraTrans, bool bSort)
{
	this->m_pCameraTransform = pCameraTrans;		//카메라 소트 기억
	this->m_bCameraSort = bSort;
}




////////////////////////////////

void cQuadParticleEmitter::StartOneParticle()
{
	//라이브 타임 랜덤
	float liveTime = GetRandom(
		m_fStartLiveTimeMin, m_fStartLiveTimeMax);

	D3DXVECTOR3 position = this->Transform.GetWorldPosition();

	//로컬이 아닌경우 자신의 월드 위치에서 시작하고 
	if (this->m_bLocal == false)
		position = this->Transform.GetWorldPosition();

	//로컬인 경우 0 에서 시작한다.
	else
		position = D3DXVECTOR3(0, 0, 0);


	//벡터 랜덤
	D3DXVECTOR3 velocity;
	velocity.x = GetRandom(m_StartVelocityMin.x, m_StartVelocityMax.x);
	velocity.y = GetRandom(m_StartVelocityMin.y, m_StartVelocityMax.y);
	velocity.z = GetRandom(m_StartVelocityMin.z, m_StartVelocityMax.z);

	D3DXVECTOR3 accelation;
	accelation.x = GetRandom(m_StartAccelateMin.x, m_StartAccelateMax.x);
	accelation.y = GetRandom(m_StartAccelateMin.y, m_StartAccelateMax.y);
	accelation.z = GetRandom(m_StartAccelateMin.z, m_StartAccelateMax.z);


	D3DXVECTOR3 setRotate;
	setRotate.x = GetRandom(m_StartSetRotateMin.x, m_StartSetRotateMax.x);
	setRotate.y = GetRandom(m_StartSetRotateMin.y, m_StartSetRotateMax.y);
	setRotate.z = GetRandom(m_StartSetRotateMin.z, m_StartSetRotateMax.z);


	//움직임의 회전량 랜덤
	D3DXVECTOR3 rotate;
	rotate.x = GetRandom(m_StartRotateMin.x, m_StartRotateMax.x);
	rotate.y = GetRandom(m_StartRotateMin.y, m_StartRotateMax.y);
	rotate.z = GetRandom(m_StartRotateMin.z, m_StartRotateMax.z);


	//움직임의 회전 가속량 랜덤
	D3DXVECTOR3 rotateAccel;
	rotateAccel.x = GetRandom(m_StartRotateAccelateMin.x, m_StartRotateAccelateMax.x);
	rotateAccel.y = GetRandom(m_StartRotateAccelateMin.y, m_StartRotateAccelateMax.y);
	rotateAccel.z = GetRandom(m_StartRotateAccelateMin.z, m_StartRotateAccelateMax.z);


	//자신의 월드 매트릭스를 가지고 온다.
	if (m_bLocal == false)
	{
		D3DXMATRIXA16 matWorld = this->Transform.GetFinalMatrix();
		D3DXVec3TransformNormal(&velocity, &velocity, &matWorld);
		D3DXVec3TransformNormal(&accelation, &accelation, &matWorld);
	}


	//스케일도 랜덤
	float scale = GetRandom(m_fStartScaleMin, m_fStartScaleMax);


	//시작 회전값 셋팅해놓는다.
	m_pPaticles[m_dwParticleCount].Transform.SetRotateLocal(
		setRotate.x, setRotate.y, setRotate.z);


	//순번대로 발생시킨다
	m_pPaticles[m_dwParticleCount].Start(
		liveTime,			//라이브 타입
		&position,			//위치
		&velocity,			//벨로 시티
		&accelation,		//가속
		&rotate,			//회전량
		&rotateAccel,		//회전 가속
		scale				//스케일
		);

	//다음 파티클을 위한 순번 증가
	m_dwParticleCount++;
	if (m_dwParticleCount == this->m_PaticleNum)
		m_dwParticleCount = 0;

}