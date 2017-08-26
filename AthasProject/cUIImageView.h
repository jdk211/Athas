#pragma once

// 4 상위 클래스 제작 후 두번째로 만드는 클래스
class cUIImageView : public cUIObject
{
protected:
	LPDIRECT3DTEXTURE9	m_pTexture;

public:
	cUIImageView(void);
	virtual ~cUIImageView(void);

	virtual void SetTexture(char* szFullPath);

	// cUIObject override
	virtual void Render(LPD3DXSPRITE pSprite) override;
};

