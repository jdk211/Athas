#include "stdafx.h"
#include "cResourceMgr_Texture.h"


cResourceMgr_Texture::cResourceMgr_Texture()
{
}


cResourceMgr_Texture::~cResourceMgr_Texture()
{
}
//�ε� ��ĸ� ������ 
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

//���� ��ĸ� ������
void cResourceMgr_Texture::ReleaseResource(LPDIRECT3DTEXTURE9 data)
{
	data->Release();
}