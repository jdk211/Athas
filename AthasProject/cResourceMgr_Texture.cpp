#include "stdafx.h"
#include "cResourceMgr_Texture.h"


cResourceMgr_Texture::cResourceMgr_Texture()
{
}


cResourceMgr_Texture::~cResourceMgr_Texture()
{
}
//로드 방식만 재정의 
LPDIRECT3DTEXTURE9 cResourceMgr_Texture::LoadResource(std::string filePath, void* pParam)
{
	LPDIRECT3DTEXTURE9 pNewTexture = NULL;

	if (FAILED(D3DXCreateTextureFromFile(
		Device,
		filePath.c_str(),
		&pNewTexture)))
	{
		return NULL;
	}

	return pNewTexture;


}

//해재 방식만 재정의
void cResourceMgr_Texture::ReleaseResource(LPDIRECT3DTEXTURE9 data)
{
	data->Release();
}