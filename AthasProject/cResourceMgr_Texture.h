#pragma once
#include "cResourceMgr.h"

class cResourceMgr_Texture :
	public cResourceMgr<LPDIRECT3DTEXTURE9, cResourceMgr_Texture>
{
public:
	cResourceMgr_Texture();
	~cResourceMgr_Texture();


protected:

	//로드 방식만 재정의 
	virtual LPDIRECT3DTEXTURE9 LoadResource(std::string filePath, void* pParam = NULL);

	//해재 방식만 재정의
	virtual void ReleaseResource(LPDIRECT3DTEXTURE9 data);
};

#define RESOURCE_TEXTURE cResourceMgr_Texture::GetInstance()