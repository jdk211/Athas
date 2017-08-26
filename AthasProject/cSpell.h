#pragma once
#include "cParticleEmitter.h"
#include  "cBoundBox.h"

class cTest;
class cArthas;

class cSpell
{
private:
	cParticleEmitter*	pParticleEmitter;
	cBoundBox*			m_BoundBox0;
	cTransform			m_BoundTrans0;
	cTest*				m_pMonster;
	bool				isFront;
	float				m_SpellCount;
public:
	cSpell();
	~cSpell();

	HRESULT init();
	void Release();
	void Update(float timeDelta, cCamera* camera, vector<cTest*> monster, cArthas* user, vector<cTest*> monster2);
	void Render();
	void Scene_ParticleRender();


	void MoveRay(vector<cTest*> monster, cCamera* camera, cArthas* user);
};

