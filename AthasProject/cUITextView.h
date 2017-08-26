#pragma once

// 7 �̹����� �������� ���� UI������Ʈ
class cUITextView : public cUIObject
{
protected:
	cFontManager::eFontType m_eFontType;		// ��Ʈ Ÿ��
	std::string				m_sText;			//������ ����
	DWORD					m_dwDrawTextFormat;	// ���Ĺ��
	D3DCOLOR				m_dwTextColor;		//�ؽ�Ʈ �÷�

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

