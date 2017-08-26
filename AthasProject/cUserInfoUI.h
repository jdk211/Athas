#pragma once
#include "cUIButton.h"
#include "cUIImageView.h"
#include "cUIObject.h"
#include "cUITextView.h"

class cInventory;
class Shop;
class cArthas;

class cUserInfoUI :public iButtonDelegate
{
private:
	cUIObject*			m_pUIRoot;
	LPD3DXSPRITE		m_Sprite;
	//Hp, Mp Bar
	int m_nHpBar;
	int m_nMpBar;
	cUIObject*         m_pUIRoot1;
	LPD3DXSPRITE      m_Sprite1;
	LPD3DXSPRITE		m_Sprite3;
	Shop* m_Shop;
	cInventory* m_Inven;
	cArthas* m_pUser;

	LPD3DXSPRITE		m_Sprite2;
	RECT Equip[3];

	bool isOpenInven;
	bool isOpenShop;
	bool isOpenEquip;
public:
	cUserInfoUI();
	~cUserInfoUI();

	HRESULT Init();
	void Release();
	void Update();
	void Render();
	void Explanation();
	void setItem();  //아이템 장착창에 보여주는 함수
	void SetHpBar(int currnetHp, int maxHp);
	void SetMpBar(int currnetMp, int maxMp);
	virtual void OnClick(cUIButton* pSender);
	void setUser(cArthas* user) { m_pUser = user; }
	void SetIsCollision(bool isCollision) { isOpenEquip = isCollision; }

};

