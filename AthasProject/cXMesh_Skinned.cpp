#include "stdafx.h"
#include "cXMesh_Skinned.h"
#include "cCamera.h"

LPD3DXEFFECT cXMesh_Skinned::pMeshEffect = NULL;
DWORD boneRenderColor = 0xFFFFFF00;		//������ �÷�

cXMesh_Skinned::cXMesh_Skinned()
:m_pNowPlayAnimationSet(NULL), m_dwWorkingPaletteSize(0), m_pmWorkingPalette(NULL)
{
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshSkinning.fx");
}


cXMesh_Skinned::~cXMesh_Skinned()
{
}



HRESULT cXMesh_Skinned::Init(std::string filePath, D3DXMATRIXA16* matCorrection)
{
	//��������� ���Դٸ�...
	if (matCorrection)
		m_matCorrection = *matCorrection;

	else
		D3DXMatrixIdentity(&m_matCorrection);

	//��Ų��޽��� ���� ���� ������ �ε������ �����ǵ� BONE_HIERARCHY Ŭ���� ��ü
	BONE_HIERARCHY boneHierachy;
	boneHierachy.SetLoadPath(filePath);
	boneHierachy.SetSkinnedMesh(this);

	//���������� �Ǿ��ִ� XFile �޽� �ε�....
	D3DXLoadMeshHierarchyFromX(
		filePath.c_str(),				//���� ���
		D3DXMESH_MANAGED,
		Device,							//����̽�
		&boneHierachy,					//���� ������ �ε� ����� �����ǵ� ��ü�� �ѱ��.
		NULL,
		&m_pRootbone,					//���� ��Ʈ ��������
		&m_pAnimController				//���� �ִϸ��̼� ��Ʈ�ѷ�
		);

	//�� ��Ʈ���� ������ ����
	InitBoneMatrixPointer((BONE*)m_pRootbone);
	

	//Animation �ε�.....

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
	if ( this->m_AnimNum > 0 )
		this->Play(0);
	
	return S_OK;
}
void cXMesh_Skinned::Release()
{
	//�ҷ��� AnimationSet ����
	for (int i = 0; i < this->m_vecAnimSet.size(); i++)
		SAFE_RELEASE(m_vecAnimSet[i]);

	//���ϸ��̼� ��Ʈ�ѷ� ����
	SAFE_RELEASE(m_pAnimController);


	//��Ų��޽��� ���� ���� ������ �ε������ �����ǵ� BONE_HIERARCHY Ŭ���� ��ü
	BONE_HIERARCHY boneHierachy;	//<- ���� ���� ����� �˰� �ִ�.
	//������... ( ��Ʈ������ �ۻ줤���� ���� .... )
	D3DXFrameDestroy(m_pRootbone, &boneHierachy);



}

void cXMesh_Skinned::Update(float timeDelta)
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
		m_pAnimController->SetTrackPosition( 0, m_Track_Desc_0.Priority);

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
		//�ִϸ��̼��� �����Ŵ
		//timeDelta ��ð��ʴ����� ����� ��ŭ Animation �� �����Ų��.
		//�̶� �÷��̵Ǵ� Animation ������� �� ���� TransformationMatrix 
		//������ ���ŵȴ�.....
		//�� �÷��̽�Ű���� �� �����Ӹ��� AdvanceTime Ÿ���� ȣ��Ǿ�� �Ѵ�...
		this->m_pAnimController->AdvanceTime(timeDelta, NULL);

		//1. AdvanceTime		������� ���� Track �� ���õ� Animation �� �÷��̵ȴ�. �̿� ���ÿ� ������ TransformationMatrix ������ Play �Ǵ� Animation ������� ���ŵȴ�.
		//2. UpdateMatrices		������� ������Ʈ�� ���� TransformationMatrix ������ ���̾��Ű�� ���� ��ȯ�� �Ͽ� CombinedTransformationMatrix ������ ���ŵȴ�.
		//3. Render				������� ������ CombinedTransformationMatrix ��� ��Ű�� ���̴��� �����Ͽ� �������� �Ѵ�.
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

void cXMesh_Skinned::Render(cTransform* pTrans, cCamera* pCam)
{
	
	D3DXMATRIXA16 matFinal;

	pMeshEffect->SetTechnique("Base");
	pMeshEffect->SetMatrix("matViewProjection", 
		&pCam->GetViewProjectionMatrix());

	pMeshEffect->SetVector("viewPos", &D3DXVECTOR4(
		pCam->Transform.GetWorldPosition(), 1));


	//Transform �� ���Դٸ�...
	if (pTrans != NULL)
	{
		D3DXMATRIXA16 matWorld = pTrans->GetFinalMatrix();
		matFinal = m_matCorrection * matWorld;
	}
	else
	{
		matFinal = m_matCorrection;
	}

	//��� ������Ʈ ( ���� ������ Animation ���� ��� ������Ʈ�� �ؾ��Ѵ� )
	//Animation �� ������� �ʾƵ� �ݵ�� ��� ������Ʈ�� ������Ѵ�.
	UpdateMatrices((BONE*)m_pRootbone, &matFinal);	

	//��Ʈ ������ Ÿ�� ����.
	RenderBone((BONE*)m_pRootbone);

}


void cXMesh_Skinned::RenderShadow(cTransform* pTrans, cCamera* pLightCam)
{
	pMeshEffect->SetTechnique("Shadow");
	pMeshEffect->SetMatrix("matViewProjection",
		&pLightCam->GetViewProjectionMatrix());

	pMeshEffect->SetVector("viewPos", &D3DXVECTOR4(
		pLightCam->Transform.GetWorldPosition(), 1));

	D3DXMATRIXA16 matFinal;
	//Transform �� ���Դٸ�...
	if (pTrans != NULL)
	{
		D3DXMATRIXA16 matWorld = pTrans->GetFinalMatrix();
		matFinal = m_matCorrection * matWorld;
	}
	else
	{
		matFinal = m_matCorrection;
	}

	//��� ������Ʈ ( ���� ������ Animation ���� ��� ������Ʈ�� �ؾ��Ѵ� )
	//Animation �� ������� �ʾƵ� �ݵ�� ��� ������Ʈ�� ������Ѵ�.
	UpdateMatrices((BONE*)m_pRootbone, &matFinal);

	//��Ʈ ������ Ÿ�� ����.
	RenderBone((BONE*)m_pRootbone);
}


//���̸��� ����Ҷ� ī�޶� �ʿ��ϴ�. ( ī�޶� ������ġ�� Text �� �Ѹ����ϱ� )
void cXMesh_Skinned::RenderBoneName(cTransform* pTrans, cCamera* pCam)
{
	D3DXMATRIXA16 matFinal;

	//Transform �� ���Դٸ�...
	if (pTrans != NULL)
	{
		D3DXMATRIXA16 matWorld = pTrans->GetFinalMatrix();
		matFinal = m_matCorrection * matWorld;
	}
	else
	{
		matFinal = m_matCorrection;
	}

	//��� ������Ʈ ( ���� ������ Animation ���� ��� ������Ʈ�� �ؾ��Ѵ� )
	//Animation �� ������� �ʾƵ� �ݵ�� ��� ������Ʈ�� ������Ѵ�.
	UpdateMatrices((BONE*)m_pRootbone, &matFinal);

	//��Ʈ ������ Ÿ�� ����.
	RenderChildBoneName(pCam, (BONE*)m_pRootbone, NULL);
}


void cXMesh_Skinned::Play(std::string animName, float crossFadeTime)
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
void cXMesh_Skinned::Play(int animIndex, float crossFadeTime)
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


void cXMesh_Skinned::PlayOneShot(std::string animName, float inCrossFadeTime, float outCrossFadeTime)
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

void cXMesh_Skinned::PlayOneShotAfterHold(std::string animName, float crossFadeTime)
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
void cXMesh_Skinned::ShowAnimationName(int x, int y)
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
void cXMesh_Skinned::SetPlaySpeed(float speed)
{
	//0 ��Ʈ�� �ӵ�����
	this->m_pAnimController->SetTrackSpeed(0, speed); 
}

//���̸����� ��ã�´�.
BONE* cXMesh_Skinned::GetFineBONE(std::string boneName)
{
	//���� �Ǵ� LPD3DXFRAME �� BONE ���� ����ȯ �Ͽ� ����
	//���� ��ã���� NULL
	LPD3DXFRAME pFrame = D3DXFrameFind(
		m_pRootbone,			//��Ʈ ������
		boneName.c_str()		//ã�� ���̸� ����
		);

	if (pFrame != NULL)
		return (BONE*)pFrame;

	return NULL;
}


/////////////////////////////////////////////////////////////



//�� ����� �����Ѵ�. ( �ʱ�ȭ )
//�� ���޽��� ����Ǵ� ������ ��������͸� ���� ���´�.
void cXMesh_Skinned::InitBoneMatrixPointer(BONE* pBone)
{
	if (pBone == NULL) return;

	//�ش� �������ӿ� �޽� �����̳� �� �����Ѵٸ�...
	if (pBone->pMeshContainer)
	{
		//�޽� �����̳ʸ� BONE_MESH ������ ����ȯ
		BONE_MESH* pBoneMesh = (BONE_MESH*)pBone->pMeshContainer;

		//�޽��� ��Ű�� ������ �ִٸ�
		if (pBoneMesh->pSkinInfo)
		{
			//�ش� ��Ų�� ����Ǵ� ���� ������ŭ
			int numBones = pBoneMesh->pSkinInfo->GetNumBones();


			//Matrix ������ �Ҵ�
			pBoneMesh->ppBoneMatrixPtrs = new D3DXMATRIXA16*[numBones];

			//����Ǵ� �� �������� ��Ʈ������ ã�� �ش� ���������� ��� �����͸� ������.
			for (int i = 0; i < numBones; i++)
			{
				//�ش� ��Ų�� ����Ǵ� i ��°�� ���̸����� �ش� �� �������� ���� ������ ã�� ����

				//D3DXFrameFind( ��Ʈ��, "ã�� ���� �̸�" );


				BONE* pB = (BONE*)D3DXFrameFind(m_pRootbone, pBoneMesh->pSkinInfo->GetBoneName(i));

				//ã�Ҵٸ�
				if (pB != NULL)
					pBoneMesh->ppBoneMatrixPtrs[i] = &pB->CombinedTransformationMatrix;

				else
					pBoneMesh->ppBoneMatrixPtrs[i] = NULL;
			}
		}

	}




	//�̿� ���� �����Ѵٸ�...
	if (pBone->pFrameSibling)
		InitBoneMatrixPointer((BONE*)pBone->pFrameSibling);

	//�ڽ� ���� �����Ѵٸ�...
	if (pBone->pFrameFirstChild)
		InitBoneMatrixPointer((BONE*)pBone->pFrameFirstChild);

}



//����� �� ������Ʈ�Ѵ�. ( ������ Animation ������� ����� ������Ʈ�� �����Ų�� )
//��� DX ���ο����� Animation �� ����ʿ� ����.
//D3DXFRAME �� ���� TransformationMatrix �� ������Ʈ�� �����ش�.
//������ �� ������Ʈ�� TransformationMatrix �� ���������� �Ӽ��� ���� ����Ǿ����� �ʴ�.
//���� �Ʒ��� UpdateMatrices �Լ����� ������������ ���� ��ȯ���� ���� ��������� ������ �ϸ�,
//�̸� BONE �� CombinedTransformationMatrix �� ���Ž����ְ� ������ �ؾ� �Ѵ�.
void cXMesh_Skinned::UpdateMatrices(BONE* pBone, D3DXMATRIXA16* pParentMatrix)
{
	if (pBone == NULL) return;

	//�θ� ����� �ִٸ�..
	if (pParentMatrix != NULL)
	{
		//Animation ó���� �Ϸ�� ���
		D3DXMATRIXA16 matTransformation = pBone->TransformationMatrix;


		//�θ� ��İ� �ڽ��� ����� ���� �������� �� ���� ��ı��� ����� ���� CombinedTransformationMatrix
		pBone->CombinedTransformationMatrix = matTransformation * (*pParentMatrix);


		//���� Transform �� ���� �ִٸ�...
		if (pBone->pApplyTransform != NULL)
		{

			D3DXMATRIXA16 pApplyMatrix = pBone->pApplyTransform->GetWorldRotateMatrix();

			//���� ��Ŀ� �̵����� ����
			D3DXMATRIXA16 mat1 = pBone->CombinedTransformationMatrix;
			mat1._41 = 0;
			mat1._42 = 0;
			mat1._43 = 0;

			//ȸ�� ���и� ���Ѵ�.
			D3DXMATRIXA16 mat2 = mat1 * pApplyMatrix;

			//���� �Ϳ� �Ʊ� �� �̵������� �߰��Ѵ�.
			mat2._41 = pBone->CombinedTransformationMatrix._41;
			mat2._42 = pBone->CombinedTransformationMatrix._42;
			mat2._43 = pBone->CombinedTransformationMatrix._43;

			pBone->CombinedTransformationMatrix = mat2;


			//���� ����� ���� NULL
			pBone->pApplyTransform = NULL;
		}





	}

	//�θ� ����� ���ٸ�..
	else
	{
		//�ڽ��� ����� ���� ����� �ȴ�.
		pBone->CombinedTransformationMatrix = pBone->TransformationMatrix;
	}

	//�� Transform �� ���� �ִٸ�....
	if (pBone->pBoneTransform != NULL)
	{
		//���� ���������� Transform �� ���� �Ѵ�.
		pBone->pBoneTransform->SetWorldMatrix(
			&pBone->CombinedTransformationMatrix);
		
		//���� ����� ���� NULL
		pBone->pBoneTransform = NULL;
	}




	//�̿� ���� �����Ѵٸ�...
	if (pBone->pFrameSibling)
		UpdateMatrices((BONE*)pBone->pFrameSibling, pParentMatrix);	//�̿��� ���������̱� ������ �θ� ���� ���� �Ű����� ���� �θ��� ����� �ѱ�� �ȴ�.

	//�ڽ� ���� �����Ѵٸ�...
	if (pBone->pFrameFirstChild)
		UpdateMatrices((BONE*)pBone->pFrameFirstChild, &pBone->CombinedTransformationMatrix);

}


//���� �׸���.
void cXMesh_Skinned::RenderBone(BONE* pBone)
{
	if (pBone == NULL) return;

	//�ش� ���� �����ִ� �޽� �����̳ʰ� �����Ѵٸ�
	if (pBone->pMeshContainer != NULL){

		//���� ���


		//���� �ִ� �޽� �����̳ʴ� BONE_MESH �̴�
		BONE_MESH* pBoneMesh = (BONE_MESH*)pBone->pMeshContainer;

		//�� �ĺ���̼� ������  
		if (pBoneMesh->BufBoneCombos != NULL)
		{
			//�ش� ���� �ĺ���̼� ������ ��´�.
			LPD3DXBONECOMBINATION pBoneComb =
				(LPD3DXBONECOMBINATION)(pBoneMesh->BufBoneCombos->GetBufferPointer());

			//���� �����ִ� �޽��� ����°�����  �Ӽ��׷���� ����
			for (DWORD i = 0; i < pBoneMesh->NumAttributesGroup; i++)
			{

				//�ش� �Ӽ��� �ȷ�Ʈ ��Ʈ�� ����ŭ ���鼭 �۾��� �ȷ�Ʈ ��� ����
				for (DWORD palEntry = 0; palEntry < pBoneMesh->NumPaletteEntries; palEntry++)
				{
					//����Ǵ� ��� ID �� ��´�
					DWORD dwMatrixIndex = pBoneComb[i].BoneId[palEntry];

					//��� �ε����� ��ȿ�ϴٸ�...
					if (dwMatrixIndex != UINT_MAX)
					{
						//�۾� ����� �����.
						D3DXMatrixMultiply(&m_pmWorkingPalette[palEntry],
							&(pBoneMesh->pBoneOffsetMatices[dwMatrixIndex]),
							pBoneMesh->ppBoneMatrixPtrs[dwMatrixIndex]);
					}
				}


				//������ ���É� �۾������ Effect �ȷ����� �����Ѵ�.
				pMeshEffect->SetMatrixArray(
					"amPalette", m_pmWorkingPalette, pBoneMesh->NumPaletteEntries);

				//����Ǵ� ������ ���ִ� ����� �� ���� �ִ� �����  -1 
				pMeshEffect->SetInt("CurNumBones", pBoneMesh->MaxNumFaceInfls - 1);

				//���͸��� �ε��� ( ���° ���͸�������... )
				DWORD materialIndex = pBoneComb[i].AttribId;


				//�ؽ��� ����
				pMeshEffect->SetTexture("Diffuse_Tex", pBoneMesh->DiffuseTexs[materialIndex]);
				pMeshEffect->SetTexture("Specular_Tex", pBoneMesh->SpecularTexs[materialIndex]);
				pMeshEffect->SetTexture("Normal_Tex", pBoneMesh->NormalTexs[materialIndex]);
				pMeshEffect->SetTexture("Emission_Tex", pBoneMesh->EmissionTexs[materialIndex]);
				pMeshEffect->SetTexture("Reflect_Tex", pBoneMesh->ReflectionTexs[materialIndex]);


				//�н������ ������.
				UINT uiPasses, uiPass;
				pMeshEffect->Begin(&uiPasses, 0);
				for (uiPass = 0; uiPass < uiPasses; ++uiPass)
				{
					pMeshEffect->BeginPass(uiPass);
					pBoneMesh->WorkingMesh->DrawSubset(i);
					pMeshEffect->EndPass();
				}
				pMeshEffect->End();


			}


		}



	}


	//�̿� ���� �����Ѵٸ�...
	if (pBone->pFrameSibling)
		RenderBone((BONE*)pBone->pFrameSibling);

	//�ڽ� ���� �����Ѵٸ�...
	if (pBone->pFrameFirstChild)
		RenderBone((BONE*)pBone->pFrameFirstChild);

}

//���̸��� ����Ѵ�.
void cXMesh_Skinned::RenderChildBoneName(cCamera* pCam, BONE* pBone, D3DXVECTOR3* parentPos)
{
	if (pBone == NULL) return;

	//����ġ
	D3DXVECTOR3 bonePos = D3DXVECTOR3(
		pBone->CombinedTransformationMatrix._41,
		pBone->CombinedTransformationMatrix._42,
		pBone->CombinedTransformationMatrix._43);

	//�θ���ġ���� ����ġ�� ��
	if (parentPos != NULL)
		GIZMO_MGR->RenderLine(&bonePos, parentPos, boneRenderColor);

	//�ش� ��ġ�� ���� �̸��� �������
	//����ġ�� �ش�Ǵ� Screen��ġ�� ����.
	POINT screenPos;
	if (pCam->GetScreenPoint(&screenPos, bonePos))
	{
		if (pBone->Name != NULL){
			DXFONT_MGR->PrintText(pBone->Name,
				screenPos.x, screenPos.y, 0xFFFFFF00);
		}
	}

	//�ش� ����ġ�� ���׸��� ���̾� ���� �׸���.
	GIZMO_MGR->DrawWireSphere(
		&bonePos, 0.3f, 0xFFFFFF00);
	


	//�̿� ���� �����Ѵٸ�...
	if (pBone->pFrameSibling)
		RenderChildBoneName(pCam, (BONE*)pBone->pFrameSibling, parentPos);

	//�ڽ� ���� �����Ѵٸ�...
	if (pBone->pFrameFirstChild)
		RenderChildBoneName(pCam, (BONE*)pBone->pFrameFirstChild, &bonePos);

}


//Animation �� �ٲ۴�.
void cXMesh_Skinned::SetAnimation(LPD3DXANIMATIONSET animSet)
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



void cXMesh_Skinned::SetCamera(cCamera* pCamera)
{
	//���̴��� �ε��Ǿ����� �ʴٸ� �����͸� �ε��Ѵ�.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshSkinning.fx");

	pMeshEffect->SetMatrix("matViewProjection", &pCamera->GetViewProjectionMatrix());
	pMeshEffect->SetVector("viewPos", &D3DXVECTOR4(pCamera->Transform.GetWorldPosition(), 1));
}
void cXMesh_Skinned::SetEnvironment(LPDIRECT3DCUBETEXTURE9 pCubeTex)
{
	//���̴��� �ε��Ǿ����� �ʴٸ� �����͸� �ε��Ѵ�.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshSkinning.fx");

	pMeshEffect->SetTexture("Environment_Tex", pCubeTex);
}

void cXMesh_Skinned::SetLight(cTransform* lightTrans, D3DXCOLOR lightColor, float ambient)
{
	//���̴��� �ε��Ǿ����� �ʴٸ� �����͸� �ε��Ѵ�.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
			"../Resources/Effects/XMeshSkinning.fx");

	pMeshEffect->SetVector("lightDir", &D3DXVECTOR4(lightTrans->GetForward(), 1));
	pMeshEffect->SetVector("lightColor", (D3DXVECTOR4*)(&lightColor));
	pMeshEffect->SetFloat("ambient", ambient);
}


