#pragma once
#include "cResourceMgr.h"
class cResourceMgr_Effect : public cResourceMgr<LPD3DXEFFECT, cResourceMgr_Effect>
{
public:
	cResourceMgr_Effect();
	~cResourceMgr_Effect();

	//로드 방식만 재정의 
	virtual LPD3DXEFFECT LoadResource(std::string filePath, void* pParam = NULL);

	//해재 방식만 재정의
	virtual void ReleaseResource(LPD3DXEFFECT data);
};


#define RESOURCE_EFFECT cResourceMgr_Effect::GetInstance()