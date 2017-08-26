#pragma once
#include "cResourceMgr.h"

class cResourceMgr_Texture :
	public cResourceMgr<LPDIRECT3DTEXTURE9, cResourceMgr_Texture>
{
public:
	cResourceMgr_Texture();
	~cResourceMgr_Texture();


protected:

	//�ε� ��ĸ� ������ 
	virtual LPDIRECT3DTEXTURE9 LoadResource(std::string filePath, void* pParam = NULL);

	//���� ��ĸ� ������
	virtual void ReleaseResource(LPDIRECT3DTEXTURE9 data);
};

#define RESOURCE_TEXTURE cResourceMgr_Texture::GetInstance()