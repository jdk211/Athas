#pragma once

class cAction;

class iActionDelegate
{
public:
	virtual void OnActionFinish(cAction* pSender) = 0;
};

class cGameObject;

class cAction : public cObject
{
protected:
	cGameObject*		m_pOwner;	// Owner);
	iActionDelegate*	m_pDelegate;// Delegate);

public:
	cAction(void);
	virtual ~cAction(void);

	virtual void Start() = 0;
	virtual void Update() = 0;

	//Get
	cGameObject*		GetOwner()		{ return m_pOwner; }
	iActionDelegate*	GetDelegate()	{ return m_pDelegate; }

	//Set
	void	SetOwner(cGameObject* owner)		{ m_pOwner = owner; }
	void	SetDelegate(iActionDelegate* dele)	{ m_pDelegate = dele; }

};

