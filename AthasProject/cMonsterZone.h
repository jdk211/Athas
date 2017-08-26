#pragma once
#include "cUser.h"
#include "cTest.h"

class cArthas;
class cTerrain;

class cMonsterZone : public cUser
{
protected:
	float atkCount;
	cArthas*			m_pUser;
	bool				isUse;
	bool				isCollision;
	D3DXVECTOR3 pos;
	vector<cTest*> m_vMonster;
public:
	cMonsterZone();
	~cMonsterZone();

	virtual HRESULT	Init(cTerrain* map, D3DXVECTOR3 v_pos);
	virtual void Release() override;
	virtual void Update(float timeDelta, cArthas * user, cCamera * pCam);
	virtual void Render(cTransform* pTrans, cCamera* pCam);

	virtual void SetTransform(cTransform* trans)
	{
		m_pTransForm = *trans;
	}

	virtual void CreateMonster(int level);
	virtual void ZenMonster(cArthas* user);
	virtual void MonsterOBB();

	vector<cTest*> GetVecMonster() { return m_vMonster; }

	void eraseMonster();


	void SetMonsterPosition();
};

