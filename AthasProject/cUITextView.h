#pragma once

// 7 이미지뷰 다음으로 만들 UI컴포넌트
class cUITextView : public cUIObject
{
protected:
	cFontManager::eFontType m_eFontType;		// 폰트 타입
	std::string				m_sText;			//쓰여질 내용
	DWORD					m_dwDrawTextFormat;	// 정렬방식
	D3DCOLOR				m_dwTextColor;		//텍스트 컬러

public:
	cUITextView(void);
	virtual ~cUITextView(void);

	virtual void Render(LPD3DXSPRITE pSprite) override;

	//Get
	cFontManager::eFontType GetFontType()		{ return m_eFontType; }
	std::string				GetText()			{ return m_sText; }
	DWORD					GetDrawTextFormat() { return m_dwDrawTextFormat; }
	D3DCOLOR				GetTextColor()		{ return m_dwTextColor; }

	//Set
	void SetFontType(cFontManager::eFontType fontType)	{ m_eFontType = fontType; }
	void SetText(std::string text)						{ m_sText = text; }
	void SetDrawTextFormat(DWORD textFormat)			{ m_dwDrawTextFormat = textFormat; }
	void SetTextColor(D3DCOLOR color)					{ m_dwTextColor = color; }
};

