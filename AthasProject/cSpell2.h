#pragma once
#include "cParticleEmitter.h"
#include  "cBoundBox.h"

class cTest;
class cArthas;

class cSpell2
{
private:
	cParticleEmitter*	pParticleEmitter;
	cBoundBox*			m_BoundBox0;
	cTransform			m_BoundTrans0;
	cTest*				m_pMonster;
	bool				isFront;
	float				m_SpellCount;
	bool				isAttack;
	float				m_attackCount;
public:
	cSpell2();
	~cSpell2();

	HRESULT init();
	void Release();
	void Update(float timeDelta, cCamera* camera, vector<cTest*> monster, cArthas* user, vector<cTest*> monster2);
	void Render();
	void Scene_ParticleRender();


	void MoveRay(vector<cTest*> monster, cCamera* camera, cArthas* user);
};

