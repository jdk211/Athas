#pragma once

class cTrailRenderer 
{
private:


public:
	cTransform		Transform;



public:
	cTrailRenderer();
	~cTrailRenderer();
	
	void Init(float trailLiveTime, float width, LPDIRECT3DTEXTURE9 ptex , D3DXCOLOR color);
	void Release();
	void Update(float timeDelta);
	void Render();

	void TrailGenStart();
	void TrailGenStop();
};

