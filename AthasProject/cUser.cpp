#include "stdafx.h"
#include "cUser.h"


cUser::cUser()
{
}


cUser::~cUser()
{
}

HRESULT cUser::Init()
{
	return S_OK;
}

void cUser::Release()
{
}

void cUser::Update(float timeDelta)
{
}

void cUser::Render(cCamera* pCam)
{
}

void cUser::AniControl()
{
}

D3DXVECTOR3 cUser::SetHeightPos(cTerrain * terrain)
{
	D3DXVECTOR3 vec;


	vec.x = m_pTransForm.GetWorldPosition().x;
	vec.y = terrain->GetHeight(m_pTransForm.GetWorldPosition().x, m_pTransForm.GetWorldPosition().z);
	vec.z = m_pTransForm.GetWorldPosition().z;
	return vec;
}
