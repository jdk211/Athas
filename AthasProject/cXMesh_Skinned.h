#pragma once

#include "BoneHierachy.h"		//�����̾� ��Ű


class cXMesh_Skinned
{
	friend class BONE_HIERARCHY;							//�����̾� ��Ű�� ��ģ��....
	friend class cSkinnedAnimation;

private:
	static LPD3DXEFFECT pMeshEffect;


private:
	LPD3DXFRAME					m_pRootbone;		//Skinned �޽��� ��Ʈ ��
	D3DXMATRIXA16				m_matCorrection;	//�������

	DWORD						m_dwWorkingPaletteSize;		//��� �۾� �ȷ�Ʈ��
	D3DXMATRIXA16*				m_pmWorkingPalette;			//��� �۾� �ȷ�Ʈ 

	//Animation ����
	///////////////////////////////////////////////////////////////////////////////// 

	typedef std::map< std::string, LPD3DXANIMATIONSET >			MAP_ANIMSET;
	typedef std::vector< LPD3DXANIMATIONSET >					VEC_ANIMSET;


	LPD3DXANIMATIONCONTROLLER		m_pAnimController;			//Animation ��Ʈ�ѷ�
	UINT							m_AnimNum;					//Animation ����
	MAP_ANIMSET						m_mapAnimSet;				//Map ���� �Ǿ��ִ� AnimationSet
	VEC_ANIMSET						m_vecAnimSet;				//Vector �� �Ǿ��ִ� AnimationSet
	
	LPD3DXANIMATIONSET				m_pNowPlayAnimationSet;		//���� �÷��̵ǰ��ִ� AnimationSet
	




	D3DXTRACK_DESC					m_Track_Desc_0;				//0�� Track_Desc ( 0 �� Track �� ���� )

	bool							m_bPlay;					//���� �÷��� ���̴�?
	bool							m_bLoop;					//���� �÷��̴�?


	float							m_fCrossFadeTime;			//ũ�ν����̵� Ÿ��
	float							m_fLeftCrossFadeTime;		//����ũ�ν����̵� Ÿ��


	LPD3DXANIMATIONSET				m_pPrevPlayAnimationSet;	//OneShot �÷��̽� �ѹ� Animation �÷��̵ǰ� �ٽ� �ǵ��ư� Animaiton
	float							m_fOutCrossFadeTime;		//OneShot Animation ������ �ǵ��ư��� CrossFade Ÿ��
	double							m_AnimationPlayFactor;		//Animation ���� Factor ( 0 �̸� ó�� 1 �ϼ��� ������ )





public:
	cXMesh_Skinned();
	~cXMesh_Skinned();

	HRESULT Init(std::string filePath, D3DXMATRIXA16* matCorrection = NULL);
	void Release();

	void Update(float timeDelta);


	void Render(cTransform* pTrans, cCamera* pCam);
	void RenderShadow(cTransform* pTrans, cCamera* pLightCam);
	void RenderBoneName(cTransform* pTrans, cCamera* pCam);		//���̸��� ����Ҷ� ī�޶� �ʿ��ϴ�. ( ī�޶� ������ġ�� Text �� �Ѹ����ϱ� )

	//Animation ������ ����Ѵ�.
	void ShowAnimationName(int x, int y);		//ȭ����ġ�� Animation �̸��� ����Ѵ�.



	void Play(std::string animName, float crossFadeTime = 0.0);
	void Play(int animIndex, float crossFadeTime = 0.0);

	void PlayOneShot(std::string animName, float inCrossFadeTime = 0.0, float outCrossFadeTime = 0.0f);
	void PlayOneShotAfterHold(std::string animName, float crossFadeTime = 0.0);

	
	void Pause(){
		this->m_bPlay = false;
	}

	void Resume(){
		this->m_bPlay = true;
	}

	bool IsPlay(){
		return this->m_bPlay;
	}


	//�ִϸ��̼� �ӵ�����
	void SetPlaySpeed(float speed);
		
	//�ش� �ε����� Animation Set �� ��´�.
	LPD3DXANIMATIONSET GetAnimSet(int index)
	{
		return m_vecAnimSet[index];
	}

	//���̸����� ��ã�´�.
	BONE* GetFineBONE(std::string boneName);
	



private:


	//�� ����� �����Ѵ�. ( �ʱ�ȭ )
	void InitBoneMatrixPointer(BONE* pBone);

	//����� �� ������Ʈ�Ѵ�. ( ������ Animation ������� ����� ������Ʈ�� �����Ų�� )
	void UpdateMatrices(BONE* pBone, D3DXMATRIXA16* pParentMatrix);

	//���� �׸���.
	void RenderBone(BONE* pBone);

	//���̸��� ����Ѵ�.
	void RenderChildBoneName(cCamera* pCam, BONE* pBone, D3DXVECTOR3* parentPos);


	//Animation �� �ٲ۴�.
	void SetAnimation(LPD3DXANIMATIONSET animSet);


public:
	static void SetCamera(cCamera* pCamera);
	static void SetEnvironment(LPDIRECT3DCUBETEXTURE9 pCubeTex);
	static void SetLight(cTransform* lightTrans, D3DXCOLOR lightColor, float ambient);


};

