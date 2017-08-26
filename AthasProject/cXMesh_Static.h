#pragma once

class cTransform;
class cCamera;
class cBoundBox;

class cXMesh_Static
{
private:
	static LPD3DXEFFECT pMeshEffect;

private:
	LPD3DXMESH		pMesh;				//로딩된 메쉬
	DWORD			NumMaterials;		//재질수
	vector<LPDIRECT3DTEXTURE9> DiffuseTextures;
	vector<LPDIRECT3DTEXTURE9> NormalTextures;
	vector<LPDIRECT3DTEXTURE9> SpecularTextures;
	vector<LPDIRECT3DTEXTURE9> EmissionTextures;
	vector<LPDIRECT3DTEXTURE9> ReflectMaskTextures;
	
	//메쉬 Vertex 의 최대 최소
	D3DXVECTOR3		Min;
	D3DXVECTOR3		Max;
	
	D3DXVECTOR3*	pVertexPos;			//정점위치
	DWORD*			pIndexArray;		//인덱스 버퍼

	cBoundBox*		pDefaultBoundBox;

public:
	cXMesh_Static();
	~cXMesh_Static();

	//메쉬 셋팅함수
	void Init(std::string filePath, D3DXMATRIXA16* matCorrection = NULL);

	//해제 함수
	void Release();

	//랜더 함수
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

	//원보 메시를 얻는다.
	LPD3DXMESH GetMesh()
	{
		return this->pMesh;
	}

	//메터리얼수를 얻는다.
	DWORD GetMaterialNum()
	{
		return this->NumMaterials;
	}
	

	//페이스 인덱스의 노말을 얻는다.
	D3DXVECTOR3 GetFaceNormal(DWORD faceIndex);



private:
	//보정행렬대로 메쉬를 수정한다.
	void MeshCorrection(const D3DXMATRIXA16* pmatCorrection);


public:
	static void SetCamera(cCamera* pCamera);
	static void SetTechnique(string name);
	static void SetEnvironment(LPDIRECT3DCUBETEXTURE9 pCubeTex);
	static void SetLight(cTransform* lightTrans, D3DXCOLOR lightColor, float ambient);


};

