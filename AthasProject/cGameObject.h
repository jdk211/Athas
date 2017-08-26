#pragma once

#include "cAction.h"

class cGameObject
	: public cObject
	, public iActionDelegate
{
protected:
	D3DXVECTOR3		m_vDirection;	//, Direction);
	D3DXVECTOR3		m_vPosition;	//, Position);

public:
	cGameObject(void);
	virtual ~cGameObject(void);

	virtual void OnActionFinish(cAction* pSender) override;

	//Get
	D3DXVECTOR3	GetDirection()	{ return m_vDirection; }
	D3DXVECTOR3	GetPosition()	{ return m_vPosition; }
	
	//Set
	void SetDirection(D3DXVECTOR3 dir)	{ m_vDirection = dir; }
	void SetPosition(D3DXVECTOR3 pos)	{ m_vPosition	= pos; }
};

