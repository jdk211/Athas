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
	//스플라이트 개체 생성
	if (FAILED(D3DXCreateSprite(Device, &m_pSprite)))
		return E_FAIL;

	return S_OK;
}
void cSpriteManager::Release()
{
	SAFE_RELEASE(m_pSprite);
}

//스플라이트 랜더 시작시 호출
void cSpriteManager::BeginSpriteRender()
{
	//스프라이트 랜더 시작
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);		//D3DXSPRITE_ALPHABLEND 플래를 넣으면 알파블렌딩이 지원 되는 Texture 를 사용할수 있다.
}

//스플라이트 랜더 종료시 호출
void cSpriteManager::EndSpriteRender()
{
	//스프라이트 랜더 종료
	m_pSprite->End();
}

void cSpriteManager::DrawTexture(
	LPDIRECT3DTEXTURE9 pTex,			//그릴 Texture
	LPRECT pSour,						//그릴 소스 영역
	float posX, float posY,				//그릴 화면 위치
	DWORD color,						//섞이는 색상
	D3DXVECTOR3* pCenter				//이미지의 중심 ( NULL 이면 좌상단 )
	)
{
	//화면 절대 좌표로 그리니 SpriteCanvas 의 Transform 을 초기화
	D3DXMATRIXA16 matFinal;
	D3DXMatrixIdentity(&matFinal);
	m_pSprite->SetTransform(&matFinal);

	//그릴화면위치 ( 주의사항 Vector2 가아니라 Vector3 이다, Z 는 일반적으로 무시되는데 깊이를 주고싶으면 사용해라... )
	D3DXVECTOR3 pos(posX, posY, 0.0f);

	//스플라이트 개페를 통해 Texture 를 화면에 그린다...
	m_pSprite->Draw(
		pTex,		//그릴 Texture
		pSour,		//그릴 이미지의 소스 영역
		(pCenter == NULL) ? &D3DXVECTOR3(0, 0, 0) : pCenter,		//그리는 이미의 중심위치
		&pos,		//그릴화면 위치 
		color		//Hit 컬러
		);
}




void cSpriteManager::DrawTexture(
	LPDIRECT3DTEXTURE9 pTex,			//그릴 Texture
	LPRECT pSour,						//그릴 소스 영역
	float posX, float posY,				//그릴 화면 위치
	float scaleX, float scaleY,			//그릴 스케일 크기
	float rotate,						//그릴 로테이션 값
	DWORD color,						//섞이는 색상
	D3DXVECTOR3* pCenter				//이미지의 중심 ( NULL 이면 좌상단 )
	)
{
	//이미지의 기준점
	D3DXVECTOR3 pivot =
		(pCenter == NULL) ? D3DXVECTOR3(0, 0, 0) : *pCenter;

	//스케일 행렬
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling( &matScale, 
		scaleX, scaleY, 1.0f );

	//회전 행렬
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationZ(&matRot,
		rotate);

	//이동 행렬
	D3DXMATRIXA16 matTrans;
	D3DXMatrixTranslation(&matTrans,
		posX, posY, 0.0f);

	//최종 행렬
	D3DXMATRIXA16 matFinal;
	matFinal = matScale * matRot * matTrans;

	//스케일 적용
	m_pSprite->SetTransform(&matFinal);

	//이미지 그려라
	m_pSprite->Draw(
		pTex,
		pSour,
		&pivot,
		&D3DXVECTOR3(0, 0, 0),	//그릴 위치가 0 이네....
		color);


}

void cSpriteManager::DrawArea(LPDIRECT3DTEXTURE9 pTex,
	LPRECT pSour,						//그릴소스의 영역
	LPRECT pDest,						//그릴화면의 영역
	DWORD color							//섞이는 색상 
	)
{
	float destWidth = pDest->right - pDest->left;
	float destHeight = pDest->bottom - pDest->top;

	float sourWidth = pSour->right - pSour->left;
	float sourHeight = pSour->bottom - pSour->top;


	//영역 비율에 의한 스케일 값
	float scaleX = destWidth / sourWidth;
	float scaleY = destHeight / sourHeight;

	//스케일 행렬
	D3DXMATRIXA16 matScale;
	D3DXMatrixScaling(&matScale,
		scaleX, scaleY, 1.0f);

	//이동행렬
	D3DXMATRIXA16 matTrans;
	D3DXMatrixTranslation(
		&matTrans,
		pDest->left, pDest->top, 0.0f);
	

	D3DXMATRIXA16 matFinal = matScale * matTrans;
	m_pSprite->SetTransform(&matFinal);

	//이미지 그려라
	m_pSprite->Draw(
		pTex,
		pSour,
		&D3DXVECTOR3(0, 0, 0),
		&D3DXVECTOR3(0, 0, 0),	//그릴 위치가 0 이네....
		color);
}

void cSpriteManager::DrawNinePatch(
	LPDIRECT3DTEXTURE9 pTex,
	LPRECT pSour,						//그릴 소스 영역
	LPRECT pDest,						//그릴 화면 영역
	LPRECT pCenter,						//나인패치 센터 영역
	DWORD color,			//섞이는 색상 
	bool bDrawCenter				//중앙 그리는지 안그리는지 여부
	)
{
	
}

void cSpriteManager::DrawLoop(
	LPDIRECT3DTEXTURE9 pTex,
	float offsetX, float offsetY,		//SourOffset 
	LPRECT pSour,						//그릴 소스 영역
	LPRECT pDest,						//그릴 화면 영역
	DWORD color			//섞이는 색상 
	)
{
}

void cSpriteManager::ResetTransform()
{
	D3DXMATRIXA16 matFinal;
	D3DXMatrixIdentity(&matFinal);
	m_pSprite->SetTransform(&matFinal);
}
