#pragma once

// 9 ���������� ���� Ŭ����

// Delegate�� ���߿� Ŭ�� ó���� �� ����
// 11 ��ư Ŭ�� ó���� ���� Delegate �������̽� ����

class cUIButton;

class iButtonDelegate
{
public:
	virtual void OnClick(cUIButton* pSender) = 0;
};

class cUIButton : public cUIObject
{
protected:
	enum eButtonState
	{
		E_NORMAL,
		E_MOVER,
		E_SELECTED,
		E_STATE_CNT
	};
	
	eButtonState		m_eButtonState;
	LPDIRECT3DTEXTURE9	m_aTexture[E_STATE_CNT];

	// 12 �߰�
	iButtonDelegate*	m_pDelegate;

public:
	cUIButton(void);
	virtual ~cUIButton(void);

	virtual void SetTexture(char* szNor, char* szOvr, char* szSel);

	// cUIObject override
	virtual void Update() override;
	virtual void Render(LPD3DXSPRITE pSprite) override;

	//Get
	iButtonDelegate* GetDelegate() { return m_pDelegate; }
	
	//Set
	void SetDelegate(iButtonDelegate* btnDelegate) { m_pDelegate = btnDelegate; }
};

