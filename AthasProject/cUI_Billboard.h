#pragma once
class cUI_Billboard
{
private:
	string m_fileName;
	D3DXVECTOR3 m_WolrdPositon;
	std::vector<ST_PCT_VERTEX>	m_vecVertex1;
	float fW_Max;
	float fH_Max;

	float a = 4.0f;
public:
	cUI_Billboard();
	~cUI_Billboard();

	HRESULT Init(string fileName, float wMax = 0.0f, float hMax = 0.0f);
	void Release();
	void Update(D3DXVECTOR3 pos);
	void Render();

	void SetBillboard(D3DXMATRIXA16 * matWorld);
	void DrawBillboard();
	void ProgressBar(int currnetHp, int maxHp);
};

