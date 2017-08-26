#pragma once

#include "cResourceMgr.h"
#include "cXMesh_Static.h"


class cResourceMgr_XMeshStatic : public cResourceMgr<cXMesh_Static*, cResourceMgr_XMeshStatic>
{
public:
	cResourceMgr_XMeshStatic();
	~cResourceMgr_XMeshStatic();

protected:
	//�ε� ��ĸ� ������ 
	virtual cXMesh_Static* LoadResource(std::string filePath, void* pParam = NULL);

	//���� ��ĸ� ������
	virtual void ReleaseResource(cXMesh_Static* data);

};

#define RESOURCE_XMESHSTATIC cResourceMgr_XMeshStatic::GetInstance()
