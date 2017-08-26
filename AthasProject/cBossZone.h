#pragma once
#include "cMonsterZone.h"
class cBossZone : public cMonsterZone
{
protected:
	float atkCount;
	cArthas*			m_pUser;
	bool				isUse;
	bool				isCollision;
	D3DXVECTOR3 pos;
	vector<cTest*> m_vMonster;
public:
	cBossZone();
	~cBossZone();


	virtual HRESULT	Init(cTerrain* map, D3DXVECTOR3 v_pos);
	virtual void Release() override;
	virtual void Update(float timeDelta, cArthas * user, cCamera * pCam) override;
	virtual void Render(cTransform* pTrans, cCamera* pCam) override;

	virtual void SetTransform(cTransform* trans)
	{
		m_pTransForm = *trans;
	}

	void CreateMonster(int level) override;
	void ZenMonster(cArthas* user) override;
	void MonsterOBB() override;

	vector<cTest*> GetVecMonster() { return m_vMonster; }

	void eraseMonster();

	void SetMonsterPosition();
};

