#pragma once



//
// �� �浹���� 
// 
class cBoundSphere
{
private:
	//�����Ŵ����� ��ģ��
	friend class cPhysicManager;

protected:
	D3DXVECTOR3 localCenter;			//���� �߽���
	D3DXVECTOR3 localHalfSize;			//���� ���� ������

public:
	cBoundSphere();
	~cBoundSphere();

	//���� ������ ���Ϳ� �������� ��´�.
	void GetWorldCenterRadius(const cTransform* pTrans, D3DXVECTOR3* outCenter, float* outRaidus);

	//���� ������ ���� ����� ��´�
	void GetWorldHalfSize(const cTransform* pTrans, D3DXVECTOR3* outHalfSize);

	//Bound ������ ���� �Ѵ�.
	virtual void SetBound(const D3DXVECTOR3* pCenter, const D3DXVECTOR3* pHalfSize);

	//Bound ������ ���� �Ѵ�.
	virtual void SetMinMax(const D3DXVECTOR3* pMin, const D3DXVECTOR3* pMax);


	//�������Ѵ�.
	virtual void RenderGizmos(const cTransform* pTrans);

};

