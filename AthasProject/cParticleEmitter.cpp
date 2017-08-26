#include "stdafx.h"
#include "cParticleEmitter.h"


cParticleEmitter::cParticleEmitter()
{
}


cParticleEmitter::~cParticleEmitter()
{
}


void cParticleEmitter::Init(
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
	int flag, 
	bool isLocal
	)
{
	//�ش� ��ƼŬ ������ �� ��ƼŬ ��
	m_PaticleNum = partcleNum;

	//����ƼŬ ����ŭ ���ؽ� �迭�� �����.
	m_ParticleVerticles = new PARTICLE_VERTEX[m_PaticleNum];

	//��ƼŬ ��ü ����
	m_pPaticles = new cParticle[m_PaticleNum];

	//�ʴ� ������
	m_fEmissionPerSec = emission;

	//�ʴ� ������ ���� �߻� ����
	m_fEmisionIntervalTime = 1.0f / m_fEmissionPerSec;

	//���� �ð��� 0
	m_fEmisionDeltaTime = 0.0f;

	//�߻� ���� false
	m_bEmission = false;

	//�÷� ����
	//m_Colors = colors;
	m_Colors.clear();
	for (int i = 0; i < colors.size(); i++)
		m_Colors.push_back(colors[i]);

	//������ ����
	//m_Scales = scales;
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

	//���ۼ��� �ʱ�ȭ
	m_dwParticleCount = 0;

	//Texture ����
	m_pTex = pPaticleTexture;

	//Additive 
	m_isAdditiveColor = additiveColor;


	//�÷��� ����
	m_flag = flag;

	//����
	this->m_isLocal = isLocal;
}

void cParticleEmitter::Release()
{
	SAFE_DELETE_ARR(m_pPaticles);
	SAFE_DELETE_ARR(m_ParticleVerticles);
}

void cParticleEmitter::Update(float timeDelta)
{
	//��� ��ƼŬ ������Ʈ
	for (int i = 0; i < m_PaticleNum; i++){
		m_pPaticles[i].Update(timeDelta);
	}

	//�ʰ� ���� �߻� ���´�?
	if (m_bEmission){

		//�ϳ� �߻��ϰ� �����ð�
		m_fEmisionDeltaTime += timeDelta;

		while (m_fEmisionIntervalTime <= m_fEmisionDeltaTime){
			this->StartOneParticle();
			this->m_fEmisionDeltaTime -= m_fEmisionIntervalTime;
		}
	}

}
void cParticleEmitter::Render()
{
	//���� �ʱ�ȭ
	if (this->m_isLocal == false)
	{
		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity(&matWorld);
		Device->SetTransform(D3DTS_WORLD, &matWorld);
	}
	else
	{
		this->Transform.UpdateToDevice();
	}

	




	//�׸� ��ƼŬ ��
	DWORD drawParticleNum = 0;

	//�׸� ��ƼŬ�� ã�� m_ParticleVerticles �迭�� ���� ����.
	for (int i = 0; i < m_PaticleNum; i++){

		//����ִ� ���ڴ�?
		if (this->m_pPaticles[i].isLive()){

			this->m_pPaticles[i].GetParticleVertex(
				&m_ParticleVerticles[drawParticleNum],
				this->m_Colors,
				this->m_Scales);

			drawParticleNum++;
		}
	}
	
	if (drawParticleNum == 0)return;



	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	if ( this->m_isAdditiveColor )
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	else
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	//���� ���� ���� Texture ���� �� ���̰�
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	//���� Test ������
	Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);




	//������ Sprite �� Ȱ��ȭ �Ѵ�.
	Device->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	Device->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);			//����Ʈ�� �����ϰ� ���̰ڴ�. FVF �� D3DFVF_PSIZE �ִ°��		


	Device->SetRenderState(D3DRS_POINTSIZE_MIN, FloatToDWORD(0.0f));	//����Ʈ�� �ּ� ũ�� ( ȭ����� )
	Device->SetRenderState(D3DRS_POINTSIZE_MAX, FloatToDWORD(1000.0f));	//����Ʈ�� �ִ� ũ�� ( ȭ����� )

	//��µǴ� POINT size 
	//finalSize = viewportHeight * pointSize *  sqrt( 1 / ( A + B(D) + C(D^2) )  );		//D �Ÿ�( 0 ~ 1 )
	//�Ʒ��� ���� �ϸ� �ڿ������� �Ÿ��� ���� �������� ��Ÿ��
	Device->SetRenderState(D3DRS_POINTSCALE_A, FloatToDWORD(0.0f));	//A
	Device->SetRenderState(D3DRS_POINTSCALE_B, FloatToDWORD(0.0f)); //B
	Device->SetRenderState(D3DRS_POINTSCALE_C, FloatToDWORD(1.0f)); //C

	Device->SetTexture(0, this->m_pTex );



	Device->SetFVF(PARTICLE_VERTEX::FVF);

	Device->DrawPrimitiveUP(
		D3DPT_POINTLIST,		//�׸��� ���� ���� ��ü �̴�.
		drawParticleNum,
		this->m_ParticleVerticles,
		sizeof(PARTICLE_VERTEX)
		);

	Device->SetTexture(0, NULL);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Device->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);





}

//��ƼŬ ���� ����
void cParticleEmitter::StartEmissionPoint()
{
	this->m_bEmission = true;
	m_EmissionType = EMISSION_POINT;
}

void cParticleEmitter::StartEmissionBox(D3DXVECTOR3 min, D3DXVECTOR3 max)
{
	this->m_bEmission = true;
	m_EmissionType = EMISSION_BOX;
	this->m_EmissionBoxMin = min;
	this->m_EmissionBoxMax = max;
}
void cParticleEmitter::StartEmisiionSphere(float radius, bool outline)
{
	this->m_bEmission = true;
	m_EmissionType = outline ? EMISSION_SPHERE_OUTLINE : EMISSION_SPHERE;
	this->m_EmissionRadius = radius;
}




//��ƼŬ ���� ����
void cParticleEmitter::StopEmission()
{
	this->m_bEmission = false;
}


void cParticleEmitter::BurstParticle(PATICLE_EMISSIONTYPE emissionType, int num)
{
	//���� �����
	PATICLE_EMISSIONTYPE prevType = this->m_EmissionType;
	this->m_EmissionType = emissionType;

	for (int i = 0; i < num; i++){
		this->StartOneParticle();
	}

	this->m_EmissionType = prevType;

}
void cParticleEmitter::BurstParticle(PATICLE_EMISSIONTYPE emissionType, int num, D3DXVECTOR3& minVelo, D3DXVECTOR3& maxVelo)
{
	//���� �� ���
	D3DXVECTOR3 prevMinVelo = this->m_StartVelocityMin;
	D3DXVECTOR3 prevMaxVelo = this->m_StartVelocityMax;
	PATICLE_EMISSIONTYPE prevType = this->m_EmissionType;

	this->m_EmissionType = emissionType;
	this->m_StartVelocityMin = minVelo;
	this->m_StartVelocityMax = maxVelo;

	for (int i = 0; i < num; i++){
		this->StartOneParticle();
	}

	//���� ����
	this->m_EmissionType = prevType;
	this->m_StartVelocityMin = prevMinVelo;
	this->m_StartVelocityMax = prevMaxVelo;

}


///////////////////////////////////////////////////////////


//��ƼŬ �ϳ� ����
void cParticleEmitter::StartOneParticle()
{
	//���̺� Ÿ�� ����
	float liveTime = GetRandom( m_fStartLiveTimeMin, m_fStartLiveTimeMax);

	//��ġ �� ����ġ
	D3DXVECTOR3 pos(0, 0, 0);
	
	
	//���� �������� �Ѹ���.
	if (this->m_isLocal == false){

		switch (this->m_EmissionType){

		case EMISSION_POINT:
			pos = Transform.GetWorldPosition();
			break;

		case EMISSION_BOX:
			{
				//Box �� Min Max ������ ��´�.
				pos.x = GetRandom(this->m_EmissionBoxMin.x, this->m_EmissionBoxMax.x);
				pos.y = GetRandom(this->m_EmissionBoxMin.y, this->m_EmissionBoxMax.y);
				pos.z = GetRandom(this->m_EmissionBoxMin.z, this->m_EmissionBoxMax.z);

				D3DXVec3TransformCoord(
					&pos, &pos, &this->Transform.GetFinalMatrix());

			}
			break;

		case EMISSION_SPHERE:
			{
				pos = Transform.GetWorldPosition();

				//��� ��ġ���� ���� �ݰ� ������ ��ŭ �������� �Ѹ���.
				D3DXVECTOR3 rand(
					GetRandom(-1, 1),
					GetRandom(-1, 1),
					GetRandom(-1, 1));

				//�븻������
				D3DXVec3Normalize(&rand, &rand);

				pos += rand * GetRandom(0, m_EmissionRadius);
			}
			break;

		case EMISSION_SPHERE_OUTLINE:
			{
				pos = Transform.GetWorldPosition();
				
				
				//��� ��ġ���� ���� �ݰ� ������ ��ŭ �������� �Ѹ���.
				D3DXVECTOR3 rand(
					GetRandom(-1, 1),
					GetRandom(-1, 1),
					GetRandom(-1, 1));
				
				//�븻������
				D3DXVec3Normalize(&rand, &rand);
				
				pos += rand * m_EmissionRadius;
			}

			break;
		}//end switch
		
	}//end if

	else{

		switch (this->m_EmissionType){

		case EMISSION_POINT:
			pos = D3DXVECTOR3( 0, 0, 0);
			break;

		case EMISSION_BOX:
			{
				//Box �� Min Max ������ ��´�.
				pos.x = GetRandom(this->m_EmissionBoxMin.x, this->m_EmissionBoxMax.x);
				pos.y = GetRandom(this->m_EmissionBoxMin.y, this->m_EmissionBoxMax.y);
				pos.z = GetRandom(this->m_EmissionBoxMin.z, this->m_EmissionBoxMax.z);

			}
			break;

		case EMISSION_SPHERE:
			{
				pos = D3DXVECTOR3(0, 0, 0 );

				//��� ��ġ���� ���� �ݰ� ������ ��ŭ �������� �Ѹ���.
				D3DXVECTOR3 rand(
					GetRandom(-1, 1),
					GetRandom(-1, 1),
					GetRandom(-1, 1));

				//�븻������
				D3DXVec3Normalize(&rand, &rand);

				pos += rand * GetRandom(0, m_EmissionRadius);
			}
			break;

		case EMISSION_SPHERE_OUTLINE:
		{
				pos = D3DXVECTOR3(0, 0, 0 );


				//��� ��ġ���� ���� �ݰ� ������ ��ŭ �������� �Ѹ���.
				D3DXVECTOR3 rand(
					GetRandom(-1, 1),
					GetRandom(-1, 1),
					GetRandom(-1, 1));

				//�븻������
				D3DXVec3Normalize(&rand, &rand);

				pos += rand * m_EmissionRadius;
		}

			break;
		}

	}//end else


	//���� ����
	D3DXVECTOR3 velocity;
	velocity.x = GetRandom(m_StartVelocityMin.x, m_StartVelocityMax.x);
	velocity.y = GetRandom(m_StartVelocityMin.y, m_StartVelocityMax.y);
	velocity.z = GetRandom(m_StartVelocityMin.z, m_StartVelocityMax.z);

	//����� ����
	if ((m_flag & PARTICLE_VELOCITY_LOCAL) && m_isLocal == false)
	{
		D3DXMATRIXA16 matFinal = this->Transform.GetFinalMatrix();
		D3DXVec3TransformNormal(&velocity, &velocity, &matFinal);
	}


	D3DXVECTOR3 accelation;
	accelation.x = GetRandom(m_StartAccelateMin.x, m_StartAccelateMax.x);
	accelation.y = GetRandom(m_StartAccelateMin.y, m_StartAccelateMax.y);
	accelation.z = GetRandom(m_StartAccelateMin.z, m_StartAccelateMax.z);


	//����� ����
	if ( ( m_flag & PARTICLE_ACCELATE_LOCAL )&& m_isLocal == false)
	{
		D3DXMATRIXA16 matFinal = this->Transform.GetFinalMatrix();
		D3DXVec3TransformNormal(&accelation, &accelation, &matFinal);
	}


	//�����ϵ� ����
	float scale = GetRandom(m_fStartScaleMin, m_fStartScaleMax);

	//������� �߻���Ų��
	m_pPaticles[m_dwParticleCount].Start(
		liveTime, &pos, &velocity, &accelation, scale);

	//���� ��ƼŬ�� ���� ���� ����
	m_dwParticleCount++;
	if (m_dwParticleCount == this->m_PaticleNum)
		m_dwParticleCount = 0;

}