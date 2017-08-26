#include "stdafx.h"
#include "cUI_Billboard.h"


cUI_Billboard::cUI_Billboard()
{
}


cUI_Billboard::~cUI_Billboard()
{
}

HRESULT cUI_Billboard::Init(string fileName, float wMax, float hMax)
{
	m_fileName = fileName;
	m_WolrdPositon = D3DXVECTOR3(0, 0, 0);
	fW_Max = wMax;
	fH_Max = hMax;
	m_vecVertex1.resize(6);
	if (wMax == 0.0f || hMax == 0.0f)
	{
		m_vecVertex1[0].p = D3DXVECTOR3(-1, -1, 0);
		m_vecVertex1[0].t = D3DXVECTOR2(0, 1);
		m_vecVertex1[0].c = D3DCOLOR_ARGB(255, 255, 255, 255);

		m_vecVertex1[1].p = D3DXVECTOR3(-1, 1, 0);
		m_vecVertex1[1].t = D3DXVECTOR2(0, 0);
		m_vecVertex1[1].c = D3DCOLOR_ARGB(255, 255, 255, 255);

		m_vecVertex1[2].p = D3DXVECTOR3(1, 1, 0); // 첫번째 버텍스 오른쪽 위
		m_vecVertex1[2].t = D3DXVECTOR2(1, 0);
		m_vecVertex1[2].c = D3DCOLOR_ARGB(255, 255, 255, 255);

		m_vecVertex1[3].p = D3DXVECTOR3(-1, -1, 0);
		m_vecVertex1[3].t = D3DXVECTOR2(0, 1);
		m_vecVertex1[3].c = D3DCOLOR_ARGB(255, 255, 255, 255);

		m_vecVertex1[4].p = D3DXVECTOR3(1, 1, 0); // 두번째 버텍스 오른쪽 위
		m_vecVertex1[4].t = D3DXVECTOR2(1, 0);
		m_vecVertex1[4].c = D3DCOLOR_ARGB(255, 255, 255, 255);

		m_vecVertex1[5].p = D3DXVECTOR3(1, -1, 0);// 두번째 버텍스 오른쪽 아래
		m_vecVertex1[5].t = D3DXVECTOR2(1, 1);
		m_vecVertex1[5].c = D3DCOLOR_ARGB(255, 255, 255, 255);
	}
	else
	{
		m_vecVertex1[0].p = D3DXVECTOR3(-wMax, -hMax, 0);
		m_vecVertex1[0].t = D3DXVECTOR2(0, 1);
		m_vecVertex1[0].c = D3DCOLOR_ARGB(255, 255, 255, 255);

		m_vecVertex1[1].p = D3DXVECTOR3(-wMax, hMax, 0);
		m_vecVertex1[1].t = D3DXVECTOR2(0, 0);
		m_vecVertex1[1].c = D3DCOLOR_ARGB(255, 255, 255, 255);

		m_vecVertex1[2].p = D3DXVECTOR3(wMax, hMax, 0);
		m_vecVertex1[2].t = D3DXVECTOR2(wMax, 0);
		m_vecVertex1[2].c = D3DCOLOR_ARGB(255, 255, 255, 255);

		m_vecVertex1[3].p = D3DXVECTOR3(-wMax, -hMax, 0);
		m_vecVertex1[3].t = D3DXVECTOR2(0, 1);
		m_vecVertex1[3].c = D3DCOLOR_ARGB(255, 255, 255, 255);

		m_vecVertex1[4].p = D3DXVECTOR3(wMax, hMax, 0);
		m_vecVertex1[4].t = D3DXVECTOR2(wMax, 0);
		m_vecVertex1[4].c = D3DCOLOR_ARGB(255, 255, 255, 255);

		m_vecVertex1[5].p = D3DXVECTOR3(wMax, -hMax, 0);
		m_vecVertex1[5].t = D3DXVECTOR2(wMax, 1);
		m_vecVertex1[5].c = D3DCOLOR_ARGB(255, 255, 255, 255);
	}

	//Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	return S_OK;
}

void cUI_Billboard::Release()
{
}

void cUI_Billboard::Update(D3DXVECTOR3 pos)
{
	m_WolrdPositon = pos;
}

void cUI_Billboard::Render()
{
	DrawBillboard();
}

void cUI_Billboard::SetBillboard(D3DXMATRIXA16 * matWorld)
{
	D3DXMATRIXA16 matView, matInvView;
	Device->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matInvView, 0, &matView);
	*matWorld = matInvView;	/// : 월드를 뷰의 인버스 매트릭스로 셋팅 해주면 됨.
}

void cUI_Billboard::DrawBillboard()
{
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);


	Device->SetRenderState(D3DRS_LIGHTING, false);
	Device->SetTexture(0, NULL);
	Device->SetFVF(ST_PCT_VERTEX::FVF);	/// ; p c t  D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1};

	//Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	//Device->SetRenderState(D3DRS_ZWRITEENABLE, false);

	{
		Device->SetTexture(0, g_pTextureManager->GetTexture(m_fileName));
	}
	{
		/// >> : 빌보드
		SetBillboard(&matWorld);
	}

	matWorld._41 = m_WolrdPositon.x;
	matWorld._42 = m_WolrdPositon.y;
	matWorld._43 = m_WolrdPositon.z;	/// : z
	Device->SetTransform(D3DTS_WORLD, &matWorld);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, &m_vecVertex1[0], sizeof(ST_PCT_VERTEX));


	//Device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	//Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

void cUI_Billboard::ProgressBar(int currnetHp, int maxHp)
{
	if (currnetHp < 0) currnetHp = 0.0f;
	float hp = (float)currnetHp / (float)maxHp;
	hp = (hp * 4) - 2;

	float wMax = hp;

	m_vecVertex1[2].p = D3DXVECTOR3(wMax, fH_Max, 0);
	m_vecVertex1[2].t = D3DXVECTOR2(wMax, 0);
	m_vecVertex1[2].c = D3DCOLOR_ARGB(255, 255, 255, 255);


	m_vecVertex1[4].p = D3DXVECTOR3(wMax, fH_Max, 0);
	m_vecVertex1[4].t = D3DXVECTOR2(wMax, 0);
	m_vecVertex1[4].c = D3DCOLOR_ARGB(255, 255, 255, 255);

	m_vecVertex1[5].p = D3DXVECTOR3(wMax, -fH_Max, 0);
	m_vecVertex1[5].t = D3DXVECTOR2(wMax, 1);
	m_vecVertex1[5].c = D3DCOLOR_ARGB(255, 255, 255, 255);
}
