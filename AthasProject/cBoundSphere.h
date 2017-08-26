#pragma once



//
// 구 충돌영역 
// 
class cBoundSphere
{
private:
	//물리매니져는 내친구
	friend class cPhysicManager;

protected:
	D3DXVECTOR3 localCenter;			//구의 중심점
	D3DXVECTOR3 localHalfSize;			//로컬 하프 사이즈

public:
	cBoundSphere();
	~cBoundSphere();

	//월드 단위의 센터와 반지름을 얻는다.
	void GetWorldCenterRadius(const cTransform* pTrans, D3DXVECTOR3* outCenter, float* outRaidus);

	//월드 단위의 하프 사이즈를 얻는다
	void GetWorldHalfSize(const cTransform* pTrans, D3DXVECTOR3* outHalfSize);

	//Bound 정보를 셋팅 한다.
	virtual void SetBound(const D3DXVECTOR3* pCenter, const D3DXVECTOR3* pHalfSize);

	//Bound 정보를 셋팅 한다.
	virtual void SetMinMax(const D3DXVECTOR3* pMin, const D3DXVECTOR3* pMax);


	//랜더링한다.
	virtual void RenderGizmos(const cTransform* pTrans);

};

