#pragma once

#include "cSingletonBase.h"

class cTextureManager : public cSingletonBase<cTextureManager>
{
private:
	std::map<std::string, LPDIRECT3DTEXTURE9>	m_mapTexture;
	std::map<std::string, D3DXIMAGE_INFO>		m_mapImageInfo;

public:
	LPDIRECT3DTEXTURE9 GetTexture(char* szFullPath);
	LPDIRECT3DTEXTURE9 GetTexture(std::string& sFullPath);

	// 새로 만들어야되는 함수
	LPDIRECT3DTEXTURE9 GetTexture(char* szFullPath, D3DXIMAGE_INFO* pImageInfo);
	LPDIRECT3DTEXTURE9 GetTexture(std::string& sFullPath, D3DXIMAGE_INFO* pImageInfo);

	void Destroy();
};

#define g_pTextureManager cTextureManager::GetInstance()
