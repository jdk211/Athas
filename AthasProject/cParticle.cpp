#include "stdafx.h"
#include "cParticle.h"


cParticle::cParticle()
:m_bLive(false)
{
}


cParticle::~cParticle()
{
}


void cParticle::Start(
	float liveTime,				//라이브 타임
	const D3DXVECTOR3* pos,		//시작 위치
	const D3DXVECTOR3* velo,	//시작 속도
	const D3DXVECTOR3* accel,	//가속 값
	float scale					//기본 스케일
	)
{
	//활성화 여부
	m_bLive = true;

	//라이브 타임 초기화
	m_fTotalLiveTime = liveTime;
	m_fDeltaLiveTime = 0.0f;
	m_fNomalizeLiveTime = 0.0f;

	//위치 값셋팅
	m_Transform.SetWorldPosition(pos->x, pos->y, pos->z);

	//시작 속도
	m_Velocity = *velo;

	//가속
	m_Accelation = *accel;

	//기본 스케일
	m_fScale = scale;

}


void cParticle::Update(float timeDelta)
{
	if (m_bLive == false) return;

	//라이브 타임이 다되었다면...
	if (m_fDeltaLiveTime >= m_fTotalLiveTime){
		m_bLive = false;
	}

	//활성화 시간 쌓는다.
	m_fDeltaLiveTime += timeDelta;

	//정규화된 시간값을 계산
	m_fNomalizeLiveTime = m_fDeltaLiveTime / m_fTotalLiveTime;
	if (m_fNomalizeLiveTime > 1.0f)
		m_fNomalizeLiveTime = 1.0f;

	//파티클 월드 이동
	m_Transform.MovePositionWorld(
		m_Velocity.x * timeDelta,
		m_Velocity.y * timeDelta,
		m_Velocity.z * timeDelta);


	//파티클 가속
	m_Velocity += m_Accelation * timeDelta;
}


//자신의 정점 정보를 바탕으로 LPPARTICLE_VERTEX 의 값을 넣어준다.
void cParticle::GetParticleVertex(
	LPPARTICLE_VERTEX pOut,
	const VEC_COLOR& colors,
	const VEC_SCALE& scales)
{
	//파티클 위치 값
	pOut->pos = m_Transform.GetWorldPosition();


	//색상보간
	D3DXCOLOR pColor;

	if (colors.size() == 1){
		pColor = colors[0];
	}
	else if (colors.size() <= 0){
		pColor = 0xFFFF00FF;
	}
	else{

		if (this->m_fNomalizeLiveTime >= 1.0f){

			pColor = colors[colors.size() - 1];

		}

		else{


			//VEC_COLOR 대로 보간을 하자.
			//컬러 배열의 마지막 인덱스
			int lastIndex = colors.size() - 1;

			//컬러 인터벌
			float interval = 1.0f / lastIndex;

			//현재 라이브 타임 량에 따른 위치
			float position = m_fNomalizeLiveTime / interval;

			//시작인덱스
			int startIndex = (int)position;

			//종료 인덱스
			int endIndex = startIndex + 1;

			//보간값
			float fN = position - startIndex;

			//컬러 선형 보간
			D3DXColorLerp(&pColor,			//결과
				&colors[startIndex],		//from
				&colors[endIndex],			//to
				fN);						//normalize Factor

		}
	}


	float pScale = 1.0f;
	//스케일 보간
	if (scales.size() == 1){
		pScale = scales[0];
	}
	else if (scales.size() <= 0){
		pScale = 0.0f;
	}
	else {

		if (this->m_fNomalizeLiveTime >= 1.0f){

			pScale = scales[scales.size() - 1];

		}

		else{

			//스케일 배열의 마지막 인덱스
			int lastIndex = scales.size() - 1;

			//스케일 인터벌
			float interval = 1.0f / lastIndex;

			//현재 라이브 타임 량에 따른 위치
			float position = m_fNomalizeLiveTime / interval;
			//position 의 값에 정수부분이 사용되는 배열의 인덱스가 되는 것이고.
			//소숫점 부분이 다음 배열값까지의 보간량이 된다.

			//시작인덱스
			int startIndex = (int)position;

			//종료 인덱스
			int endIndex = startIndex + 1;

			//보간값
			float fN = position - startIndex;
			pScale = lerp(scales[startIndex], scales[endIndex], fN);
		}

	}


	//결정된 값 셋팅
	pOut->color = pColor;
	pOut->size = pScale * this->m_fScale;
}

