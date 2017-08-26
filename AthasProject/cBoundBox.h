#pragma once

#include "cBoundSphere.h"

class cBoundBox : public cBoundSphere
{
private:
	//�����Ŵ����� ��ģ��
	friend class cPhysicsManager;

protected:

	//�࿡ ���ĵ� AABB �����϶� �� ��ȿ�ϴ� ( ���� OBB �� ������ 8 ���� ����ġ�� �ʿ��ϴ� )

	D3DXVECTOR3 localMinPos;
	D3DXVECTOR3 localMaxPos;
	D3DCOLOR    m_Color;


public:
	cBoundBox();
	~cBoundBox();

	//���� �� ��ȯ�� 8 ���� ���� ��´�. ( outBoxPos 8 �� �迭������ )
	void GetWorldBox(const cTransform* pTrans, D3DXVECTOR3* outBoxPos);

	//���� �� ��ȯ�� 8 ���� ���� MinMax �� ��´�.
	void GetWorldAABBMinMax(const cTransform* pTrans, D3DXVECTOR3* min, D3DXVECTOR3* max);

	//���� ����
	virtual void SetBound(const D3DXVECTOR3* pCenter, const D3DXVECTOR3* pHalfSize) override;
	virtual void SetMinMax(const D3DXVECTOR3* pMin, const D3DXVECTOR3* pMax) override;


	//�������Ѵ�.
	virtual void RenderGizmos(const cTransform* pTrans) override;

	D3DCOLOR GetRenderColor() { return m_Color; }
	void SetRenderColor(D3DCOLOR color)
	{
		m_Color = color;
	}
};

