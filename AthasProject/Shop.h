#pragma once
#include "stdafx.h"

#include "cUIButton.h"

class cArthas;
class cInventory;
using namespace std;

class Shop :public iButtonDelegate
{
private:
	cInventory* m_inven;

public:
	cArthas* m_Uesr;
	int itemCount;

	int itemCount1;
	int WinPointX;
	int WinPointY;

	cUIObject*			m_pUIRoot;
	cUIObject*			m_pUIRoot1;
	cUIObject*			m_pUIRoot2;
	Shop();
	~Shop();
	LPD3DXSPRITE		m_pSprite;
	LPD3DXSPRITE		m_pSprite1;
	LPD3DXSPRITE        m_PSptite12;

	RECT m_shopRC[16];
	vector<tagItemInfo> m_vecInfo;
	vector<tagItemInfo>::iterator m_veciterInfo;
	HRESULT init();
	void Update();
	void Render();

	void SetItem();
	void deleteAllI();

	void additem(tagItemInfo item);
	tagItemInfo buyitem(tagItemInfo item);
	void buyState(tagItemInfo item);


	void ItemOutPut();
	void Explanation();
	virtual void OnClick(cUIButton* pSender) override;
	vector<tagItemInfo> getVector() { return m_vecInfo; }

	void getInven(cInventory* inven) { m_inven = inven; }
	void getPlayer(cArthas* Uesr) { m_Uesr = Uesr; }
};