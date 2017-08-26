#pragma once

#include "BoneHierachy.h"		//본하이어 라키


class cXMesh_Skinned
{
	friend class BONE_HIERARCHY;							//본하이어 라키는 내친구....
	friend class cSkinnedAnimation;

private:
	static LPD3DXEFFECT pMeshEffect;


private:
	LPD3DXFRAME					m_pRootbone;		//Skinned 메쉬의 루트 본
	D3DXMATRIXA16				m_matCorrection;	//보정행렬

	DWORD						m_dwWorkingPaletteSize;		//행렬 작업 팔래트수
	D3DXMATRIXA16*				m_pmWorkingPalette;			//행렬 작업 팔래트 

	//Animation 관련
	///////////////////////////////////////////////////////////////////////////////// 

	typedef std::map< std::string, LPD3DXANIMATIONSET >			MAP_ANIMSET;
	typedef std::vector< LPD3DXANIMATIONSET >					VEC_ANIMSET;


	LPD3DXANIMATIONCONTROLLER		m_pAnimController;			//Animation 컨트롤러
	UINT							m_AnimNum;					//Animation 갯수
	MAP_ANIMSET						m_mapAnimSet;				//Map 으로 되어있는 AnimationSet
	VEC_ANIMSET						m_vecAnimSet;				//Vector 로 되어있는 AnimationSet
	
	LPD3DXANIMATIONSET				m_pNowPlayAnimationSet;		//현제 플레이되고있는 AnimationSet
	




	D3DXTRACK_DESC					m_Track_Desc_0;				//0번 Track_Desc ( 0 번 Track 의 정보 )

	bool							m_bPlay;					//지금 플레이 중이니?
	bool							m_bLoop;					//루프 플레이니?


	float							m_fCrossFadeTime;			//크로스페이드 타임
	float							m_fLeftCrossFadeTime;		//남은크로스페이드 타임


	LPD3DXANIMATIONSET				m_pPrevPlayAnimationSet;	//OneShot 플레이시 한번 Animation 플레이되고 다시 되돌아갈 Animaiton
	float							m_fOutCrossFadeTime;		//OneShot Animation 끝나고 되돌아갈때 CrossFade 타임
	double							m_AnimationPlayFactor;		//Animation 진행 Factor ( 0 이면 처음 1 일수록 마지막 )





public:
	cXMesh_Skinned();
	~cXMesh_Skinned();

	HRESULT Init(std::string filePath, D3DXMATRIXA16* matCorrection = NULL);
	void Release();

	void Update(float timeDelta);


	void Render(cTransform* pTrans, cCamera* pCam);
	void RenderShadow(cTransform* pTrans, cCamera* pLightCam);
	void RenderBoneName(cTransform* pTrans, cCamera* pCam);		//본이름을 출력할때 카메라가 필요하다. ( 카메라 동차위치에 Text 로 뿌릴꺼니깐 )

	//Animation 정보를 출력한다.
	void ShowAnimationName(int x, int y);		//화면위치에 Animation 이름을 출력한다.



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


	//애니메이션 속도조정
	void SetPlaySpeed(float speed);
		
	//해당 인덱스의 Animation Set 을 얻는다.
	LPD3DXANIMATIONSET GetAnimSet(int index)
	{
		return m_vecAnimSet[index];
	}

	//본이름으로 본찾는다.
	BONE* GetFineBONE(std::string boneName);
	



private:


	//본 행렬을 셋팅한다. ( 초기화 )
	void InitBoneMatrixPointer(BONE* pBone);

	//본행렬 을 업데이트한다. ( 랜더시 Animation 정보대로 본행렬 업데이트를 진행시킨다 )
	void UpdateMatrices(BONE* pBone, D3DXMATRIXA16* pParentMatrix);

	//본을 그린다.
	void RenderBone(BONE* pBone);

	//본이름을 출력한다.
	void RenderChildBoneName(cCamera* pCam, BONE* pBone, D3DXVECTOR3* parentPos);


	//Animation 을 바꾼다.
	void SetAnimation(LPD3DXANIMATIONSET animSet);


public:
	static void SetCamera(cCamera* pCamera);
	static void SetEnvironment(LPDIRECT3DCUBETEXTURE9 pCubeTex);
	static void SetLight(cTransform* lightTrans, D3DXCOLOR lightColor, float ambient);


};

