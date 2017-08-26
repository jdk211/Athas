#pragma once

#include "cBoundSphere.h"

class cBoundBox : public cBoundSphere
{
private:
	//물리매니져는 내친구
	friend class cPhysicsManager;

protected:

	//축에 정렬된 AABB 형태일때 만 유효하다 ( 월드 OBB 로 뺄때는 8 개의 점위치가 필요하다 )

	D3DXVECTOR3 localMinPos;
	D3DXVECTOR3 localMaxPos;
	D3DCOLOR    m_Color;


public:
	cBoundBox();
	~cBoundBox();

	//월드 로 변환된 8 개의 점을 얻는다. ( outBoxPos 8 개 배열포인터 )
	void GetWorldBox(const cTransform* pTrans, D3DXVECTOR3* outBoxPos);

	//월드 로 변환된 8 개의 점의 MinMax 를 얻는다.
	void GetWorldAABBMinMax(const cTransform* pTrans, D3DXVECTOR3* min, D3DXVECTOR3* max);

	//정보 셋팅
	virtual void SetBound(const D3DXVECTOR3* pCenter, const D3DXVECTOR3* pHalfSize) override;
	virtual void SetMinMax(const D3DXVECTOR3* pMin, const D3DXVECTOR3* pMax) override;


	//랜더링한다.
	virtual void RenderGizmos(const cTransform* pTrans) override;

	D3DCOLOR GetRenderColor() { return m_Color; }
	void SetRenderColor(D3DCOLOR color)
	{
		m_Color = color;
	}
};

