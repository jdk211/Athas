#pragma once



class cMainGame
{
private:	


public:
	cMainGame(void);
	~cMainGame(void);

	//셋팅
	HRESULT Init(void);

	//해제
	void Release();

	//업데이트
	void Update();

	//드로우
	void Draw();
	
public:

	//프로시져
	LRESULT MainProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam );

};
