#pragma once

//1 제일 먼저 만드는 클래스
class cUIObject : public cObject
{
protected:
	// 2 처음 추가할 멤버변수
	std::vector<cUIObject*> m_vecChild;
	D3DXMATRIXA16			m_matWorld;
	
	D3DXVECTOR3				m_vPosition;	// Position); // 부모로부터상대위치
	cUIObject*				m_pParent;		// , Parent);
	ST_SIZEN				m_stSize;		// , Size);
	
	// 나중에 추가할 멤버변수
	// 18 추가
	int						m_nTag;			//, Tag);
	bool					m_isHidden;
	bool					m_isDrawBounding; //DrawBounding);
	
public:
	cUIObject(void);
	virtual ~cUIObject(void);

	// 3 처음 추가할 멤버함수
	virtual void AddChild(cUIObject* pChild);
	virtual void Update();
	virtual void Render(LPD3DXSPRITE pSprite);
	virtual void Render(LPD3DXSPRITE pSprite, int num);
	virtual void Destroy();
	virtual void SetPosition(float x, float y, float z = 0);

	// 나중 추가할 멤버함수
	// 19 추가
	virtual cUIObject* FindChildByTag(int nTag);
	virtual void SetHidden(bool isHidden);

	//Get
	D3DXVECTOR3	GetPosition()		{ return m_vPosition; }
	cUIObject*	GetParent()			{ return m_pParent; }
	ST_SIZEN	GetSize()			{ return m_stSize; }
	int			GetTag()			{ return m_nTag; }
	bool		GetDrawBounding()	{ return m_isDrawBounding; }

	//Set
	void SetPosition(D3DXVECTOR3 position)		{  m_vPosition = position; }
	void SetParent(cUIObject* pParent)			{  m_pParent = pParent; }
	void SetSize(ST_SIZEN size)					{  m_stSize = size; }
	void SetTag(int tag)						{  m_nTag = tag; }
	void SetDrawBounding(bool isDrawbounding)	{  m_isDrawBounding = isDrawbounding; }
};

