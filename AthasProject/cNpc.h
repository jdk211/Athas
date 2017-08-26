#pragma once

#include "cXMesh_Skinned.h"
#include "cXMesh_Static.h"
#include "cSkinnedAnimation.h"
#include "cTransform.h"
#include "cTerrain.h"

class cNpc
{
protected:
	cXMesh_Skinned*		m_pSkinnedMesh;
	cXMesh_Static*		m_pSkinnedStatic;
	cSkinnedAnimation*	m_pSkinnedAnim;
	cTransform			m_pTransForm;

public:
	cNpc();
	~cNpc();

	virtual HRESULT	Init();
	virtual void Release();
	virtual void Update(float timeDelta);
	virtual void Render(cCamera* pCam);
	virtual D3DXVECTOR3 SetHeightPos(cTerrain* terrain);

	virtual cTransform*		GetTransform() { return &m_pTransForm; }

	virtual cXMesh_Static* GetSkinnedStatic() { return m_pSkinnedStatic; }
	virtual cSkinnedAnimation* GetSkinnedMeshAnimation() { return m_pSkinnedAnim; }

	virtual void SetTransform(cTransform* trans)
	{
		m_pTransForm = *trans;
	}
};

