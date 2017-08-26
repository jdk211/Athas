#include "stdafx.h"
#include "cResourceMgr_XMeshStatic.h"


cResourceMgr_XMeshStatic::cResourceMgr_XMeshStatic()
{
}


cResourceMgr_XMeshStatic::~cResourceMgr_XMeshStatic()
{
}

//로드 방식만 재정의 
cXMesh_Static* cResourceMgr_XMeshStatic::LoadResource(std::string filePath, void* pParam)
{
	cXMesh_Static* newMesh = new cXMesh_Static();
	newMesh->Init(filePath, (D3DXMATRIXA16*)pParam);

	return newMesh;
}

//해재 방식만 재정의
void cResourceMgr_XMeshStatic::ReleaseResource(cXMesh_Static* data)
{
	data->Release();
	SAFE_DELETE(data);
}
