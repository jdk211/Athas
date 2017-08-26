#pragma once
class cFrustum
{
private:
	D3DXVECTOR3 rhwPos[8];		//���� ��ǥ 8 ��
	D3DXVECTOR3 frustumPos[8];	//�������������� ��ȯ�� �� 8 ��
	D3DXPLANE frustumPlane[6];	//����ü ��� 6��

public:
	cFrustum();
	~cFrustum();

	//���������� ������Ʈ �Ѵ�.
	void UpdateFrustum(D3DXMATRIXA16* matViewProjection);

	//�������� �ȿ� �����ִ�?
	bool IsInFrustum(D3DXVECTOR3* pCenter, float radius);

	//����� �׷�����.
	void RenderGizmos();
};

