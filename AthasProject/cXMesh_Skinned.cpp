#include "stdafx.h"
#include "cXMesh_Skinned.h"
#include "cCamera.h"

LPD3DXEFFECT cXMesh_Skinned::pMeshEffect = NULL;
DWORD boneRenderColor = 0xFFFFFF00;		//본랜더 컬러

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
	//보정행렬이 들어왔다면...
	if (matCorrection)
		m_matCorrection = *matCorrection;

	else
		D3DXMatrixIdentity(&m_matCorrection);

	//스킨드메쉬와 본의 계층 구조의 로딩방식이 재정의된 BONE_HIERARCHY 클래스 객체
	BONE_HIERARCHY boneHierachy;
	boneHierachy.SetLoadPath(filePath);
	boneHierachy.SetSkinnedMesh(this);

	//계층구조로 되어있는 XFile 메시 로딩....
	D3DXLoadMeshHierarchyFromX(
		filePath.c_str(),				//파일 경로
		D3DXMESH_MANAGED,
		Device,							//디바이스
		&boneHierachy,					//계층 구조의 로딩 방식이 재정의된 객체를 넘긴다.
		NULL,
		&m_pRootbone,					//얻어온 루트 본프레임
		&m_pAnimController				//얻어올 애니메이션 컨트롤러
		);

	//본 매트릭스 포인터 생성
	InitBoneMatrixPointer((BONE*)m_pRootbone);
	

	//Animation 로딩.....

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
	if ( this->m_AnimNum > 0 )
		this->Play(0);
	
	return S_OK;
}
void cXMesh_Skinned::Release()
{
	//불러온 AnimationSet 해제
	for (int i = 0; i < this->m_vecAnimSet.size(); i++)
		SAFE_RELEASE(m_vecAnimSet[i]);

	//에니메이션 콘트롤러 해제
	SAFE_RELEASE(m_pAnimController);


	//스킨드메쉬와 본의 계층 구조의 로딩방식이 재정의된 BONE_HIERARCHY 클래스 객체
	BONE_HIERARCHY boneHierachy;	//<- 예는 해제 방법도 알고 있다.
	//본해제... ( 루트본부터 작살ㄴ내고 들어간다 .... )
	D3DXFrameDestroy(m_pRootbone, &boneHierachy);



}

void cXMesh_Skinned::Update(float timeDelta)
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
		m_pAnimController->SetTrackPosition( 0, m_Track_Desc_0.Priority);

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
		//애니메이션을 진행시킴
		//timeDelta 즉시간초단위로 진행된 만큼 Animation 을 진행시킨다.
		//이때 플레이되는 Animation 정보대로 각 본의 TransformationMatrix 
		//정보가 갱신된다.....
		//즉 플레이시키려면 매 프레임맏다 AdvanceTime 타임이 호출되어야 한다...
		this->m_pAnimController->AdvanceTime(timeDelta, NULL);

		//1. AdvanceTime		명령으로 현제 Track 에 셋팅된 Animation 이 플레이된다. 이와 동시에 각본의 TransformationMatrix 정보가 Play 되는 Animation 정보대로 갱신된다.
		//2. UpdateMatrices		명령으로 업데이트된 본의 TransformationMatrix 정보로 하이어라키와 월드 변환을 하여 CombinedTransformationMatrix 정보가 갱신된다.
		//3. Render				명령으로 각본의 CombinedTransformationMatrix 대로 스키닝 셰이더를 구현하여 랜더링을 한다.
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

void cXMesh_Skinned::Render(cTransform* pTrans, cCamera* pCam)
{
	
	D3DXMATRIXA16 matFinal;

	pMeshEffect->SetTechnique("Base");
	pMeshEffect->SetMatrix("matViewProjection", 
		&pCam->GetViewProjectionMatrix());

	pMeshEffect->SetVector("viewPos", &D3DXVECTOR4(
		pCam->Transform.GetWorldPosition(), 1));


	//Transform 이 들어왔다면...
	if (pTrans != NULL)
	{
		D3DXMATRIXA16 matWorld = pTrans->GetFinalMatrix();
		matFinal = m_matCorrection * matWorld;
	}
	else
	{
		matFinal = m_matCorrection;
	}

	//행렬 업데이트 ( 본을 현재의 Animation 정보 대로 업데이트를 해야한다 )
	//Animation 이 진행되지 않아도 반드시 행렬 업데이트를 해줘야한다.
	UpdateMatrices((BONE*)m_pRootbone, &matFinal);	

	//루트 본부터 타고 들어간다.
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
	//Transform 이 들어왔다면...
	if (pTrans != NULL)
	{
		D3DXMATRIXA16 matWorld = pTrans->GetFinalMatrix();
		matFinal = m_matCorrection * matWorld;
	}
	else
	{
		matFinal = m_matCorrection;
	}

	//행렬 업데이트 ( 본을 현재의 Animation 정보 대로 업데이트를 해야한다 )
	//Animation 이 진행되지 않아도 반드시 행렬 업데이트를 해줘야한다.
	UpdateMatrices((BONE*)m_pRootbone, &matFinal);

	//루트 본부터 타고 들어간다.
	RenderBone((BONE*)m_pRootbone);
}


//본이름을 출력할때 카메라가 필요하다. ( 카메라 동차위치에 Text 로 뿌릴꺼니깐 )
void cXMesh_Skinned::RenderBoneName(cTransform* pTrans, cCamera* pCam)
{
	D3DXMATRIXA16 matFinal;

	//Transform 이 들어왔다면...
	if (pTrans != NULL)
	{
		D3DXMATRIXA16 matWorld = pTrans->GetFinalMatrix();
		matFinal = m_matCorrection * matWorld;
	}
	else
	{
		matFinal = m_matCorrection;
	}

	//행렬 업데이트 ( 본을 현재의 Animation 정보 대로 업데이트를 해야한다 )
	//Animation 이 진행되지 않아도 반드시 행렬 업데이트를 해줘야한다.
	UpdateMatrices((BONE*)m_pRootbone, &matFinal);

	//루트 본부터 타고 들어간다.
	RenderChildBoneName(pCam, (BONE*)m_pRootbone, NULL);
}


void cXMesh_Skinned::Play(std::string animName, float crossFadeTime)
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
void cXMesh_Skinned::Play(int animIndex, float crossFadeTime)
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


void cXMesh_Skinned::PlayOneShot(std::string animName, float inCrossFadeTime, float outCrossFadeTime)
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

void cXMesh_Skinned::PlayOneShotAfterHold(std::string animName, float crossFadeTime)
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



//애니메이션 속도조정
void cXMesh_Skinned::SetPlaySpeed(float speed)
{
	//0 번트랙 속도셋팅
	this->m_pAnimController->SetTrackSpeed(0, speed); 
}

//본이름으로 본찾는다.
BONE* cXMesh_Skinned::GetFineBONE(std::string boneName)
{
	//리턴 되는 LPD3DXFRAME 를 BONE 으로 형변환 하여 리턴
	//만약 못찾으면 NULL
	LPD3DXFRAME pFrame = D3DXFrameFind(
		m_pRootbone,			//루트 본전달
		boneName.c_str()		//찾을 본이름 전달
		);

	if (pFrame != NULL)
		return (BONE*)pFrame;

	return NULL;
}


/////////////////////////////////////////////////////////////



//본 행렬을 셋팅한다. ( 초기화 )
//각 본메쉬에 적용되는 본들의 행렬포인터를 물어 놓는다.
void cXMesh_Skinned::InitBoneMatrixPointer(BONE* pBone)
{
	if (pBone == NULL) return;

	//해당 본프레임에 메쉬 컨테이너 가 존재한다면...
	if (pBone->pMeshContainer)
	{
		//메쉬 컨테이너를 BONE_MESH 형으로 형변환
		BONE_MESH* pBoneMesh = (BONE_MESH*)pBone->pMeshContainer;

		//메쉬에 스키닝 정보가 있다면
		if (pBoneMesh->pSkinInfo)
		{
			//해당 스킨에 적용되는 본의 갯수만큼
			int numBones = pBoneMesh->pSkinInfo->GetNumBones();


			//Matrix 포인터 할당
			pBoneMesh->ppBoneMatrixPtrs = new D3DXMATRIXA16*[numBones];

			//적용되는 폰 프레임의 매트릭스를 찾아 해당 본프레임의 행렬 포인터를 물린다.
			for (int i = 0; i < numBones; i++)
			{
				//해당 스킨에 적용되는 i 번째의 본이름으로 해당 본 프레임을 루프 본부터 찾아 들어간가

				//D3DXFrameFind( 루트본, "찾을 본의 이름" );


				BONE* pB = (BONE*)D3DXFrameFind(m_pRootbone, pBoneMesh->pSkinInfo->GetBoneName(i));

				//찾았다면
				if (pB != NULL)
					pBoneMesh->ppBoneMatrixPtrs[i] = &pB->CombinedTransformationMatrix;

				else
					pBoneMesh->ppBoneMatrixPtrs[i] = NULL;
			}
		}

	}




	//이웃 본이 존재한다면...
	if (pBone->pFrameSibling)
		InitBoneMatrixPointer((BONE*)pBone->pFrameSibling);

	//자식 본이 존재한다면...
	if (pBone->pFrameFirstChild)
		InitBoneMatrixPointer((BONE*)pBone->pFrameFirstChild);

}



//본행렬 을 업데이트한다. ( 랜더시 Animation 정보대로 본행렬 업데이트를 진행시킨다 )
//사실 DX 내부에서는 Animation 이 진행됨에 따라.
//D3DXFRAME 이 지닌 TransformationMatrix 만 업데이트를 시켜준다.
//문제는 이 업데이트된 TransformationMatrix 는 계층구조의 속성이 전혀 적용되어있지 않다.
//따라서 아래의 UpdateMatrices 함수에서 모든계층구조와 월드 변환까지 끈난 행렬정보를 만들어야 하며,
//이를 BONE 의 CombinedTransformationMatrix 에 갱신시켜주고 랜더링 해야 한다.
void cXMesh_Skinned::UpdateMatrices(BONE* pBone, D3DXMATRIXA16* pParentMatrix)
{
	if (pBone == NULL) return;

	//부모 행렬이 있다면..
	if (pParentMatrix != NULL)
	{
		//Animation 처리가 완료된 행렬
		D3DXMATRIXA16 matTransformation = pBone->TransformationMatrix;


		//부모 행렬과 자신의 행렬을 곱해 계층구조 및 월드 행렬까지 적용된 본의 CombinedTransformationMatrix
		pBone->CombinedTransformationMatrix = matTransformation * (*pParentMatrix);


		//적용 Transform 이 물려 있다면...
		if (pBone->pApplyTransform != NULL)
		{

			D3DXMATRIXA16 pApplyMatrix = pBone->pApplyTransform->GetWorldRotateMatrix();

			//최종 행렬에 이동성분 빼고
			D3DXMATRIXA16 mat1 = pBone->CombinedTransformationMatrix;
			mat1._41 = 0;
			mat1._42 = 0;
			mat1._43 = 0;

			//회전 성분만 곱한다.
			D3DXMATRIXA16 mat2 = mat1 * pApplyMatrix;

			//곱한 것에 아까 뺀 이동성분을 추가한다.
			mat2._41 = pBone->CombinedTransformationMatrix._41;
			mat2._42 = pBone->CombinedTransformationMatrix._42;
			mat2._43 = pBone->CombinedTransformationMatrix._43;

			pBone->CombinedTransformationMatrix = mat2;


			//다음 사용을 위해 NULL
			pBone->pApplyTransform = NULL;
		}





	}

	//부모 행렬이 없다면..
	else
	{
		//자신의 행렬이 현재 행렬이 된다.
		pBone->CombinedTransformationMatrix = pBone->TransformationMatrix;
	}

	//본 Transform 이 물려 있다면....
	if (pBone->pBoneTransform != NULL)
	{
		//월드 행렬정보대로 Transform 을 셋팅 한다.
		pBone->pBoneTransform->SetWorldMatrix(
			&pBone->CombinedTransformationMatrix);
		
		//다음 사용을 위해 NULL
		pBone->pBoneTransform = NULL;
	}




	//이웃 본이 존재한다면...
	if (pBone->pFrameSibling)
		UpdateMatrices((BONE*)pBone->pFrameSibling, pParentMatrix);	//이웃은 같은레벨이기 때문에 부모가 같다 따라서 매개변로 받은 부모의 행렬을 넘기면 된다.

	//자식 본이 존재한다면...
	if (pBone->pFrameFirstChild)
		UpdateMatrices((BONE*)pBone->pFrameFirstChild, &pBone->CombinedTransformationMatrix);

}


//본을 그린다.
void cXMesh_Skinned::RenderBone(BONE* pBone)
{
	if (pBone == NULL) return;

	//해당 본에 물려있는 메시 컨테이너가 존재한다면
	if (pBone->pMeshContainer != NULL){

		//랜더 명령


		//본에 있는 메쉬 컨테이너는 BONE_MESH 이다
		BONE_MESH* pBoneMesh = (BONE_MESH*)pBone->pMeshContainer;

		//본 컴비네이션 정보가  
		if (pBoneMesh->BufBoneCombos != NULL)
		{
			//해당 본의 컴비네이션 정보를 얻는다.
			LPD3DXBONECOMBINATION pBoneComb =
				(LPD3DXBONECOMBINATION)(pBoneMesh->BufBoneCombos->GetBufferPointer());

			//본에 물려있는 메쉬의 서브셋갯수는  속성그룹수와 같다
			for (DWORD i = 0; i < pBoneMesh->NumAttributesGroup; i++)
			{

				//해당 속성의 팔래트 엔트리 수만큼 돌면서 작업용 팔래트 행렬 갱신
				for (DWORD palEntry = 0; palEntry < pBoneMesh->NumPaletteEntries; palEntry++)
				{
					//적용되는 행렬 ID 를 얻는다
					DWORD dwMatrixIndex = pBoneComb[i].BoneId[palEntry];

					//행렬 인덱스가 유효하다면...
					if (dwMatrixIndex != UINT_MAX)
					{
						//작업 행렬을 만든다.
						D3DXMatrixMultiply(&m_pmWorkingPalette[palEntry],
							&(pBoneMesh->pBoneOffsetMatices[dwMatrixIndex]),
							pBoneMesh->ppBoneMatrixPtrs[dwMatrixIndex]);
					}
				}


				//위에서 셋팅됭 작업행렬을 Effect 팔래스에 적용한다.
				pMeshEffect->SetMatrixArray(
					"amPalette", m_pmWorkingPalette, pBoneMesh->NumPaletteEntries);

				//적용되는 정점의 본최대 영향수 를 대입 최대 영향수  -1 
				pMeshEffect->SetInt("CurNumBones", pBoneMesh->MaxNumFaceInfls - 1);

				//메터리얼 인덱스 ( 몇번째 매터리얼인지... )
				DWORD materialIndex = pBoneComb[i].AttribId;


				//텍스쳐 셋팅
				pMeshEffect->SetTexture("Diffuse_Tex", pBoneMesh->DiffuseTexs[materialIndex]);
				pMeshEffect->SetTexture("Specular_Tex", pBoneMesh->SpecularTexs[materialIndex]);
				pMeshEffect->SetTexture("Normal_Tex", pBoneMesh->NormalTexs[materialIndex]);
				pMeshEffect->SetTexture("Emission_Tex", pBoneMesh->EmissionTexs[materialIndex]);
				pMeshEffect->SetTexture("Reflect_Tex", pBoneMesh->ReflectionTexs[materialIndex]);


				//패스수대로 돌린다.
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


	//이웃 본이 존재한다면...
	if (pBone->pFrameSibling)
		RenderBone((BONE*)pBone->pFrameSibling);

	//자식 본이 존재한다면...
	if (pBone->pFrameFirstChild)
		RenderBone((BONE*)pBone->pFrameFirstChild);

}

//본이름을 출력한다.
void cXMesh_Skinned::RenderChildBoneName(cCamera* pCam, BONE* pBone, D3DXVECTOR3* parentPos)
{
	if (pBone == NULL) return;

	//본위치
	D3DXVECTOR3 bonePos = D3DXVECTOR3(
		pBone->CombinedTransformationMatrix._41,
		pBone->CombinedTransformationMatrix._42,
		pBone->CombinedTransformationMatrix._43);

	//부모위치에서 내위치의 선
	if (parentPos != NULL)
		GIZMO_MGR->RenderLine(&bonePos, parentPos, boneRenderColor);

	//해당 위치에 본의 이름을 출력하자
	//본위치에 해당되는 Screen위치를 얻자.
	POINT screenPos;
	if (pCam->GetScreenPoint(&screenPos, bonePos))
	{
		if (pBone->Name != NULL){
			DXFONT_MGR->PrintText(pBone->Name,
				screenPos.x, screenPos.y, 0xFFFFFF00);
		}
	}

	//해당 본위치에 조그만한 와이어 구를 그리자.
	GIZMO_MGR->DrawWireSphere(
		&bonePos, 0.3f, 0xFFFFFF00);
	


	//이웃 본이 존재한다면...
	if (pBone->pFrameSibling)
		RenderChildBoneName(pCam, (BONE*)pBone->pFrameSibling, parentPos);

	//자식 본이 존재한다면...
	if (pBone->pFrameFirstChild)
		RenderChildBoneName(pCam, (BONE*)pBone->pFrameFirstChild, &bonePos);

}


//Animation 을 바꾼다.
void cXMesh_Skinned::SetAnimation(LPD3DXANIMATIONSET animSet)
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



void cXMesh_Skinned::SetCamera(cCamera* pCamera)
{
	//셰이더가 로딩되어있지 않다면 셰이터를 로딩한다.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshSkinning.fx");

	pMeshEffect->SetMatrix("matViewProjection", &pCamera->GetViewProjectionMatrix());
	pMeshEffect->SetVector("viewPos", &D3DXVECTOR4(pCamera->Transform.GetWorldPosition(), 1));
}
void cXMesh_Skinned::SetEnvironment(LPDIRECT3DCUBETEXTURE9 pCubeTex)
{
	//셰이더가 로딩되어있지 않다면 셰이터를 로딩한다.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshSkinning.fx");

	pMeshEffect->SetTexture("Environment_Tex", pCubeTex);
}

void cXMesh_Skinned::SetLight(cTransform* lightTrans, D3DXCOLOR lightColor, float ambient)
{
	//셰이더가 로딩되어있지 않다면 셰이터를 로딩한다.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
			"../Resources/Effects/XMeshSkinning.fx");

	pMeshEffect->SetVector("lightDir", &D3DXVECTOR4(lightTrans->GetForward(), 1));
	pMeshEffect->SetVector("lightColor", (D3DXVECTOR4*)(&lightColor));
	pMeshEffect->SetFloat("ambient", ambient);
}


