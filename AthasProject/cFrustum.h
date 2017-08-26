#pragma once
class cFrustum
{
private:
	D3DXVECTOR3 rhwPos[8];		//동차 좌표 8 개
	D3DXVECTOR3 frustumPos[8];	//뷰프로젝션으로 변환된 점 8 개
	D3DXPLANE frustumPlane[6];	//절두체 평면 6개

public:
	cFrustum();
	~cFrustum();

	//프러스텀을 업데이트 한다.
	void UpdateFrustum(D3DXMATRIXA16* matViewProjection);

	//프러스텀 안에 걸져있니?
	bool IsInFrustum(D3DXVECTOR3* pCenter, float radius);

	//기즈모를 그려본다.
	void RenderGizmos();
};

