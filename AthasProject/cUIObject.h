#pragma once

//1 ���� ���� ����� Ŭ����
class cUIObject : public cObject
{
protected:
	// 2 ó�� �߰��� �������
	std::vector<cUIObject*> m_vecChild;
	D3DXMATRIXA16			m_matWorld;
	
	D3DXVECTOR3				m_vPosition;	// Position); // �θ�κ��ͻ����ġ
	cUIObject*				m_pParent;		// , Parent);
	ST_SIZEN				m_stSize;		// , Size);
	
	// ���߿� �߰��� �������
	// 18 �߰�
	int						m_nTag;			//, Tag);
	bool					m_isHidden;
	bool					m_isDrawBounding; //DrawBounding);
	
public:
	cUIObject(void);
	virtual ~cUIObject(void);

	// 3 ó�� �߰��� ����Լ�
	virtual void AddChild(cUIObject* pChild);
	virtual void Update();
	virtual void Render(LPD3DXSPRITE pSprite);
	virtual void Render(LPD3DXSPRITE pSprite, int num);
	virtual void Destroy();
	virtual void SetPosition(float x, float y, float z = 0);

	// ���� �߰��� ����Լ�
	// 19 �߰�
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

