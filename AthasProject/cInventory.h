#pragma once
#include "cUIButton.h"
class Shop;
class cInventory :public iButtonDelegate
{
private:

	LPD3DXSPRITE		m_pSprite;
	LPD3DXSPRITE		m_pSprite1;
	Shop* p_Shop;
	int					temp;

	bool _sellitem;
	RECT _index[5];
	bool _useitem;
	int itemCount;
public:
	cUIObject*			m_pUIRoot;
	cUIObject*			m_pUIRoot1;
	cUIObject*			m_pUIRoot2;
	cUIObject*			m_pUIRoot3;
	RECT m_weaRC;
	RECT m_arRC;
	RECT m_ringRC;
	RECT m_invenRC[16];
	bool Equip;
	bool Equip1;
	bool Equip2;

	void getShop(Shop* _sp) { p_Shop = _sp; }
	tagItemInfo _equipArmor;
	tagItemInfo _equipWeapon;
	tagItemInfo _equipRing;

	HRESULT init();
	void Relese();
	void Update();
	void Render();
	void addItem(tagItemInfo item);
	void Explanation();
	void EquipItem(tagItemInfo item);
	void ItemOutPut();

	tagItemInfo getEquipArmor() { return _equipArmor; }
	void setEquipArmor(tagItemInfo equipArmor) { _equipArmor = equipArmor; }

	tagItemInfo getEquipWeapon() { return _equipWeapon; }
	void setEquipWeapon(tagItemInfo equipWeapon) { _equipWeapon = equipWeapon; }

	tagItemInfo getEquipRing() { return _equipRing; }
	void setEquipRing(tagItemInfo equipRing) { _equipRing = equipRing; }
	virtual void OnClick(cUIButton* pSender) override;

	bool setSell(bool type) { return _sellitem = type; }
	bool setUse(bool type) { return _useitem = type; }
	cInventory();
	~cInventory();
};

