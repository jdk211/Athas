#include "stdafx.h"
#include "cQuadParticleEmitter.h"


cQuadParticleEmitter::cQuadParticleEmitter()
{
}


cQuadParticleEmitter::~cQuadParticleEmitter()
{
}



HRESULT cQuadParticleEmitter::Init(
	DWORD partcleNum,					//�� ��ƼŬ ��
	float emission,						//�ʴ� �߻���
	float liveTimeMin,
	float liveTimeMax,
	const D3DXVECTOR3& velocityMin,
	const D3DXVECTOR3& velocityMax,
	const D3DXVECTOR3& accelMin,
	const D3DXVECTOR3& accelMax,
	const D3DXVECTOR3& startRotateMin,		//�����Ҷ� ȸ�� �ּҰ� 
	const D3DXVECTOR3& startRotateMax,		//�����Ҷ� ȸ�� �ִ밪
	const D3DXVECTOR3& rotateMin,			//�����ӿ����� ȸ�� �ּҷ�
	const D3DXVECTOR3& rotateMax,			//�����ӿ����� ȸ�� �ִ뷮
	const D3DXVECTOR3& rotateAccelMin,		//������ ���ӿ����� ȸ�� �ּҷ�
	const D3DXVECTOR3& rotateAccelMax,		//������ ���ӿ����� ȸ�� �ִ뷮
	const VEC_COLOR& colors,
	const VEC_SCALE& scales,
	float scaleMin,
	float scaleMax,
	LPDIRECT3DTEXTURE9 pTex,
	bool bLocal
	)
{

	//�ش� ��ƼŬ ������ �� ��ƼŬ ��
	m_PaticleNum = partcleNum;

	//����ƼŬ �� * 4 ��ŭ ���ؽ��� �����
	m_ParticleVerticles = new PARTICLEQUAD_VERTEX[m_PaticleNum * 4];

	//����ƼŬ �� * 6 ��ŭ �ε����� �����.
	m_ParticleIndex = new DWORD[m_PaticleNum * 6];

	//��ƼŬ ��ü ����
	m_pPaticles = new cQuadParticle[m_PaticleNum];

	//��ƼŬ ��Ʈ�� Array
	m_pSortArray = new cQuadParticle*[m_PaticleNum];

	//�ʴ� ������
	m_fEmissionPerSec = emission;

	//�ʴ� ������ ���� �߻� ����
	m_fEmisionIntervalTime = 1.0f / m_fEmissionPerSec;

	//���� �ð��� 0
	m_fEmisionDeltaTime = 0.0f;

	//�߻� ���� false
	m_bEmission = false;

	//�÷� ����
	m_Colors.clear();
	for (int i = 0; i < colors.size(); i++)
		m_Colors.push_back(colors[i]);

	//������ ����
	m_Scales.clear();
	for (int i = 0; i < scales.size(); i++)
		m_Scales.push_back(scales[i]);


	//���� ���̺� Ÿ�� ����
	m_fStartLiveTimeMin = liveTimeMin;
	m_fStartLiveTimeMax = liveTimeMax;

	//���� �ӵ� ����
	m_StartVelocityMin = velocityMin;
	m_StartVelocityMax = velocityMax;

	//���� ���� ����
	m_StartAccelateMin = accelMin;
	m_StartAccelateMax = accelMax;

	//���� ������ ����
	m_fStartScaleMin = scaleMin;
	m_fStartScaleMax = scaleMax;

	//���۽� ȸ������ ���� �Լ�
	m_StartSetRotateMax = startRotateMax;
	m_StartSetRotateMin = startRotateMin;

	//����ȸ�� �� ����
	m_StartRotateMin = rotateMin;
	m_StartRotateMax = rotateMax;

	//ȸ�� ���� �� ����
	m_StartRotateAccelateMin = rotateAccelMin;
	m_StartRotateAccelateMax = rotateAccelMax;

	//���ۼ��� �ʱ�ȭ
	m_dwParticleCount = 0;

	//��ƼŬ �ؽ���
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
	//��� ��ƼŬ ������Ʈ
	for (int i = 0; i < m_PaticleNum; i++){
		m_pPaticles[i].Update(timeDelta);
	}


	//�ʰ� ���� �߻� ���´�?
	if (m_bEmission){
		//�ϳ� �߻��ϰ� �����ð�
		m_fEmisionDeltaTime += timeDelta;

		while (m_fEmisionDeltaTime > m_fEmisionIntervalTime){
			m_fEmisionDeltaTime -= m_fEmisionIntervalTime;

			//��ƼŬ �ϳ� �߻�
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


//ī�޶� ������ �Ҳ���?
void cQuadParticleEmitter::SetCameraSort(cTransform* pCameraTrans, bool bSort)
{
	this->m_pCameraTransform = pCameraTrans;		//ī�޶� ��Ʈ ���
	this->m_bCameraSort = bSort;
}




////////////////////////////////

void cQuadParticleEmitter::StartOneParticle()
{
	//���̺� Ÿ�� ����
	float liveTime = GetRandom(
		m_fStartLiveTimeMin, m_fStartLiveTimeMax);

	D3DXVECTOR3 position = this->Transform.GetWorldPosition();

	//������ �ƴѰ�� �ڽ��� ���� ��ġ���� �����ϰ� 
	if (this->m_bLocal == false)
		position = this->Transform.GetWorldPosition();

	//������ ��� 0 ���� �����Ѵ�.
	else
		position = D3DXVECTOR3(0, 0, 0);


	//���� ����
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


	//�������� ȸ���� ����
	D3DXVECTOR3 rotate;
	rotate.x = GetRandom(m_StartRotateMin.x, m_StartRotateMax.x);
	rotate.y = GetRandom(m_StartRotateMin.y, m_StartRotateMax.y);
	rotate.z = GetRandom(m_StartRotateMin.z, m_StartRotateMax.z);


	//�������� ȸ�� ���ӷ� ����
	D3DXVECTOR3 rotateAccel;
	rotateAccel.x = GetRandom(m_StartRotateAccelateMin.x, m_StartRotateAccelateMax.x);
	rotateAccel.y = GetRandom(m_StartRotateAccelateMin.y, m_StartRotateAccelateMax.y);
	rotateAccel.z = GetRandom(m_StartRotateAccelateMin.z, m_StartRotateAccelateMax.z);


	//�ڽ��� ���� ��Ʈ������ ������ �´�.
	if (m_bLocal == false)
	{
		D3DXMATRIXA16 matWorld = this->Transform.GetFinalMatrix();
		D3DXVec3TransformNormal(&velocity, &velocity, &matWorld);
		D3DXVec3TransformNormal(&accelation, &accelation, &matWorld);
	}


	//�����ϵ� ����
	float scale = GetRandom(m_fStartScaleMin, m_fStartScaleMax);


	//���� ȸ���� �����س��´�.
	m_pPaticles[m_dwParticleCount].Transform.SetRotateLocal(
		setRotate.x, setRotate.y, setRotate.z);


	//������� �߻���Ų��
	m_pPaticles[m_dwParticleCount].Start(
		liveTime,			//���̺� Ÿ��
		&position,			//��ġ
		&velocity,			//���� ��Ƽ
		&accelation,		//����
		&rotate,			//ȸ����
		&rotateAccel,		//ȸ�� ����
		scale				//������
		);

	//���� ��ƼŬ�� ���� ���� ����
	m_dwParticleCount++;
	if (m_dwParticleCount == this->m_PaticleNum)
		m_dwParticleCount = 0;

}