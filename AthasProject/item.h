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
	ITEM itemKind; // ����������
	RECT itemRC;
	cUIButton* image;
	cUIImageView* image1;
	cUIImageView* image2;
	char* itemName; // �������̸�
	char* itemDescription; // ������ ����
	int attrinute; //������ ����
	int price; //�����۰���
	int count; // �����۰���
};