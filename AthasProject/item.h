#pragma once
#include "cUIButton.h"
#include "cUIImageView.h"
#include "stdafx.h"
using namespace std;

enum ITEM
{
	ITEM_POTION,
	ITEM_POTION1,
	ITEM_ARMOR,
	ITEM_WEAPON,
	ITEM_RING,
	ITEM_StrPOTION,
	ITEM_IntPOTION,
	ITEM_AllPOTION,
	ITEM_SOLDOUT,
	ITEM_EMPTY

};

struct tagItemInfo
{
	ITEM itemKind; // 아이템종류
	RECT itemRC;
	cUIButton* image;
	cUIImageView* image1;
	cUIImageView* image2;
	char* itemName; // 아이템이름
	char* itemDescription; // 아이템 설명
	int attrinute; //아이템 성능
	int price; //아이템가격
	int count; // 아이템갯수
};