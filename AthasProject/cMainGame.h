#pragma once



class cMainGame
{
private:	


public:
	cMainGame(void);
	~cMainGame(void);

	//����
	HRESULT Init(void);

	//����
	void Release();

	//������Ʈ
	void Update();

	//��ο�
	void Draw();
	
public:

	//���ν���
	LRESULT MainProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam );

};
