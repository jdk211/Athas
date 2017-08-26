#include "stdafx.h"
#include "cQuadParticle.h"


cQuadParticle::cQuadParticle()
	: m_bLive(false)
{
}


cQuadParticle::~cQuadParticle()
{
}

void cQuadParticle::Start(
	float liveTime,				//라이브 타임
	const D3DXVECTOR3* pos,		//시작 위치
	const D3DXVECTOR3* velo,		//시작 속도
	const D3DXVECTOR3* accel,	//가속 값
	const D3DXVECTOR3* rotate,	//초당 회전 값
	const D3DXVECTOR3* rotateAccel,	//초당 회전 증가값
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
	Transform.SetWorldPosition(
		pos->x, pos->y, pos->z);

	//시작 속도
	m_Velocity = *velo;

	//가속
	m_Accelation = *accel;

	//회전
	m_Rotate = *rotate;

	//회전 가속
	m_RotateAccel = *rotateAccel;

	//기본 스케일
	m_fScale = scale;
}

void cQuadParticle::Update(float timeDelta)
{
	if (m_bLive == false) return;

	//라이브 타임이 다되었다면...
	if (m_fDeltaLiveTime > m_fTotalLiveTime){
		m_bLive = false;
	}

	//활성화 시간 쌓는다.
	m_fDeltaLiveTime += timeDelta;

	//정규화된 시간값을 계산
	m_fNomalizeLiveTime = m_fDeltaLiveTime / m_fTotalLiveTime;
	if (m_fNomalizeLiveTime > 1.0f)
		m_fNomalizeLiveTime = 1.0f;

	//파티클 월드 이동
	Transform.MovePositionWorld(
		m_Velocity.x * timeDelta,
		m_Velocity.y * timeDelta,
		m_Velocity.z * timeDelta);

	Transform.RotateSelf(
		m_Rotate.x * timeDelta,
		m_Rotate.y * timeDelta,
		m_Rotate.z * timeDelta);

	//파티클 가속
	m_Velocity += m_Accelation * timeDelta;

	//파티클 회전 가속
	m_Rotate += m_RotateAccel * timeDelta;
}

//자신의 정점 정보를 바탕으로 LPPARTICLEQUAD_VERTEX 의 값을 넣어준다.
void cQuadParticle::GetParticleVertex(
	LPPARTICLEQUAD_VERTEX pOut,
	DWORD* pIndex,
	const VEC_COLOR& colors,
	const VEC_SCALE& scales,
	DWORD dwParticleNum)
{







	//파티클 위치 값
	D3DXVECTOR3 center = Transform.GetWorldPosition();

	DWORD dwcolor = 0;
	float scale = 0.0f;





	//죽을때 다되면 마지막 값으로.....
	if (m_fNomalizeLiveTime == 1.0f){
		dwcolor = colors[colors.size() - 1];
		scale = m_fScale * scales[scales.size() - 1];
		return;
	}

	//태어나기 시작했으면 처음값으로....
	else if (m_fNomalizeLiveTime == 0.0f){
		dwcolor = colors[0];
		scale = m_fScale * scales[0];
		return;
	}


	//
	// 스케일 보간
	//


	//보간 필요 없다...
	if (scales.size() == 1){
		//최종 스케일 값 셋팅
		scale = m_fScale * scales[0];
	}

	//보간 필요함
	else{

		// 스케일 보간 결과
		float s = 0.0f;

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
		s = lerp(scales[startIndex], scales[endIndex], fN);

		//최종 스케일 값 셋팅
		scale = m_fScale * s;
	}

	//
	// 컬러 보간
	//

	if (colors.size() == 1){

		//첫번째 값 대입
		dwcolor = colors[0];

		//D3DXCOLOR 는 DWORD 형으로 캐스팅이 가능하여 DWORD 형에 직접 대입가능
		//DWORD c = D3DXCOLOR( 0.5f, 0.1f, 0.7f, 1.0f);

	}

	else{

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



		//최종 컬러값
		D3DXCOLOR color;

		//컬러 선형 보간
		D3DXColorLerp(&color,			//결과
			&colors[startIndex],		//from
			&colors[endIndex],			//to
			fN);						//normalize Factor


		//최종 컬러값 셋팅
		dwcolor = (DWORD)(color);
	}

	//스케일에 따른 halfSize;
	float halfScale = scale * 0.5f;

	//방향
	D3DXVECTOR3 x = Transform.GetRight();
	D3DXVECTOR3 y = Transform.GetUp();

	//정점 정보 대입
	(pOut + 0)->pos = center + (-x * halfScale) + (y * halfScale);
	(pOut + 1)->pos = center + (x * halfScale) + (y * halfScale);
	(pOut + 2)->pos = center + (-x * halfScale) + (-y * halfScale);
	(pOut + 3)->pos = center + (x * halfScale) + (-y * halfScale);

	//UV Animation 을 주자
	(pOut + 0)->uv = D3DXVECTOR2(0, 0);
	(pOut + 1)->uv = D3DXVECTOR2(1, 0);
	(pOut + 2)->uv = D3DXVECTOR2(0, 1);
	(pOut + 3)->uv = D3DXVECTOR2(1, 1);

	(pOut + 0)->color = dwcolor;
	(pOut + 1)->color = dwcolor;
	(pOut + 2)->color = dwcolor;
	(pOut + 3)->color = dwcolor;

	//0----1
	//|   /|
	//|  / |
	//| /  |
	//|/   |
	//2----3

	//인덱스 정보 대입 ( 인덱스 넣을때 지금까지 그려지는 Quad 수 (dwParticleNum) 만큼 점프해한 값을 넣어야 한다 )
	*(pIndex + 0) = (dwParticleNum * 4) + 0;
	*(pIndex + 1) = (dwParticleNum * 4) + 1;
	*(pIndex + 2) = (dwParticleNum * 4) + 2;
	*(pIndex + 3) = (dwParticleNum * 4) + 2;
	*(pIndex + 4) = (dwParticleNum * 4) + 1;
	*(pIndex + 5) = (dwParticleNum * 4) + 3;
}
