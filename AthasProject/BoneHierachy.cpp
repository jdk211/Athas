#include "stdafx.h"
#include "BoneHierachy.h"
#include "cXMesh_Skinned.h"

void BONE_HIERARCHY::SetLoadPath(std::string filePath)
{
	int pathIndex = filePath.find_last_of("/");
	if (pathIndex == -1)
	{
		pathIndex = filePath.find_last_of("\\");

		//��������´ٸ� filePath ��ΰ� ���ԵǾ����� �ʴ�
		if (pathIndex == -1)
		{
			m_pFileName = filePath;
			m_pFilePath.clear();
			return;
		}
	}
	m_pFileName = filePath.substr(pathIndex + 1, filePath.size());
	m_pFilePath = filePath.substr(0, pathIndex + 1);
}


void BONE_HIERARCHY::SetSkinnedMesh(cXMesh_Skinned* pSkinnedMesh)
{
	this->m_pSkinnedMesh = pSkinnedMesh;
}






//
// �� ������ ( ����� ) �Ҵ�
//
HRESULT BONE_HIERARCHY::CreateFrame(THIS_
	LPCSTR Name,					//������ �̸� ( �ҷ��� ���� �� �������� �̸��� ���´� )
	LPD3DXFRAME* ppNewFrame			//���Ӱ� �Ҵ�Ǿ� ������ ������ ( �츮�� ���⿡�� BONE �Ҵ��Ͽ� �Ű������� ���� �̸��� �����Ͽ� �Ҵ�����־�� �Ѵ� )
	)
{
	//ppNewFrame ���� �Ѱ��� ���ο� BONE ����
	BONE*		newBone = new BONE;
	ZeroMemory(newBone, sizeof(BONE));	//�޸��ʱ�

	//���̸� ����
	if (Name != NULL)
	{
		int len = strlen(Name);		//���� �˰�
		newBone->Name = new char[len + 1];		//���̸�ŭ �Ҵ��ϰ�
		strcpy_s(newBone->Name, len + 1, Name);	//����
	}

	//��Ʈ���� ���� �ʱ�ȭ
	D3DXMatrixIdentity(&newBone->TransformationMatrix);
	D3DXMatrixIdentity(&newBone->CombinedTransformationMatrix);

	//���� �Ǵ� Transform ���� �ʱ�ȭ
	newBone->pBoneTransform = NULL;
	newBone->pApplyTransform = NULL;

	(*ppNewFrame) = newBone;
	
	return S_OK;
}



//
// ��������( ����� ) �� �޽ø� �Ҵ�
//
HRESULT BONE_HIERARCHY::CreateMeshContainer(THIS_
	LPCSTR Name,										//�Ҵ� �Ǵ� �޽��� �̸�
	CONST D3DXMESHDATA* pMeshData,						//�Ҵ� �Ǵ� �޽��� ������ 
	CONST D3DXMATERIAL* pMaterials,						//�Ҵ� �Ǵ� �޽��� ���͸���
	CONST D3DXEFFECTINSTANCE* pEffectInstance,			//Effect �� ����ϴ� �޽��� ��� Effect �ν��Ͻ� 
	DWORD NumMaterials,									//�Ҵ� �Ǵ� �޽��� ������ 
	CONST DWORD* pAdjacency,							//�Ҵ� �Ǵ� �޽��� ���� ���� ����
	LPD3DXSKININFO pSkinInfo,							//�Ҵ� �Ǵ� �޽��� ��Ű�� ���� ( ���� �����ӿ� ���� ��Ű�� ���� )
	LPD3DXMESHCONTAINER* ppNewMeshContainer				//���Ӱ� �Ҵ�Ǿ� ������ �޽� ( �츮�� ���⿡�� ���� �Ű������� ���� ������� BONE_MESH �� �Ҵ��Ͽ� ���� �� �־�� �Ѵ� )
	)
{
	//ppNewMeshContainer �� �Ѱ��� ���ο� BONEMESH ����
	BONE_MESH* boneMesh = new BONE_MESH;
	ZeroMemory(boneMesh, sizeof(BONE_MESH));

	//�Ű������� ���� �޽� �������� Device �� ��´�.
	LPDIRECT3DDEVICE9 d3dDevice = NULL;		//���� �ʰ� �׳� Device �ᵵ ���� ��? ( ���� Device �� �ε��ҰŴϱ�... ��� Device �� �ϸ� �ִٰ� �����ϸ� �ȵȴ�... )
	pMeshData->pMesh->GetDevice(&d3dDevice);

	//�޽õ����͸� ������.
	boneMesh->MeshData.pMesh = pMeshData->pMesh;
	boneMesh->MeshData.Type = D3DXMESHTYPE_MESH;

	
	//�޽õ������� ���۷��� ī��Ʈ ����
	pMeshData->pMesh->AddRef();
	//���۷��� ī��Ʈ�� ��������� ������ �ʴ´ٸ� �Ű������� ���� pMeshData �� �Լ� ȣ���� ���� �Ǿ� ������!
	//�� �Լ� ȣ���Ŀ��� ���������� �������� 0 �̸� �ڵ����� �����Ǵ� ���̴�.
	//���� ���θ������ BONEMESH �ɹ������Ϳ� pMeshData �����͸� ������ �̴�
	//���۷��� ī��Ʈ ���� ���� ���� �Լ��� �����ڿ��� ������ ���´�.


	//�̸� ����
	if (Name != NULL)
	{
		int len = strlen(Name);
		boneMesh->Name = new char[len + 1];
		strcpy_s(boneMesh->Name, len + 1, Name);
	}


	//�������� ���� ( ���� ������ ��� 3���� ���ϰ� �ȴ� �ֳĸ� �������� �������� 3���ϱ� )
	DWORD numAdjacency = 3 * (boneMesh->MeshData.pMesh->GetNumFaces());		//���������� ��
	boneMesh->pAdjacency = new DWORD[numAdjacency];							//���� ������ ����ŭ ����
	memcpy(boneMesh->pAdjacency, pAdjacency, sizeof(DWORD)* numAdjacency);


	//���͸��� ���� ����
	boneMesh->NumMaterials = NumMaterials;

	string texturePath = "";
	string textureFileName = "";	//Texture �����̸�
	string textureExpName = "";		//Texture Ȯ�����̸�
	string temp;



	//���͸��� �� ��ŭ ����
	for (DWORD i = 0; i < NumMaterials; i++)
	{
		//���͸��� ����ϴ� �̸��� �ִٸ�...
		if (pMaterials[i].pTextureFilename != NULL)
		{
			//Texture ���ϸ��� �ϴ��� �ӽ÷�...
			temp = pMaterials[i].pTextureFilename;
			int dotIndex = temp.find_last_of('.');	//���ϸ��� .��ġ�� �ε����� ã�´�.

			textureFileName = temp.substr(0, dotIndex);		//. ������
			
			//Ȯ������ ���ϼ�
			int expNum = temp.length() - textureFileName.length();

			textureExpName = temp.substr(dotIndex, expNum); //. ����



			//
			// Diffuse Texture �ε�
			//


			// Diffuse Texture Pass
			texturePath = m_pFilePath + textureFileName + textureExpName;

			//Texture �����̸�
			LPDIRECT3DTEXTURE9 pTex =
				RESOURCE_TEXTURE->GetResource(texturePath.c_str());

			if (pTex != NULL)
			{
				//�ε��� texture �� ���Ϳ� �־�
				boneMesh->DiffuseTexs.push_back(pTex);
			}

			else
			{
				boneMesh->DiffuseTexs.push_back(RESOURCE_TEXTURE->GetResource("./Resources/diffuseDefault.png"));
			}



			//
			// Normal Texture �ε�
			//
			// Normal Texture Pass
			texturePath = m_pFilePath + textureFileName + "_N" + textureExpName;
			pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

			if (pTex != NULL)
			{
				//�ε��� texture �� ���Ϳ� �־�
				boneMesh->NormalTexs.push_back(pTex);
			}

			else
			{
				boneMesh->NormalTexs.push_back(RESOURCE_TEXTURE->GetResource("./Resources/normalDefault.png"));
			}

			//
			// Specular Texture �ε�
			//
			// Specular Texture Pass
			texturePath = m_pFilePath + textureFileName + "_S" + textureExpName;
			pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

			if (pTex != NULL)
			{
				//�ε��� texture �� ���Ϳ� �־�
				boneMesh->SpecularTexs.push_back(pTex);
			}

			else
			{
				boneMesh->SpecularTexs.push_back(RESOURCE_TEXTURE->GetResource("./Resources/specularDefault.png"));
			}



			//
			// Emission Texture �ε�
			//
			// Emission Texture Pass
			texturePath = m_pFilePath + textureFileName + "_E" + textureExpName;
			pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

			if (pTex != NULL)
			{
				//�ε��� texture �� ���Ϳ� �־�
				boneMesh->EmissionTexs.push_back(pTex);
			}

			else
			{
				boneMesh->EmissionTexs.push_back(RESOURCE_TEXTURE->GetResource("./Resources/emissionDefault.png"));
			}

			//
			// Reflection Texture �ε�
			//

			// Reflection Texture Pass
			texturePath = m_pFilePath + textureFileName + "_R" + textureExpName;
			pTex = RESOURCE_TEXTURE->GetResource(texturePath.c_str());

			if (pTex != NULL)
			{
				//�ε��� texture �� ���Ϳ� �־�
				boneMesh->ReflectionTexs.push_back(pTex);
			}

			else
			{
				boneMesh->ReflectionTexs.push_back(RESOURCE_TEXTURE->GetResource("./Resources/reflectionDefault.png"));
			}

		}

		//Texture ���������� ���ٸ�...
		else
		{
			//�⺻ Texture �ִ´�.
			boneMesh->DiffuseTexs.push_back(RESOURCE_TEXTURE->GetResource("./Resources/diffuseDefault.png"));
			boneMesh->NormalTexs.push_back(RESOURCE_TEXTURE->GetResource("./Resources/normalDefault.png"));
			boneMesh->SpecularTexs.push_back(RESOURCE_TEXTURE->GetResource("./Resources/specularDefault.png"));
			boneMesh->EmissionTexs.push_back(RESOURCE_TEXTURE->GetResource("./Resources/emissionDefault.png"));
			boneMesh->ReflectionTexs.push_back(RESOURCE_TEXTURE->GetResource("./Resources/reflectionDefault.png"));
		}
	}

	//�Ӽ����̺� ���� ��´�. ( ����� ���� ���� )
	boneMesh->MeshData.pMesh->GetAttributeTable(NULL, &boneMesh->NumAttributesGroup);			//�Ӽ� �׷� ������ ��´�.

	//��Ű�� ���� �� �ִٸ�....
	if (pSkinInfo != NULL)
	{
		//��Ű�� ������ ������´�
		boneMesh->pSkinInfo = pSkinInfo;

		//��Ų���� ���۷��� ī��Ʈ ���� ( ���� �޽õ������� ���۷��� ī��Ʈ�� ������ ���� )
		pSkinInfo->AddRef();

		//�̸޽ÿ� ���Ǵ� ���� ���� ��´�.
		int numBone = pSkinInfo->GetNumBones();

		//���Ǵ� ���� ����ŭ ��� �����Ҵ�
		boneMesh->pBoneOffsetMatices = new D3DXMATRIXA16[numBone];
		//�� Offset ��� ����
		for (int i = 0; i < numBone; i++)
			boneMesh->pBoneOffsetMatices[i] = *(boneMesh->pSkinInfo->GetBoneOffsetMatrix(i));

		//Effect ���� ���Ǵ� ��� �ȷ�Ʈ ���� ��
		UINT iPaletteSize = 35;

		//�⺻������ ����Ǵ� ���� ��ŭ �ȷ�Ʈ�� �Ҵ��ϴµ� Effect �� �ִ� MATRIX_PALETTE_SIZE ���� ����Ǵ� ���� 
		//���� ���ٸ� �ȷ�Ʈ ���� Effect MATRIX_PALETTE_SIZE ���� ���� �Ѵ�.
		boneMesh->NumPaletteEntries = min(iPaletteSize, boneMesh->pSkinInfo->GetNumBones());

		//���� �۾����̾� Mesh��  ����Ǵ� bone�� ���� Effect ���� �����Ǵ� MATRIX_PALETTE_SIZE ������ ���� ���
		//boneMesh->NumPaletteEntries ���� ���߾ Mesh �� SubSet �� ���� �ʿ䰡 �ִ�.
		//�Ʒ��� �Լ��� �׷��� �۾� ���� WorkingMesh �� ���ְ� ���� WorkingMesh �� 
		//���� ����ġ ������ �߰����ش�.

		boneMesh->pSkinInfo->ConvertToIndexedBlendedMesh(
			boneMesh->MeshData.pMesh,		//���� �޽�
			D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,	//�޽� �ɼ�
			boneMesh->NumPaletteEntries,		//�� �ȷ�Ʈ ���� �ѱ��.
			boneMesh->pAdjacency,			//���� ���۸� �ѱ��.
			NULL,							//���ŵ� ���� ���۸� �޾ƿ´� ( �ʿ������ NULL )
			NULL,							//���ŵ� ������(�����ε���) ���۸� �޾ƿ´� ( �ʿ������ NULL )
			NULL,							//���ŵ� ���� ���۸� �޾ƿ´� ( �ʿ������ NULL )
			&boneMesh->MaxNumFaceInfls,		//�� ���ؽ��� ������ �ִ� ���� �ִ� ������ ��´�.
			&boneMesh->NumAttributesGroup,	//���� �ĺ���̼� �Ӽ��׷��� ���� ��´٤�.
			&boneMesh->BufBoneCombos,		//���� �ĺ���̼� ���۸� ��´�.
			&boneMesh->WorkingMesh			//�����޽����� �۾��� �޽�
			);
		//���� �ĺ���̼� �� �޽��� Subset ���� ��� ���� ������ �޴����� ���� ������ ���ϰ� �ȴ�.
		//����Ʈ���� �ε��� = ���ĺ���̼�[Subset].BoneID[�ȷ�Ʈ]


		
		//cMesh_Skinned �� �۾��� ���Ǵ� �ȷ�Ʈ ��� ���� �����ϸ� 
		//�ٽ� ���Ҵ� �Ѵ�.
		if (m_pSkinnedMesh->m_dwWorkingPaletteSize < boneMesh->NumPaletteEntries)
		{
			if (m_pSkinnedMesh->m_pmWorkingPalette)
				delete[] m_pSkinnedMesh->m_pmWorkingPalette;
			
			m_pSkinnedMesh->m_dwWorkingPaletteSize = boneMesh->NumPaletteEntries;
			m_pSkinnedMesh->m_pmWorkingPalette = new D3DXMATRIXA16[boneMesh->NumPaletteEntries];
		}


	}


	//������ ���ϵ� LPD3DXMESHCONTAINER �� ���޽� ����
	*ppNewMeshContainer = boneMesh;

	//���� Device ����
	SAFE_RELEASE(d3dDevice);

	return S_OK;
}





//
// ������ ( ����� ) �� ����
//
HRESULT BONE_HIERARCHY::DestroyFrame(THIS_
	LPD3DXFRAME pFrameToFree							//������ ���������� �ּ�( ���� CreateFrame ���� �Ҵ�� BONE ��ü�� �ּҰ� ���´� ) 
	)
{
	if (pFrameToFree)
	{
		//�̸� �Ҵ�Ȱ� �ۻ쳿....
		SAFE_DELETE_ARR(pFrameToFree->Name);

		//����ü�� �ۻ쳿...
		SAFE_DELETE(pFrameToFree);
	}

	return S_OK;
}


//
// ������ ( ����� ) �޽õ����͸� ����
//
HRESULT BONE_HIERARCHY::DestroyMeshContainer(THIS_
	LPD3DXMESHCONTAINER pMeshContainerToFree			//������ ���޽� �������� �ּ� ( ���� CreateMeshContainer ���� �Ҵ�� BONE_MESH ��ü�� �ּҰ� ���´� ) 
	)
{
	//boneMesh �� ����ȯ
	BONE_MESH* boneMesh = (BONE_MESH*)pMeshContainerToFree;

	SAFE_DELETE_ARR(boneMesh->Name);
	SAFE_DELETE_ARR(boneMesh->pAdjacency);
	
	SAFE_RELEASE(boneMesh->MeshData.pMesh);
	SAFE_RELEASE(boneMesh->WorkingMesh);
	SAFE_RELEASE(boneMesh->pSkinInfo);
	
	SAFE_DELETE_ARR(boneMesh->ppBoneMatrixPtrs);		//�̰� SkinnedMesh ���� �Ҵ��....
	SAFE_DELETE_ARR(boneMesh->pBoneOffsetMatices);
	SAFE_RELEASE(boneMesh->BufBoneCombos);


	SAFE_DELETE(boneMesh);

	return S_OK;

}