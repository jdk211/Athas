#include "stdafx.h"
#include "cSkinnedAnimation.h"


cSkinnedAnimation::cSkinnedAnimation()
: m_fAnimDelta(0.0f)
{
}


cSkinnedAnimation::~cSkinnedAnimation()
{
}

void cSkinnedAnimation::Init(cXMesh_Skinned* pSkinnedMesh)
{
	//사용하는 원본 XMeshSkinned 를 물린다.
	m_pSkinnedMesh = pSkinnedMesh;

	//SKinned Mesh 에 Animation 를 복사한다.
	m_pSkinnedMesh->m_pAnimController->CloneAnimationController(
		m_pSkinnedMesh->m_pAnimController->GetMaxNumAnimationOutputs(),		//최대 Animation 출력정보
		m_pSkinnedMesh->m_pAnimController->GetMaxNumAnimationSets(),		//최대 Animation Set 정보
		m_pSkinnedMesh->m_pAnimController->GetMaxNumTracks(),				//최대 Animation Track 수
		m_pSkinnedMesh->m_pAnimController->GetMaxNumEvents(),				//최대 Event 수
		&m_pAnimController		//복사된 클론 AnimationController
		);

	//Animation 갯수를 얻는다.
	m_AnimNum = m_pAnimController->GetNumAnimationSets();

	for (UINT i = 0; i < m_AnimNum; i++)
	{
		//에니메이션 셋 정보
		LPD3DXANIMATIONSET animSet;

		//i 번째 Animation 셋 정보를 얻는다.
		m_pAnimController->GetAnimationSet(i, &animSet);

		//AnimationSet Vector 에 저장
		this->m_vecAnimSet.push_back(animSet);

		//AnimationSet Map 에 저장
		this->m_mapAnimSet.insert(std::make_pair(
			animSet->GetName(),
			animSet));
	}

	//그냥 시작시 0 번 Animation 이 돌게 하자.
	if (this->m_AnimNum > 0)
		this->Play(0);

}


void cSkinnedAnimation::Release()
{
	//불러온 AnimationSet 해제
	for (int i = 0; i < this->m_vecAnimSet.size(); i++)
		SAFE_RELEASE(m_vecAnimSet[i]);

	//에니메이션 콘트롤러 해제
	SAFE_RELEASE(m_pAnimController);
}


void cSkinnedAnimation::Update(float timeDelta)
{
	//0 번 Track 정보를 얻는다( 매프레임마다 갱신해야 한다 )
	m_pAnimController->GetTrackDesc(0, &m_Track_Desc_0);

	//m_pNowPlayAnimationSet->GetPeriod();		//현제 애니메이션의 전체 길이
	//m_Track_Desc_0.Position;					//Track 0 번에 플레이되고있는 Animation 플레이 Position

	//현재 Animation Play 기간을 0 ~ 1 로 봤을때 진행도....
	//주의 사항 루프시 ( 1 이 넘어감 )
	m_AnimationPlayFactor = m_Track_Desc_0.Position / m_pNowPlayAnimationSet->GetPeriod();


	//Loop 가 안되는 Animation 이 한번 플레이되고 끝나면...
	if (m_AnimationPlayFactor >= 0.99 && this->m_bLoop == false)
	{
		//트랙을 마지막으로 고정
		m_pAnimController->SetTrackPosition(0, m_Track_Desc_0.Priority);

		//돌아갈 Animation 이 있다면..
		if (this->m_pPrevPlayAnimationSet != NULL)
		{

			m_fCrossFadeTime = m_fOutCrossFadeTime;
			m_fLeftCrossFadeTime = m_fOutCrossFadeTime;
			m_bLoop = true;
			this->SetAnimation(m_pPrevPlayAnimationSet);
			this->m_pPrevPlayAnimationSet = NULL;
		}

		//돌아갈 Animation 이 없다면.
		else
		{

			this->m_bPlay = false;
		}

	}




	if (m_bPlay){

		//인스턴싱된 SkinnedAnimation 클레스에서 AdvanceTime 진행시켜버리면
		//하나밖에업는 본이 해당 SkinnedAnimation 인스턴스의 Animation 정보로 
		//업데이트 된다
		//따라서 인스턴싱된 SkinnedAnimation 은 업데이트 량을 기억해놓았다가
		//Rendering 될때 AdvanceTime 각각 진행시킨다.
		m_fAnimDelta = timeDelta;
	}
	else{
		m_fAnimDelta = 0.0f;
	}



	//크로스 페이드가 진행중이라면..
	if (m_fLeftCrossFadeTime > 0.0f)	//남은 크로스페이드 타임이 존재하는가 Play 할때 셋팅이된다.
	{
		//남은 크로스페이드 시간 뺀다
		this->m_fLeftCrossFadeTime -= timeDelta;

		//크로스페이드 가끝났다.
		if (m_fLeftCrossFadeTime <= 0.0f)
		{
			//블렌딩 끝
			m_fLeftCrossFadeTime = 0.0f;
			m_pAnimController->SetTrackWeight(0, 1);		//0 번 Track 가중치 1
			m_pAnimController->SetTrackEnable(1, false);	//1 번 Track 비활성화
		}

		else
		{
			float w1 = (m_fLeftCrossFadeTime / m_fCrossFadeTime);		//1번 Track 가중치
			float w0 = 1.0f - w1;										//0번 Track 가중치

			//가중치 셋팅
			m_pAnimController->SetTrackWeight(0, w0);
			m_pAnimController->SetTrackWeight(1, w1);
		}
	}




}

void cSkinnedAnimation::Render(cTransform* pTrans, cCamera* pCam)
{
	//애니메이션을 진행시킴
	this->m_pAnimController->AdvanceTime(this->m_fAnimDelta, NULL);
	//진해시켰으면 초기화
	this->m_fAnimDelta = 0.0f;

	//각 본에 Transform 대입
	MAP_BONETRANSFORM::iterator iterBone;
	for (iterBone = m_mapBoneTransform.begin(); iterBone != m_mapBoneTransform.end(); ++iterBone)
		iterBone->first->pBoneTransform = iterBone->second;
	for (iterBone = m_mapApplyBoneTransform.begin(); iterBone != m_mapApplyBoneTransform.end(); ++iterBone)
		iterBone->first->pApplyTransform = iterBone->second;


	this->m_pSkinnedMesh->Render(pTrans, pCam);

}

void cSkinnedAnimation::RenderShadow(cTransform* pTrans, cCamera* pLightCam)
{
	//애니메이션을 진행시킴
	this->m_pAnimController->AdvanceTime(this->m_fAnimDelta, NULL);
	//진해시켰으면 초기화
	this->m_fAnimDelta = 0.0f;

	//각 본에 Transform 대입
	MAP_BONETRANSFORM::iterator iterBone;
	for (iterBone = m_mapBoneTransform.begin(); iterBone != m_mapBoneTransform.end(); ++iterBone)
		iterBone->first->pBoneTransform = iterBone->second;
	for (iterBone = m_mapApplyBoneTransform.begin(); iterBone != m_mapApplyBoneTransform.end(); ++iterBone)
		iterBone->first->pApplyTransform = iterBone->second;

	this->m_pSkinnedMesh->RenderShadow(pTrans, pLightCam);

}

//본이름을 출력할때 카메라가 필요하다. ( 카메라 동차위치에 Text 로 뿌릴꺼니깐 )
void cSkinnedAnimation::RenderBoneName(cTransform* pTrans, cCamera* pCam)
{
	//애니메이션을 진행시킴
	this->m_pAnimController->AdvanceTime(this->m_fAnimDelta, NULL);
	//진해시켰으면 초기화
	this->m_fAnimDelta = 0.0f;


	//각 본에 Transform 대입
	MAP_BONETRANSFORM::iterator iterBone;
	for (iterBone = m_mapBoneTransform.begin(); iterBone != m_mapBoneTransform.end(); ++iterBone)
		iterBone->first->pBoneTransform = iterBone->second;
	for (iterBone = m_mapApplyBoneTransform.begin(); iterBone != m_mapApplyBoneTransform.end(); ++iterBone)
		iterBone->first->pApplyTransform = iterBone->second;


	this->m_pSkinnedMesh->RenderBoneName(pTrans, pCam);
}


void cSkinnedAnimation::Play(std::string animName, float crossFadeTime)
{
	m_bPlay = true;
	m_bLoop = true;

	//맵에서 가져와 처리
	MAP_ANIMSET::iterator find = this->m_mapAnimSet.find(animName);
	if (find != this->m_mapAnimSet.end()){

		this->m_pPrevPlayAnimationSet = NULL;

		//크로스 페이드 타임 기억
		m_fCrossFadeTime = crossFadeTime;
		m_fLeftCrossFadeTime = crossFadeTime;

		this->SetAnimation(find->second);
	}
}
void cSkinnedAnimation::Play(int animIndex, float crossFadeTime)
{
	m_bPlay = true;
	m_bLoop = true;

	//벡터에서 가져와 처리
	if (animIndex < this->m_AnimNum){

		this->m_pPrevPlayAnimationSet = NULL;

		//크로스 페이드 타임 기억
		m_fCrossFadeTime = crossFadeTime;
		m_fLeftCrossFadeTime = crossFadeTime;


		this->SetAnimation(m_vecAnimSet[animIndex]);
	}
}


void cSkinnedAnimation::PlayOneShot(std::string animName, float inCrossFadeTime, float outCrossFadeTime)
{

	m_bPlay = true;
	m_bLoop = false;
	
	MAP_ANIMSET::iterator find = this->m_mapAnimSet.find(animName);
	if (find != this->m_mapAnimSet.end()){

		if (this->m_pPrevPlayAnimationSet == NULL)
		{
			//현재 Animaiton 을 기억한다.
			this->m_pPrevPlayAnimationSet = this->m_pNowPlayAnimationSet;
		}


		//크로스 페이드 타임 기억
		m_fCrossFadeTime = inCrossFadeTime;
		m_fLeftCrossFadeTime = inCrossFadeTime;

		//나갈때 크로스페이드 타입 기억
		m_fOutCrossFadeTime = outCrossFadeTime;

		this->SetAnimation(find->second);
	}
}

void cSkinnedAnimation::PlayOneShotAfterHold(std::string animName, float crossFadeTime)
{
	m_bPlay = true;
	m_bLoop = false;

	MAP_ANIMSET::iterator find = this->m_mapAnimSet.find(animName);
	if (find != this->m_mapAnimSet.end()){

		//돌아갈 Animation 은 존재하지 않는다.
		this->m_pPrevPlayAnimationSet = NULL;

		//크로스 페이드 타임 기억
		m_fCrossFadeTime = crossFadeTime;
		m_fLeftCrossFadeTime = crossFadeTime;

		this->SetAnimation(find->second);
	}
}

//Animation 정보를 출력한다.
//화면위치에 Animation 이름을 출력한다.
void cSkinnedAnimation::ShowAnimationName(int x, int y)
{
	int i = 0;
	char szTemp[256];
	for (i = 0; i < m_AnimNum; i++)
	{
		sprintf_s(szTemp, "%d. %s", i, this->m_vecAnimSet[i]->GetName());

		DXFONT_MGR->PrintText(
			szTemp, x, y + i * 20, 0xffffffff);
	}
}



//애니메이션 속도조정
void cSkinnedAnimation::SetPlaySpeed(float speed)
{
	//0 번트랙 속도셋팅
	this->m_pAnimController->SetTrackSpeed(0, speed);
}

//본이름으로 본찾는다.
BONE* cSkinnedAnimation::GetFineBONE(std::string boneName)
{
	return this->m_pSkinnedMesh->GetFineBONE(boneName);
}



//BoneTransform 추가
void cSkinnedAnimation::AddBoneTransform(std::string boneName, cTransform* pTransform)
{
	//해당 이름으로 본을 찾는다.
	BONE* pBone = this->m_pSkinnedMesh->GetFineBONE(boneName);
	if (pBone != NULL){
		this->m_mapBoneTransform[pBone] = pTransform;
	}

}

//ApplyTransform 추가
void cSkinnedAnimation::AddApplyTransform(std::string boneName, cTransform* pTransform)
{
	//해당 이름으로 본을 찾는다.
	BONE* pBone = this->m_pSkinnedMesh->GetFineBONE(boneName);
	if (pBone != NULL){
		this->m_mapApplyBoneTransform[pBone] = pTransform;
	}
}

//BoneTransform 제거
void cSkinnedAnimation::RemoveBoneTransform(std::string boneName)
{
	BONE* pBone = this->m_pSkinnedMesh->GetFineBONE(boneName);
	MAP_BONETRANSFORM::iterator boneIter = m_mapBoneTransform.find(pBone);
	if (boneIter != m_mapBoneTransform.end())
	{
		m_mapBoneTransform.erase(boneIter);
	}
}

//ApplyTransform 제거
void cSkinnedAnimation::RemoveApplyTransform(std::string boneName)
{
	BONE* pBone = this->m_pSkinnedMesh->GetFineBONE(boneName);
	MAP_BONETRANSFORM::iterator boneIter = m_mapApplyBoneTransform.find(pBone);
	if (boneIter != m_mapApplyBoneTransform.end())
	{
		m_mapApplyBoneTransform.erase(boneIter);
	}
}


/////////////////////////////////////////////////////////////////////////////////////

//Animation 을 바꾼다.
void cSkinnedAnimation::SetAnimation(LPD3DXANIMATIONSET animSet)
{
	//이미 플레이되고 있는 Animation 이라면 실행이 되지 않는다. ( 같은 Animation 중복되지 않게 막았다 )
	if (this->m_pNowPlayAnimationSet != NULL &&
		animSet == this->m_pNowPlayAnimationSet)
		return;

	//크로스 페이드가 존재한다면..
	if (this->m_fCrossFadeTime > 0.0f)
	{

		//
		// 현재의 Animation 을 1 번 track 으로 전달....
		//

		//현제 Animation 을 1 번Track 으로 셋팅
		m_pAnimController->SetTrackAnimationSet(
			1, m_pNowPlayAnimationSet);

		//1번 트랙의 위치를 이전에 플레이 되던 위치로 셋팅
		m_pAnimController->SetTrackPosition(
			1,
			m_Track_Desc_0.Position);

		//1 번 Track 활성화
		m_pAnimController->SetTrackEnable(1, true);

		//1 번 Track 가중치
		m_pAnimController->SetTrackWeight(1, 1.0f);

		//1 번 Track 에 기존 0 번의 Animation 속도도 전달.
		m_pAnimController->SetTrackSpeed(1,
			m_Track_Desc_0.Speed);


		//
		// 지금셋팅되는 Animation 을 0 번Track 으로 셋팅
		//

		//새로운 Animation 0 번으로 셋팅
		m_pAnimController->SetTrackAnimationSet(0, animSet);
		//0번 Track Animation 위치 0.0 타임에서 시작 ( 처음부터 시작하겠다는예기 )
		m_pAnimController->SetTrackPosition(0, 0.0f);
		//가중치는 0 으로 //가중치는 0 으로 
		m_pAnimController->SetTrackWeight(0, 0.0f);

		//현재 플레이 되고 있는 AnimationSet 갱신
		this->m_pNowPlayAnimationSet = animSet;
	}

	//크로스 페이드가 없다면...
	else
	{

		//Track 의 포지션을 맨압으로 돌린다.
		this->m_pAnimController->SetTrackPosition(
			0,			//Track 번호 
			0.0			//PlayPosition
			);

		//매개변수로 받은 AnimationSet 으로 Animation 0 번 Track에 플레이
		this->m_pAnimController->SetTrackAnimationSet(0, animSet);

		//현재 플레이 되고 있는 AnimationSet 갱신
		this->m_pNowPlayAnimationSet = animSet;

	}



}

