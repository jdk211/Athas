#pragma once

class cTransform;
class cCamera;
class cBoundBox;

class cXMesh_Static
{
private:
	static LPD3DXEFFECT pMeshEffect;

private:
	LPD3DXMESH		pMesh;				//�ε��� �޽�
	DWORD			NumMaterials;		//������
	vector<LPDIRECT3DTEXTURE9> DiffuseTextures;
	vector<LPDIRECT3DTEXTURE9> NormalTextures;
	vector<LPDIRECT3DTEXTURE9> SpecularTextures;
	vector<LPDIRECT3DTEXTURE9> EmissionTextures;
	vector<LPDIRECT3DTEXTURE9> ReflectMaskTextures;
	
	//�޽� Vertex �� �ִ� �ּ�
	D3DXVECTOR3		Min;
	D3DXVECTOR3		Max;
	
	D3DXVECTOR3*	pVertexPos;			//������ġ
	DWORD*			pIndexArray;		//�ε��� ����

	cBoundBox*		pDefaultBoundBox;

public:
	cXMesh_Static();
	~cXMesh_Static();

	//�޽� �����Լ�
	void Init(std::string filePath, D3DXMATRIXA16* matCorrection = NULL);

	//���� �Լ�
	void Release();

	//���� �Լ�
	void Render(cTransform* pTransform, cCamera* pCam);
	void RenderShadow(cTransform* pTransform, cCamera* pLightCam);

	void RenderNoiseCutout(cTransform* pTransform, cCamera* pCam, LPDIRECT3DTEXTURE9 noiseTex, float amount );
	void RenderNoiseCutoutShadow(cTransform* pTransform, cCamera* pCam, LPDIRECT3DTEXTURE9 noiseTex, float amount);


	void RenderTree(cTransform* pTransform, cCamera* pCam);
	void RenderTreeShadow(cTransform* pTransform, cCamera* pLightCam);



	cBoundBox* GetDefaultBound()
	{
		return this->pDefaultBoundBox;
	}

	//���� �޽ø� ��´�.
	LPD3DXMESH GetMesh()
	{
		return this->pMesh;
	}

	//���͸������ ��´�.
	DWORD GetMaterialNum()
	{
		return this->NumMaterials;
	}
	

	//���̽� �ε����� �븻�� ��´�.
	D3DXVECTOR3 GetFaceNormal(DWORD faceIndex);



private:
	//������Ĵ�� �޽��� �����Ѵ�.
	void MeshCorrection(const D3DXMATRIXA16* pmatCorrection);


public:
	static void SetCamera(cCamera* pCamera);
	static void SetTechnique(string name);
	static void SetEnvironment(LPDIRECT3DCUBETEXTURE9 pCubeTex);
	static void SetLight(cTransform* lightTrans, D3DXCOLOR lightColor, float ambient);


};

