#pragma once

#include "cXMesh_Skinned.h"
#include "cXMesh_Static.h"
#include "cSkinnedAnimation.h"
#include "cTransform.h"
#include "cTerrain.h"
#include "cUIButton.h"
struct stStatus
{
	int level;
	int str;
	int wis;
	int con;
	int hp;
	int maxHp;
	int maxMp;
	int mp;
	int pDam;
	int mDam;
};

enum eState
{
	STAND,
	WALK,
	DEATH,
	RUN
};

enum eStateAttack
{
	NON,
	ATTACK,
	ATTACK2
};

class cUser
{

protected:
	stStatus			m_UserStatus;
	cXMesh_Skinned*		m_pSkinnedMesh;
	cXMesh_Static*		m_pSkinnedStatic;
	cSkinnedAnimation*	m_pSkinnedAnim;
	cTransform			m_pTransForm;
	eStateAttack		m_UserStateAttack;

	eState				m_UserState;
	cTransform		    m_ItemTrans;

private:

public:
	cUser();
	~cUser();

	virtual HRESULT	Init();
	virtual void Release();
	virtual void Update(float timeDelta);
	virtual void Render(cCamera* pCam);
	virtual void AniControl();
	virtual D3DXVECTOR3 SetHeightPos(cTerrain* terrain);

	virtual cTransform*		GetTransform() { return &m_pTransForm; }
	virtual cTransform*		GetItemTransform() { return &m_ItemTrans; }
	virtual eStateAttack	GetStateAttack() { return m_UserStateAttack; }

	virtual cXMesh_Static* GetSkinnedStatic() { return m_pSkinnedStatic; }
	virtual cSkinnedAnimation* GetSkinnedMeshAnimation() { return m_pSkinnedAnim; }

	virtual stStatus GetStatus() { return m_UserStatus; }
	virtual int		 GetStr() { return m_UserStatus.str; }
	virtual int		 GetCon() { return m_UserStatus.con; }
	virtual int		 GetWis() { return m_UserStatus.wis; }
	virtual void SetHp(int hp) { m_UserStatus.hp = hp; }
	virtual void SetMp(int mp) { m_UserStatus.mp = mp; }
	virtual void SetStr(int Str) { m_UserStatus.str = Str; }
	virtual void SetWis(int Wis) { m_UserStatus.wis = Wis; }
	virtual void SetCon(int Con) { m_UserStatus.con = Con; }
	virtual void SetTransform(cTransform* trans)
	{
		m_pTransForm = *trans;
	}
	
};

