#pragma once


#include "cScene.h"
#include "cPlayer.h"
#include "cInventory.h"
#include "Shop.h"
#include "cSpell.h"
#include "cSpell2.h"
class cLight_Directional;

class cSkinnedAnimation;
class cLight_Point;
class cArthas;
class cMerchant;
class cNightElf;
class cTest;
class cParticleEmitter;
class cTerrain;
class cMonsterZone;
class cBossZone;
//:>>

#define LIGHT_NUM 1000

enum stState
{
	stand,
	run
};

class cScene_Town : public cScene
{
private:

	cArthas* m_pUser;
	cMerchant* m_pMerchant;
	cNightElf* m_pNightElf;
	cMonsterZone* m_pM_Zone;
	cBossZone*	m_pB_Zone;
	vector<cTest*> m_vecMonster;
	cTest*	 m_pMonster;

	cTransform trans;
	cTransform lightTrans;

	cSkinnedAnimation*  pSkinned;
	cXMesh_Static*	    pShip;
	stState			    m_stState;
	cTransform		    applyTrans;
	cLight_Point*	    pPointLights[LIGHT_NUM];
	cLight_Directional*	pDirectionalLight;
	cParticleEmitter*	pParticleEmitter;
	LPD3DXFONT			m_pFont;

	//아이템

	cSpell*				m_pSpell;
	cSpell2*			m_pSpell2;
	//하우스 오브젝트
	cXMesh_Static*		m_pHouse;
	float	angle;
	bool shopopen;
	bool inventoryopen;
	bool Playerinfoopen;
	bool hit;
	float posY, posZ;
	cInventory* m_inven;
	Player* m_Player;
	Shop* m_Shop;

public:
	typedef struct ST_BUILD
	{
		cXMesh_Static*		skin;
		char*				skinPath;
		cTransform			trans;
		D3DXVECTOR3			position;
		cBoundBox*			box;
	}STBUILDE, *LPSTBUILD;

private:
	cTerrain*				pTerrain;
	cLight_Directional*		m_pDirectionalLight;
	vector<LPSTBUILD>		m_vecSetBuild;

public:
	cScene_Town();
	~cScene_Town();

	virtual HRESULT Scene_Init();
	virtual void Scene_Release();
	virtual void Scene_Update(float timeDelta);
	virtual void Scene_Render();

	virtual void Scene_ShadowRender() override;

	virtual void Scene_LightRender() override;

	virtual void Scene_ParticleRender() override;

	virtual void OnClick(cUIButton* pSender) override;

	void CreateBuild();

};

                         