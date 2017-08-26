#pragma once
#include "cUser.h"

#include "cBoundBox.h"
#include "cBoundSphere.h"
#include "cUserInfoUI.h"
#include "cInventory.h"

class Shop;
class cTest;
class cUserInfoUI;

class cArthas : public cUser
{
private:
	float atkCount;
	cBoundBox*			m_BoundBox0;
	cBoundBox*			m_BoundBox1;
	cTransform			m_BoundTrans0;
	cTransform			m_BoundTrans1;
	cBoundSphere		m_SphereBox;
	cUserInfoUI*		m_pUI;

	Shop*		m_Shop;
	float m_fDistans;
	float m_fDegree;
	cTransform* RayTrans;
	Ray 	   pRay;
	//cTransform		m_SwordTrans;

	cInventory*			m_inven;
	bool m_bfootwWark;
	
	bool countAttack;

public:
	cArthas();
	~cArthas();
	virtual HRESULT	Init() override;
	virtual void Release() override;
	virtual void Update(float timeDelta) override;
	void Render(cTransform* pTrans,cCamera* pCam);
	virtual void AniControl() override;

	virtual cTransform* GetTransform() { return &m_pTransForm; }
	virtual eStateAttack	GetStateAttack() { return m_UserStateAttack; }

	cBoundBox*	 GetBoundBox0() { return m_BoundBox0; }
	cBoundBox*	 GetBoundBox1() { return m_BoundBox1; }
	cTransform*	 GetBoundTrans0() { return &m_BoundTrans0; }
	cTransform*	 GetBoundTrans1() { return &m_BoundTrans1; }
	cBoundSphere GetSphereBox() { return m_SphereBox; }

	void MoveRay(cTerrain* terrain, cCamera* camera);
	//20170223 Ãß°¡
	void MoveRay(cTest* mon, cCamera* camera);
	virtual cXMesh_Static* GetSkinnedStatic() { return m_pSkinnedStatic; }

	virtual stStatus GetStatus() { return m_UserStatus; }
	virtual void SetHp(int hp) { m_UserStatus.hp = hp; }
	virtual void SetMp(int mp) { m_UserStatus.mp = mp; }

	void hitMonster(cTest* monster); 
	void hitMonster(vector<cTest*> monster);
	virtual void SetStr(int Str) { m_UserStatus.str = Str; }
	virtual void SetCon(int Con) { m_UserStatus.con = Con; }
	virtual void SetWis(int wis) { m_UserStatus.wis = wis; }
	void SetUI(cUserInfoUI* UI) { m_pUI = UI; }

	cInventory* getinven() { return m_inven; }
};

