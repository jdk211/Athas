#include "StdAfx.h"
#include "cGameObject.h"

cGameObject::cGameObject(void)
	: m_vDirection(0, 0, 1)
	, m_vPosition(0, 0, 0)
{
}

cGameObject::~cGameObject(void)
{
}

void cGameObject::OnActionFinish( cAction* pSender )
{

}
