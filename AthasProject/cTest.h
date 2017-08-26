#pragma once
#include "cUser.h"

#include "cBoundBox.h"
#include "cUITextView.h"
#include "cUIImageView.h"
#include "cUIObject.h"
#include "cUI_Billboard.h"

class cArthas;

class cTest : public cUser
{
private:
	float atkCount;
	float deathCount;
	cBoundBox*			m_BoundBox0;
	cBoundBox*			m_BoundBox1;
	cTransform			m_BoundTrans0;
	cTransform			m_BoundTrans1;
	cArthas*			m_pUser;
	bool				countAttack;
	bool				isDeath;

	cUI_Billboard*       HPPROGRESS;
	string				m_fName;
	string				m_bName;
public:
	cTest();
	~cTest();
	virtual HRESULT	Init();
	virtual HRESULT	Init(string fName, D3DXVECTOR3 uScale, D3DXVECTOR3 sScale, string sFName = NULL, string bName = NULL);
	virtual void Release() override;
	virtual void Update(float timeDelta) override;
	void Render(cTransform* pTrans, cCamera* pCam);
	virtual void AniControl() override;

	virtual cTransform* GetTransform() { return &m_pTransForm; }
	virtual eStateAttack	GetStateAttack() { return m_UserStateAttack; }
	virtual void SetTransform(cTransform* trans)
	{
		m_pTransForm = *trans;
	}
	cBoundBox*  GetBoundBox0() { return m_BoundBox0; }
	cBoundBox*  GetBoundBox1() { return m_BoundBox1; }
	cTransform* GetBoundTrans0() { return &m_BoundTrans0; }
	cTransform* GetBoundTrans1() { return &m_BoundTrans1; }

	void SetDam(int dam)
	{
		m_UserStatus.pDam = dam;
	}

	void SetHp(int hp)
	{
		m_UserStatus.hp = hp;
		m_UserStatus.maxHp = hp;
	}

	void SetCurrentHp(int hp)
	{
		m_UserStatus.hp = hp;
	}

	virtual stStatus GetStatus() { return m_UserStatus; }
	void MoveRay(cArthas* user, cCamera* camera);
	float GetDeathCount() { return deathCount; }
	void MonsterDeath();
	
};

