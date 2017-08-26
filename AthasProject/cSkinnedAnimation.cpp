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
	//����ϴ� ���� XMeshSkinned �� ������.
	m_pSkinnedMesh = pSkinnedMesh;

	//SKinned Mesh �� Animation �� �����Ѵ�.
	m_pSkinnedMesh->m_pAnimController->CloneAnimationController(
		m_pSkinnedMesh->m_pAnimController->GetMaxNumAnimationOutputs(),		//�ִ� Animation �������
		m_pSkinnedMesh->m_pAnimController->GetMaxNumAnimationSets(),		//�ִ� Animation Set ����
		m_pSkinnedMesh->m_pAnimController->GetMaxNumTracks(),				//�ִ� Animation Track ��
		m_pSkinnedMesh->m_pAnimController->GetMaxNumEvents(),				//�ִ� Event ��
		&m_pAnimController		//����� Ŭ�� AnimationController
		);

	//Animation ������ ��´�.
	m_AnimNum = m_pAnimController->GetNumAnimationSets();

	for (UINT i = 0; i < m_AnimNum; i++)
	{
		//���ϸ��̼� �� ����
		LPD3DXANIMATIONSET animSet;

		//i ��° Animation �� ������ ��´�.
		m_pAnimController->GetAnimationSet(i, &animSet);

		//AnimationSet Vector �� ����
		this->m_vecAnimSet.push_back(animSet);

		//AnimationSet Map �� ����
		this->m_mapAnimSet.insert(std::make_pair(
			animSet->GetName(),
			animSet));
	}

	//�׳� ���۽� 0 �� Animation �� ���� ����.
	if (this->m_AnimNum > 0)
		this->Play(0);

}


void cSkinnedAnimation::Release()
{
	//�ҷ��� AnimationSet ����
	for (int i = 0; i < this->m_vecAnimSet.size(); i++)
		SAFE_RELEASE(m_vecAnimSet[i]);

	//���ϸ��̼� ��Ʈ�ѷ� ����
	SAFE_RELEASE(m_pAnimController);
}


void cSkinnedAnimation::Update(float timeDelta)
{
	//0 �� Track ������ ��´�( �������Ӹ��� �����ؾ� �Ѵ� )
	m_pAnimController->GetTrackDesc(0, &m_Track_Desc_0);

	//m_pNowPlayAnimationSet->GetPeriod();		//���� �ִϸ��̼��� ��ü ����
	//m_Track_Desc_0.Position;					//Track 0 ���� �÷��̵ǰ��ִ� Animation �÷��� Position

	//���� Animation Play �Ⱓ�� 0 ~ 1 �� ������ ���൵....
	//���� ���� ������ ( 1 �� �Ѿ )
	m_AnimationPlayFactor = m_Track_Desc_0.Position / m_pNowPlayAnimationSet->GetPeriod();


	//Loop �� �ȵǴ� Animation �� �ѹ� �÷��̵ǰ� ������...
	if (m_AnimationPlayFactor >= 0.99 && this->m_bLoop == false)
	{
		//Ʈ���� ���������� ����
		m_pAnimController->SetTrackPosition(0, m_Track_Desc_0.Priority);

		//���ư� Animation �� �ִٸ�..
		if (this->m_pPrevPlayAnimationSet != NULL)
		{

			m_fCrossFadeTime = m_fOutCrossFadeTime;
			m_fLeftCrossFadeTime = m_fOutCrossFadeTime;
			m_bLoop = true;
			this->SetAnimation(m_pPrevPlayAnimationSet);
			this->m_pPrevPlayAnimationSet = NULL;
		}

		//���ư� Animation �� ���ٸ�.
		else
		{

			this->m_bPlay = false;
		}

	}




	if (m_bPlay){

		//�ν��Ͻ̵� SkinnedAnimation Ŭ�������� AdvanceTime ������ѹ�����
		//�ϳ��ۿ����� ���� �ش� SkinnedAnimation �ν��Ͻ��� Animation ������ 
		//������Ʈ �ȴ�
		//���� �ν��Ͻ̵� SkinnedAnimation �� ������Ʈ ���� ����س��Ҵٰ�
		//Rendering �ɶ� AdvanceTime ���� �����Ų��.
		m_fAnimDelta = timeDelta;
	}
	else{
		m_fAnimDelta = 0.0f;
	}



	//ũ�ν� ���̵尡 �������̶��..
	if (m_fLeftCrossFadeTime > 0.0f)	//���� ũ�ν����̵� Ÿ���� �����ϴ°� Play �Ҷ� �����̵ȴ�.
	{
		//���� ũ�ν����̵� �ð� ����
		this->m_fLeftCrossFadeTime -= timeDelta;

		//ũ�ν����̵� ��������.
		if (m_fLeftCrossFadeTime <= 0.0f)
		{
			//���� ��
			m_fLeftCrossFadeTime = 0.0f;
			m_pAnimController->SetTrackWeight(0, 1);		//0 �� Track ����ġ 1
			m_pAnimController->SetTrackEnable(1, false);	//1 �� Track ��Ȱ��ȭ
		}

		else
		{
			float w1 = (m_fLeftCrossFadeTime / m_fCrossFadeTime);		//1�� Track ����ġ
			float w0 = 1.0f - w1;										//0�� Track ����ġ

			//����ġ ����
			m_pAnimController->SetTrackWeight(0, w0);
			m_pAnimController->SetTrackWeight(1, w1);
		}
	}




}

void cSkinnedAnimation::Render(cTransform* pTrans, cCamera* pCam)
{
	//�ִϸ��̼��� �����Ŵ
	this->m_pAnimController->AdvanceTime(this->m_fAnimDelta, NULL);
	//���ؽ������� �ʱ�ȭ
	this->m_fAnimDelta = 0.0f;

	//�� ���� Transform ����
	MAP_BONETRANSFORM::iterator iterBone;
	for (iterBone = m_mapBoneTransform.begin(); iterBone != m_mapBoneTransform.end(); ++iterBone)
		iterBone->first->pBoneTransform = iterBone->second;
	for (iterBone = m_mapApplyBoneTransform.begin(); iterBone != m_mapApplyBoneTransform.end(); ++iterBone)
		iterBone->first->pApplyTransform = iterBone->second;


	this->m_pSkinnedMesh->Render(pTrans, pCam);

}

void cSkinnedAnimation::RenderShadow(cTransform* pTrans, cCamera* pLightCam)
{
	//�ִϸ��̼��� �����Ŵ
	this->m_pAnimController->AdvanceTime(this->m_fAnimDelta, NULL);
	//���ؽ������� �ʱ�ȭ
	this->m_fAnimDelta = 0.0f;

	//�� ���� Transform ����
	MAP_BONETRANSFORM::iterator iterBone;
	for (iterBone = m_mapBoneTransform.begin(); iterBone != m_mapBoneTransform.end(); ++iterBone)
		iterBone->first->pBoneTransform = iterBone->second;
	for (iterBone = m_mapApplyBoneTransform.begin(); iterBone != m_mapApplyBoneTransform.end(); ++iterBone)
		iterBone->first->pApplyTransform = iterBone->second;

	this->m_pSkinnedMesh->RenderShadow(pTrans, pLightCam);

}

//���̸��� ����Ҷ� ī�޶� �ʿ��ϴ�. ( ī�޶� ������ġ�� Text �� �Ѹ����ϱ� )
void cSkinnedAnimation::RenderBoneName(cTransform* pTrans, cCamera* pCam)
{
	//�ִϸ��̼��� �����Ŵ
	this->m_pAnimController->AdvanceTime(this->m_fAnimDelta, NULL);
	//���ؽ������� �ʱ�ȭ
	this->m_fAnimDelta = 0.0f;


	//�� ���� Transform ����
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

	//�ʿ��� ������ ó��
	MAP_ANIMSET::iterator find = this->m_mapAnimSet.find(animName);
	if (find != this->m_mapAnimSet.end()){

		this->m_pPrevPlayAnimationSet = NULL;

		//ũ�ν� ���̵� Ÿ�� ���
		m_fCrossFadeTime = crossFadeTime;
		m_fLeftCrossFadeTime = crossFadeTime;

		this->SetAnimation(find->second);
	}
}
void cSkinnedAnimation::Play(int animIndex, float crossFadeTime)
{
	m_bPlay = true;
	m_bLoop = true;

	//���Ϳ��� ������ ó��
	if (animIndex < this->m_AnimNum){

		this->m_pPrevPlayAnimationSet = NULL;

		//ũ�ν� ���̵� Ÿ�� ���
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
			//���� Animaiton �� ����Ѵ�.
			this->m_pPrevPlayAnimationSet = this->m_pNowPlayAnimationSet;
		}


		//ũ�ν� ���̵� Ÿ�� ���
		m_fCrossFadeTime = inCrossFadeTime;
		m_fLeftCrossFadeTime = inCrossFadeTime;

		//������ ũ�ν����̵� Ÿ�� ���
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

		//���ư� Animation �� �������� �ʴ´�.
		this->m_pPrevPlayAnimationSet = NULL;

		//ũ�ν� ���̵� Ÿ�� ���
		m_fCrossFadeTime = crossFadeTime;
		m_fLeftCrossFadeTime = crossFadeTime;

		this->SetAnimation(find->second);
	}
}

//Animation ������ ����Ѵ�.
//ȭ����ġ�� Animation �̸��� ����Ѵ�.
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



//�ִϸ��̼� �ӵ�����
void cSkinnedAnimation::SetPlaySpeed(float speed)
{
	//0 ��Ʈ�� �ӵ�����
	this->m_pAnimController->SetTrackSpeed(0, speed);
}

//���̸����� ��ã�´�.
BONE* cSkinnedAnimation::GetFineBONE(std::string boneName)
{
	return this->m_pSkinnedMesh->GetFineBONE(boneName);
}



//BoneTransform �߰�
void cSkinnedAnimation::AddBoneTransform(std::string boneName, cTransform* pTransform)
{
	//�ش� �̸����� ���� ã�´�.
	BONE* pBone = this->m_pSkinnedMesh->GetFineBONE(boneName);
	if (pBone != NULL){
		this->m_mapBoneTransform[pBone] = pTransform;
	}

}

//ApplyTransform �߰�
void cSkinnedAnimation::AddApplyTransform(std::string boneName, cTransform* pTransform)
{
	//�ش� �̸����� ���� ã�´�.
	BONE* pBone = this->m_pSkinnedMesh->GetFineBONE(boneName);
	if (pBone != NULL){
		this->m_mapApplyBoneTransform[pBone] = pTransform;
	}
}

//BoneTransform ����
void cSkinnedAnimation::RemoveBoneTransform(std::string boneName)
{
	BONE* pBone = this->m_pSkinnedMesh->GetFineBONE(boneName);
	MAP_BONETRANSFORM::iterator boneIter = m_mapBoneTransform.find(pBone);
	if (boneIter != m_mapBoneTransform.end())
	{
		m_mapBoneTransform.erase(boneIter);
	}
}

//ApplyTransform ����
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

//Animation �� �ٲ۴�.
void cSkinnedAnimation::SetAnimation(LPD3DXANIMATIONSET animSet)
{
	//�̹� �÷��̵ǰ� �ִ� Animation �̶�� ������ ���� �ʴ´�. ( ���� Animation �ߺ����� �ʰ� ���Ҵ� )
	if (this->m_pNowPlayAnimationSet != NULL &&
		animSet == this->m_pNowPlayAnimationSet)
		return;

	//ũ�ν� ���̵尡 �����Ѵٸ�..
	if (this->m_fCrossFadeTime > 0.0f)
	{

		//
		// ������ Animation �� 1 �� track ���� ����....
		//

		//���� Animation �� 1 ��Track ���� ����
		m_pAnimController->SetTrackAnimationSet(
			1, m_pNowPlayAnimationSet);

		//1�� Ʈ���� ��ġ�� ������ �÷��� �Ǵ� ��ġ�� ����
		m_pAnimController->SetTrackPosition(
			1,
			m_Track_Desc_0.Position);

		//1 �� Track Ȱ��ȭ
		m_pAnimController->SetTrackEnable(1, true);

		//1 �� Track ����ġ
		m_pAnimController->SetTrackWeight(1, 1.0f);

		//1 �� Track �� ���� 0 ���� Animation �ӵ��� ����.
		m_pAnimController->SetTrackSpeed(1,
			m_Track_Desc_0.Speed);


		//
		// ���ݼ��õǴ� Animation �� 0 ��Track ���� ����
		//

		//���ο� Animation 0 ������ ����
		m_pAnimController->SetTrackAnimationSet(0, animSet);
		//0�� Track Animation ��ġ 0.0 Ÿ�ӿ��� ���� ( ó������ �����ϰڴٴ¿��� )
		m_pAnimController->SetTrackPosition(0, 0.0f);
		//����ġ�� 0 ���� //����ġ�� 0 ���� 
		m_pAnimController->SetTrackWeight(0, 0.0f);

		//���� �÷��� �ǰ� �ִ� AnimationSet ����
		this->m_pNowPlayAnimationSet = animSet;
	}

	//ũ�ν� ���̵尡 ���ٸ�...
	else
	{

		//Track �� �������� �Ǿ����� ������.
		this->m_pAnimController->SetTrackPosition(
			0,			//Track ��ȣ 
			0.0			//PlayPosition
			);

		//�Ű������� ���� AnimationSet ���� Animation 0 �� Track�� �÷���
		this->m_pAnimController->SetTrackAnimationSet(0, animSet);

		//���� �÷��� �ǰ� �ִ� AnimationSet ����
		this->m_pNowPlayAnimationSet = animSet;

	}



}

