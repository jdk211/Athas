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


//�޽� �����Լ�
void cXMesh_Static::Init(std::string filePath, D3DXMATRIXA16* matCorrection)
{


	//���͸��� ������ �������� ���� ����
	LPD3DXBUFFER pMaterial = NULL;

	//���� ���۸� �������� ���� ����
	LPD3DXBUFFER pAdjacency = NULL;


	//XFile �� �ε��Ѵ�.
	D3DXLoadMeshFromX(
		filePath.c_str(),	//���
		D3DXMESH_MANAGED,							//�ɼ��� ������ D3DXMESH_MANAGED
		Device,										//Deivce
		&pAdjacency,								//���� ���� ����
		&pMaterial,									//���͸��� ���� ����
		NULL,										//Effect ���� ���� ( �̰ǾȾ��� ������ NULL )
		&NumMaterials,								//���͸��� ����
		&pMesh);									//�ε��� �޽� ������


	//���� ������ �ε��� �Ǿ��ٸ�...
	if (pMaterial != NULL){


		//XFile �� �̸��� ¥���� �����ϰ� ��� ������.
		string pathDirectory = "";
		int index = filePath.find_last_of('/');//'/' �� �ڿ��� ���� ã�� ã���� �ش� �ε����� �����ϴµ� ��ã���� -1 �� �����Ѵ�...
		if (index == -1){
			index = filePath.find_last_of('\\'); //'/' �� ��ã���� '\\' �� ã�ƺ���
		}

		//¥�� �ε����� ã�Ҵٸ�...
		if (index != -1){

			//filePath �� ��Ʈ���� 0 ��°���� index + 1 ������ ©�� pathDirectory �� ����
			pathDirectory = filePath.substr(0, index + 1);

		}


		string texturePath = "";
		string textureFileName = "";	//Texture �����̸�
		string textureExpName = "";		//Texture Ȯ�����̸�
		string temp;


		//�ε��� ���͸��� ���ۿ��� D3DXMATERIAL ������ �迭 ���������Ͱ� �����ִ�.
		LPD3DXMATERIAL pMaterials = (LPD3DXMATERIAL)pMaterial->GetBufferPointer();

		//�迭�� ������ �ε��� ���Դ� NumMaterials �� ������ �ִ�.
		for (DWORD i = 0; i < NumMaterials; i++){
			D3DXMATERIAL mat = pMaterials[i];

			//mat.pTextureFilename �ش� Material ���� ����ϴ� Texture ���� �̸��� �ִ�.

			if (mat.pTextureFilename != NULL)
			{

				//Texture ���ϸ��� �ϴ��� �ӽ÷�...
				temp = mat.pTextureFilename;
				int dotIndex = temp.find_last_of('.');	//���ϸ��� .��ġ�� �ε����� ã�´�.

				textureFileName = temp.substr(0, dotIndex);		//. ������
				//Ȯ������ ���ϼ�
				int expNum = temp.length() - textureFileName.length();

				textureExpName = temp.substr(dotIndex, expNum); //. ����



				//
				// Diffuse Texture �ε�
				//


				// Diffuse Texture Pass
				texturePath = pathDirectory + textureFileName + textureExpName;

				//Texture �����̸�
				LPDIRECT3DTEXTURE9 pTex =
					RESOURCE_TEXTURE->GetResource(texturePath.c_str());

				if (pTex != NULL)
				{
					//�ε��� texture �� ���Ϳ� �־�
					DiffuseTextures.push_back(pTex);
				}

				else
				{
					DiffuseTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/diffuseDefault.png"));
				}



				//
				// Normal Texture �ε�
				//
				// Normal Texture Pass
				texturePath = pathDirectory + textureFileName + "_N" + textureExpName;
				pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

				if (pTex != NULL)
				{
					//�ε��� texture �� ���Ϳ� �־�
					NormalTextures.push_back(pTex);
				}

				else
				{
					NormalTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/normalDefault.png"));
				}

				//
				// Specular Texture �ε�
				//
				// Specular Texture Pass
				texturePath = pathDirectory + textureFileName + "_S" + textureExpName;
				pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

				if (pTex != NULL)
				{
					//�ε��� texture �� ���Ϳ� �־�
					SpecularTextures.push_back(pTex);
				}

				else
				{
					SpecularTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/specularDefault.png"));
				}



				//
				// Emission Texture �ε�
				//
				// Emission Texture Pass
				texturePath = pathDirectory + textureFileName + "_E" + textureExpName;
				pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

				if (pTex != NULL)
				{
					//�ε��� texture �� ���Ϳ� �־�
					EmissionTextures.push_back(pTex);
				}

				else
				{
					EmissionTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/emissionDefault.png"));
				}

				//
				// Reflection Texture �ε�
				//

				// Reflection Texture Pass
				texturePath = pathDirectory + textureFileName + "_R" + textureExpName;
				pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

				if (pTex != NULL)
				{
					//�ε��� texture �� ���Ϳ� �־�
					ReflectMaskTextures.push_back(pTex);
				}

				else
				{
					this->ReflectMaskTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/reflectionDefault.png"));
				}

			}


			//Texture ���������� ���ٸ�...
			else
			{
				//�⺻ Texture �ִ´�.
				this->DiffuseTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/diffuseDefault.png"));
				this->NormalTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/normalDefault.png"));
				this->SpecularTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/specularDefault.png"));
				this->EmissionTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/emissionDefault.png"));
				this->ReflectMaskTextures.push_back(RESOURCE_TEXTURE->GetResource("./Resources/reflectionDefault.png"));


			}




		}


		//���� ���۴� ���� ����
		SAFE_RELEASE(pMaterial);

	}

	//�޽� ����ȭ


	//�������۰� �ִٸ� �޽� ����ȭ�� ����
	if (pAdjacency != NULL){


		//�޽� ����ȭ ( ���� ���۸� �̿��Ͽ� �޽��� ����ȭ �Ѵ� )
		pMesh->OptimizeInplace(
			D3DXMESHOPT_ATTRSORT |		//�޽� ����¼������ ���� ( DrawSubset ȿ���� ���δ� )
			D3DXMESHOPT_COMPACT |		//�޽����� ������ �ʴ� ������ �ε��� ����
			D3DXMESHOPT_VERTEXCACHE,	//���� Cache ��Ʈ�� ������.
			(DWORD*)pAdjacency->GetBufferPointer(),		//�������� �Ѱܾ� �Ѵ�.
			NULL,						//����ȭ�� ������ ����� ��� �������� ( ���̻� �ʿ������ NULL )
			NULL,						//����ȭ�� �ε��� ����
			NULL						//����ȭ�� ���� ���� ���� 
			);


		//���� ���۴� ���� ����
		SAFE_RELEASE(pAdjacency);

	}


	if (matCorrection == NULL)
	{
		//�Ű������� NULL �̸� �׵���ķ� �����Ѵ�.
		D3DXMATRIXA16 matIden;
		D3DXMatrixIdentity(&matIden);
		this->MeshCorrection(&matIden);
	}

	else
	{
		//�Ű������� ���� �޽� �����Ѵ�.
		this->MeshCorrection(matCorrection);


	}




}

//���� �Լ�
void cXMesh_Static::Release()
{
	//�ε��� �޽ô� �ؤ��� ����
	SAFE_RELEASE(pMesh);

	SAFE_DELETE(pDefaultBoundBox);


	SAFE_DELETE_ARR(pVertexPos);
	SAFE_DELETE_ARR(pIndexArray);


}

//���� �Լ�
void cXMesh_Static::Render(cTransform* pTransform, cCamera* pCam)
{
	//���̴��� �ε��Ǿ����� �ʴٸ� �����͸� �ε��Ѵ�.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTechnique("Base");

	//���� 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//��
	pMeshEffect->SetMatrix("matViewProjection", &pCam->GetViewProjectionMatrix());

	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//�������� �޽��� Subset ���� �����ϴ�.
		//���� �� ������� �������ϱ� ���� �޽� ����ŭ �������� �ϸ� �ȴ�.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{

			pMeshEffect->SetTexture("Normal_Tex", NormalTextures[i]);
			pMeshEffect->SetTexture("Diffuse_Tex", DiffuseTextures[i]);
			pMeshEffect->SetTexture("Specular_Tex", SpecularTextures[i]);
			pMeshEffect->SetTexture("Emission_Tex", EmissionTextures[i]);
			pMeshEffect->SetTexture("Reflect_Tex", ReflectMaskTextures[i]);

			//�Ϲ������� Effect �� Begin ���� ���� ���̴��� ������(������Ƽ)��
			//������ �� ������.

			//Begin �� �Ŀ� �����ϱ� ���ؼ��� ������ ���� �ڵ带 ���ָ�ȴ�.
			pMeshEffect->CommitChanges();


			//�ش� ����� ��ο�
			this->pMesh->DrawSubset(i);
		}

		pMeshEffect->EndPass();

	}

	pMeshEffect->End();




}

void cXMesh_Static::RenderShadow(cTransform* pTransform, cCamera* pLightCam)
{
	//���̴��� �ε��Ǿ����� �ʴٸ� �����͸� �ε��Ѵ�.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTechnique("Shadow");

	//���� 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//��
	pMeshEffect->SetMatrix("matViewProjection", &pLightCam->GetViewProjectionMatrix());

	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//�������� �޽��� Subset ���� �����ϴ�.
		//���� �� ������� �������ϱ� ���� �޽� ����ŭ �������� �ϸ� �ȴ�.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{
			//�ش� ����� ��ο�
			this->pMesh->DrawSubset(i);
		}

		pMeshEffect->EndPass();
	}

	pMeshEffect->End();
}


void cXMesh_Static::RenderNoiseCutout(cTransform* pTransform, cCamera* pCam, LPDIRECT3DTEXTURE9 noiseTex, float amount)
{

	//���̴��� �ε��Ǿ����� �ʴٸ� �����͸� �ε��Ѵ�.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTechnique("NoiseCutout");

	//���� 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//��
	pMeshEffect->SetMatrix("matViewProjection", &pCam->GetViewProjectionMatrix());

	//������ Texture
	pMeshEffect->SetTexture("Noise_Tex", noiseTex);


	pMeshEffect->SetFloat("clipAmount", amount);


	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//�������� �޽��� Subset ���� �����ϴ�.
		//���� �� ������� �������ϱ� ���� �޽� ����ŭ �������� �ϸ� �ȴ�.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{

			pMeshEffect->SetTexture("Normal_Tex", NormalTextures[i]);
			pMeshEffect->SetTexture("Diffuse_Tex", DiffuseTextures[i]);
			pMeshEffect->SetTexture("Specular_Tex", SpecularTextures[i]);
			pMeshEffect->SetTexture("Emission_Tex", EmissionTextures[i]);
			pMeshEffect->SetTexture("Reflect_Tex", ReflectMaskTextures[i]);

			//�Ϲ������� Effect �� Begin ���� ���� ���̴��� ������(������Ƽ)��
			//������ �� ������.

			//Begin �� �Ŀ� �����ϱ� ���ؼ��� ������ ���� �ڵ带 ���ָ�ȴ�.
			pMeshEffect->CommitChanges();


			//�ش� ����� ��ο�
			this->pMesh->DrawSubset(i);
		}

		pMeshEffect->EndPass();

	}

	pMeshEffect->End();



}

void cXMesh_Static::RenderNoiseCutoutShadow(cTransform* pTransform, cCamera* pLightCam, LPDIRECT3DTEXTURE9 noiseTex, float amount)
{

	//���̴��� �ε��Ǿ����� �ʴٸ� �����͸� �ε��Ѵ�.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTechnique("ShadowCutout");

	//���� 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//��
	pMeshEffect->SetMatrix("matViewProjection", &pLightCam->GetViewProjectionMatrix());


	//������ Texture
	pMeshEffect->SetTexture("Noise_Tex", noiseTex);


	pMeshEffect->SetFloat("clipAmount", amount);



	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//�������� �޽��� Subset ���� �����ϴ�.
		//���� �� ������� �������ϱ� ���� �޽� ����ŭ �������� �ϸ� �ȴ�.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{
			//�ش� ����� ��ο�
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

	//���� 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//��
	pMeshEffect->SetMatrix("matViewProjection", &pCam->GetViewProjectionMatrix());

	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//�������� �޽��� Subset ���� �����ϴ�.
		//���� �� ������� �������ϱ� ���� �޽� ����ŭ �������� �ϸ� �ȴ�.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{

			pMeshEffect->SetTexture("Normal_Tex", NormalTextures[i]);
			pMeshEffect->SetTexture("Diffuse_Tex", DiffuseTextures[i]);
			pMeshEffect->SetTexture("Specular_Tex", SpecularTextures[i]);
			pMeshEffect->SetTexture("Emission_Tex", EmissionTextures[i]);
			pMeshEffect->SetTexture("Reflect_Tex", ReflectMaskTextures[i]);

			//�Ϲ������� Effect �� Begin ���� ���� ���̴��� ������(������Ƽ)��
			//������ �� ������.

			//Begin �� �Ŀ� �����ϱ� ���ؼ��� ������ ���� �ڵ带 ���ָ�ȴ�.
			pMeshEffect->CommitChanges();


			//�ش� ����� ��ο�
			this->pMesh->DrawSubset(i);
		}

		pMeshEffect->EndPass();

	}

	pMeshEffect->End();


}

void cXMesh_Static::RenderTreeShadow(cTransform* pTransform, cCamera* pLightCam)
{
	//���̴��� �ε��Ǿ����� �ʴٸ� �����͸� �ε��Ѵ�.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTechnique("TreeShadow");

	//���� 
	pMeshEffect->SetMatrix("matWorld", &pTransform->GetFinalMatrix());

	//��
	pMeshEffect->SetMatrix("matViewProjection", &pLightCam->GetViewProjectionMatrix());

	UINT pass;
	pMeshEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++){

		pMeshEffect->BeginPass(p);

		//�������� �޽��� Subset ���� �����ϴ�.
		//���� �� ������� �������ϱ� ���� �޽� ����ŭ �������� �ϸ� �ȴ�.
		for (DWORD i = 0; i < this->NumMaterials; i++)
		{
			pMeshEffect->SetTexture("Diffuse_Tex", DiffuseTextures[i]);

			//�Ϲ������� Effect �� Begin ���� ���� ���̴��� ������(������Ƽ)��
			//������ �� ������.

			//Begin �� �Ŀ� �����ϱ� ���ؼ��� ������ ���� �ڵ带 ���ָ�ȴ�.
			pMeshEffect->CommitChanges();
			//�ش� ����� ��ο�
			this->pMesh->DrawSubset(i);
		}

		pMeshEffect->EndPass();
	}

	pMeshEffect->End();
}






//�Ű������� ���� ��Ĵ�� �޽��� �����Ѵ�.
void cXMesh_Static::MeshCorrection(const D3DXMATRIXA16* pmatCorrection)
{
	//���� Element �� ��� ���� ������ ����
	D3DVERTEXELEMENT9 pVerElement[MAX_FVF_DECL_SIZE];

	//pVerElement �迭���� ������ ��� ������ ���� �ȴ�.
	pMesh->GetDeclaration(pVerElement);

	//���� Position Offset;
	int positionOffset = -1;

	//���� Normal Offset;
	int normalOffet = -1;

	//���� Tangent Offset;
	int tangentOffet = -1;

	//���� Binormal Offset;
	int binormalOffet = -1;

	for (DWORD i = 0; i < MAX_FVF_DECL_SIZE; i++){

		//���� �����ٸ�....
		if (pVerElement[i].Type == D3DDECLTYPE_UNUSED){
			break;
		}


		//������ġ ������ �����ٸ�...
		if (pVerElement[i].Usage == D3DDECLUSAGE_POSITION){
			positionOffset = pVerElement[i].Offset;

			



		}


		//�����븻 ������ �����ٸ�...
		else if (pVerElement[i].Usage == D3DDECLUSAGE_NORMAL){
			normalOffet = pVerElement[i].Offset;
		}

		//����ź��Ʈ ������ �����ٸ�...
		else if (pVerElement[i].Usage == D3DDECLUSAGE_TANGENT){
			tangentOffet = pVerElement[i].Offset;
		}

		//�������̳븻 ������ �����ٸ�...
		else if (pVerElement[i].Usage == D3DDECLUSAGE_BINORMAL){
			binormalOffet = pVerElement[i].Offset;
		}

	}

	//������� �´ٸ� ���� �������� Offset �� ��� �ľ��� ���� ��

	//�޽��� ���ؽ� ����
	DWORD verNum = pMesh->GetNumVertices();

	//�޽��� ���� Decl ������ �̿��Ͽ� �����ϳ��� ũ�⸦ �˾Ƴ���.
	DWORD verSize = D3DXGetDeclVertexSize(pVerElement, 0);


	//�޽��� ���ؽ� ���۸� Lock �Ѵ�
	void* p = NULL;
	pMesh->LockVertexBuffer(0, &p);

	//������ �ִ��ּ� �ʱ�ȭ
	this->Min = D3DXVECTOR3( 100000,  100000,  100000);
	this->Max = D3DXVECTOR3(-100000, -100000, -100000);

	//���ؽ� ������� �����迭�Ҵ��Ѵ�.
	this->pVertexPos = new D3DXVECTOR3[verNum];


	//���� ����ŭ ���� �糤��...
	for (DWORD i = 0; i < verNum; i++){

		//void �������ʹ� + ������ �ȵȴ�....

		//i ��°�� ������ ���� �ּҰ� ���ִ�.
		char* pVertex = ((char*)p) + (i * verSize);


		//���� ��ġ�� �ִٸ�...
		if (positionOffset != -1){

			D3DXVECTOR3* pos = (D3DXVECTOR3*)(pVertex + positionOffset);

			//������ķ� ��ȯ
			D3DXVec3TransformCoord(
				pos,
				pos,
				pmatCorrection);

			//������ķ� ��ȯ�� ( ������ �ִ��ּҸ� �������� )
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

			//���� ���
			this->pVertexPos[i] = *pos;
		}

		//�븻������ �ִٸ�..
		if (normalOffet != -1){

			D3DXVECTOR3* nor = (D3DXVECTOR3*)(pVertex + normalOffet);
			D3DXVec3TransformNormal(
				nor,
				nor,
				pmatCorrection);

			//������ �Ծ �븻���Ͱ� ����������� ����ȭ
			D3DXVec3Normalize(nor, nor);
		}


		//tangent ������ �ִٸ�.
		if (tangentOffet != -1){

			D3DXVECTOR3* tangent = (D3DXVECTOR3*)(pVertex + tangentOffet);

			D3DXVec3TransformNormal(
				tangent,
				tangent,
				pmatCorrection);

			D3DXVec3Normalize(tangent, tangent);

		}

		//binormal ������ �ִٸ�
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

	//���ŵ� �ִ� �ּҷ� �ٿ�� �ڽ��� ����
	this->pDefaultBoundBox = new cBoundBox();
	this->pDefaultBoundBox->SetMinMax(
		&this->Min, &this->Max);


	//
	// �ε��� ���۸� ��´�
	//
	LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
	pMesh->GetIndexBuffer(&pIndexBuffer);

	//�ε��� ���ۿ� ���� ������ ��´�.
	D3DINDEXBUFFER_DESC desc;
	pIndexBuffer->GetDesc(&desc);

	//�޽��� �� ����
	DWORD triNum = pMesh->GetNumFaces();

	//�鰹�� ��� �ε��� �迭����
	pIndexArray = new DWORD[triNum * 3];

	//16 ��Ʈ �ε������...
	if (desc.Format == D3DFMT_INDEX16){

		WORD* pIdx = NULL;
		pIndexBuffer->Lock(0, 0, (void**)&pIdx, 0);

		for (DWORD i = 0; i < triNum * 3; i++){

			this->pIndexArray[i] = pIdx[i];
		}

		pIndexBuffer->Unlock();
	}
	//32 ��Ʈ �ε������...
	else if (desc.Format == D3DFMT_INDEX32)
	{
		DWORD* pIdx = NULL;

		pIndexBuffer->Lock(0, 0, (void**)&pIdx, 0);

		for (DWORD i = 0; i < triNum * 3; i++){

			this->pIndexArray[i] = pIdx[i];
		}
		
		pIndexBuffer->Unlock();

	}

	//���� �ΉK�� ��������
	SAFE_RELEASE(pIndexBuffer);



	//�޽��� �ε��� ���۸� Lock �Ѵ�
	//this->pMesh->LockIndexBuffer(0, &p);
	//�ٽ����� Unlock
	//this->pMesh->UnlockIndexBuffer();

}

//���̽� �ε����� �븻�� ��´�.
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
	//���̴��� �ε��Ǿ����� �ʴٸ� �����͸� �ε��Ѵ�.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetMatrix("matViewProjection", &pCamera->GetViewProjectionMatrix());
	pMeshEffect->SetVector("viewPos", &D3DXVECTOR4(pCamera->Transform.GetWorldPosition(), 1));
}
void cXMesh_Static::SetEnvironment(LPDIRECT3DCUBETEXTURE9 pCubeTex)
{
	//���̴��� �ε��Ǿ����� �ʴٸ� �����͸� �ε��Ѵ�.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
		"./Resources/Effects2/XMeshStatic.fx");

	pMeshEffect->SetTexture("Environment_Tex", pCubeTex);
}
void cXMesh_Static::SetLight(cTransform* lightTrans, D3DXCOLOR lightColor, float ambient)
{
	//���̴��� �ε��Ǿ����� �ʴٸ� �����͸� �ε��Ѵ�.
	if (pMeshEffect == NULL)
		pMeshEffect = RESOURCE_EFFECT->GetResource(
			"../Resources/Effects/XMeshStatic.fx");

	pMeshEffect->SetVector("lightDir", &D3DXVECTOR4(lightTrans->GetForward(), 1));
	pMeshEffect->SetVector("lightColor", (D3DXVECTOR4*)(&lightColor));
	pMeshEffect->SetFloat("ambient", ambient);
}

