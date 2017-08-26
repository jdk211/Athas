#include "stdafx.h"
#include "cResourceMgr_XMeshSkinned.h"


cResourceMgr_XMeshSkinned::cResourceMgr_XMeshSkinned()
{
}


cResourceMgr_XMeshSkinned::~cResourceMgr_XMeshSkinned()
{
}


//로드 방식만 재정의 
cXMesh_Skinned* cResourceMgr_XMeshSkinned::LoadResource(std::string filePath, void* pParam)
{
	cXMesh_Skinned* newMesh = new cXMesh_Skinned();
	newMesh->Init(filePath, (D3DXMATRIXA16*)pParam);

	return newMesh;
}

//해재 방식만 재정의
void cResourceMgr_XMeshSkinned::ReleaseResource(cXMesh_Skinned* data)
{
	data->Release();
	SAFE_DELETE(data);
}
