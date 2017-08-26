#include "stdafx.h"
#include "cParticleEmitter.h"


cParticleEmitter::cParticleEmitter()
{
}


cParticleEmitter::~cParticleEmitter()
{
}


void cParticleEmitter::Init(
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
	int flag, 
	bool isLocal
	)
{
	//해당 파티클 랜더의 총 파티클 수
	m_PaticleNum = partcleNum;

	//총파티클 수만큼 버텍스 배열을 만든다.
	m_ParticleVerticles = new PARTICLE_VERTEX[m_PaticleNum];

	//파티클 객체 생성
	m_pPaticles = new cParticle[m_PaticleNum];

	//초당 생성량
	m_fEmissionPerSec = emission;

	//초당 생성량 따른 발생 간격
	m_fEmisionIntervalTime = 1.0f / m_fEmissionPerSec;

	//지난 시간도 0
	m_fEmisionDeltaTime = 0.0f;

	//발생 여부 false
	m_bEmission = false;

	//컬러 대입
	//m_Colors = colors;
	m_Colors.clear();
	for (int i = 0; i < colors.size(); i++)
		m_Colors.push_back(colors[i]);

	//사이즈 대입
	//m_Scales = scales;
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

	//시작순번 초기화
	m_dwParticleCount = 0;

	//Texture 참조
	m_pTex = pPaticleTexture;

	//Additive 
	m_isAdditiveColor = additiveColor;


	//플래그 대입
	m_flag = flag;

	//로컬
	this->m_isLocal = isLocal;
}

void cParticleEmitter::Release()
{
	SAFE_DELETE_ARR(m_pPaticles);
	SAFE_DELETE_ARR(m_ParticleVerticles);
}

void cParticleEmitter::Update(float timeDelta)
{
	//모든 파티클 업데이트
	for (int i = 0; i < m_PaticleNum; i++){
		m_pPaticles[i].Update(timeDelta);
	}

	//너가 지금 발생 상태니?
	if (m_bEmission){

		//하나 발생하고 지난시간
		m_fEmisionDeltaTime += timeDelta;

		while (m_fEmisionIntervalTime <= m_fEmisionDeltaTime){
			this->StartOneParticle();
			this->m_fEmisionDeltaTime -= m_fEmisionIntervalTime;
		}
	}

}
void cParticleEmitter::Render()
{
	//월드 초기화
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

	




	//그릴 파티클 수
	DWORD drawParticleNum = 0;

	//그릴 파티클을 찾아 m_ParticleVerticles 배열에 값을 쓴다.
	for (int i = 0; i < m_PaticleNum; i++){

		//살아있는 입자니?
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


	//정점 알파 값과 Texture 알파 값 섞이게
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	//뎁스 Test 하지마
	Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);




	//정점을 Sprite 로 활성화 한다.
	Device->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	Device->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);			//포인트의 스케일값 먹이겠다. FVF 에 D3DFVF_PSIZE 있는경우		


	Device->SetRenderState(D3DRS_POINTSIZE_MIN, FloatToDWORD(0.0f));	//포인트의 최소 크기 ( 화면기준 )
	Device->SetRenderState(D3DRS_POINTSIZE_MAX, FloatToDWORD(1000.0f));	//포인트의 최대 크기 ( 화면기준 )

	//출력되는 POINT size 
	//finalSize = viewportHeight * pointSize *  sqrt( 1 / ( A + B(D) + C(D^2) )  );		//D 거리( 0 ~ 1 )
	//아래와 같이 하면 자연스러운 거리에 따른 스케일이 나타남
	Device->SetRenderState(D3DRS_POINTSCALE_A, FloatToDWORD(0.0f));	//A
	Device->SetRenderState(D3DRS_POINTSCALE_B, FloatToDWORD(0.0f)); //B
	Device->SetRenderState(D3DRS_POINTSCALE_C, FloatToDWORD(1.0f)); //C

	Device->SetTexture(0, this->m_pTex );



	Device->SetFVF(PARTICLE_VERTEX::FVF);

	Device->DrawPrimitiveUP(
		D3DPT_POINTLIST,		//그리는 것은 정점 개체 이다.
		drawParticleNum,
		this->m_ParticleVerticles,
		sizeof(PARTICLE_VERTEX)
		);

	Device->SetTexture(0, NULL);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Device->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);





}

//파티클 생성 시작
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




//파티클 생성 중지
void cParticleEmitter::StopEmission()
{
	this->m_bEmission = false;
}


void cParticleEmitter::BurstParticle(PATICLE_EMISSIONTYPE emissionType, int num)
{
	//이전 값기억
	PATICLE_EMISSIONTYPE prevType = this->m_EmissionType;
	this->m_EmissionType = emissionType;

	for (int i = 0; i < num; i++){
		this->StartOneParticle();
	}

	this->m_EmissionType = prevType;

}
void cParticleEmitter::BurstParticle(PATICLE_EMISSIONTYPE emissionType, int num, D3DXVECTOR3& minVelo, D3DXVECTOR3& maxVelo)
{
	//이전 값 기억
	D3DXVECTOR3 prevMinVelo = this->m_StartVelocityMin;
	D3DXVECTOR3 prevMaxVelo = this->m_StartVelocityMax;
	PATICLE_EMISSIONTYPE prevType = this->m_EmissionType;

	this->m_EmissionType = emissionType;
	this->m_StartVelocityMin = minVelo;
	this->m_StartVelocityMax = maxVelo;

	for (int i = 0; i < num; i++){
		this->StartOneParticle();
	}

	//원상 복구
	this->m_EmissionType = prevType;
	this->m_StartVelocityMin = prevMinVelo;
	this->m_StartVelocityMax = prevMaxVelo;

}


///////////////////////////////////////////////////////////


//파티클 하나 생성
void cParticleEmitter::StartOneParticle()
{
	//라이브 타임 랜덤
	float liveTime = GetRandom( m_fStartLiveTimeMin, m_fStartLiveTimeMax);

	//위치 는 내위치
	D3DXVECTOR3 pos(0, 0, 0);
	
	
	//월드 개념으로 뿌린다.
	if (this->m_isLocal == false){

		switch (this->m_EmissionType){

		case EMISSION_POINT:
			pos = Transform.GetWorldPosition();
			break;

		case EMISSION_BOX:
			{
				//Box 의 Min Max 랜덤을 얻는다.
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

				//출력 위치에서 부터 반경 반지름 만큼 랜덤으로 뿌린다.
				D3DXVECTOR3 rand(
					GetRandom(-1, 1),
					GetRandom(-1, 1),
					GetRandom(-1, 1));

				//노말라이즈
				D3DXVec3Normalize(&rand, &rand);

				pos += rand * GetRandom(0, m_EmissionRadius);
			}
			break;

		case EMISSION_SPHERE_OUTLINE:
			{
				pos = Transform.GetWorldPosition();
				
				
				//출력 위치에서 부터 반경 반지름 만큼 랜덤으로 뿌린다.
				D3DXVECTOR3 rand(
					GetRandom(-1, 1),
					GetRandom(-1, 1),
					GetRandom(-1, 1));
				
				//노말라이즈
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
				//Box 의 Min Max 랜덤을 얻는다.
				pos.x = GetRandom(this->m_EmissionBoxMin.x, this->m_EmissionBoxMax.x);
				pos.y = GetRandom(this->m_EmissionBoxMin.y, this->m_EmissionBoxMax.y);
				pos.z = GetRandom(this->m_EmissionBoxMin.z, this->m_EmissionBoxMax.z);

			}
			break;

		case EMISSION_SPHERE:
			{
				pos = D3DXVECTOR3(0, 0, 0 );

				//출력 위치에서 부터 반경 반지름 만큼 랜덤으로 뿌린다.
				D3DXVECTOR3 rand(
					GetRandom(-1, 1),
					GetRandom(-1, 1),
					GetRandom(-1, 1));

				//노말라이즈
				D3DXVec3Normalize(&rand, &rand);

				pos += rand * GetRandom(0, m_EmissionRadius);
			}
			break;

		case EMISSION_SPHERE_OUTLINE:
		{
				pos = D3DXVECTOR3(0, 0, 0 );


				//출력 위치에서 부터 반경 반지름 만큼 랜덤으로 뿌린다.
				D3DXVECTOR3 rand(
					GetRandom(-1, 1),
					GetRandom(-1, 1),
					GetRandom(-1, 1));

				//노말라이즈
				D3DXVec3Normalize(&rand, &rand);

				pos += rand * m_EmissionRadius;
		}

			break;
		}

	}//end else


	//벡터 랜덤
	D3DXVECTOR3 velocity;
	velocity.x = GetRandom(m_StartVelocityMin.x, m_StartVelocityMax.x);
	velocity.y = GetRandom(m_StartVelocityMin.y, m_StartVelocityMax.y);
	velocity.z = GetRandom(m_StartVelocityMin.z, m_StartVelocityMax.z);

	//월드로 돌려
	if ((m_flag & PARTICLE_VELOCITY_LOCAL) && m_isLocal == false)
	{
		D3DXMATRIXA16 matFinal = this->Transform.GetFinalMatrix();
		D3DXVec3TransformNormal(&velocity, &velocity, &matFinal);
	}


	D3DXVECTOR3 accelation;
	accelation.x = GetRandom(m_StartAccelateMin.x, m_StartAccelateMax.x);
	accelation.y = GetRandom(m_StartAccelateMin.y, m_StartAccelateMax.y);
	accelation.z = GetRandom(m_StartAccelateMin.z, m_StartAccelateMax.z);


	//월드로 돌려
	if ( ( m_flag & PARTICLE_ACCELATE_LOCAL )&& m_isLocal == false)
	{
		D3DXMATRIXA16 matFinal = this->Transform.GetFinalMatrix();
		D3DXVec3TransformNormal(&accelation, &accelation, &matFinal);
	}


	//스케일도 랜덤
	float scale = GetRandom(m_fStartScaleMin, m_fStartScaleMax);

	//순번대로 발생시킨다
	m_pPaticles[m_dwParticleCount].Start(
		liveTime, &pos, &velocity, &accelation, scale);

	//다음 파티클을 위한 순번 증가
	m_dwParticleCount++;
	if (m_dwParticleCount == this->m_PaticleNum)
		m_dwParticleCount = 0;

}