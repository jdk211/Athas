#pragma once

#include "cResourceMgr.h"
#include "cXMesh_Skinned.h"


class cResourceMgr_XMeshSkinned : public cResourceMgr<cXMesh_Skinned*, cResourceMgr_XMeshSkinned>
{
public:
	cResourceMgr_XMeshSkinned();
	~cResourceMgr_XMeshSkinned();

protected:
	//로드 방식만 재정의 
	virtual cXMesh_Skinned* LoadResource(std::string filePath, void* pParam = NULL);

	//해재 방식만 재정의
	virtual void ReleaseResource(cXMesh_Skinned* data);

};

#define RESOURCE_XMESHSKINNED cResourceMgr_XMeshSkinned::GetInstance()