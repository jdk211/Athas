#include "stdafx.h"
#include "cSpriteManager.h"


cSpriteManager::cSpriteManager()
:m_pSprite( NULL)
{
}


cSpriteManager::~cSpriteManager()
{
}



HRESULT cSpriteManager::Init()
{
	//���ö���Ʈ ��ü ����
	if (FAILED(D3DXCreateSprite(Device, &m_pSprite)))
		return E_FAIL;

	return S_OK;
}
void cSpriteManager::Release()
{
	SAFE_RELEASE(m_pSprite);
}

//���ö���Ʈ ���� ���۽� ȣ��
void cSpriteManager::BeginSpriteRender()
{
	//��������Ʈ ���� ����
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);		//D3DXSPRITE_ALPHABLEND �÷��� ������ ���ĺ����� ���� �Ǵ� Texture �� ����Ҽ� �ִ�.
}

//���ö���Ʈ ���� ����� ȣ��
void cSpriteManager::EndSpriteRender()
{
	//��������Ʈ ���� ����
	m_pSprite->End();
}

void cSpriteManager::DrawTexture(
	LPDIRECT3DTEXTURE9 pTex,			//�׸� Texture
	LPRECT pSour,						//�׸� �ҽ� ����
	float posX, float posY,				//�׸� ȭ�� ��ġ
	DWORD color,						//���̴� ����
	D3DXVECTOR3* pCenter				//�̹����� �߽� ( NULL �̸� �»�� )
	)
{
	//ȭ�� ���� ��ǥ�� �׸��� SpriteCanvas �� Transform �� �ʱ�ȭ
	D3DXMATRIXA16 matFinal;
	D3DXMatrixIdentity(&matFinal);
	m_pSprite->SetTransform(&matFinal);

	//�׸�ȭ����ġ ( ���ǻ��� Vector2 ���ƴ϶� Vector3 �̴�, Z �� �Ϲ������� ���õǴµ� ���̸� �ְ������ ����ض�... )
	D3DXVECTOR3 pos(posX, posY, 0.0f);

	//���ö���Ʈ ���並 ���� Texture �� ȭ�鿡 �׸���...
	m_pSprite->Draw(
		pTex,		//�׸� Texture
		pSour,		//�׸� �̹����� �ҽ� ����
		(pCenter == NULL) ? &D3DXVECTOR3(0, 0, 0) : pCenter,		//�׸��� �̹��� �߽���ġ
		&pos,		//�׸�ȭ�� ��ġ 
		color		//Hit �÷�
		);
}




void cSpriteManager::DrawTexture(
	LPDIRECT3DTEXTURE9 pTex,			//�׸� Texture
	LPRECT pSour,						//�׸� �ҽ� ����
	float posX, float posY,				//�׸� ȭ�� ��ġ
	float scaleX, float scaleY,			//�׸� ������ ũ��
	float rotate,						//�׸� �����̼� ��
	DWORD color,						//���̴� ����
	D3DXVECTOR3* pCenter				//�̹����� �߽� ( NULL �̸� �»�� )
	)
{
	//�̹����� ������
	D3DXVECTOR3 pivot =
		(pCenter == NULL) ? D3DXVECTOR3(0, 0, 0) : *pCenter;

	//������ ���
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling( &matScale, 
		scaleX, scaleY, 1.0f );

	//ȸ�� ���
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationZ(&matRot,
		rotate);

	//�̵� ���
	D3DXMATRIXA16 matTrans;
	D3DXMatrixTranslation(&matTrans,
		posX, posY, 0.0f);

	//���� ���
	D3DXMATRIXA16 matFinal;
	matFinal = matScale * matRot * matTrans;

	//������ ����
	m_pSprite->SetTransform(&matFinal);

	//�̹��� �׷���
	m_pSprite->Draw(
		pTex,
		pSour,
		&pivot,
		&D3DXVECTOR3(0, 0, 0),	//�׸� ��ġ�� 0 �̳�....
		color);


}

void cSpriteManager::DrawArea(LPDIRECT3DTEXTURE9 pTex,
	LPRECT pSour,						//�׸��ҽ��� ����
	LPRECT pDest,						//�׸�ȭ���� ����
	DWORD color							//���̴� ���� 
	)
{
	float destWidth = pDest->right - pDest->left;
	float destHeight = pDest->bottom - pDest->top;

	float sourWidth = pSour->right - pSour->left;
	float sourHeight = pSour->bottom - pSour->top;


	//���� ������ ���� ������ ��
	float scaleX = destWidth / sourWidth;
	float scaleY = destHeight / sourHeight;

	//������ ���
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale,
		scaleX, scaleY, 1.0f);

	//�̵����
	D3DXMATRIXA16 matTrans;
	D3DXMatrixTranslation(
		&matTrans,
		pDest->left, pDest->top, 0.0f);
	

	D3DXMATRIXA16 matFinal = matScale * matTrans;
	m_pSprite->SetTransform(&matFinal);

	//�̹��� �׷���
	m_pSprite->Draw(
		pTex,
		pSour,
		&D3DXVECTOR3(0, 0, 0),
		&D3DXVECTOR3(0, 0, 0),	//�׸� ��ġ�� 0 �̳�....
		color);
}

void cSpriteManager::DrawNinePatch(
	LPDIRECT3DTEXTURE9 pTex,
	LPRECT pSour,						//�׸� �ҽ� ����
	LPRECT pDest,						//�׸� ȭ�� ����
	LPRECT pCenter,						//������ġ ���� ����
	DWORD color,			//���̴� ���� 
	bool bDrawCenter				//�߾� �׸����� �ȱ׸����� ����
	)
{
	
}

void cSpriteManager::DrawLoop(
	LPDIRECT3DTEXTURE9 pTex,
	float offsetX, float offsetY,		//SourOffset 
	LPRECT pSour,						//�׸� �ҽ� ����
	LPRECT pDest,						//�׸� ȭ�� ����
	DWORD color			//���̴� ���� 
	)
{
}

void cSpriteManager::ResetTransform()
{
	D3DXMATRIXA16 matFinal;
	D3DXMatrixIdentity(&matFinal);
	m_pSprite->SetTransform(&matFinal);
}
