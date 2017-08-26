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
	DWORD				m_PaticleNum;				//������ �ѷ�
	cParticle*			m_pPaticles;				//��ƼŬ �迭
	LPPARTICLE_VERTEX	m_ParticleVerticles;		//��ƼŬ ���ؽ�

	VEC_COLOR			m_Colors;					//��ƼŬ �÷� �迭
	VEC_SCALE			m_Scales;					//��ƼŬ ������ �迭

	//��ƼŬ ���̺� Ÿ�� �ִ� �ּ�
	float				m_fStartLiveTimeMin;
	float				m_fStartLiveTimeMax;

	//��ƼŬ ���� �ӵ� �ִ� �ּ�
	D3DXVECTOR3			m_StartVelocityMin;
	D3DXVECTOR3			m_StartVelocityMax;

	//��ƼŬ ���� ���� �ִ� �ּ�
	D3DXVECTOR3			m_StartAccelateMin;
	D3DXVECTOR3			m_StartAccelateMax;

	//��ƼŬ ���� ������ �ִ� �ּ�
	float				m_fStartScaleMin;
	float				m_fStartScaleMax;


	bool				m_bEmission;				//���� �������̴�?
	float				m_fEmissionPerSec;			//�ʴ� �߻���
	float				m_fEmisionIntervalTime;		//���� ���� �ð�
	float				m_fEmisionDeltaTime;		//�ϳ� �����ϱ� �����ð�


	DWORD				m_dwParticleCount;			//Ȱ��ȭ ����

	//particle Texture 
	LPDIRECT3DTEXTURE9	m_pTex;


	bool				m_isAdditiveColor;			//���� ���̴�?

	DWORD				m_flag;
	bool				m_isLocal;


	//�߻� Ÿ��
	PATICLE_EMISSIONTYPE m_EmissionType;

	//�߻�Ÿ�Կ� �ʿ��� ����
	D3DXVECTOR3			m_EmissionBoxMin;
	D3DXVECTOR3			m_EmissionBoxMax;
	float				m_EmissionRadius;
	



public:
	cParticleEmitter();
	~cParticleEmitter();

	void Init(
		DWORD partcleNum,					//�� ��ƼŬ ��
		float emission,						//�ʴ� �߻���
		float liveTimeMin,					//��ƼŬ�ϳ��� �ð�
		float liveTimeMax,
		const D3DXVECTOR3& velocityMin,		//��ƼŬ ���� �ӵ�
		const D3DXVECTOR3& velocityMax,
		const D3DXVECTOR3& accelMin,		//��ƼŬ ����
		const D3DXVECTOR3& accelMax,
		const VEC_COLOR& colors,			//��ƼŬ �÷� ���͹迭
		const VEC_SCALE& scales,			//��ƼŬ ������ �÷��迭
		float scaleMin,						//��ƼŬ ������
		float scaleMax,
		LPDIRECT3DTEXTURE9 pPaticleTexture,	//��ƼŬ Texture
		bool additiveColor,					//���¿��?
		int flag = 0,
		bool isLocal = false
		);

	void Release();

	void Update(float timeDelta);		
	void Render();


	//��ƼŬ ���� ����
	void StartEmissionPoint();			
	void StartEmissionBox(D3DXVECTOR3 min, D3DXVECTOR3 max);
	void StartEmisiionSphere(float radius, bool outline);
	void StopEmission();			//��ƼŬ ���� ����


	void BurstParticle(PATICLE_EMISSIONTYPE emissionType, int num);
	void BurstParticle(PATICLE_EMISSIONTYPE emissionType, int num, D3DXVECTOR3& minVelo, D3DXVECTOR3& maxVelo);




private:
	void StartOneParticle();		//��ƼŬ �ϳ� ����

};

