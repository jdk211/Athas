#include "stdafx.h"
#include "cUserInfoUI.h"
#include "cInventory.h"
#include "Shop.h"
#include "cScene_Town.h"
#include "cArthas.h"


cUserInfoUI::cUserInfoUI()
		: isOpenInven(false)
		, isOpenShop(false)
		, isOpenEquip(false)
		, m_pUIRoot1(NULL)
		, m_Sprite1(NULL)
		, m_Sprite2(NULL)
{
}


cUserInfoUI::~cUserInfoUI()
{
}

HRESULT cUserInfoUI::Init()
{
	D3DXCreateSprite(Device, &m_Sprite1);
	cUIImageView* EquipImg = new cUIImageView;
	EquipImg->SetTexture("./Resources/UI/equip.png");
	EquipImg->SetPosition(D3DXVECTOR3(930, 0, 0));
	m_pUIRoot1 = new cUIObject;
	m_pUIRoot1->SetPosition(D3DXVECTOR3(0, 0, 0));
	m_pUIRoot1->AddChild(EquipImg);
	//hp, mp 초기화
	m_nHpBar = 180;
	m_nMpBar = 180;

	//Sprite Setting
	D3DXCreateSprite(Device, &m_Sprite);

	//HeadUp UI
	cUIImageView* HeadUpUI = new cUIImageView;
	HeadUpUI->SetTexture("./Resources/UI/userInfo.png");
	HeadUpUI->SetPosition(D3DXVECTOR3(0, 10, 0));

	cUIImageView* hpBar = new cUIImageView;
	hpBar->SetTexture("./Resources/UI/hpBar.png");
	hpBar->SetPosition(D3DXVECTOR3(165, 70, 0));
	hpBar->SetTag(1);

	cUIImageView* mpBar = new cUIImageView;
	mpBar->SetTexture("./Resources/UI/mpBar.png");
	mpBar->SetPosition(D3DXVECTOR3(165, 85, 0));
	mpBar->SetTag(2);


	cUIImageView* menuBar = new cUIImageView;
	menuBar->SetTexture("./Resources/UI/menuBar.png");
	menuBar->SetPosition(D3DXVECTOR3(0, 720, 0));

	cUIImageView* Potion = new cUIImageView;
	Potion->SetTexture("./Resources/Shop/작은빨포.jpg");
	Potion->SetPosition(D3DXVECTOR3(284, 756, 0));

	cUIImageView* Potion1 = new cUIImageView;
	Potion1->SetTexture("./Resources/Shop/작은파포.jpg");
	Potion1->SetPosition(D3DXVECTOR3(326, 756, 0));

	//스킬이미지
	cUIImageView* attack = new cUIImageView;
	attack->SetTexture("./Resources/UI/attack.png");
	attack->SetPosition(D3DXVECTOR3(108, 756, 0));
	//스킬이미지
	cUIImageView* whirlwind = new cUIImageView;
	whirlwind->SetTexture("./Resources/UI/whirlwind.png");
	whirlwind->SetPosition(D3DXVECTOR3(151, 756, 0));
	//스킬이미지
	cUIImageView* iceSpell = new cUIImageView;
	iceSpell->SetTexture("./Resources/UI/iceSpell.png");
	iceSpell->SetPosition(D3DXVECTOR3(194, 756, 0));
	//스킬이미지
	cUIImageView* fireball = new cUIImageView;
	fireball->SetTexture("./Resources/UI/fire_ball.png");
	fireball->SetPosition(D3DXVECTOR3(237, 756, 0));

	//쿨타임용 박스
	cUIImageView* frontImg = new cUIImageView;
	frontImg->SetTexture("./Resources/UI/black.png");
	frontImg->SetPosition(D3DXVECTOR3(151, 756, 0));
	frontImg->SetSize(ST_SIZEN(20, 20));

	cUIImageView* frontImg1 = new cUIImageView;
	frontImg1->SetTexture("./Resources/UI/black.png");
	frontImg1->SetPosition(D3DXVECTOR3(194, 756, 0));
	frontImg1->SetSize(ST_SIZEN(20, 20));

	cUIImageView* frontImg2 = new cUIImageView;
	frontImg2->SetTexture("./Resources/UI/black.png");
	frontImg2->SetPosition(D3DXVECTOR3(237, 756, 0));
	frontImg2->SetSize(ST_SIZEN(20, 20));	

	//UI Root
	m_pUIRoot = new cUIObject;
	m_pUIRoot->SetPosition(D3DXVECTOR3(0, 0, 0));
	m_pUIRoot->AddChild(hpBar);
	m_pUIRoot->AddChild(mpBar);
	m_pUIRoot->AddChild(HeadUpUI);
	m_pUIRoot->AddChild(menuBar);
	m_pUIRoot->AddChild(Potion);
	m_pUIRoot->AddChild(Potion1);
	m_pUIRoot->AddChild(attack);
	m_pUIRoot->AddChild(whirlwind);
	m_pUIRoot->AddChild(iceSpell);
	m_pUIRoot->AddChild(fireball);
	m_pUIRoot->AddChild(frontImg);
	m_pUIRoot->AddChild(frontImg1);
	m_pUIRoot->AddChild(frontImg2);

	m_Shop = new Shop;
	m_Shop->init();
	m_Inven = new cInventory;
	m_Inven->init();

	m_Shop->getInven(m_Inven);
	m_Inven->getShop(m_Shop);


	Equip[0] = RectMake(960, 85, 60, 60);
	Equip[1] = RectMake(1020, 85, 60, 60);
	Equip[2] = RectMake(1100, 85, 60, 60);
	return S_OK;
}

void cUserInfoUI::Release()
{
	SAFE_RELEASE(m_pUIRoot);
	SAFE_RELEASE(m_Sprite);
}

void cUserInfoUI::Update()
{
	m_pUIRoot->Update();
		
	m_pUIRoot->FindChildByTag(1)->SetSize(ST_SIZEN(m_nHpBar, 17));
	m_pUIRoot->FindChildByTag(2)->SetSize(ST_SIZEN(m_nMpBar, 17));

	m_Shop->getPlayer(m_pUser);
	if (KEY_MGR->IsOnceDown('P'))
	{
		SetHpBar(80, 100);
	}
#pragma region 장착

	if (!isOpenEquip)
	{
		if (KEY_MGR->IsOnceDown('L'))
		{
			isOpenEquip = true;
		}

	}
	if (isOpenEquip)
	{
		if (KEY_MGR->IsOnceDown('L'))
		{
			isOpenEquip = false;
		}
	}
	if (isOpenEquip)
	{
		m_pUIRoot1->Update();
	}
#pragma endregion

#pragma region 상점

	if (!isOpenShop)
	{
		if (KEY_MGR->IsOnceDown('F'))
		{
			isOpenShop = true;
			SOUND_MGR->play("상인", 0.5f);
			m_Inven->setUse(false);
			m_Inven->setSell(true);
		}
	}
	else
	{
		if (KEY_MGR->IsOnceDown('F'))
		{
			isOpenShop = false;
			m_Inven->setUse(true);
			m_Inven->setSell(false);

		}
	}
	if (!isOpenInven)
	{
		if (KEY_MGR->IsOnceDown('I'))
		{
			isOpenInven = true;
		}
	}
	else
	{
		if (KEY_MGR->IsOnceDown('I'))
		{
			isOpenInven = false;
		}
	}
	if (isOpenInven)   m_Inven->Update();
	if (isOpenShop) m_Shop->Update();
#pragma endregion

}

void cUserInfoUI::Render()
{
	char str[128];
	if (m_pUIRoot)m_pUIRoot->Render(m_Sprite);
	if (isOpenInven)m_Inven->Render();
	if (isOpenShop) m_Shop->Render();
	setItem();
	Explanation();
	if (isOpenEquip)
	{
		m_pUIRoot1->Render(m_Sprite1);

		DXFONT_MGR->PrintText(itoa(m_pUser->GetStatus().str, str, 10), 1030, 153, 0xFF000000);
		DXFONT_MGR->PrintText(itoa(m_pUser->GetStatus().wis, str, 10), 1030, 203, 0xFF000000);
		DXFONT_MGR->PrintText(itoa(m_pUser->GetStatus().con, str, 10), 1030, 253, 0xFF000000);

	}
	DXFONT_MGR->PrintText(itoa(m_Shop->itemCount, str, 10), 280, 745, 0xFFFFFF00);
	DXFONT_MGR->PrintText(itoa(m_Shop->itemCount1, str, 10), 320, 745, 0xFFFFFF00);


}

void cUserInfoUI::Explanation()
{
	RECT _EX;
	char str[128];
	if (PtInRect(&m_Inven->_equipWeapon.itemRC, _ptMouse))
	{
		if (m_Inven->_equipWeapon.itemKind == ITEM_WEAPON)
		{
			_EX = RectMake(_ptMouse.x, _ptMouse.y, 200, 200);
			DXFONT_MGR->PrintText("이름:", _EX.left, _EX.top - 10, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(m_Inven->_equipWeapon.itemName, _EX.left, _EX.top + 10, 0xFFFFFFFF);
			DXFONT_MGR->PrintText("가격:", _EX.left, _EX.top + 30, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(itoa(m_Inven->_equipWeapon.price, str, 10), _EX.left, _EX.top + 50, 0xFFFFFFFF);
			DXFONT_MGR->PrintText("설명:", _EX.left, _EX.top + 70, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(m_Inven->_equipWeapon.itemDescription, _EX.left, _EX.top + 90, 0xFFFFFFFF);
			DXFONT_MGR->PrintText("성능:", _EX.left, _EX.top + 110, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(itoa(m_Inven->_equipWeapon.attrinute, str, 10), _EX.left, _EX.top + 130, 0xFFFFFFFF);
			if (KEY_MGR->IsStayDown(VK_RBUTTON))
			{
				m_Inven->addItem(m_Inven->_equipWeapon);
				m_Inven->_equipWeapon.attrinute = 0;
				m_Inven->_equipWeapon.itemKind = ITEM_EMPTY;
			}
		}
	}
	if (PtInRect(&m_Inven->_equipArmor.itemRC, _ptMouse))
	{
		if (m_Inven->_equipArmor.itemKind == ITEM_WEAPON)
		{
			_EX = RectMake(_ptMouse.x, _ptMouse.y, 200, 200);
			DXFONT_MGR->PrintText("이름:", _EX.left, _EX.top - 10, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(m_Inven->_equipArmor.itemName, _EX.left, _EX.top + 10, 0xFFFFFFFF);
			DXFONT_MGR->PrintText("가격:", _EX.left, _EX.top + 30, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(itoa(m_Inven->_equipArmor.price, str, 10), _EX.left, _EX.top + 50, 0xFFFFFFFF);
			DXFONT_MGR->PrintText("설명:", _EX.left, _EX.top + 70, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(m_Inven->_equipArmor.itemDescription, _EX.left, _EX.top + 90, 0xFFFFFFFF);
			DXFONT_MGR->PrintText("성능:", _EX.left, _EX.top + 110, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(itoa(m_Inven->_equipArmor.attrinute, str, 10), _EX.left, _EX.top + 130, 0xFFFFFFFF);
			if (KEY_MGR->IsStayDown(VK_RBUTTON))
			{
				m_Inven->addItem(m_Inven->_equipArmor);
				m_Inven->_equipArmor.attrinute = 0;
				m_Inven->_equipArmor.itemKind = ITEM_EMPTY;
			}
		}
	}
	if (PtInRect(&m_Inven->_equipRing.itemRC, _ptMouse))
	{
		if (m_Inven->_equipRing.itemKind == ITEM_WEAPON)
		{
			_EX = RectMake(_ptMouse.x, _ptMouse.y, 200, 200);
			DXFONT_MGR->PrintText("이름:", _EX.left, _EX.top - 10, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(m_Inven->_equipRing.itemName, _EX.left, _EX.top + 10, 0xFFFFFFFF);
			DXFONT_MGR->PrintText("가격:", _EX.left, _EX.top + 30, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(itoa(m_Inven->_equipRing.price, str, 10), _EX.left, _EX.top + 50, 0xFFFFFFFF);
			DXFONT_MGR->PrintText("설명:", _EX.left, _EX.top + 70, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(m_Inven->_equipRing.itemDescription, _EX.left, _EX.top + 90, 0xFFFFFFFF);
			DXFONT_MGR->PrintText("성능:", _EX.left, _EX.top + 110, 0xFFFFFFFF);
			DXFONT_MGR->PrintText(itoa(m_Inven->_equipRing.attrinute, str, 10), _EX.left, _EX.top + 130, 0xFFFFFFFF);
			if (KEY_MGR->IsStayDown(VK_RBUTTON))
			{
				m_Inven->addItem(m_Inven->_equipRing);
				m_Inven->_equipRing.attrinute = 0;
				m_Inven->_equipRing.itemKind = ITEM_EMPTY;
			}
		}
	}
}

void cUserInfoUI::setItem()
{
	if (isOpenEquip)
	{
		if (m_Inven->_equipWeapon.itemKind != ITEM_EMPTY)
		{
			//m_Inven->_equipWeapon.image2->SetPosition(960,85,0);
			//m_Inven->_equipWeapon.image2->Render(m_Sprite1);
			m_pUIRoot1->AddChild(m_Inven->_equipWeapon.image2);
			m_pUIRoot1->FindChildByTag(10)->SetPosition(D3DXVECTOR3(960, 85, 0));
			m_pUIRoot1->Render(m_Sprite1);
			if (m_Inven->Equip == true)
			{
				m_pUser->SetStr(m_pUser->GetStatus().str + m_Inven->_equipWeapon.attrinute);
				m_Inven->Equip = false;
			}
		}
		if (m_Inven->_equipArmor.itemKind != ITEM_EMPTY)
		{
			m_pUIRoot1->AddChild(m_Inven->_equipArmor.image2);
			m_pUIRoot1->FindChildByTag(12)->SetPosition(D3DXVECTOR3(1020, 85, 0));
			m_pUIRoot1->Render(m_Sprite1);
			//m_Inven->_equipArmor.image2->SetPosition(1020,85, 0);
			//m_Inven->_equipArmor.image2->Render(m_Sprite1);
			if (m_Inven->Equip1 == true)
			{
				m_pUser->SetCon(m_pUser->GetStatus().con + m_Inven->_equipArmor.attrinute);
				m_Inven->Equip1 = false;
			}
		}
		if (m_Inven->_equipRing.itemKind != ITEM_EMPTY)
		{
			m_pUIRoot1->AddChild(m_Inven->_equipRing.image2);
			m_pUIRoot1->FindChildByTag(14)->SetPosition(D3DXVECTOR3(1080, 85, 0));
			m_pUIRoot1->Render(m_Sprite1);
			//	m_Inven->_equipRing.image2->SetPosition(1080,85, 0);
			//m_Inven->_equipRing.image2->Render(m_Sprite1);
			if (m_Inven->Equip2 == true)
			{
				m_pUser->SetWis(m_pUser->GetStatus().wis + m_Inven->_equipRing.attrinute);
				m_Inven->Equip2 = false;
			}
		}
	}
	if (KEY_MGR->IsOnceDown('5'))
	{
		m_Shop->itemCount--;
		m_pUser->SetHp(m_pUser->GetStatus().hp + 100);
	}
	if (KEY_MGR->IsOnceDown('6'))
	{
		m_Shop->itemCount1--;
		m_pUser->SetMp(m_pUser->GetStatus().mp + 100);
	}
}

void cUserInfoUI::SetHpBar(int currnetHp, int maxHp)
{
	int hp = (int)(((float)currnetHp / (float)maxHp) * 100.0f);
	hp = (hp * 18) / 10;
	m_nHpBar = hp;
}

void cUserInfoUI::SetMpBar(int currnetMp, int maxMp)
{
	int mp = (int)(((float)currnetMp / (float)maxMp) * 100.0f);
	mp = (mp * 18) / 10;
	m_nMpBar = mp;
}

void cUserInfoUI::OnClick(cUIButton * pSender)
{
}
