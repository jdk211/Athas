#include "stdafx.h"
#include "cResourceMgr_XMeshStatic.h"


cResourceMgr_XMeshStatic::cResourceMgr_XMeshStatic()
{
}


cResourceMgr_XMeshStatic::~cResourceMgr_XMeshStatic()
{
}

//�ε� ��ĸ� ������ 
cXMesh_Static* cResourceMgr_XMeshStatic::LoadResource(std::string filePath, void* pParam)
{
	cXMesh_Static* newMesh = new cXMesh_Static();
	newMesh->Init(filePath, (D3DXMATRIXA16*)pParam);

	return newMesh;
}

//���� ��ĸ� ������
void cResourceMgr_XMeshStatic::ReleaseResource(cXMesh_Static* data)
{
	data->Release();
	SAFE_DELETE(data);
}
