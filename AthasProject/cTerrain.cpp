#include "stdafx.h"
#include "cTerrain.h"
#include "cQuadTree.h"

cTerrain::cTerrain()
	: m_pTerrainVB(NULL), m_pTerrainIB(NULL),
	m_pTerrainDecl(NULL), m_pTerrainVertices(NULL), m_pTerrainTri(NULL), m_pQuadTree(NULL)
{
}

cTerrain::~cTerrain()
{
}

//�ʱ�ȭ 
HRESULT cTerrain::Init(
	const char* szHeightMapName,		//���̸� �̸�
	const char* szTile_0,				// Ÿ�� �̹��� 0
	const char* szTile_1,				// Ÿ�� �̹��� 1
	const char* szTile_2,				// Ÿ�� �̹��� 2
	const char* szTile_3,				// Ÿ�� �̹��� 3
	const char* szTileSplat,			// Ÿ�� �̹��� ��Ʈ�� ( RGB Black )
	const char* heightTxtPath,			// 
	float cellSize,						// �ϳ��� ��ũ��
	float heightScale,					// ���� ������
	int smoothLevel,					// ������ ó�� Ƚ��
	int tileNum							// Ÿ�ϸ� ����
)
{
	//Terrain Effect �ε�
	this->m_pTerrainEffect = RESOURCE_EFFECT->GetResource("./Resources/Effects2/Terrain.fx");

	//�����ϰ� ����
	m_fHeightScale = heightScale;
	m_fCellScale = cellSize;

	m_cpMapPath = "./Resources/MapTool/TownMap.bmp";
	m_sTxtPath = heightTxtPath;

	fopen_s(&infile, m_cpMapPath, "rb");
	if (infile == NULL) { printf("No Image File"); return 0; }

	fread(&hf, sizeof(BITMAPFILEHEADER), 1, infile);

	if (hf.bfType != 0x4D42) exit(1);

	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, infile);

	if (hInfo.biBitCount != 24) { printf("Bad File Format!!"); return 0; }

	fclose(infile);

	//�̹��� ������ ����
	m_nMapSize = hInfo.biWidth;					// ex) 257
	m_nTileNum = m_nMapSize - 1;				// ex) 257 - 1
	m_nTotalVertex = m_nMapSize * m_nMapSize;	// ex) 257 * 257

												//���μ��� ���� ���� ���Ѵ�.
	m_nVerNumX = m_nMapSize;		//���� ���������� ���� �ʿ� ���� �ػ� + 1 �� ����. ( ������ ����Ʈ�������� ���������� 2��N�� + 1 �̿��� �ϱ� ������ )
	m_nVerNumZ = m_nMapSize;		//���� ��������� ���� �ʿ� ���� �ػ� + 1 �� ����. ( ������ ����Ʈ�������� ���������� 2��N�� + 1 �̿��� �ϱ� ������ )
	m_nTotalVerNum = m_nVerNumX * m_nVerNumZ;		//�� ���� ����

													//���μ��� ����
	m_nCellNumX = m_nMapSize - 1;
	m_nCellNumZ = m_nMapSize - 1;
	m_nTotalCellNum = m_nCellNumX * m_nCellNumZ;

	//�� �ﰢ������
	m_nTotalTri = m_nTotalCellNum * 2;

	//�ͷ����� �����.
	if (FAILED(CreateTerrain(
		smoothLevel,			//������ ���� ( �̰��� Ŭ���� ������ �ε巯������ )
		tileNum					//Ÿ�� Texture �� ��� ������������ ����
	))) {
		Release();
		return E_FAIL;
	}

	//�ͷ��� ũ��
	m_fTerrainSizeX = m_nCellNumX * m_fCellScale;
	m_fTerrainSizeZ = m_nCellNumZ * m_fCellScale;

	//�ͷ��� ����
	m_fTerrainStartX = m_pTerrainVertices[0].pos.x;
	m_fTerrainStartZ = m_pTerrainVertices[0].pos.z;
	m_fTerrainEndX = m_pTerrainVertices[m_nTotalVerNum - 1].pos.x;
	m_fTerrainEndZ = m_pTerrainVertices[m_nTotalVerNum - 1].pos.z;

	//���� Ʈ�� ����
	this->m_pQuadTree = new cQuadTree();
	this->m_pQuadTree->Init(
		this->m_pTerrainVertices,
		this->m_nVerNumX);

	//
	// Texture �ε�....
	//
	string fileNames[4];
	fileNames[0] = szTile_0;
	fileNames[1] = szTile_1;
	fileNames[2] = szTile_2;
	fileNames[3] = szTile_3;

	for (int i = 0; i < 4; i++) {

		//Diffuse Texture ���...
		LPDIRECT3DTEXTURE9 pDiffuse = RESOURCE_TEXTURE->GetResource(fileNames[i]);
		if (pDiffuse == NULL) {
			pDiffuse = RESOURCE_TEXTURE->GetResource("./Resources/diffuseDefault.png");
		}
		m_pTexTile[i].diffuse = pDiffuse;

		string texPath = fileNames[i];
		int dotIndex = texPath.find_last_of('.');				//4
		string nonExp = texPath.substr(0, dotIndex);			//.�� ���Ծȵȴ�... ( 0 ���� 4 �� ) aa/t

																//Ȯ���� ���
		string exp = texPath.substr(dotIndex, texPath.length() - nonExp.length());

		//Normal Texture �ε�
		texPath = nonExp + "_N" + exp;
		LPDIRECT3DTEXTURE9 pNormal = RESOURCE_TEXTURE->GetResource(texPath);
		if (pNormal == NULL) {
			pNormal = RESOURCE_TEXTURE->GetResource("./Resources/normalDefault.png");
		}
		m_pTexTile[i].normal = pNormal;

		//Specular Texture �ε�
		texPath = nonExp + "_S" + exp;
		LPDIRECT3DTEXTURE9 pSpecular = RESOURCE_TEXTURE->GetResource(texPath);
		if (pSpecular == NULL) {
			pSpecular = RESOURCE_TEXTURE->GetResource("./Resources/emissionDefault.png");
		}
		m_pTexTile[i].specular = pSpecular;
	}


	//Splat Map �ε�....
	//m_pTexSlat = RESOURCE_TEXTURE->GetResource(szTileSplat);


	return S_OK;
}


//����
void cTerrain::Release()
{
	SAFE_RELEASE(m_pTerrainVB);
	SAFE_RELEASE(m_pTerrainIB);
	SAFE_RELEASE(m_pTerrainDecl);

	SAFE_DELETE_ARR(m_pTerrainVertices);
	SAFE_DELETE_ARR(m_pTerrainTri);

	SAFE_DELETE(m_pQuadTree);
}

//�Ϲ� ����
void cTerrain::Render(cCamera* pCam)
{
	Device->SetRenderState(D3DRS_LIGHTING, false);

	this->m_pTerrainEffect->SetMatrix(
		"matViewProjection", &pCam->GetViewProjectionMatrix());

	this->m_pTerrainEffect->SetTexture("Tile_Diffuse_0", this->m_pTexTile[0].diffuse);
	this->m_pTerrainEffect->SetTexture("Tile_Diffuse_1", this->m_pTexTile[1].diffuse);
	this->m_pTerrainEffect->SetTexture("Tile_Diffuse_2", this->m_pTexTile[2].diffuse);
	this->m_pTerrainEffect->SetTexture("Tile_Diffuse_3", this->m_pTexTile[3].diffuse);

	this->m_pTerrainEffect->SetTexture("Tile_Normal_0", this->m_pTexTile[0].normal);
	this->m_pTerrainEffect->SetTexture("Tile_Normal_1", this->m_pTexTile[1].normal);
	this->m_pTerrainEffect->SetTexture("Tile_Normal_2", this->m_pTexTile[2].normal);
	this->m_pTerrainEffect->SetTexture("Tile_Normal_3", this->m_pTexTile[3].normal);

	this->m_pTerrainEffect->SetTexture("Tile_Specular_0", this->m_pTexTile[0].specular);
	this->m_pTerrainEffect->SetTexture("Tile_Specular_1", this->m_pTexTile[1].specular);
	this->m_pTerrainEffect->SetTexture("Tile_Specular_2", this->m_pTexTile[2].specular);
	this->m_pTerrainEffect->SetTexture("Tile_Specular_3", this->m_pTexTile[3].specular);

	//this->m_pTerrainEffect->SetTexture("Splat_Tex", this->m_pTexSlat);

	this->m_pTerrainEffect->SetTechnique("Terrain");

	UINT pass;
	this->m_pTerrainEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++) {
		this->m_pTerrainEffect->BeginPass(p);

		Device->SetStreamSource(0, this->m_pTerrainVB, 0, sizeof(TERRAINVERTEX));
		Device->SetVertexDeclaration(this->m_pTerrainDecl);
		Device->SetIndices(this->m_pTerrainIB);
		Device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0, 0, this->m_nTotalVerNum, 0, this->m_nTotalTri);


		this->m_pTerrainEffect->EndPass();
	}

	this->m_pTerrainEffect->End();

}

//�׸��� ����
void cTerrain::RenderShadow(cCamera* pLightCam)
{
	this->m_pTerrainEffect->SetMatrix(
		"matViewProjection", &pLightCam->GetViewProjectionMatrix());

	this->m_pTerrainEffect->SetTechnique("TerrainShadow");

	UINT pass;
	this->m_pTerrainEffect->Begin(&pass, 0);

	for (UINT p = 0; p < pass; p++) {
		this->m_pTerrainEffect->BeginPass(p);


		Device->SetStreamSource(0, this->m_pTerrainVB, 0, sizeof(TERRAINVERTEX));
		Device->SetVertexDeclaration(this->m_pTerrainDecl);
		Device->SetIndices(this->m_pTerrainIB);
		Device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0, 0, this->m_nTotalVerNum, 0, this->m_nTotalTri);

		this->m_pTerrainEffect->EndPass();
	}

	this->m_pTerrainEffect->End();

}

//����
void cTerrain::RenderWire(cCamera* pCam)
{
	pCam->UpdateToDevice();		//ī�޶� ������Ʈ �а�

								//������ ��� Wire ��....
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	Device->SetStreamSource(0, this->m_pTerrainVB, 0, sizeof(TERRAINVERTEX));
	Device->SetIndices(this->m_pTerrainIB);
	Device->SetVertexDeclaration(this->m_pTerrainDecl);
	Device->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,
		0,
		0,
		m_nTotalVerNum,
		0,
		m_nTotalTri);

	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

D3DXVECTOR3  cTerrain::GetRandomPos()
{
	float posX = GetRandom(m_fTerrainStartX, m_fTerrainEndX);
	float posZ = GetRandom(m_fTerrainEndZ, m_fTerrainStartZ);

	float posY = GetHeight(posX, posZ);

	return D3DXVECTOR3(posX, posY, posZ);
}

//�ش� X, Z ��ġ�� ������ ���̸� ��´�.
float cTerrain::GetHeight(float x, float z)
{
	//�ͷ��� ������ �Ѿ�� 0.0 ���� �����Ѵ�
	if (x < m_fTerrainStartX || x > m_fTerrainEndX ||
		z < m_fTerrainStartZ || z > m_fTerrainEndZ) {
		return 0.0f;
	}

	//Terrain �� �»�� 0 �� �������� ���� Terrain �� ������ ��ġ�� ã��
	float pX = x - m_fTerrainStartX;
	float pZ = z + m_fTerrainEndZ;

	//�ش� ��ġ�� ��� ���� ���ԵǴ��� �ľ�
	pX /= m_fCellScale;
	pZ /= m_fCellScale;

	//�ش� ��ġ�� �� �ε���
	int idxX = static_cast<int>(pX);
	int idxZ = static_cast<int>(pZ);

	//���� �ױ��� ������ ��´�.
	// lt-----rt
	//  |\    |
	//  | \   |
	//  |  \  |
	//  |   \ |
	//  |    \|
	// lb-----rb
	D3DXVECTOR3 lt = m_pTerrainVertices[idxZ * m_nVerNumX + idxX].pos;
	D3DXVECTOR3 rt = m_pTerrainVertices[idxZ * m_nVerNumX + idxX + 1].pos;
	D3DXVECTOR3 lb = m_pTerrainVertices[(idxZ + 1) * m_nVerNumX + idxX].pos;
	D3DXVECTOR3 rb = m_pTerrainVertices[(idxZ + 1) * m_nVerNumX + idxX + 1].pos;

	//�ش� �п����� delta ���� ���Ѵ�.
	float dX = pX - static_cast<float>(idxX);
	float dZ = pZ - static_cast<float>(idxZ);

	float fHeight = 0.0f;

	//�ش� ���� �»�ܿ� �ִ�?
	if ((dX + dZ) <= 1.0f)
	{
		float deltaU = rt.y - lt.y;	//lt �������� rt ���������� y ��ġ�� ������
		float deltaV = lb.y - lt.y; //lt �������� lb ���������� y ��ġ�� ������

		fHeight = lt.y + (deltaU * dX) + (deltaV * dZ);
	}
	//�ش� ���� ���ϴ��� �ִ�?
	else
	{
		float deltaU = lb.y - rb.y;
		float deltaV = rt.y - rb.y;


		fHeight = rb.y + (deltaU * (1 - dX)) + (deltaV * (1 - dZ));
	}

	return fHeight;
}

bool cTerrain::GetHeight(IN float x, OUT float y, IN float z)
{
	//�ͷ��� ������ �Ѿ�� 0.0 ���� �����Ѵ�
	if (x < m_fTerrainStartX || x > m_fTerrainEndX ||
		z < m_fTerrainStartZ || z > m_fTerrainEndZ) {
		y = 0.0f;
		return false;
	}

	//Terrain �� �»�� 0 �� �������� ���� Terrain �� ������ ��ġ�� ã��
	float pX = x - m_fTerrainStartX;
	float pZ = z + m_fTerrainEndZ;

	//�ش� ��ġ�� ��� ���� ���ԵǴ��� �ľ�
	pX /= m_fCellScale;
	pZ /= m_fCellScale;

	//�ش� ��ġ�� �� �ε���
	int idxX = static_cast<int>(pX);
	int idxZ = static_cast<int>(pZ);

	//���� �ױ��� ������ ��´�.
	// lt-----rt
	//  |\    |
	//  | \   |
	//  |  \  |
	//  |   \ |
	//  |    \|
	// lb-----rb
	D3DXVECTOR3 lt = m_pTerrainVertices[idxZ * m_nVerNumX + idxX].pos;
	D3DXVECTOR3 rt = m_pTerrainVertices[idxZ * m_nVerNumX + idxX + 1].pos;
	D3DXVECTOR3 lb = m_pTerrainVertices[(idxZ + 1) * m_nVerNumX + idxX].pos;
	D3DXVECTOR3 rb = m_pTerrainVertices[(idxZ + 1) * m_nVerNumX + idxX + 1].pos;


	//�ش� �п����� delta ���� ���Ѵ�.
	float dX = pX - static_cast<float>(idxX);
	float dZ = pZ - static_cast<float>(idxZ);

	float fHeight = 0.0f;

	//�ش� ���� �»�ܿ� �ִ�?
	if ((dX + dZ) <= 1.0f)
	{
		float deltaU = rt.y - lt.y;	//lt �������� rt ���������� y ��ġ�� ������
		float deltaV = lb.y - lt.y; //lt �������� lb ���������� y ��ġ�� ������

		y = lt.y + (deltaU * dX) + (deltaV * dZ);
	}
	//�ش� ���� ���ϴ��� �ִ�?
	else
	{
		float deltaU = lb.y - rb.y;
		float deltaV = rt.y - rb.y;

		y = rb.y + (deltaU * (1 - dX)) + (deltaV * (1 - dZ));
	}

	return true;
}

//�ش� X, Z ��ġ�� ��� ���͸� ��´�.
bool cTerrain::GetSlant(D3DXVECTOR3* pOut, float gravityPower, float x, float z)
{

	//�ͷ��� ������ �Ѿ�� false ���� �����Ѵ�
	if (x < m_fTerrainStartX || x > m_fTerrainEndX ||
		z > m_fTerrainStartZ || z < m_fTerrainEndZ) {
		return false;
	}

	//Terrain �� �»�� 0 �� �������� ���� Terrain �� ������ ��ġ�� ã��
	float pX = x - m_fTerrainStartX;
	float pZ = -(z + m_fTerrainEndZ);

	//�ش� ��ġ�� ��� ���� ���ԵǴ��� �ľ�
	float invCell = 1.0f / m_fCellScale;
	pX *= invCell;
	pZ *= invCell;

	//�ش� ��ġ�� �� �ε���
	int idxX = static_cast<int>(pX);
	int idxZ = static_cast<int>(pZ);

	//���� �ױ��� ������ ��´�.
	// lt-----rt
	//  |\    |
	//  | \   |
	//  |  \  |
	//  |   \ |
	//  |	 \|
	// lb-----rb

	D3DXVECTOR3 lt = m_pTerrainVertices[idxZ * m_nVerNumX + idxX].pos;
	D3DXVECTOR3 rt = m_pTerrainVertices[idxZ * m_nVerNumX + idxX + 1].pos;
	D3DXVECTOR3 lb = m_pTerrainVertices[(idxZ + 1) * m_nVerNumX + idxX].pos;
	D3DXVECTOR3 rb = m_pTerrainVertices[(idxZ + 1) * m_nVerNumX + idxX + 1].pos;

	//�ش� �п����� delta ���� ���Ѵ�.
	float dX = pX - static_cast<float>(idxX);
	float dZ = pZ - static_cast<float>(idxZ);

	//�������� �븻 ���� ( ���� ��ġ�� ������ �븻 ����
	D3DXVECTOR3 normal;

	//�ش� ���� �»�ܿ� �ִ�?
	if ((dX + dZ) <= 1.0f)
	{
		//�ش��������� ���� ���͸� ���Ѵ�.
		D3DXVECTOR3 edge1 = rt - lt;
		D3DXVECTOR3 edge2 = lb - lt;
		D3DXVec3Cross(&normal, &edge1, &edge2);

	}
	//�ش� ���� ���ϴ��� �ִ�?
	else
	{
		//�ش��������� ���� ���͸� ���Ѵ�.
		D3DXVECTOR3 edge1 = rt - lb;
		D3DXVECTOR3 edge2 = rb - lb;
		D3DXVec3Cross(&normal, &edge1, &edge2);
	}

	//�븻�� ����ȭ
	D3DXVec3Normalize(&normal, &normal);

	//�߷� ����
	D3DXVECTOR3 gravity(0, -gravityPower, 0);

	//������ ����
	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &normal, &gravity);

	//�������Ϳ��� �������͸� ������ ����� ������ ��� ������ �ȴ�.
	D3DXVec3Cross(pOut, &right, &normal);


	return true;
}

//Ray ��Ʈ ��ġ�� ��´�.
bool cTerrain::IsIntersectRayMoosic(D3DXVECTOR3* pOut, const LPRay pRay)
{
	bool hit = false;
	float minDistance = 1000000.0f;

	//�ϴ� �ﰢ�� ����� �ϴٵ���.
	for (int i = 0; i < this->m_nTotalTri; i++) {

		//�ش� �ﰢ�� �ε���
		DWORD i0 = this->m_pTerrainTri[i].dw0;
		DWORD i1 = this->m_pTerrainTri[i].dw1;
		DWORD i2 = this->m_pTerrainTri[i].dw2;

		float dist = 0;
		float u = 0.0f;
		float v = 0.0f;

		checkNum++;
		//�ﰢ���� ������ �浹 ��ġ�� �˷��ش�.
		if (D3DXIntersectTri(
			&this->m_pTerrainVertices[i0].pos,		//�ﰢ�� ���� 1
			&this->m_pTerrainVertices[i1].pos,		//�ﰢ�� ���� 2
			&this->m_pTerrainVertices[i2].pos,		//�ﰢ�� ���� 3
			&pRay->origin,			//���� ��ġ
			&pRay->direction,		//���̹��� ( ����� ����ȭ )
			&u,				//�浹��ġ �ﰢ���� edge1 �����
			&v,				//�浹��ġ �ﰢ���� edge2 �����
			&dist			//������ġ�� �浹��ġ�� �Ÿ�
		))
		{
			//�� �������� �浹�� �Ǿ��ٸ�.. ����
			if (minDistance > dist)
			{
				minDistance = dist;
				hit = true;
			}
		}
	}

	if (hit)
	{
		//��Ʈ�Ÿ� �� ��Ʈ ��ġ ���Ѵ�.
		(*pOut) = pRay->origin + pRay->direction * minDistance;
	}

	return hit;

}

//Ray ��Ʈ ��ġ�� ��´�.
bool cTerrain::IsIntersectRay(D3DXVECTOR3* pOut, const LPRay pRay)
{
	//��� ���� �޾ƿ� ���� �غ�
	static vector<D3DXVECTOR3> hits;
	hits.clear();

	//����Ʈ�� �� ���� ���� üũ...
	this->m_pQuadTree->GetRayHits(
		&hits, pRay);

	if (hits.size() == 0)
		return false;


	//�浹�Ǿ���... ( origin ���� ���� ����� �� ���� hit ��ġ )

	//ù��° �浹����
	D3DXVECTOR3 hitPos = hits[0];

	//Ray���� �Ÿ�
	D3DXVECTOR3 dirToHit = hitPos - pRay->origin;
	float hitDistanceSQ = D3DXVec3LengthSq(&dirToHit);

	for (int i = 1; i < hits.size(); i++)
	{
		D3DXVECTOR3 newHitPos = hits[i];
		dirToHit = newHitPos - pRay->origin;

		float newHitDistanceSQ = D3DXVec3LengthSq(&dirToHit);

		//������ ����� �𺸴� �� �۴ٸ�..
		if (newHitDistanceSQ < hitDistanceSQ) {
			hitDistanceSQ = newHitDistanceSQ;
			hitPos = hits[i];
		}
	}

	*pOut = hitPos;

	return true;
}

/////////////////////////////////////////////////////


//�ͷ��� �����
HRESULT cTerrain::CreateTerrain(int smooth, int tileNum)
{
	//
	// ���� ��ġ ���Ѵ�. ------------------------------------------------------------
	//

	//Ÿ�ϸ� ������ ���� ���� ( ������ uv ���� )
	float tileIntervalX = static_cast<float>(tileNum) / m_nCellNumX;
	float tileIntervalY = static_cast<float>(tileNum) / m_nCellNumZ;

	//�ͷ��� ���� ������ �־�� �Ѵ�.
	m_pTerrainVertices = new TERRAINVERTEX[m_nTotalVerNum];

	float* tmpY = new float[m_nTotalVerNum];

	FILE* openHeight;
	fopen_s(&openHeight, m_sTxtPath, "r");

	for (int i = 0; i < m_nTotalVerNum; i++)
	{
		fscanf_s(openHeight, "%f ", &tmpY[i]);
	}

	//���� ����
	for (int i = 0; i < m_nTotalVertex; ++i)
	{
		PNTVERTEX v;

		D3DXVECTOR3 pos;

		pos.x = i % m_nMapSize - (m_nMapSize / 2) * m_fCellScale;
		pos.y = tmpY[i];
		pos.z = i / m_nMapSize - (m_nMapSize / 2) * m_fCellScale;

		m_pTerrainVertices[i].pos = pos;
	}

	//���� ������ �����ش�
	for (int z = 0; z < m_nTileNum; ++z)
	{
		for (int x = 0; x < m_nTileNum; ++x)
		{
			int _0 = (z + 0) * m_nMapSize + x;
			int _1 = (z + 1) * m_nMapSize + x;
			int _2 = (z + 0) * m_nMapSize + x + 1;
			int _3 = (z + 1) * m_nMapSize + x + 1;

			m_vecIndex.push_back(_0);
			m_vecIndex.push_back(_1);
			m_vecIndex.push_back(_2);

			m_vecIndex.push_back(_3);
			m_vecIndex.push_back(_2);
			m_vecIndex.push_back(_1);
		}
	}
	//������ġ�� ���� UV �� �������....
	for (int z = 0; z < m_nVerNumZ; z++) {
		for (int x = 0; x < m_nVerNumX; x++) {

			//���� �迭 �ε���
			int idx = z * m_nVerNumX + x;

			//Terrain Tile UV
			D3DXVECTOR2 tileUV;
			tileUV.x = x * tileIntervalX;
			tileUV.y = z * tileIntervalY;

			//Terrain Tile Splating UV
			D3DXVECTOR2 baseUV;
			baseUV.x = x / static_cast<float>(m_nVerNumX - 1);
			baseUV.y = z / static_cast<float>(m_nVerNumZ - 1);

			//������ ����
			/*m_pTerrainVertices[idx].pos = pos;*/
			m_pTerrainVertices[idx].binormal = D3DXVECTOR3(0, 0, 0);
			m_pTerrainVertices[idx].tangent = D3DXVECTOR3(0, 0, 0);
			m_pTerrainVertices[idx].normal = D3DXVECTOR3(0, 0, 0);	//�Ʒ����� ���� �븻 ���Ҷ� �������� �ϴ� 0 ���ͷ� �ʱ�ȭ
			m_pTerrainVertices[idx].baseUV = baseUV;
			m_pTerrainVertices[idx].tileUV = tileUV;
		}
	}

	SAFE_DELETE_ARR(tmpY);

	//���� �����̼���
	this->SmoothTerrain(smooth);

	//
	// ���� �ε����� ���Ѵ�.....
	//
	LPTERRAINTRI pIndices = new tagTERRAINTRI[m_nTotalTri];

	//�ε��� �迭 �ε���
	int idx = 0;

	//�������� �ε��� ���
	for (DWORD z = 0; z < m_nCellNumZ; z++)
	{
		for (DWORD x = 0; x < m_nCellNumX; x++) {


			// lt-----rt
			//  |    /|
			//  |   / |
			//  |  /  |
			//  | /   |
			//  |/    |
			// lb-----rb

			//�ش� ���� ���� ���� �ε����� ����
			DWORD _0 = (z + 0) * m_nVerNumX + x;
			DWORD _1 = (z + 1) * m_nVerNumX + x;
			DWORD _2 = (z + 0) * m_nVerNumX + x + 1;
			DWORD _3 = (z + 1) * m_nVerNumX + x + 1;

			//���� �ﰢ�� �ϳ�
			pIndices[idx].dw0 = _0;
			pIndices[idx].dw1 = _1;
			pIndices[idx].dw2 = _2;

			idx++;

			//���� �ﰢ�� �ϳ�
			pIndices[idx].dw0 = _3;
			pIndices[idx].dw1 = _2;
			pIndices[idx].dw2 = _1;
			idx++;
		}
	}

	//
	// �븻�̶� Binormal �̶� Tangent �������...
	//
	D3DXVECTOR3* poses = new D3DXVECTOR3[m_nTotalVerNum];		//������ġ
	D3DXVECTOR2* uvs = new D3DXVECTOR2[m_nTotalVerNum];
	DWORD* indices = (DWORD*)pIndices;

	D3DXVECTOR3* normals = new D3DXVECTOR3[m_nTotalVerNum];
	D3DXVECTOR3* tangents = new D3DXVECTOR3[m_nTotalVerNum];
	D3DXVECTOR3* binormals = new D3DXVECTOR3[m_nTotalVerNum];

	//������ġ �� UV ����
	for (int i = 0; i < this->m_nTotalVerNum; i++) {
		poses[i] = this->m_pTerrainVertices[i].pos;
		uvs[i] = this->m_pTerrainVertices[i].baseUV;
	}

	//�븻���
	ComputeNormal(
		normals,
		poses,
		m_nTotalVerNum,
		indices,
		m_nTotalTri * 3);

	//ź��Ʈ ���̳븻 ���
	ComputeTangentAngBinormal(
		tangents,
		binormals,
		poses,
		normals,
		uvs,
		indices,
		m_nTotalTri,
		m_nTotalVerNum);

	//���Ȱ� ����
	for (int i = 0; i < this->m_nTotalVerNum; i++) {
		this->m_pTerrainVertices[i].normal = normals[i];
		this->m_pTerrainVertices[i].binormal = binormals[i];
		this->m_pTerrainVertices[i].tangent = tangents[i];
	}

	//�ӽ÷� ������� �迭 �ϴ�.
	SAFE_DELETE_ARR(poses);
	SAFE_DELETE_ARR(normals);
	SAFE_DELETE_ARR(tangents);
	SAFE_DELETE_ARR(binormals);
	SAFE_DELETE_ARR(uvs);

	//
	// ���� ����
	//
	//�������� �����.
	Device->CreateVertexBuffer(
		sizeof(TERRAINVERTEX)* m_nTotalVerNum,
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&m_pTerrainVB,
		0);

	//������� ���� ���۸� Lock �Ͽ� �ͷ��� ���� ���� ����.
	void* p = NULL;
	m_pTerrainVB->Lock(0, 0, &p, 0);
	memcpy(p, m_pTerrainVertices, sizeof(TERRAINVERTEX)* m_nTotalVerNum);
	m_pTerrainVB->Unlock();

	//�ε��� ���� �����.
	Device->CreateIndexBuffer(
		sizeof(TERRAINTRI)* m_nTotalTri,
		D3DUSAGE_WRITEONLY,				//D3DUSAGE_WRITEONLY ���� �����ϰ� ���������� ���۳����� �����ɶ� �ְ����� ����, D3DUSAGE_DYNAMIC �� �� �����Ͽ� ���� ������ �ٲ�� ���
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,				//D3DUSAGE_DYNAMIC �� ������ ������ D3DPOOL_DEFAULT
		&m_pTerrainIB,
		0);
	//�ε��� ���ۿ� ���� ����.

	m_pTerrainIB->Lock(0, 0, &p, 0);
	memcpy(p, pIndices, sizeof(TERRAINTRI)* m_nTotalTri);
	m_pTerrainIB->Unlock();

	//
	// ���� ����θ� �����.
	//

	//������ ���¸� �˷��ִ� �迭
	D3DVERTEXELEMENT9 vertElement[7];			//�迭�� �������� ���� + 1

												//Position 
	vertElement[0].Stream = 0;							//Stream �� 0
	vertElement[0].Offset = 0;							//�޸��� ���� Byte ���� 0
	vertElement[0].Type = D3DDECLTYPE_FLOAT3;			//�ڷ��� Ÿ��
	vertElement[0].Method = D3DDECLMETHOD_DEFAULT;		//�ϴ� ������  D3DDECLMETHOD_DEFAULT
	vertElement[0].Usage = D3DDECLUSAGE_POSITION;		//���� Ÿ��
	vertElement[0].UsageIndex = 0;						//UsageIndex �ϴ� 0


														//Normal
	vertElement[1].Stream = 0;					//Stream �� 0
	vertElement[1].Offset = 12;					//�޸��� ���� Byte ���� 0
	vertElement[1].Type = D3DDECLTYPE_FLOAT3;	//�ڷ��� Ÿ��
	vertElement[1].Method = D3DDECLMETHOD_DEFAULT;		//�ϴ� ������  D3DDECLMETHOD_DEFAULT
	vertElement[1].Usage = D3DDECLUSAGE_NORMAL;		//���� Ÿ��
	vertElement[1].UsageIndex = 0;						//UsageIndex �ϴ� 0

														//BiNormal
	vertElement[2].Stream = 0;					//Stream �� 0
	vertElement[2].Offset = 24;					//�޸��� ���� Byte ���� 0
	vertElement[2].Type = D3DDECLTYPE_FLOAT3;	//�ڷ��� Ÿ��
	vertElement[2].Method = D3DDECLMETHOD_DEFAULT;		//�ϴ� ������  D3DDECLMETHOD_DEFAULT
	vertElement[2].Usage = D3DDECLUSAGE_BINORMAL;		//���� Ÿ��
	vertElement[2].UsageIndex = 0;						//UsageIndex �ϴ� 0

														//Tangent
	vertElement[3].Stream = 0;					//Stream �� 0
	vertElement[3].Offset = 36;					//�޸��� ���� Byte ���� 0
	vertElement[3].Type = D3DDECLTYPE_FLOAT3;	//�ڷ��� Ÿ��
	vertElement[3].Method = D3DDECLMETHOD_DEFAULT;		//�ϴ� ������  D3DDECLMETHOD_DEFAULT
	vertElement[3].Usage = D3DDECLUSAGE_TANGENT;		//���� Ÿ��
	vertElement[3].UsageIndex = 0;						//UsageIndex �ϴ� 0


														//BaseUV
	vertElement[4].Stream = 0;						//Stream �� 0
	vertElement[4].Offset = 48;						//�޸��� ���� Byte ���� 0
	vertElement[4].Type = D3DDECLTYPE_FLOAT2;		//�ڷ��� Ÿ��
	vertElement[4].Method = D3DDECLMETHOD_DEFAULT;		//�ϴ� ������  D3DDECLMETHOD_DEFAULT
	vertElement[4].Usage = D3DDECLUSAGE_TEXCOORD;		//���� Ÿ��
	vertElement[4].UsageIndex = 0;						//UsageIndex �ϴ� 0


														//tileUV
	vertElement[5].Stream = 0;						//Stream �� 0
	vertElement[5].Offset = 56;						//�޸��� ���� Byte ���� 0
	vertElement[5].Type = D3DDECLTYPE_FLOAT2;		//�ڷ��� Ÿ��
	vertElement[5].Method = D3DDECLMETHOD_DEFAULT;		//�ϴ� ������  D3DDECLMETHOD_DEFAULT
	vertElement[5].Usage = D3DDECLUSAGE_TEXCOORD;		//���� Ÿ��
	vertElement[5].UsageIndex = 1;						//UsageIndex �ι�° UV 1 


														//���̻� ����
	D3DVERTEXELEMENT9 end = D3DDECL_END(); //{0xFF,0,D3DDECLTYPE_UNUSED,0,0,0}
	vertElement[6] = end;

	//
	// LPDIRECT3DVERTEXDECLARATION9 ����
	//
	Device->CreateVertexDeclaration(
		vertElement,			//�տ��� ���� D3DVERTEXELEMENT9 �迭 ������
		&m_pTerrainDecl			//���� LPDIRECT3DVERTEXDECLARATION9 ������
	);

	//������� Terrain �ﰢ �ε��� �迭 ���� ���´�.
	this->m_pTerrainTri = pIndices;

	return S_OK;
}


//�ͷ��� ������
void cTerrain::SmoothTerrain(int passed)
{
	//��� ���̰� ���������� �迭
	float* smooth = new float[m_nTotalVerNum];

	for (int p = 0; p < passed; p++)
	{
		for (int z = 0; z < m_nVerNumZ; z++) {
			for (int x = 0; x < m_nVerNumX; x++) {

				int adjacentSections = 0;		//��� ������ ��հ��� ����?
				float totalSections = 0.0f;		//�ֺ��� ���� ���� ������ �󸶴�?


												//����üũ
				if ((x - 1) > 0) {
					totalSections += m_pTerrainVertices[(z * m_nVerNumX) + (x - 1)].pos.y;
					adjacentSections++;


					//���� ���
					if ((z - 1) > 0) {
						totalSections += m_pTerrainVertices[((z - 1) * m_nVerNumX) + (x - 1)].pos.y;
						adjacentSections++;
					}


					//���� �ϴ�
					if ((z + 1) < m_nVerNumZ) {
						totalSections += m_pTerrainVertices[((z + 1) * m_nVerNumX) + (x - 1)].pos.y;
						adjacentSections++;
					}

				}


				//������ üũ
				if ((x + 1) < m_nVerNumX) {
					totalSections += m_pTerrainVertices[(z * m_nVerNumX) + (x + 1)].pos.y;
					adjacentSections++;

					//������ ���
					if ((z - 1) > 0) {
						totalSections += m_pTerrainVertices[((z - 1) * m_nVerNumX) + (x + 1)].pos.y;
						adjacentSections++;
					}

					//������ �ϴ� 
					if ((z + 1) < m_nVerNumZ) {
						totalSections += m_pTerrainVertices[((z + 1) * m_nVerNumX) + (x + 1)].pos.y;
						adjacentSections++;
					}

				}

				//���
				if ((z - 1) > 0)
				{
					totalSections += m_pTerrainVertices[((z - 1) * m_nVerNumX) + x].pos.y;
					adjacentSections++;
				}

				//�ϴ�
				if ((z + 1) < m_nVerNumZ)
				{
					totalSections += m_pTerrainVertices[((z + 1) * m_nVerNumX) + x].pos.y;
					adjacentSections++;
				}


				//�ֺ� ���
				float jubyunAvg = (totalSections / adjacentSections);

				float centerY = m_pTerrainVertices[(z * m_nVerNumX) + x].pos.y;

				smooth[(z * m_nVerNumX) + x] = (centerY + jubyunAvg) * 0.5f;


			}
		}

		//������ ���� y ������ ����
		for (int i = 0; i < m_nTotalVerNum; i++) {
			m_pTerrainVertices[i].pos.y = smooth[i];
		}

	}

	SAFE_DELETE_ARR(smooth);

}