#include "stdafx.h"
#include "cResourceMgr_XMeshSkinned.h"


cResourceMgr_XMeshSkinned::cResourceMgr_XMeshSkinned()
{
}


cResourceMgr_XMeshSkinned::~cResourceMgr_XMeshSkinned()
{
}


//�ε� ��ĸ� ������ 
cXMesh_Skinned* cResourceMgr_XMeshSkinned::LoadResource(std::string filePath, void* pParam)
{
	cXMesh_Skinned* newMesh = new cXMesh_Skinned();
	newMesh->Init(filePath, (D3DXMATRIXA16*)pParam);

	return newMesh;
}

//���� ��ĸ� ������
void cResourceMgr_XMeshSkinned::ReleaseResource(cXMesh_Skinned* data)
{
	data->Release();
	SAFE_DELETE(data);
}
