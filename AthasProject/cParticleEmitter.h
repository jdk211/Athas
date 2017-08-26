#pragma once


#include "cParticle.h"

#define PARTICLE_VELOCITY_LOCAL 0x10
#define PARTICLE_VELOCITY_WORLD 0x00
#define PARTICLE_ACCELATE_LOCAL 0x20
#define PARTICLE_ACCELATE_WORLD 0x00



enum PATICLE_EMISSIONTYPE{
	EMISSION_POINT, 
	EMISSION_BOX,
	EMISSION_SPHERE, 
	EMISSION_SPHERE_OUTLINE
};


class cParticleEmitter
{
public:
	cTransform			Transform;

private:
	DWORD				m_PaticleNum;				//입자의 총량
	cParticle*			m_pPaticles;				//파티클 배열
	LPPARTICLE_VERTEX	m_ParticleVerticles;		//파티클 버텍스

	VEC_COLOR			m_Colors;					//파티클 컬러 배열
	VEC_SCALE			m_Scales;					//파티클 사이즈 배열

	//파티클 라이브 타임 최대 최소
	float				m_fStartLiveTimeMin;
	float				m_fStartLiveTimeMax;

	//파티클 시작 속도 최대 최소
	D3DXVECTOR3			m_StartVelocityMin;
	D3DXVECTOR3			m_StartVelocityMax;

	//파티클 시작 가속 최대 최소
	D3DXVECTOR3			m_StartAccelateMin;
	D3DXVECTOR3			m_StartAccelateMax;

	//파티클 시작 사이즈 최대 최소
	float				m_fStartScaleMin;
	float				m_fStartScaleMax;


	bool				m_bEmission;				//현재 생성중이니?
	float				m_fEmissionPerSec;			//초당 발생량
	float				m_fEmisionIntervalTime;		//생성 간격 시간
	float				m_fEmisionDeltaTime;		//하나 생성하구 지난시간


	DWORD				m_dwParticleCount;			//활성화 순번

	//particle Texture 
	LPDIRECT3DTEXTURE9	m_pTex;


	bool				m_isAdditiveColor;			//색이 쌓이니?

	DWORD				m_flag;
	bool				m_isLocal;


	//발생 타입
	PATICLE_EMISSIONTYPE m_EmissionType;

	//발생타입에 필요한 변수
	D3DXVECTOR3			m_EmissionBoxMin;
	D3DXVECTOR3			m_EmissionBoxMax;
	float				m_EmissionRadius;
	



public:
	cParticleEmitter();
	~cParticleEmitter();

	void Init(
		DWORD partcleNum,					//총 파티클 량
		float emission,						//초당 발생량
		float liveTimeMin,					//파티클하나의 시간
		float liveTimeMax,
		const D3DXVECTOR3& velocityMin,		//파티클 시작 속도
		const D3DXVECTOR3& velocityMax,
		const D3DXVECTOR3& accelMin,		//파티클 가속
		const D3DXVECTOR3& accelMax,
		const VEC_COLOR& colors,			//파티클 컬러 벡터배열
		const VEC_SCALE& scales,			//파티클 스케일 컬러배열
		float scaleMin,						//파티클 스케일
		float scaleMax,
		LPDIRECT3DTEXTURE9 pPaticleTexture,	//파티클 Texture
		bool additiveColor,					//색태우니?
		int flag = 0,
		bool isLocal = false
		);

	void Release();

	void Update(float timeDelta);		
	void Render();


	//파티클 생성 시작
	void StartEmissionPoint();			
	void StartEmissionBox(D3DXVECTOR3 min, D3DXVECTOR3 max);
	void StartEmisiionSphere(float radius, bool outline);
	void StopEmission();			//파티클 생성 중지


	void BurstParticle(PATICLE_EMISSIONTYPE emissionType, int num);
	void BurstParticle(PATICLE_EMISSIONTYPE emissionType, int num, D3DXVECTOR3& minVelo, D3DXVECTOR3& maxVelo);




private:
	void StartOneParticle();		//파티클 하나 생성

};

