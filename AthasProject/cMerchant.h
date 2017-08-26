#pragma once
#include "cNpc.h"

#include "cBoundBox.h"
#include "cUserInfoUI.h"

class cParticleEmitter;
class cArthas;

class cMerchant : public cNpc
{
private:
	cBoundBox*			m_BoundBox0;
	cTransform			m_BoundTrans0;
	cParticleEmitter*	pParticleEmitter;
	D3DXMATRIXA16 mat;
	float distence;
public:
	cMerchant();
	~cMerchant();

	virtual HRESULT	Init(float x, float y, float z);
	virtual void Release() override;
	virtual void Update(float timeDelta, cArthas* user);
	void Render(cTransform* pTrans, cCamera* pCam);

	virtual cTransform* GetTransform() { return &m_pTransForm; }

	cBoundBox*	 GetBoundBox0() { return m_BoundBox0; }
	cTransform*	 GetBoundTrans0() { return &m_BoundTrans0; }

	virtual cXMesh_Static* GetSkinnedStatic() { return m_pSkinnedStatic; }

	void particleRender(cArthas* user);


};

