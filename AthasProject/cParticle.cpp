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
	float liveTime,				//���̺� Ÿ��
	const D3DXVECTOR3* pos,		//���� ��ġ
	const D3DXVECTOR3* velo,	//���� �ӵ�
	const D3DXVECTOR3* accel,	//���� ��
	float scale					//�⺻ ������
	)
{
	//Ȱ��ȭ ����
	m_bLive = true;

	//���̺� Ÿ�� �ʱ�ȭ
	m_fTotalLiveTime = liveTime;
	m_fDeltaLiveTime = 0.0f;
	m_fNomalizeLiveTime = 0.0f;

	//��ġ ������
	m_Transform.SetWorldPosition(pos->x, pos->y, pos->z);

	//���� �ӵ�
	m_Velocity = *velo;

	//����
	m_Accelation = *accel;

	//�⺻ ������
	m_fScale = scale;

}


void cParticle::Update(float timeDelta)
{
	if (m_bLive == false) return;

	//���̺� Ÿ���� �ٵǾ��ٸ�...
	if (m_fDeltaLiveTime >= m_fTotalLiveTime){
		m_bLive = false;
	}

	//Ȱ��ȭ �ð� �״´�.
	m_fDeltaLiveTime += timeDelta;

	//����ȭ�� �ð����� ���
	m_fNomalizeLiveTime = m_fDeltaLiveTime / m_fTotalLiveTime;
	if (m_fNomalizeLiveTime > 1.0f)
		m_fNomalizeLiveTime = 1.0f;

	//��ƼŬ ���� �̵�
	m_Transform.MovePositionWorld(
		m_Velocity.x * timeDelta,
		m_Velocity.y * timeDelta,
		m_Velocity.z * timeDelta);


	//��ƼŬ ����
	m_Velocity += m_Accelation * timeDelta;
}


//�ڽ��� ���� ������ �������� LPPARTICLE_VERTEX �� ���� �־��ش�.
void cParticle::GetParticleVertex(
	LPPARTICLE_VERTEX pOut,
	const VEC_COLOR& colors,
	const VEC_SCALE& scales)
{
	//��ƼŬ ��ġ ��
	pOut->pos = m_Transform.GetWorldPosition();


	//���󺸰�
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


			//VEC_COLOR ��� ������ ����.
			//�÷� �迭�� ������ �ε���
			int lastIndex = colors.size() - 1;

			//�÷� ���͹�
			float interval = 1.0f / lastIndex;

			//���� ���̺� Ÿ�� ���� ���� ��ġ
			float position = m_fNomalizeLiveTime / interval;

			//�����ε���
			int startIndex = (int)position;

			//���� �ε���
			int endIndex = startIndex + 1;

			//������
			float fN = position - startIndex;

			//�÷� ���� ����
			D3DXColorLerp(&pColor,			//���
				&colors[startIndex],		//from
				&colors[endIndex],			//to
				fN);						//normalize Factor

		}
	}


	float pScale = 1.0f;
	//������ ����
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

			//������ �迭�� ������ �ε���
			int lastIndex = scales.size() - 1;

			//������ ���͹�
			float interval = 1.0f / lastIndex;

			//���� ���̺� Ÿ�� ���� ���� ��ġ
			float position = m_fNomalizeLiveTime / interval;
			//position �� ���� �����κ��� ���Ǵ� �迭�� �ε����� �Ǵ� ���̰�.
			//�Ҽ��� �κ��� ���� �迭�������� �������� �ȴ�.

			//�����ε���
			int startIndex = (int)position;

			//���� �ε���
			int endIndex = startIndex + 1;

			//������
			float fN = position - startIndex;
			pScale = lerp(scales[startIndex], scales[endIndex], fN);
		}

	}


	//������ �� ����
	pOut->color = pColor;
	pOut->size = pScale * this->m_fScale;
}

