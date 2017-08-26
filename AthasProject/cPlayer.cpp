#include "stdafx.h"
#include "cPlayer.h"
#include "cInventory.h"

Player::Player()
{
}
Player::~Player()
{
}

HRESULT Player::init(void)
{
	_lv = 1;
	_hp = 100;
	_currentHP = 100;
	_mp = 100;
	_currentMP = 100;
	_att = 10;
	_def = 5;
	_exp = 0;
	addAtt = addDef = addMP = 0;

	_rc = RectMake(WINSIZE_X / 2 - 200, WINSIZE_Y / 2, 300, 400);

	_EX = RectMake(WINSIZE_X / 2, WINSIZE_Y / 2, 50, 100);



	return S_OK;
}
void Player::release(void)
{
	
}
void Player::update(void)
{
	_rc2 = RectMake(_rc.left + 100, _rc.top, 150, 50);

	if (KEY_MGR->IsStayDown(VK_LBUTTON))
	{
		if (PtInRect(&_rc2, _ptMouse))
		{
			_rc.left = _ptMouse.x - 150;
			_rc.right = _ptMouse.x + 150;
			_rc.top = _ptMouse.y - 20;
			_rc.bottom = _ptMouse.y + 380;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		_index[i] = RectMake((_rc.left + 70) + i * 80, _rc.bottom - 65, 60, 60);
	}

	if (_exp >= 100)
	{
		_lv++;
		_exp = 0;
		_att++;
		_def++;
		_currentHP = _hp += 10;
		_currentMP = _mp += 10;
	}


	addAtt = _inventory->_equipWeapon.attrinute;
	addDef = _inventory->_equipArmor.attrinute;
	addMP = _inventory->_equipRing.attrinute;


}
void Player::render(void)
{
	if (KEY_MGR->IsOnceDown(VK_F1))
	{
		Rectangle(GetDC(g_hWnd), _rc.left, _rc.top, _rc.right, _rc.bottom);
		for (int i = 0; i < 3; i++)
		{
			Rectangle(GetDC(g_hWnd), _index[i].left, _index[i].top, _index[i].right, _index[i].bottom);
		}
	}


	
	if (_inventory->_equipWeapon.itemKind != ITEM_EMPTY)
	{
		m_WeaRc = RectMake(200, 200, 50, 50);
		
	}
	if (_inventory->_equipArmor.itemKind != ITEM_EMPTY)
	{
		m_ArRc = RectMake(250, 200, 50, 50);
		
	}
	if (_inventory->_equipRing.itemKind != ITEM_EMPTY)
	{
		m_RingRc = RectMake(300, 200, 50, 50);
	
	}
	Explanation();
}


//착용아이템 관련
void Player::Explanation()
{
	char str[128];

	if (PtInRect(&m_WeaRc, _ptMouse))
	{
		if (_inventory->_equipWeapon.itemKind == ITEM_WEAPON)
		{
			if (KEY_MGR->IsStayDown(VK_RBUTTON))
			{
				_inventory->addItem(_inventory->_equipWeapon);
				_inventory->_equipWeapon.attrinute = 0;
				_inventory->_equipWeapon.itemKind = ITEM_EMPTY;
			}
		}
	}

	if (PtInRect(&m_ArRc, _ptMouse))
	{
		if (_inventory->_equipArmor.itemKind == ITEM_ARMOR)
		{
			if (KEY_MGR->IsStayDown(VK_RBUTTON))
			{
				_inventory->addItem(_inventory->_equipArmor);
				_inventory->_equipArmor.attrinute = 0;
				_inventory->_equipArmor.itemKind = ITEM_EMPTY;
			}
		}
	}

	if (PtInRect(&m_RingRc, _ptMouse))
	{
		if (_inventory->_equipRing.itemKind == ITEM_ARMOR)
		{
			if (KEY_MGR->IsStayDown(VK_RBUTTON))
			{
				_inventory->addItem(_inventory->_equipRing);
				_inventory->_equipRing.attrinute = 0;
				_inventory->_equipRing.itemKind = ITEM_EMPTY;
			}
		}
	}

}