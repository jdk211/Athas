#include "stdafx.h"
#include "cDXFontManager.h"


cDXFontManager::cDXFontManager()
: pFontSprite( NULL )
{
}


cDXFontManager::~cDXFontManager()
{
}


HRESULT cDXFontManager::Init()
{
	//Default Font ����
	if (FAILED(D3DXCreateFont(
		Device,					//����̽��� �ѱ��
		30.0f,					//��Ʈ�� ����, 
		0.0f,				//��Ʈ�� �� ( �̰��� 0 �̸� �տ��� ������ ��Ʈ ���̿� ����Ͽ� ���� ��Ʈ�� ������ �ִ� ��Ⱦ�� ���� ���� ��ũ�Ⱑ �ڵ����� �������� ) 	
		FW_BOLD,			//��Ʈ�� �β� ����( FW_BOLD ��Ʈ�� BOLD ��Ÿ�� ũ�⸦ ���´� )
		1,					//�Ӹ� ( �׳� 1  )
		FALSE,				//���ڸ� ����
		DEFAULT_CHARSET,	//ĳ���� Set ( �׳� DEFAULT_CHARSET )
		OUT_DEFAULT_PRECIS,	//�ܰ� ó����� ( �׳� OUT_DEFAULT_PRECIS )
		DEFAULT_QUALITY,			//����Ƽ ( �׳� DEFAULT_QUALITY ),
		DEFAULT_PITCH | FF_DONTCARE,		//�۽�ü �йи��� ���� (�׳� DEFAULT_PITCH | FF_DONTCARE )
		"���� ���",					//���� ��Ʈ�̸� �׳� ��Ʈ�̸��� ��Ʈ������ ���� �ȴ�.
		&pDefultFont)))
	{
		return E_FAIL;
	}

	return S_OK;

}
void cDXFontManager::Release()
{
	SAFE_RELEASE(pDefultFont);
}

void cDXFontManager::PrintText(std::string str, int x, int y, DWORD color)
{
	RECT rc = { x, y, 0, 0 };

	pDefultFont->DrawTextA(
		this->pFontSprite,		//��������Ʈ �ν��Ͻ�  ( �ϴ� NULL )
		str.c_str() ,//���ڿ� �ּ�
		-1,					//���� ���ڿ� ���� ( -1 �̸� ��ü )
		&rc,				//����
		DT_LEFT | DT_NOCLIP,	//DT_NOCLIP �̿ɼ��� ���� �簢�� ������ ���� ���� �ʴ´�
		color);
}
void cDXFontManager::PrintTextShadow(std::string str, int x, int y, DWORD color, DWORD shadow)
{
	//������ 1�ȼ� ���� �о���
	PrintText(str, x + 1, y + 1, shadow);

	//���� Text �� ��´�.
	PrintText(str, x, y, color);
}
void cDXFontManager::PrintTextOutline(std::string str, int x, int y, DWORD color, DWORD outLine)
{
	//�ܰ� ��´�
	PrintText(str, x + 1, y, outLine);
	PrintText(str, x, y + 1, outLine);
	PrintText(str, x - 1, y, outLine);
	PrintText(str, x, y - 1, outLine);

	//���� Text �� ��´�.
	PrintText(str, x, y, color);
}