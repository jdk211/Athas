#include "stdafx.h"
#include "cXMesh_Static.h"
#include "cBoundBox.h"


LPD3DXEFFECT cXMesh_Static::pMeshEffect = NULL;

cXMesh_Static::cXMesh_Static()
: pDefaultBoundBox( NULL )
{
	if ( pMeshEffect == NULL )
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");
}


cXMesh_Static::~cXMesh_Static()
{
}


//메쉬 셋팅함수
void cXMesh_Static::Init(std::string filePath, D3DXMATRIXA16* matCorrection)
{


	//매터리얼 정보를 가져오기 위한 버퍼
	LPD3DXBUFFER pMaterial = NULL;

	//인접 버퍼를 가져오기 위한 버퍼
	LPD3DXBUFFER pAdjacency = NULL;


	//XFile 을 로딩한다.
	D3DXLoadMeshFromX(
		filePath.c_str(),	//경로
		D3DXMESH_MANAGED,							//옵션은 무조건 D3DXMESH_MANAGED
		Device,										//Deivce
		&pAdjacency,								//인접 버퍼 정보
		&pMaterial,									//메터리얼 버퍼 정보
		NULL,										//Effect 버퍼 정보 ( 이건안쓴다 무조건 NULL )
		&NumMaterials,								//메터리얼 갯수
		&pMesh);									//로딩될 메쉬 포인터


	//재질 정보가 로딩이 되었다면...
	if (pMaterial != NULL){


		//XFile 의 이름만 짜르고 순수하게 경로 만얻자.
		string pathDirectory = "";
		int index = filePath.find_last_of('/');//'/' 를 뒤에서 부터 찾고 찾으면 해당 인덱스를 리턴하는데 못찾느면 -1 을 리턴한다...
		if (index == -1){
			index = filePath.find_last_of('\\'); //'/' 을 못찾으면 '\\' 로 찾아보자
		}

		//짜를 인덱스를 찾았다면...
		if (index != -1){

			//filePath 의 스트링을 0 번째부터 index + 1 개까지 짤라 pathDirectory 에 대입
			pathDirectory = filePath.substr(0, index + 1);

		}


		string texturePath = "";
		string textureFileName = "";	//Texture 파일이름
		string textureExpName = "";		//Texture 확장자이름
		string temp;


		//로딩된 메터리얼 버퍼에는 D3DXMATERIAL 정보의 배열 시작포인터가 물려있다.
		LPD3DXMATERIAL pMaterials = (LPD3DXMATERIAL)pMaterial->GetBufferPointer();

		//배열의 갯수는 로딩시 얻어왔던 NumMaterials 로 얻을수 있다.
		for (DWORD i = 0; i < NumMaterials; i++){
			D3DXMATERIAL mat = pMaterials[i];

			//mat.pTextureFilename 해당 Material 에서 사용하는 Texture 파일 이름이 있다.

			if (mat.pTextureFilename != NULL)
			{

				//Texture 파일명을 일단은 임시로...
				temp = mat.pTextureFilename;
				int dotIndex = temp.find_last_of('.');	//파일명에서 .위치의 인덱스를 찾는다.

				textureFileName = temp.substr(0, dotIndex);		//. 미포함
				//확장자의 파일수
				int expNum = temp.length() - textureFileName.length();

				textureExpName = temp.substr(dotIndex, expNum); //. 포함



				//
				// Diffuse Texture 로딩
				//


				// Diffuse Texture Pass
				texturePath = pathDirectory + textureFileName + textureExpName;

				//Texture 파일이름
				LPDIRECT3DTEXTURE9 pTex =
					RESOURCE_TEXTURE->GetResource(texturePath.c_str());

				if (pTex != NULL)
				{
					//로딩된 texture 를 벡터에 넣어
					DiffuseTextures.push_back(pTex);
				}

				else
				{
					DiffuseTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/diffuseDefault.png"));
				}



				//
				// Normal Texture 로딩
				//
				// Normal Texture Pass
				texturePath = pathDirectory + textureFileName + "_N" + textureExpName;
				pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

				if (pTex != NULL)
				{
					//로딩된 texture 를 벡터에 넣어
					NormalTextures.push_back(pTex);
				}

				else
				{
					NormalTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/normalDefault.png"));
				}

				//
				// Specular Texture 로딩
				//
				// Specular Texture Pass
				texturePath = pathDirectory + textureFileName + "_S" + textureExpName;
				pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

				if (pTex != NULL)
				{
					//로딩된 texture 를 벡터에 넣어
					SpecularTextures.push_back(pTex);
				}

				else
				{
					SpecularTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/specularDefault.png"));
				}



				//
				// Emission Texture 로딩
				//
				// Emission Texture Pass
				texturePath = pathDirectory + textureFileName + "_E" + textureExpName;
				pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

				if (pTex != NULL)
				{
					//로딩된 texture 를 벡터에 넣어
					EmissionTextures.push_back(pTex);
				}

				else
				{
					EmissionTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/emissionDefault.png"));
				}

				//
				// Reflection Texture 로딩
				//

				// Reflection Texture Pass
				texturePath = pathDirectory + textureFileName + "_R" + textureExpName;
				pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

				if (pTex != NULL)
				{
					//로딩된 texture 를 벡터에 넣어
					ReflectMaskTextures.push_back(pTex);
				}

				else
				{
					this->ReflectMaskTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/reflectionDefault.png"));
				}

			}


			//Texture 파일정보가 없다면...
			else
			{
				//기본 Texture 넣는다.
				this->DiffuseTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/diffuseDefault.png"));
				this->NormalTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/normalDefault.png"));
				this->SpecularTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/specularDefault.png"));
				this->EmissionTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/emissionDefault.png"));
				this->ReflectMaskTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/reflectionDefault.png"));


			}




		}


		//사용된 버퍼는 해제 하자
		SAFE_RELEASE(pMaterial);

	}

	//메쉬 최적화


	//인접버퍼가 있다면 메쉬 최적화를 하자
	if (pAdjacency != NULL){


		//메쉬 최적화 ( 인접 버퍼를 이용하여 메쉬를 최적화 한다 )
		pMesh->OptimizeInplace(
			D3DXMESHOPT_ATTRSORT |		//메쉬 서브셋순서대로 정렬 ( DrawSubset 효율을 높인다 )
			D3DXMESHOPT_COMPACT |		//메쉬에서 사용되지 않는 정점과 인덱스 해제
			D3DXMESHOPT_VERTEXCACHE,	//정점 Cache 히트율 높힌다.
			(DWORD*)pAdjacency->GetBufferPointer(),		//인접버퍼 넘겨야 한다.
			NULL,						//최적화를 마지고 결과로 얻는 인접정보 ( 더이상 필요없으면 NULL )
			NULL,						//최적화된 인덱스 정보
			NULL						//최적화된 정점 버퍼 정보 
			);


		//사용된 버퍼는 해제 하자
		SAFE_RELEASE(pAdjacency);

	}


	if (matCorrection == NULL)
	{
		//매개변수가 NULL 이면 항등행렬로 보정한다.
		D3DXMATRIXA16 matIden;
		D3DXMatrixIdentity(&matIden);
		this->MeshCorrection(&matIden);
	}

	else
	{
		//매개변수로 받은 메쉬 보정한다.
		this->MeshCorrection(matCorrection);


	}




}

//해제 함수
void cXMesh_Static::Release()
{
	//로딩된 메시는 해ㅐ제 하자
	SAFE_RELEASE(pMesh);

	SAFE_DELETE(pDefaultBoundBox);


	SAFE_DELETE_ARR(pVertexPos);
	SAFE_DELETE_ARR(pIndexArray);


}

//랜더 함수
void cXMesh_Static::Render(cTransform* pTransform, cCamera* pCam)
{
	//셰이더가 로딩되어있지 않다면 셰이터를 로딩한다.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTechnique("Base");

	//월드 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//뷰
	pMeshEffect->SetMatrix("matViewProjection", &pCam->GetViewProjectionMatrix());

	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//재질수는 메쉬의 Subset 수와 동일하다.
		//따라서 각 서브셋을 랜더링하기 위해 메쉬 수만큼 랜더링을 하면 된다.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{

			pMeshEffect->SetTexture("Normal_Tex", NormalTextures[i]);
			pMeshEffect->SetTexture("Diffuse_Tex", DiffuseTextures[i]);
			pMeshEffect->SetTexture("Specular_Tex", SpecularTextures[i]);
			pMeshEffect->SetTexture("Emission_Tex", EmissionTextures[i]);
			pMeshEffect->SetTexture("Reflect_Tex", ReflectMaskTextures[i]);

			//일반적으로 Effect 의 Begin 이후 부터 셰이더의 변수값(프로퍼티)을
			//변경할 수 없지만.

			//Begin 이 후에 변경하기 위해서는 다음과 같은 코드를 써주면된다.
			pMeshEffect->CommitChanges();


			//해당 서브셋 드로우
			this->pMesh->DrawSubset(i);
		}

		pMeshEffect->EndPass();

	}

	pMeshEffect->End();




}

void cXMesh_Static::RenderShadow(cTransform* pTransform, cCamera* pLightCam)
{
	//셰이더가 로딩되어있지 않다면 셰이터를 로딩한다.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTechnique("Shadow");

	//월드 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//뷰
	pMeshEffect->SetMatrix("matViewProjection", &pLightCam->GetViewProjectionMatrix());

	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//재질수는 메쉬의 Subset 수와 동일하다.
		//따라서 각 서브셋을 랜더링하기 위해 메쉬 수만큼 랜더링을 하면 된다.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{
			//해당 서브셋 드로우
			this->pMesh->DrawSubset(i);
		}

		pMeshEffect->EndPass();
	}

	pMeshEffect->End();
}


void cXMesh_Static::RenderNoiseCutout(cTransform* pTransform, cCamera* pCam, LPDIRECT3DTEXTURE9 noiseTex, float amount)
{

	//셰이더가 로딩되어있지 않다면 셰이터를 로딩한다.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTechnique("NoiseCutout");

	//월드 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//뷰
	pMeshEffect->SetMatrix("matViewProjection", &pCam->GetViewProjectionMatrix());

	//노이즈 Texture
	pMeshEffect->SetTexture("Noise_Tex", noiseTex);


	pMeshEffect->SetFloat("clipAmount", amount);


	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//재질수는 메쉬의 Subset 수와 동일하다.
		//따라서 각 서브셋을 랜더링하기 위해 메쉬 수만큼 랜더링을 하면 된다.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{

			pMeshEffect->SetTexture("Normal_Tex", NormalTextures[i]);
			pMeshEffect->SetTexture("Diffuse_Tex", DiffuseTextures[i]);
			pMeshEffect->SetTexture("Specular_Tex", SpecularTextures[i]);
			pMeshEffect->SetTexture("Emission_Tex", EmissionTextures[i]);
			pMeshEffect->SetTexture("Reflect_Tex", ReflectMaskTextures[i]);

			//일반적으로 Effect 의 Begin 이후 부터 셰이더의 변수값(프로퍼티)을
			//변경할 수 없지만.

			//Begin 이 후에 변경하기 위해서는 다음과 같은 코드를 써주면된다.
			pMeshEffect->CommitChanges();


			//해당 서브셋 드로우
			this->pMesh->DrawSubset(i);
		}

		pMeshEffect->EndPass();

	}

	pMeshEffect->End();



}

void cXMesh_Static::RenderNoiseCutoutShadow(cTransform* pTransform, cCamera* pLightCam, LPDIRECT3DTEXTURE9 noiseTex, float amount)
{

	//셰이더가 로딩되어있지 않다면 셰이터를 로딩한다.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTechnique("ShadowCutout");

	//월드 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//뷰
	pMeshEffect->SetMatrix("matViewProjection", &pLightCam->GetViewProjectionMatrix());


	//노이즈 Texture
	pMeshEffect->SetTexture("Noise_Tex", noiseTex);


	pMeshEffect->SetFloat("clipAmount", amount);



	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//재질수는 메쉬의 Subset 수와 동일하다.
		//따라서 각 서브셋을 랜더링하기 위해 메쉬 수만큼 랜더링을 하면 된다.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{
			//해당 서브셋 드로우
			this->pMesh->DrawSubset(i);
		}

		pMeshEffect->EndPass();
	}

	pMeshEffect->End();
}







void cXMesh_Static::RenderTree(cTransform* pTransform, cCamera* pCam)
{
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTechnique("Tree");

	//월드 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//뷰
	pMeshEffect->SetMatrix("matViewProjection", &pCam->GetViewProjectionMatrix());

	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//재질수는 메쉬의 Subset 수와 동일하다.
		//따라서 각 서브셋을 랜더링하기 위해 메쉬 수만큼 랜더링을 하면 된다.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{

			pMeshEffect->SetTexture("Normal_Tex", NormalTextures[i]);
			pMeshEffect->SetTexture("Diffuse_Tex", DiffuseTextures[i]);
			pMeshEffect->SetTexture("Specular_Tex", SpecularTextures[i]);
			pMeshEffect->SetTexture("Emission_Tex", EmissionTextures[i]);
			pMeshEffect->SetTexture("Reflect_Tex", ReflectMaskTextures[i]);

			//일반적으로 Effect 의 Begin 이후 부터 셰이더의 변수값(프로퍼티)을
			//변경할 수 없지만.

			//Begin 이 후에 변경하기 위해서는 다음과 같은 코드를 써주면된다.
			pMeshEffect->CommitChanges();


			//해당 서브셋 드로우
			this->pMesh->DrawSubset(i);
		}

		pMeshEffect->EndPass();

	}

	pMeshEffect->End();


}

void cXMesh_Static::RenderTreeShadow(cTransform* pTransform, cCamera* pLightCam)
{
	//셰이더가 로딩되어있지 않다면 셰이터를 로딩한다.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTechnique("TreeShadow");

	//월드 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//뷰
	pMeshEffect->SetMatrix("matViewProjection", &pLightCam->GetViewProjectionMatrix());

	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//재질수는 메쉬의 Subset 수와 동일하다.
		//따라서 각 서브셋을 랜더링하기 위해 메쉬 수만큼 랜더링을 하면 된다.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{
			pMeshEffect->SetTexture("Diffuse_Tex", DiffuseTextures[i]);

			//일반적으로 Effect 의 Begin 이후 부터 셰이더의 변수값(프로퍼티)을
			//변경할 수 없지만.

			//Begin 이 후에 변경하기 위해서는 다음과 같은 코드를 써주면된다.
			pMeshEffect->CommitChanges();
			//해당 서브셋 드로우
			this->pMesh->DrawSubset(i);
		}

		pMeshEffect->EndPass();
	}

	pMeshEffect->End();
}






//매개변수로 받은 행렬대로 메쉬를 보정한다.
void cXMesh_Static::MeshCorrection(const D3DXMATRIXA16* pmatCorrection)
{
	//정점 Element 를 얻어 정점 정보를 얻자
	D3DVERTEXELEMENT9 pVerElement[MAX_FVF_DECL_SIZE];

	//pVerElement 배열에는 정점의 요소 정보가 들어가게 된다.
	pMesh->GetDeclaration(pVerElement);

	//정점 Position Offset;
	int positionOffset = -1;

	//정점 Normal Offset;
	int normalOffet = -1;

	//정점 Tangent Offset;
	int tangentOffet = -1;

	//정점 Binormal Offset;
	int binormalOffet = -1;

	for (DWORD i = 0; i < MAX_FVF_DECL_SIZE; i++){

		//끝을 만났다면....
		if (pVerElement[i].Type == D3DDECLTYPE_UNUSED){
			break;
		}


		//정점위치 정보를 만났다면...
		if (pVerElement[i].Usage == D3DDECLUSAGE_POSITION){
			positionOffset = pVerElement[i].Offset;

			



		}


		//정점노말 정보를 만났다면...
		else if (pVerElement[i].Usage == D3DDECLUSAGE_NORMAL){
			normalOffet = pVerElement[i].Offset;
		}

		//정점탄젠트 정보를 만났다면...
		else if (pVerElement[i].Usage == D3DDECLUSAGE_TANGENT){
			tangentOffet = pVerElement[i].Offset;
		}

		//정점바이노말 정보를 만났다면...
		else if (pVerElement[i].Usage == D3DDECLUSAGE_BINORMAL){
			binormalOffet = pVerElement[i].Offset;
		}

	}

	//여기까지 온다면 정점 정보들의 Offset 을 모두 파악한 것이 됨

	//메쉬의 버텍스 갯수
	DWORD verNum = pMesh->GetNumVertices();

	//메쉬의 정점 Decl 정보를 이용하여 정점하나당 크기를 알아내자.
	DWORD verSize = D3DXGetDeclVertexSize(pVerElement, 0);


	//메쉬의 버텍스 버퍼를 Lock 한다
	void* p = NULL;
	pMesh->LockVertexBuffer(0, &p);

	//정점의 최대최소 초기화
	this->Min = D3DXVECTOR3( 100000,  100000,  100000);
	this->Max = D3DXVECTOR3(-100000, -100000, -100000);

	//버텍스 갯수대로 정점배열할당한다.
	this->pVertexPos = new D3DXVECTOR3[verNum];


	//정점 수만큼 돌아 재낀다...
	for (DWORD i = 0; i < verNum; i++){

		//void 형포인터는 + 연산이 안된다....

		//i 번째의 정점의 시작 주소가 들어가있다.
		char* pVertex = ((char*)p) + (i * verSize);


		//정점 위치가 있다면...
		if (positionOffset != -1){

			D3DXVECTOR3* pos = (D3DXVECTOR3*)(pVertex + positionOffset);

			//보정행렬로 변환
			D3DXVec3TransformCoord(
				pos,
				pos,
				pmatCorrection);

			//보정행렬로 변환후 ( 정점의 최대최소를 갱신하자 )
			if (pos->x < Min.x)
				Min.x = pos->x;
			else if (pos->x > Max.x)
				Max.x = pos->x;

			if (pos->y < Min.y)
				Min.y = pos->y;
			else if (pos->y > Max.y)
				Max.y = pos->y;

			if (pos->z < Min.z)
				Min.z = pos->z;
			else if (pos->z > Max.z)
				Max.z = pos->z;

			//정점 기억
			this->pVertexPos[i] = *pos;
		}

		//노말정보가 있다면..
		if (normalOffet != -1){

			D3DXVECTOR3* nor = (D3DXVECTOR3*)(pVertex + normalOffet);
			D3DXVec3TransformNormal(
				nor,
				nor,
				pmatCorrection);

			//스케일 먹어서 노말벡터가 쭐어들수있으니 정규화
			D3DXVec3Normalize(nor, nor);
		}


		//tangent 정보가 있다면.
		if (tangentOffet != -1){

			D3DXVECTOR3* tangent = (D3DXVECTOR3*)(pVertex + tangentOffet);

			D3DXVec3TransformNormal(
				tangent,
				tangent,
				pmatCorrection);

			D3DXVec3Normalize(tangent, tangent);

		}

		//binormal 정보가 있다면
		if (binormalOffet != -1){

			D3DXVECTOR3* binor = (D3DXVECTOR3*)(pVertex + binormalOffet);

			D3DXVec3TransformNormal(
				binor,
				binor,
				pmatCorrection);

			D3DXVec3Normalize(binor, binor);
		}





	}

	pMesh->UnlockVertexBuffer();

	//갱신된 최대 최소로 바운드 박스를 셋팅
	this->pDefaultBoundBox = new cBoundBox();
	this->pDefaultBoundBox->SetMinMax(
		&this->Min, &this->Max);


	//
	// 인덱스 버퍼를 얻는다
	//
	LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
	pMesh->GetIndexBuffer(&pIndexBuffer);

	//인덱스 버퍼에 대한 정보를 얻는다.
	D3DINDEXBUFFER_DESC desc;
	pIndexBuffer->GetDesc(&desc);

	//메쉬의 면 갯수
	DWORD triNum = pMesh->GetNumFaces();

	//면갯수 대로 인덱스 배열생성
	pIndexArray = new DWORD[triNum * 3];

	//16 비트 인덱스라면...
	if (desc.Format == D3DFMT_INDEX16){

		WORD* pIdx = NULL;
		pIndexBuffer->Lock(0, 0, (void**)&pIdx, 0);

		for (DWORD i = 0; i < triNum * 3; i++){

			this->pIndexArray[i] = pIdx[i];
		}

		pIndexBuffer->Unlock();
	}
	//32 비트 인덱스라면...
	else if (desc.Format == D3DFMT_INDEX32)
	{
		DWORD* pIdx = NULL;

		pIndexBuffer->Lock(0, 0, (void**)&pIdx, 0);

		for (DWORD i = 0; i < triNum * 3; i++){

			this->pIndexArray[i] = pIdx[i];
		}
		
		pIndexBuffer->Unlock();

	}

	//얻어온 인덷스 버퍼해제
	SAFE_RELEASE(pIndexBuffer);



	//메쉬의 인덱스 버퍼를 Lock 한다
	//this->pMesh->LockIndexBuffer(0, &p);
	//다썼으면 Unlock
	//this->pMesh->UnlockIndexBuffer();

}

//페이스 인덱스의 노말을 얻는다.
D3DXVECTOR3 cXMesh_Static::GetFaceNormal(DWORD faceIndex)
{
	D3DXVECTOR3 normal(0, 0, 0);

	DWORD i0 = this->pIndexArray[faceIndex * 3 + 0];
	DWORD i1 = this->pIndexArray[faceIndex * 3 + 1];
	DWORD i2 = this->pIndexArray[faceIndex * 3 + 2];

	D3DXVECTOR3 p0 = this->pVertexPos[i0];
	D3DXVECTOR3 p1 = this->pVertexPos[i1];
	D3DXVECTOR3 p2 = this->pVertexPos[i2];

	D3DXVECTOR3 edge1 = p1 - p0;
	D3DXVECTOR3 edge2 = p2 - p0;


	D3DXVec3Cross(&normal, &edge1, &edge2);

	D3DXVec3Normalize(&normal, &normal);


	return normal;

}
void cXMesh_Static::SetTechnique(string name)
{
	pMeshEffect->SetTechnique(name.c_str());
}


void cXMesh_Static::SetCamera(cCamera* pCamera)
{
	//셰이더가 로딩되어있지 않다면 셰이터를 로딩한다.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetMatrix("matViewProjection", &pCamera->GetViewProjectionMatrix());
	pMeshEffect->SetVector("viewPos", &D3DXVECTOR4(pCamera->Transform.GetWorldPosition(), 1));
}
void cXMesh_Static::SetEnvironment(LPDIRECT3DCUBETEXTURE9 pCubeTex)
{
	//셰이더가 로딩되어있지 않다면 셰이터를 로딩한다.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTexture("Environment_Tex", pCubeTex);
}
void cXMesh_Static::SetLight(cTransform* lightTrans, D3DXCOLOR lightColor, float ambient)
{
	//셰이더가 로딩되어있지 않다면 셰이터를 로딩한다.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
			"../Resources/Effects/XMeshStatic.fx");

	pMeshEffect->SetVector("lightDir", &D3DXVECTOR4(lightTrans->GetForward(), 1));
	pMeshEffect->SetVector("lightColor", (D3DXVECTOR4*)(&lightColor));
	pMeshEffect->SetFloat("ambient", ambient);
}

