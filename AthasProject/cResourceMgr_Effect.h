#pragma once
#include "cResourceMgr.h"
class cResourceMgr_Effect : public cResourceMgr<LPD3DXEFFECT, cResourceMgr_Effect>
{
public:
	cResourceMgr_Effect();
	~cResourceMgr_Effect();

	//�ε� ��ĸ� ������ 
	virtual LPD3DXEFFECT LoadResource(std::string filePath, void* pParam = NULL);

	//���� ��ĸ� ������
	virtual void ReleaseResource(LPD3DXEFFECT data);
};


#define RESOURCE_EFFECT cResourceMgr_Effect::GetInstance()