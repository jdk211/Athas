#include "stdafx.h"
#include "Shop.h"
#include "cInventory.h"
#include "cUIImageView.h"
#include "cArthas.h"

Shop::Shop()
	: m_pUIRoot(NULL)
	, m_pUIRoot1(NULL)
	, m_pUIRoot2(NULL)
	, m_pSprite(NULL)
	, m_pSprite1(NULL)
{
	itemCount = 0;
	itemCount1 = 0;
}


Shop::~Shop()
{
	SAFE_RELEASE(m_pSprite);
	SAFE_RELEASE(m_pSprite1);
	if (m_pUIRoot)
		m_pUIRoot->Destroy();

	if (m_pUIRoot1)
		m_pUIRoot1->Destroy();

	if (m_pUIRoot2)
		m_pUIRoot2->Destroy();
}
HRESULT Shop::init()
{
	//for (int i = 0; i < 8; i++)
	//{
	//	for (int j = 0; j < 8; j++)
	//	{
	//		m_shopRC[j * 8 + i] = RectMake(WINSIZE_X / 2 - 110 + 67 * i, WINSIZE_Y / 2 + 20 + j * 115, 50, 50);
	//	}
	//}

	m_Uesr = NULL;
	SetItem();

	static cUIImageView*   ShopImg;
	ShopImg = new cUIImageView;
	D3DXCreateSprite(Device, &m_pSprite);

	ShopImg->SetTexture("./Resources/Shop/상점슬롯.jpg");
	ShopImg->SetPosition(D3DXVECTOR3(0, 0, 0));

	m_pUIRoot = new cUIObject;
	m_pUIRoot->SetPosition(D3DXVECTOR3(10, 10, 0));
	m_pUIRoot->SetSize(ST_SIZEN(40, 40));
	m_pUIRoot->AddChild(ShopImg);

	//for (auto &iter : m_vecInfo)
	//{
	//	m_pUIRoot1 = new cUIObject;
	//	m_pUIRoot1->SetPosition(D3DXVECTOR3(10, 10, 0));
	//	m_pUIRoot1->SetSize(ST_SIZEN(50, 50));
	//	m_pUIRoot1->AddChild(iter.image);
	//}


	return S_OK;
}

void Shop::Update()
{
	//SetItem();

	if (m_pUIRoot)m_pUIRoot->Update();
	if (m_pUIRoot1)m_pUIRoot1->Update();
	WinPointX = _ptMouse.x;
	WinPointX = _ptMouse.y;

}

void Shop::Render()
{
	if (m_pUIRoot)	m_pUIRoot->Render(m_pSprite);
	if (m_pUIRoot1)	m_pUIRoot1->Render(m_pSprite);
	ItemOutPut();
	Explanation();
}

void Shop::SetItem()
{
	D3DXCreateSprite(Device, &m_pSprite1);
	tagItemInfo weapon1;
	weapon1.itemKind = ITEM_WEAPON;
	weapon1.image = new cUIButton;
	weapon1.itemRC = RectMake(5, 5, 50, 50);
	weapon1.image->SetTexture("./Resources/Shop/서리한.jpg", "./Resources/Shop/서리한.jpg", "./Resources/Shop/서리한.jpg");
	weapon1.image->SetPosition(D3DXVECTOR3((float)weapon1.itemRC.left, (float)weapon1.itemRC.top, 0));
	weapon1.image->SetSize(ST_SIZEN(60, 60));
	weapon1.image->SetDelegate(this);
	weapon1.image->SetTag(1);
	weapon1.image1 = new cUIImageView;
	weapon1.image1->SetTexture("./Resources/Shop/서리한.jpg");
	weapon1.image1->SetTag(9);
	weapon1.image2 = new cUIImageView;
	weapon1.image2->SetTexture("./Resources/Shop/서리한.jpg");
	weapon1.image2->SetTag(10);
	weapon1.count = 1;
	weapon1.itemName = "서리한";
	weapon1.price = 100000;
	weapon1.itemDescription = "아서스의검";
	weapon1.attrinute = 100;
	m_vecInfo.push_back(weapon1);

	tagItemInfo armor1;
	armor1.itemKind = ITEM_ARMOR;
	armor1.image = new cUIButton;
	armor1.itemRC = RectMake(70, 5, 50, 50);
	armor1.image->SetTexture("./Resources/Shop/방어구.jpg", "./Resources/Shop/방어구.jpg", "./Resources/Shop/방어구.jpg");
	armor1.image1 = new cUIImageView;
	armor1.image1->SetTexture("./Resources/Shop/방어구.jpg");
	armor1.image1->SetTag(11);
	armor1.image2 = new cUIImageView;
	armor1.image2->SetTexture("./Resources/Shop/방어구.jpg");
	armor1.image2->SetTag(12);
	armor1.image->SetPosition(D3DXVECTOR3((float)armor1.itemRC.left, (float)armor1.itemRC.top, 0));
	armor1.image->SetSize(ST_SIZEN(60, 60));
	armor1.image->SetDelegate(this);
	armor1.image->SetTag(2);
	armor1.count = 1;
	armor1.itemName = "아서스갑옷";
	armor1.price = 100000;
	armor1.attrinute = 100;
	armor1.itemDescription = "아서스의갑옷";
	m_vecInfo.push_back(armor1);

	tagItemInfo potion1;
	potion1.itemKind = ITEM_POTION;
	potion1.image = new cUIButton;
	potion1.itemRC = RectMake(130, 5, 50, 50);
	potion1.image->SetPosition(D3DXVECTOR3((float)potion1.itemRC.left, (float)potion1.itemRC.top, 0));
	potion1.image->SetTexture("./Resources/Shop/빨포.jpg", "./Resources/Shop/빨포.jpg", "./Resources/Shop/빨포.jpg");
	potion1.image->SetSize(ST_SIZEN(60, 60));
	potion1.image->SetDelegate(this);
	potion1.image->SetTag(3);
	potion1.count = 100;
	potion1.itemName = "체력포션";
	potion1.attrinute = 100;
	potion1.itemDescription = "체력회복포션";
	potion1.price = 500;
	m_vecInfo.push_back(potion1);

	tagItemInfo potion2;
	potion2.itemKind = ITEM_POTION1;
	potion2.count = 100;
	potion2.image = new cUIButton;
	potion2.itemRC = RectMake(190, 5, 50, 50);
	potion2.image->SetTexture("./Resources/Shop/파포.jpg", "./Resources/Shop/파포.jpg", "./Resources/Shop/파포.jpg");
	potion2.image->SetPosition(D3DXVECTOR3((float)potion2.itemRC.left, (float)potion2.itemRC.top, 0));
	potion2.image->SetSize(ST_SIZEN(60, 60));
	potion2.image->SetDelegate(this);
	potion2.image->SetTag(4);
	potion2.itemName = "마력포션";
	potion2.attrinute = 100;
	potion2.itemDescription = "마력회복포션";
	potion2.price = 500;
	m_vecInfo.push_back(potion2);

	tagItemInfo ring1;
	ring1.itemKind = ITEM_RING;
	ring1.image = new cUIButton;
	ring1.itemRC = RectMake(255, 5, 50, 50);
	ring1.image->SetTexture("./Resources/Shop/반지.jpg", "./Resources/Shop/반지.jpg", "./Resources/Shop/반지.jpg");
	ring1.image1 = new cUIImageView;
	ring1.image1->SetTexture("./Resources/Shop/반지.jpg");
	ring1.image1->SetTag(13);
	ring1.image2 = new cUIImageView;
	ring1.image2->SetTexture("./Resources/Shop/반지.jpg");
	ring1.image2->SetTag(14);
	ring1.image->SetPosition(D3DXVECTOR3((float)ring1.itemRC.left, (float)ring1.itemRC.top, 0));
	ring1.image->SetSize(ST_SIZEN(60, 60));
	ring1.image->SetDelegate(this);
	ring1.image->SetTag(5);
	ring1.count = 1;
	ring1.itemName = "아서스반지";
	ring1.attrinute = 100;
	ring1.price = 100000;
	ring1.itemDescription = "아서스의반지";
	m_vecInfo.push_back(ring1);

	tagItemInfo strPotiin;
	strPotiin.itemKind = ITEM_StrPOTION;
	strPotiin.image = new cUIButton;
	strPotiin.itemRC = RectMake(5, 65, 50, 50);
	strPotiin.image->SetTexture("./Resources/Shop/strbook.jpg", "./Resources/Shop/strbook.jpg", "./Resources/Shop/strbook.jpg");
	strPotiin.image->SetPosition(D3DXVECTOR3((float)strPotiin.itemRC.left, (float)strPotiin.itemRC.top, 0));
	strPotiin.image->SetSize(ST_SIZEN(60, 60));
	strPotiin.image->SetDelegate(this);
	strPotiin.image->SetTag(6);
	strPotiin.count = 10;
	strPotiin.attrinute = 1;
	strPotiin.price = 500;
	strPotiin.itemDescription = "힘책";
	strPotiin.itemName = "힘책";
	m_vecInfo.push_back(strPotiin);

	tagItemInfo intPotiin;
	intPotiin.itemKind = ITEM_IntPOTION;
	intPotiin.image = new cUIButton;
	intPotiin.itemRC = RectMake(70, 65, 50, 50);
	intPotiin.image->SetTexture("./Resources/Shop/conbook.jpg", "./Resources/Shop/conbook.jpg", "./Resources/Shop/conbook.jpg");
	intPotiin.image->SetPosition(D3DXVECTOR3((float)intPotiin.itemRC.left, (float)intPotiin.itemRC.top, 0));
	intPotiin.image->SetSize(ST_SIZEN(60, 60));
	intPotiin.image->SetDelegate(this);
	intPotiin.image->SetTag(7);
	intPotiin.count = 10;
	intPotiin.attrinute = 1;
	intPotiin.price = 500;
	intPotiin.itemDescription = "지능책";
	intPotiin.itemName = "지능책";
	m_vecInfo.push_back(intPotiin);


	tagItemInfo AllPotoin;
	AllPotoin.itemKind = ITEM_AllPOTION;
	AllPotoin.image = new cUIButton;
	AllPotoin.itemRC = RectMake(130, 65, 50, 50);
	AllPotoin.image->SetTexture("./Resources/Shop/allbook.jpg", "./Resources/Shop/allbook.jpg", "./Resources/Shop/allbook.jpg");
	AllPotoin.image->SetPosition(AllPotoin.itemRC.left, AllPotoin.itemRC.top);
	AllPotoin.image->SetSize(ST_SIZEN(60, 60));
	AllPotoin.image->SetDelegate(this);
	AllPotoin.image->SetTag(15);
	AllPotoin.count = 10;
	AllPotoin.attrinute = 1;
	AllPotoin.price = 1500;
	AllPotoin.itemDescription = "올스택책";
	AllPotoin.itemName = "올스택책";
	m_vecInfo.push_back(AllPotoin);
	m_pUIRoot1 = new cUIObject;
	m_pUIRoot1->SetPosition(D3DXVECTOR3(10, 10, 0));
	//m_pUIRoot1->SetSize(ST_SIZEN(50, 50));
	m_pUIRoot1->AddChild(AllPotoin.image);
	//	m_pUIRoot1->AddChild(ring1.image1);
	m_pUIRoot1->AddChild(ring1.image2);
	//m_pUIRoot1->AddChild(weapon1.image1);
	m_pUIRoot1->AddChild(weapon1.image2);
	//m_pUIRoot1->AddChild(armor1.image1);
	m_pUIRoot1->AddChild(armor1.image2);
	m_pUIRoot1->AddChild(ring1.image);
	m_pUIRoot1->AddChild(weapon1.image);
	m_pUIRoot1->AddChild(armor1.image);
	m_pUIRoot1->AddChild(potion1.image);
	m_pUIRoot1->AddChild(potion2.image);
	m_pUIRoot1->AddChild(strPotiin.image);
	m_pUIRoot1->AddChild(intPotiin.image);

	//	m_pUIRoot1->FindChildByTag(9)->SetHidden(false);
	m_pUIRoot1->FindChildByTag(10)->SetHidden(false);
	//	m_pUIRoot1->FindChildByTag(11)->SetHidden(false);
	m_pUIRoot1->FindChildByTag(12)->SetHidden(false);
	//	m_pUIRoot1->FindChildByTag(13)->SetHidden(false);
	m_pUIRoot1->FindChildByTag(14)->SetHidden(false);
}

void Shop::deleteAllI()
{
	m_vecInfo.clear();
}

void Shop::additem(tagItemInfo item)
{

	if (item.itemKind == ITEM_SOLDOUT) return;
	bool isNewItem = true;
	m_iterVecInfo = m_vecInfo.begin();
	for (m_iterVecInfo; m_iterVecInfo != m_vecInfo.end();)
	{
		if (m_iterVecInfo->itemKind != item.itemKind) continue;
		if (strcmp(m_iterVecInfo->itemName, item.itemName) == 0)
		{
			m_iterVecInfo->count++;
			isNewItem = false;
			break;
		}
		if (isNewItem)
		{
			m_vecInfo.push_back(item);
		}
	}
}

tagItemInfo Shop::buyitem(tagItemInfo item)
{
	if (Gold >= item.price)
	{
		Gold -= item.price;
		return item;
	}
	else
	{
		tagItemInfo nullitem;
		nullitem.itemKind = ITEM_EMPTY;
		nullitem.count = 0;
		return nullitem;
	}

}

void Shop::buyState(tagItemInfo item)
{
	if (item.itemKind == ITEM_StrPOTION || item.itemKind == ITEM_IntPOTION || item.itemKind == ITEM_AllPOTION)
	{
		if (Gold <= 0) return;
		Gold -= item.price;
	}
}

void Shop::ItemOutPut()
{
	//int itemCount = 0;
	//m_veciterInfo = m_vecInfo.begin();
	//for (m_veciterInfo; m_veciterInfo != m_vecInfo.end(); ++m_veciterInfo)
	//{
	//		m_veciterInfo->image->SetPosition(m_veciterInfo->itemRC.left, m_veciterInfo->itemRC.top);
	//		m_veciterInfo->image->Render(m_pSprite);
	//		
	//	itemCount++;
	//}
}

void Shop::Explanation()
{
	_ptMouse = GetMousePos();
	int count = 0;;
	char str[256];
	ZeroMemory(str, sizeof(str));
	if (m_vecInfo.size() != NULL)
	{
		for (auto &iter : m_vecInfo)
		{	//내가 선택한 아이템종류만 나와라
			if (PtInRect(&iter.itemRC, _ptMouse))
			{
				RECT _EX = RectMake(_ptMouse.x, _ptMouse.y, 200, 200);
				DXFONT_MGR->PrintText("이름:", _EX.left, _EX.top - 10, 0xFFFFFFFF);
				DXFONT_MGR->PrintText(iter.itemName, _EX.left, _EX.top + 10, 0xFFFFFFFF);
				DXFONT_MGR->PrintText("가격:", _EX.left, _EX.top + 30, 0xFFFFFFFF);
				DXFONT_MGR->PrintText(itoa(iter.price, str, 10), _EX.left, _EX.top + 50, 0xFFFFFFFF);
				DXFONT_MGR->PrintText("설명:", _EX.left, _EX.top + 70, 0xFFFFFFFF);
				DXFONT_MGR->PrintText(iter.itemDescription, _EX.left, _EX.top + 90, 0xFFFFFFFF);
				DXFONT_MGR->PrintText("성능:", _EX.left, _EX.top + 110, 0xFFFFFFFF);
				DXFONT_MGR->PrintText(itoa(iter.attrinute, str, 10), _EX.left, _EX.top + 130, 0xFFFFFFFF);
			}
		}
	}


}

void Shop::OnClick(cUIButton * pSender)
{
	if (m_Uesr == NULL) return;
	switch (pSender->GetTag())
	{
	case 1:
		m_Uesr->getinven()->addItem(buyitem(m_vecInfo[0]));
		SOUND_MGR->play("구매");
		break;
	case 2:
		m_Uesr->getinven()->addItem(buyitem(m_vecInfo[1]));
		SOUND_MGR->play("구매");
		break;
	case 3:
		m_Uesr->getinven()->addItem(buyitem(m_vecInfo[2]));
		SOUND_MGR->play("구매");
		if (Gold >= m_vecInfo[2].price)
		{
			itemCount++;
		}
		break;
	case 4:
		m_Uesr->getinven()->addItem(buyitem(m_vecInfo[3]));
		SOUND_MGR->play("구매");
		if (Gold >= m_vecInfo[3].price)
		{
			itemCount1++;
		}
		break;
	case 5:
		m_Uesr->getinven()->addItem(buyitem(m_vecInfo[4]));
		SOUND_MGR->play("구매");
		break;
	}

	switch (pSender->GetTag())
	{
	case 6:
		buyState(m_vecInfo[5]);
		SOUND_MGR->play("구매");
		m_Uesr->SetStr(m_Uesr->GetStr() + m_vecInfo[5].attrinute);
		break;
	case 7:
		buyState(m_vecInfo[6]);
		SOUND_MGR->play("구매");
		m_Uesr->SetCon(m_Uesr->GetCon() + m_vecInfo[6].attrinute);
		break;
	case 15:
		buyState(m_vecInfo[7]);
		SOUND_MGR->play("구매");
		m_Uesr->SetStr(m_Uesr->GetStr() + m_vecInfo[7].attrinute);
		m_Uesr->SetCon(m_Uesr->GetCon() + m_vecInfo[7].attrinute);
		m_Uesr->SetWis(m_Uesr->GetWis() + m_vecInfo[7].attrinute);
		break;
	default:
		break;
	}


}