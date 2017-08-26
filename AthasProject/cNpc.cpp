#include "stdafx.h"
#include "cNpc.h"


cNpc::cNpc()
{
}


cNpc::~cNpc()
{
}

HRESULT cNpc::Init()
{
	return S_OK;
}

void cNpc::Release()
{
}

void cNpc::Update(float timeDelta)
{
}

void cNpc::Render(cCamera * pCam)
{
}

D3DXVECTOR3 cNpc::SetHeightPos(cTerrain * terrain)
{
	D3DXVECTOR3 vec;

	vec.x = m_pTransForm.GetWorldPosition().x;
	vec.y = terrain->GetHeight(m_pTransForm.GetWorldPosition().x, m_pTransForm.GetWorldPosition().z);
	vec.z = m_pTransForm.GetWorldPosition().z;

	return vec;
}
