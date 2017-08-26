#include "StdAfx.h"
#include "cUIImageView.h"


cUIImageView::cUIImageView(void)
	: m_pTexture(NULL)
{
}


cUIImageView::~cUIImageView(void)
{
}

void cUIImageView::SetTexture( char* szFullPath )
{
	D3DXIMAGE_INFO stImageInfo;
	
	// 함수를 먼저 오버로딩
	m_pTexture = g_pTextureManager->GetTexture(szFullPath, &stImageInfo);

	m_stSize.nWidth = stImageInfo.Width;
	m_stSize.nHeight = stImageInfo.Height;
}

void cUIImageView::Render( LPD3DXSPRITE pSprite )
{
	if(m_isHidden)
		return;

	pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	pSprite->SetTransform(&m_matWorld);

	RECT rc;
	SetRect(&rc, 0, 0, m_stSize.nWidth, m_stSize.nHeight);
	pSprite->Draw(m_pTexture,
		&rc,
		&D3DXVECTOR3(0, 0, 0),
		&D3DXVECTOR3(0, 0, 0),
		D3DCOLOR_ARGB(255, 255, 255, 255));

	pSprite->End();

	cUIObject::Render(pSprite);
}

