#include "stdafx.h"
#include "cInventory.h"
#include "Shop.h"
#include "cUIImageView.h"
#include "cUITextView.h"



cInventory::cInventory()
	:m_pUIRoot(NULL)
	, m_pUIRoot1(NULL)
	, m_pSprite(NULL)
	, m_pSprite1(NULL)
	, Equip(NULL)
	, Equip1(NULL)
	, Equip2(NULL)
	, m_pUIRoot2(NULL)
	, m_pUIRoot3(NULL)
{

	_index[0] = RectMake(593, 0, 60, 60);
	_index[1] = RectMake(660, 0, 60, 60);
	_index[2] = RectMake(725, 0, 60, 60);
	_index[3] = RectMake(284, 756, 60, 60);
	_index[4] = RectMake(326, 756, 60, 60);
	itemCount = 0;

}


cInventory::~cInventory()
{
	SAFE_RELEASE(m_pSprite);
	SAFE_RELEASE(m_pSprite1);
	if (m_pUIRoot)
		m_pUIRoot->Destroy();

	if (m_pUIRoot1)
	{
		m_pUIRoot1->Release();
	}
}

HRESULT cInventory::init()
{


	//cUIImageView* ImgView = new cUIImageView;
	//D3DXCreateSprite(Device, &m_pSprite);
	//ImgView->SetTexture("./Resources/Shop/상점슬롯.jpg");
	//ImgView->SetPosition(D3DXVECTOR3(300,10, 0));
	//m_pUIRoot1 = new cUIObject;
	//m_pUIRoot1->AddChild(ImgView);


	D3DXCreateSprite(Device, &m_pSprite);
	cUIImageView*   ImgView;
	ImgView = new cUIImageView;
	ImgView->SetTexture("./Resources/Shop/상점슬롯.jpg");
	ImgView->SetPosition(D3DXVECTOR3(600, 0, 0));


	m_pUIRoot = new cUIObject;
	m_pUIRoot->AddChild(ImgView);

	D3DXCreateSprite(Device, &m_pSprite1);
	//	int itemCount = 0;
	m_pUIRoot1 = new cUIObject;
	m_pUIRoot1->SetPosition(0, 0);
	temp = 0;
	m_pUIRoot2 = new cUIObject;
	m_pUIRoot2->SetPosition(0, 0);
	m_pUIRoot3 = new cUIObject;
	m_pUIRoot3->SetPosition(0, 0);
	_equipArmor.attrinute = 0;
	_equipArmor.itemKind = ITEM_EMPTY;
	_equipWeapon.attrinute = 0;
	_equipWeapon.itemKind = ITEM_EMPTY;
	_equipRing.attrinute = 0;
	_equipRing.itemKind = ITEM_EMPTY;

	return S_OK;
}

void cInventory::Relese()
{
	if (m_pUIRoot1)
		m_pUIRoot1->Destroy();
}

void cInventory::Update()
{


	if (m_pUIRoot)m_pUIRoot->Update();
	if (m_pUIRoot1)m_pUIRoot1->Update();

	if (m_pUIRoot2)m_pUIRoot2->Update();

	if (m_pUIRoot3)m_pUIRoot3->Update();
	/*for (int i = 0; i < m_vecItemInfo.size(); i++)
	{
	if (KEY_MGR->IsOnceDown(VK_RBUTTON))
	{
	EquipItem(m_vecItemInfo[i]);
	m_vecItemInfo[i].IsBuy = ITEM_EMPTY;
	}
	}*/
	//for (auto iter = m_vecItemInfo.begin(); iter != m_vecItemInfo.end(); iter++)
	//{
	//	if (m_vecItemInfo.size() == 0) continue;
	//	iter->image->SetTag(temp++);
	//	m_pUIRoot1->AddChild(iter->image);
	//}

}
void cInventory::ItemOutPut()
{
	//_index[0] = RectMake(593, 0, 60, 60);
	//_index[1] = RectMake(660, 0, 60, 60);
	//_index[2] = RectMake(725, 0, 60, 60);

	for (int i = 0; i < m_vecItemInfo.size(); i++)
	{
		if (m_vecItemInfo[i].itemKind == ITEM_SOLDOUT)
		{
			m_vecItemInfo.erase(m_vecItemInfo.begin());
		}
		else
		{
			if (m_vecItemInfo[i].itemKind == ITEM_WEAPON)
			{
				m_pUIRoot1->AddChild(m_vecItemInfo[i].image1);
				m_pUIRoot1->SetPosition(593, 0, 0);
				m_pUIRoot1->Render(m_pSprite1);
				//m_vecItemInfo[i].itemRC = _index[0];
				//m_vecItemInfo[i].image1->SetPosition(m_vecItemInfo[i].itemRC.left, m_vecItemInfo[i].itemRC.top, 0);
				//m_vecItemInfo[i].image1->Render(m_pSprite1);
			}
			else if (m_vecItemInfo[i].itemKind == ITEM_ARMOR)
			{
				m_pUIRoot2->AddChild(m_vecItemInfo[i].image1);
				m_pUIRoot2->SetPosition(660, 0, 0);
				m_pUIRoot2->Render(m_pSprite1);
				//m_vecItemInfo[i].itemRC = _index[1];
				//m_vecItemInfo[i].image1->SetPosition(m_vecItemInfo[i].itemRC.left, m_vecItemInfo[i].itemRC.top, 0);
				//m_vecItemInfo[i].image1->Render(m_pSprite1);
			}
			else if (m_vecItemInfo[i].itemKind == ITEM_RING)
			{
				m_pUIRoot3->AddChild(m_vecItemInfo[i].image1);
				m_pUIRoot3->SetPosition(725, 0, 0);
				m_pUIRoot3->Render(m_pSprite1);
				//m_vecItemInfo[i].itemRC = _index[2];
				//m_vecItemInfo[i].image1->SetPosition(m_vecItemInfo[i].itemRC.left, m_vecItemInfo[i].itemRC.top, 0);
				//m_vecItemInfo[i].image1->Render(m_pSprite1);
			}
		}
	}

	//	_index[0]
	//	_index[1]
	//	_index[2]
	//_index[3]
	//	_index[4]
}
void cInventory::Render()
{
	if (m_pUIRoot)	m_pUIRoot->Render(m_pSprite);
	ItemOutPut();
	Explanation();
	char str[128];
	DXFONT_MGR->PrintText("Gold:", 650, 280, 0xFFFFFF00);
	DXFONT_MGR->PrintText(itoa(Gold, str, 10), 700, 280, 0xFFFFFF00);


	//DXFONT_MGR->PrintText("x:", 0, 0, 0xFFFFFF00);
	//DXFONT_MGR->PrintText(itoa(_ptMouse.x, str, 10), 0, 50, 0xFFFFFF00);
	//DXFONT_MGR->PrintText("y:", 0, 70, 0xFFFFFF00);
	//DXFONT_MGR->PrintText(itoa(_ptMouse.y, str, 10), 0, 90, 0xFFFFFF00);

	//Rectangle(GetDC(g_hWnd), _index[0].left, _index[0].top, _index[0].right, _index[0].bottom);
	//Rectangle(GetDC(g_hWnd), _index[1].left, _index[1].top, _index[1].right, _index[1].bottom);
	//Rectangle(GetDC(g_hWnd), _index[2].left, _index[2].top, _index[2].right, _index[2].bottom);
	//Rectangle(GetDC(g_hWnd), _index[3].left, _index[3].top, _index[3].right, _index[3].bottom);

	//int itemCount = 0;
	//POINT ptMouse = GetMousePos();
	//m_iterVecInfo = m_vecItemInfo.begin();
	//for (m_iterVecInfo; m_iterVecInfo != m_vecItemInfo.end(); ++m_iterVecInfo )
	//{
	//	for (int i = 0; i < m_vecItemInfo.size(); i++)
	//	{
	//	if (m_vecItemInfo.size() != NULL)
	//	{
	//		m_iterVecInfo[i].image->SetPosition(400+(60*itemCount), -5, 0);
	//		
	//	}
	//	itemCount++;
	//	break;
	//	}
	//}
}

void cInventory::addItem(tagItemInfo item) {

	if (item.itemKind != ITEM_EMPTY)
	{
		m_vecItemInfo.push_back(item);
	}
}

void cInventory::Explanation()
{
	_ptMouse = GetMousePos();
	int count = 0;;
	char str[256];
	ZeroMemory(str, sizeof(str));
	if (m_vecItemInfo.size() != NULL)
	{
		for (auto &iter : m_vecItemInfo)
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
	m_iterVecInfo = m_vecItemInfo.begin();
	for (m_iterVecInfo; m_iterVecInfo != m_vecItemInfo.end(); ++m_iterVecInfo)
	{
		for (int i = 0; i < 5; i++)
		{
			if (_sellitem)
			{
				if (KEY_MGR->IsOnceDown(VK_LBUTTON))
				{
					if (PtInRect(&_index[0], _ptMouse))
					{
						if (m_iterVecInfo->itemKind = ITEM_WEAPON)
						{
							int sellPrice = m_iterVecInfo->price / 2;
							Gold += sellPrice;
							m_iterVecInfo->itemKind = ITEM_SOLDOUT;
							SOUND_MGR->play("구매", 0.3f);
						}
					}
					if (PtInRect(&_index[1], _ptMouse))
					{
						if (m_iterVecInfo->itemKind = ITEM_ARMOR)
						{
							int sellPrice = m_iterVecInfo->price / 2;
							Gold += sellPrice;
							m_iterVecInfo->itemKind = ITEM_SOLDOUT;
							SOUND_MGR->play("구매", 0.3f);
						}
					}
					if (PtInRect(&_index[2], _ptMouse))
					{
						if (m_iterVecInfo->itemKind = ITEM_RING)
						{
							int sellPrice = m_iterVecInfo->price / 2;
							Gold += sellPrice;
							m_iterVecInfo->itemKind = ITEM_SOLDOUT;
							SOUND_MGR->play("구매", 0.3f);
						}
					}
				}
			}

		}
		break;
	}
	//for (int i = 0; i < m_vecItemInfo.size(); i++)
	//{
	//	if (_sellitem)
	//	{
	//		if (KEY_MGR->IsOnceDown(VK_LBUTTON))
	//		{
	//			if (PtInRect(&_index[0], _ptMouse))
	//			{
	//				int sellPrice = m_vecItemInfo[i].price / 2;
	//				Gold += sellPrice;
	//				m_vecItemInfo[i].itemKind = ITEM_SOLDOUT;
	//				SOUND_MGR->play("구매", 0.3f);
	//			}
	//		}
	//	}
	//}
	for (int i = 0; i<m_vecItemInfo.size(); i++)
	{
		if (_useitem)
		{
			if (KEY_MGR->IsOnceDown(VK_LBUTTON))
			{
				if (PtInRect(&_index[0], _ptMouse))
				{
					EquipItem(m_vecItemInfo[i]);
					m_vecItemInfo.erase(m_vecItemInfo.begin());
					SOUND_MGR->play("상인");
					Equip = true;
					i++;
				}
				if (PtInRect(&_index[1], _ptMouse))
				{
					EquipItem(m_vecItemInfo[i]);
					m_vecItemInfo.erase(m_vecItemInfo.begin());
					SOUND_MGR->play("상인");
					Equip1 = true;
					i++;
				}
				if (PtInRect(&_index[2], _ptMouse))
				{
					EquipItem(m_vecItemInfo[i]);
					m_vecItemInfo.erase(m_vecItemInfo.begin());
					SOUND_MGR->play("상인");
					Equip2 = true;
					i++;
				}
				if (i > 3)
				{
					i = 0;
				}
			}
		}
	}
}

void cInventory::EquipItem(tagItemInfo item)
{
	tagItemInfo beforeItem;
	switch (m_iterVecInfo->itemKind)
	{
	case ITEM_ARMOR:
		beforeItem = getEquipArmor();
		setEquipArmor(*m_iterVecInfo);
		if (beforeItem.itemKind == ITEM_ARMOR)
		{
			addItem(beforeItem);
		}
		break;
	case ITEM_WEAPON:
		beforeItem = getEquipWeapon();
		setEquipWeapon(*m_iterVecInfo);
		if (beforeItem.itemKind == ITEM_WEAPON)
		{
			addItem(beforeItem);
		}
		break;
	case ITEM_RING:
		beforeItem = getEquipRing();
		setEquipRing(*m_iterVecInfo);
		if (beforeItem.itemKind == ITEM_RING)
		{
			addItem(beforeItem);
		}
		break;
	}
}

void cInventory::OnClick(cUIButton * pSender)
{
}