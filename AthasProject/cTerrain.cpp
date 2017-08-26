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

//초기화 
HRESULT cTerrain::Init(
	const char* szHeightMapName,		//높이맵 이름
	const char* szTile_0,				// 타일 이미지 0
	const char* szTile_1,				// 타일 이미지 1
	const char* szTile_2,				// 타일 이미지 2
	const char* szTile_3,				// 타일 이미지 3
	const char* szTileSplat,			// 타일 이미지 컨트롤 ( RGB Black )
	const char* heightTxtPath,			// 
	float cellSize,						// 하나의 셀크기
	float heightScale,					// 높이 스케일
	int smoothLevel,					// 스무싱 처리 횟수
	int tileNum							// 타일링 갯수
)
{
	//Terrain Effect 로딩
	this->m_pTerrainEffect = RESOURCE_EFFECT->GetResource("./Resources/Effects2/Terrain.fx");

	//스케일값 대입
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

	//이미지 사이즈 정보
	m_nMapSize = hInfo.biWidth;					// ex) 257
	m_nTileNum = m_nMapSize - 1;				// ex) 257 - 1
	m_nTotalVertex = m_nMapSize * m_nMapSize;	// ex) 257 * 257

												//가로세로 정점 수를 구한다.
	m_nVerNumX = m_nMapSize;		//가로 정점갯수는 높이 맵에 가로 해상도 + 1 과 같다. ( 이유는 쿼드트리쓰려면 정점갯수가 2의N승 + 1 이여야 하기 때문에 )
	m_nVerNumZ = m_nMapSize;		//세로 정점잿수는 높이 맵에 세로 해상도 + 1 과 같다. ( 이유는 쿼드트리쓰려면 정점갯수가 2의N승 + 1 이여야 하기 때문에 )
	m_nTotalVerNum = m_nVerNumX * m_nVerNumZ;		//총 정점 갯수

													//가로세로 셀수
	m_nCellNumX = m_nMapSize - 1;
	m_nCellNumZ = m_nMapSize - 1;
	m_nTotalCellNum = m_nCellNumX * m_nCellNumZ;

	//총 삼각형수는
	m_nTotalTri = m_nTotalCellNum * 2;

	//터레인을 만든다.
	if (FAILED(CreateTerrain(
		smoothLevel,			//스무싱 레벨 ( 이값이 클수록 지형이 부드러워진다 )
		tileNum					//타일 Texture 가 몇개로 나누어질건지 갯수
	))) {
		Release();
		return E_FAIL;
	}

	//터레인 크기
	m_fTerrainSizeX = m_nCellNumX * m_fCellScale;
	m_fTerrainSizeZ = m_nCellNumZ * m_fCellScale;

	//터레인 범위
	m_fTerrainStartX = m_pTerrainVertices[0].pos.x;
	m_fTerrainStartZ = m_pTerrainVertices[0].pos.z;
	m_fTerrainEndX = m_pTerrainVertices[m_nTotalVerNum - 1].pos.x;
	m_fTerrainEndZ = m_pTerrainVertices[m_nTotalVerNum - 1].pos.z;

	//쿼드 트리 생성
	this->m_pQuadTree = new cQuadTree();
	this->m_pQuadTree->Init(
		this->m_pTerrainVertices,
		this->m_nVerNumX);

	//
	// Texture 로딩....
	//
	string fileNames[4];
	fileNames[0] = szTile_0;
	fileNames[1] = szTile_1;
	fileNames[2] = szTile_2;
	fileNames[3] = szTile_3;

	for (int i = 0; i < 4; i++) {

		//Diffuse Texture 경로...
		LPDIRECT3DTEXTURE9 pDiffuse = RESOURCE_TEXTURE->GetResource(fileNames[i]);
		if (pDiffuse == NULL) {
			pDiffuse = RESOURCE_TEXTURE->GetResource("./Resources/diffuseDefault.png");
		}
		m_pTexTile[i].diffuse = pDiffuse;

		string texPath = fileNames[i];
		int dotIndex = texPath.find_last_of('.');				//4
		string nonExp = texPath.substr(0, dotIndex);			//.은 포함안된다... ( 0 부터 4 개 ) aa/t

																//확장자 경로
		string exp = texPath.substr(dotIndex, texPath.length() - nonExp.length());

		//Normal Texture 로딩
		texPath = nonExp + "_N" + exp;
		LPDIRECT3DTEXTURE9 pNormal = RESOURCE_TEXTURE->GetResource(texPath);
		if (pNormal == NULL) {
			pNormal = RESOURCE_TEXTURE->GetResource("./Resources/normalDefault.png");
		}
		m_pTexTile[i].normal = pNormal;

		//Specular Texture 로딩
		texPath = nonExp + "_S" + exp;
		LPDIRECT3DTEXTURE9 pSpecular = RESOURCE_TEXTURE->GetResource(texPath);
		if (pSpecular == NULL) {
			pSpecular = RESOURCE_TEXTURE->GetResource("./Resources/emissionDefault.png");
		}
		m_pTexTile[i].specular = pSpecular;
	}


	//Splat Map 로딩....
	//m_pTexSlat = RESOURCE_TEXTURE->GetResource(szTileSplat);


	return S_OK;
}


//해제
void cTerrain::Release()
{
	SAFE_RELEASE(m_pTerrainVB);
	SAFE_RELEASE(m_pTerrainIB);
	SAFE_RELEASE(m_pTerrainDecl);

	SAFE_DELETE_ARR(m_pTerrainVertices);
	SAFE_DELETE_ARR(m_pTerrainTri);

	SAFE_DELETE(m_pQuadTree);
}

//일반 랜더
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

//그림자 랜더
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

//랜더
void cTerrain::RenderWire(cCamera* pCam)
{
	pCam->UpdateToDevice();		//카메라를 업데이트 학고

								//랜더링 모드 Wire 로....
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

//해당 X, Z 위치의 지형의 높이를 얻는다.
float cTerrain::GetHeight(float x, float z)
{
	//터레인 범위을 넘어가면 0.0 값을 리턴한다
	if (x < m_fTerrainStartX || x > m_fTerrainEndX ||
		z < m_fTerrainStartZ || z > m_fTerrainEndZ) {
		return 0.0f;
	}

	//Terrain 의 좌상단 0 을 기준으로 월드 Terrain 의 상태적 위치를 찾자
	float pX = x - m_fTerrainStartX;
	float pZ = z + m_fTerrainEndZ;

	//해당 위치가 어느 셀에 포함되는지 파악
	pX /= m_fCellScale;
	pZ /= m_fCellScale;

	//해당 위치의 셀 인덱스
	int idxX = static_cast<int>(pX);
	int idxZ = static_cast<int>(pZ);

	//셀의 네군데 정점을 얻는다.
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

	//해당 셸에서의 delta 량을 구한다.
	float dX = pX - static_cast<float>(idxX);
	float dZ = pZ - static_cast<float>(idxZ);

	float fHeight = 0.0f;

	//해당 점이 좌상단에 있니?
	if ((dX + dZ) <= 1.0f)
	{
		float deltaU = rt.y - lt.y;	//lt 정점에서 rt 정점까지의 y 위치의 변위량
		float deltaV = lb.y - lt.y; //lt 정점에서 lb 정점까지의 y 위치의 변위량

		fHeight = lt.y + (deltaU * dX) + (deltaV * dZ);
	}
	//해당 점이 우하단이 있니?
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
	//터레인 범위을 넘어가면 0.0 값을 리턴한다
	if (x < m_fTerrainStartX || x > m_fTerrainEndX ||
		z < m_fTerrainStartZ || z > m_fTerrainEndZ) {
		y = 0.0f;
		return false;
	}

	//Terrain 의 좌상단 0 을 기준으로 월드 Terrain 의 상태적 위치를 찾자
	float pX = x - m_fTerrainStartX;
	float pZ = z + m_fTerrainEndZ;

	//해당 위치가 어느 셀에 포함되는지 파악
	pX /= m_fCellScale;
	pZ /= m_fCellScale;

	//해당 위치의 셀 인덱스
	int idxX = static_cast<int>(pX);
	int idxZ = static_cast<int>(pZ);

	//셀의 네군데 정점을 얻는다.
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


	//해당 셸에서의 delta 량을 구한다.
	float dX = pX - static_cast<float>(idxX);
	float dZ = pZ - static_cast<float>(idxZ);

	float fHeight = 0.0f;

	//해당 점이 좌상단에 있니?
	if ((dX + dZ) <= 1.0f)
	{
		float deltaU = rt.y - lt.y;	//lt 정점에서 rt 정점까지의 y 위치의 변위량
		float deltaV = lb.y - lt.y; //lt 정점에서 lb 정점까지의 y 위치의 변위량

		y = lt.y + (deltaU * dX) + (deltaV * dZ);
	}
	//해당 점이 우하단이 있니?
	else
	{
		float deltaU = lb.y - rb.y;
		float deltaV = rt.y - rb.y;

		y = rb.y + (deltaU * (1 - dX)) + (deltaV * (1 - dZ));
	}

	return true;
}

//해당 X, Z 위치의 경사 벡터를 얻는다.
bool cTerrain::GetSlant(D3DXVECTOR3* pOut, float gravityPower, float x, float z)
{

	//터레인 범위을 넘어가면 false 값을 리턴한다
	if (x < m_fTerrainStartX || x > m_fTerrainEndX ||
		z > m_fTerrainStartZ || z < m_fTerrainEndZ) {
		return false;
	}

	//Terrain 의 좌상단 0 을 기준으로 월드 Terrain 의 상태적 위치를 찾자
	float pX = x - m_fTerrainStartX;
	float pZ = -(z + m_fTerrainEndZ);

	//해당 위치가 어느 셀에 포함되는지 파악
	float invCell = 1.0f / m_fCellScale;
	pX *= invCell;
	pZ *= invCell;

	//해당 위치의 셀 인덱스
	int idxX = static_cast<int>(pX);
	int idxZ = static_cast<int>(pZ);

	//셀의 네군데 정점을 얻는다.
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

	//해당 셸에서의 delta 량을 구한다.
	float dX = pX - static_cast<float>(idxX);
	float dZ = pZ - static_cast<float>(idxZ);

	//폴리곤의 노말 벡터 ( 현제 위치의 폴리곤 노말 벡터
	D3DXVECTOR3 normal;

	//해당 점이 좌상단에 있니?
	if ((dX + dZ) <= 1.0f)
	{
		//해당폴리곤의 법선 벡터를 구한다.
		D3DXVECTOR3 edge1 = rt - lt;
		D3DXVECTOR3 edge2 = lb - lt;
		D3DXVec3Cross(&normal, &edge1, &edge2);

	}
	//해당 점이 우하단이 있니?
	else
	{
		//해당폴리곤의 법선 벡터를 구한다.
		D3DXVECTOR3 edge1 = rt - lb;
		D3DXVECTOR3 edge2 = rb - lb;
		D3DXVec3Cross(&normal, &edge1, &edge2);
	}

	//노말은 정규화
	D3DXVec3Normalize(&normal, &normal);

	//중력 방향
	D3DXVECTOR3 gravity(0, -gravityPower, 0);

	//경사면의 우측
	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &normal, &gravity);

	//우측벡터에서 법선벡터를 외적한 결과가 경사면의 경사 방향이 된다.
	D3DXVec3Cross(pOut, &right, &normal);


	return true;
}

//Ray 히트 위치를 얻는다.
bool cTerrain::IsIntersectRayMoosic(D3DXVECTOR3* pOut, const LPRay pRay)
{
	bool hit = false;
	float minDistance = 1000000.0f;

	//일단 삼각형 수대로 싹다돈다.
	for (int i = 0; i < this->m_nTotalTri; i++) {

		//해당 삼각형 인덱스
		DWORD i0 = this->m_pTerrainTri[i].dw0;
		DWORD i1 = this->m_pTerrainTri[i].dw1;
		DWORD i2 = this->m_pTerrainTri[i].dw2;

		float dist = 0;
		float u = 0.0f;
		float v = 0.0f;

		checkNum++;
		//삼각형과 레이의 충돌 위치를 알려준다.
		if (D3DXIntersectTri(
			&this->m_pTerrainVertices[i0].pos,		//삼각형 정점 1
			&this->m_pTerrainVertices[i1].pos,		//삼각형 정점 2
			&this->m_pTerrainVertices[i2].pos,		//삼각형 정점 3
			&pRay->origin,			//레이 위치
			&pRay->direction,		//레이방향 ( 절대로 정규화 )
			&u,				//충돌위치 삼각면의 edge1 진행률
			&v,				//충돌위치 삼각면의 edge2 진행률
			&dist			//레이위치와 충돌위치의 거리
		))
		{
			//더 가까운데에서 충돌이 되었다면.. 갱신
			if (minDistance > dist)
			{
				minDistance = dist;
				hit = true;
			}
		}
	}

	if (hit)
	{
		//히트거리 로 히트 위치 구한다.
		(*pOut) = pRay->origin + pRay->direction * minDistance;
	}

	return hit;

}

//Ray 히트 위치를 얻는다.
bool cTerrain::IsIntersectRay(D3DXVECTOR3* pOut, const LPRay pRay)
{
	//결과 값을 받아올 벡터 준비
	static vector<D3DXVECTOR3> hits;
	hits.clear();

	//쿼드트리 를 통해 레이 체크...
	this->m_pQuadTree->GetRayHits(
		&hits, pRay);

	if (hits.size() == 0)
		return false;


	//충돌되었다... ( origin 에서 가장 가까운 놈 으로 hit 위치 )

	//첫번째 충돌지점
	D3DXVECTOR3 hitPos = hits[0];

	//Ray와의 거리
	D3DXVECTOR3 dirToHit = hitPos - pRay->origin;
	float hitDistanceSQ = D3DXVec3LengthSq(&dirToHit);

	for (int i = 1; i < hits.size(); i++)
	{
		D3DXVECTOR3 newHitPos = hits[i];
		dirToHit = newHitPos - pRay->origin;

		float newHitDistanceSQ = D3DXVec3LengthSq(&dirToHit);

		//기존에 저장된 놈보다 더 작다면..
		if (newHitDistanceSQ < hitDistanceSQ) {
			hitDistanceSQ = newHitDistanceSQ;
			hitPos = hits[i];
		}
	}

	*pOut = hitPos;

	return true;
}

/////////////////////////////////////////////////////


//터레인 만들기
HRESULT cTerrain::CreateTerrain(int smooth, int tileNum)
{
	//
	// 정점 위치 구한다. ------------------------------------------------------------
	//

	//타일링 갯수에 따른 간격 ( 정점당 uv 간격 )
	float tileIntervalX = static_cast<float>(tileNum) / m_nCellNumX;
	float tileIntervalY = static_cast<float>(tileNum) / m_nCellNumZ;

	//터레인 정점 가지고 있어야 한다.
	m_pTerrainVertices = new TERRAINVERTEX[m_nTotalVerNum];

	float* tmpY = new float[m_nTotalVerNum];

	FILE* openHeight;
	fopen_s(&openHeight, m_sTxtPath, "r");

	for (int i = 0; i < m_nTotalVerNum; i++)
	{
		fscanf_s(openHeight, "%f ", &tmpY[i]);
	}

	//점을 정의
	for (int i = 0; i < m_nTotalVertex; ++i)
	{
		PNTVERTEX v;

		D3DXVECTOR3 pos;

		pos.x = i % m_nMapSize - (m_nMapSize / 2) * m_fCellScale;
		pos.y = tmpY[i];
		pos.z = i / m_nMapSize - (m_nMapSize / 2) * m_fCellScale;

		m_pTerrainVertices[i].pos = pos;
	}

	//점에 순서를 정해준다
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
	//정정위치와 정점 UV 를 계산했음....
	for (int z = 0; z < m_nVerNumZ; z++) {
		for (int x = 0; x < m_nVerNumX; x++) {

			//정점 배열 인덱스
			int idx = z * m_nVerNumX + x;

			//Terrain Tile UV
			D3DXVECTOR2 tileUV;
			tileUV.x = x * tileIntervalX;
			tileUV.y = z * tileIntervalY;

			//Terrain Tile Splating UV
			D3DXVECTOR2 baseUV;
			baseUV.x = x / static_cast<float>(m_nVerNumX - 1);
			baseUV.y = z / static_cast<float>(m_nVerNumZ - 1);

			//정점에 대입
			/*m_pTerrainVertices[idx].pos = pos;*/
			m_pTerrainVertices[idx].binormal = D3DXVECTOR3(0, 0, 0);
			m_pTerrainVertices[idx].tangent = D3DXVECTOR3(0, 0, 0);
			m_pTerrainVertices[idx].normal = D3DXVECTOR3(0, 0, 0);	//아래에서 정점 노말 구할때 더해지니 일단 0 벡터로 초기화
			m_pTerrainVertices[idx].baseUV = baseUV;
			m_pTerrainVertices[idx].tileUV = tileUV;
		}
	}

	SAFE_DELETE_ARR(tmpY);

	//정점 스무싱서리
	this->SmoothTerrain(smooth);

	//
	// 정점 인덱스를 구한다.....
	//
	LPTERRAINTRI pIndices = new tagTERRAINTRI[m_nTotalTri];

	//인덱스 배열 인덱스
	int idx = 0;

	//셀단위로 인덱스 계산
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

			//해당 셀에 대한 정점 인덱스를 얻자
			DWORD _0 = (z + 0) * m_nVerNumX + x;
			DWORD _1 = (z + 1) * m_nVerNumX + x;
			DWORD _2 = (z + 0) * m_nVerNumX + x + 1;
			DWORD _3 = (z + 1) * m_nVerNumX + x + 1;

			//셀의 삼각형 하나
			pIndices[idx].dw0 = _0;
			pIndices[idx].dw1 = _1;
			pIndices[idx].dw2 = _2;

			idx++;

			//셀의 삼각형 하나
			pIndices[idx].dw0 = _3;
			pIndices[idx].dw1 = _2;
			pIndices[idx].dw2 = _1;
			idx++;
		}
	}

	//
	// 노말이랑 Binormal 이랑 Tangent 계산하자...
	//
	D3DXVECTOR3* poses = new D3DXVECTOR3[m_nTotalVerNum];		//정점위치
	D3DXVECTOR2* uvs = new D3DXVECTOR2[m_nTotalVerNum];
	DWORD* indices = (DWORD*)pIndices;

	D3DXVECTOR3* normals = new D3DXVECTOR3[m_nTotalVerNum];
	D3DXVECTOR3* tangents = new D3DXVECTOR3[m_nTotalVerNum];
	D3DXVECTOR3* binormals = new D3DXVECTOR3[m_nTotalVerNum];

	//정점위치 및 UV 대입
	for (int i = 0; i < this->m_nTotalVerNum; i++) {
		poses[i] = this->m_pTerrainVertices[i].pos;
		uvs[i] = this->m_pTerrainVertices[i].baseUV;
	}

	//노말계산
	ComputeNormal(
		normals,
		poses,
		m_nTotalVerNum,
		indices,
		m_nTotalTri * 3);

	//탄젠트 바이노말 계산
	ComputeTangentAngBinormal(
		tangents,
		binormals,
		poses,
		normals,
		uvs,
		indices,
		m_nTotalTri,
		m_nTotalVerNum);

	//계산된거 대입
	for (int i = 0; i < this->m_nTotalVerNum; i++) {
		this->m_pTerrainVertices[i].normal = normals[i];
		this->m_pTerrainVertices[i].binormal = binormals[i];
		this->m_pTerrainVertices[i].tangent = tangents[i];
	}

	//임시로 만들어진 배열 턴다.
	SAFE_DELETE_ARR(poses);
	SAFE_DELETE_ARR(normals);
	SAFE_DELETE_ARR(tangents);
	SAFE_DELETE_ARR(binormals);
	SAFE_DELETE_ARR(uvs);

	//
	// 버퍼 생성
	//
	//정점버퍼 만든다.
	Device->CreateVertexBuffer(
		sizeof(TERRAINVERTEX)* m_nTotalVerNum,
		D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&m_pTerrainVB,
		0);

	//만들어진 정점 버퍼를 Lock 하여 터레인 정점 값을 쓴다.
	void* p = NULL;
	m_pTerrainVB->Lock(0, 0, &p, 0);
	memcpy(p, m_pTerrainVertices, sizeof(TERRAINVERTEX)* m_nTotalVerNum);
	m_pTerrainVB->Unlock();

	//인덱스 버퍼 만든다.
	Device->CreateIndexBuffer(
		sizeof(TERRAINTRI)* m_nTotalTri,
		D3DUSAGE_WRITEONLY,				//D3DUSAGE_WRITEONLY 예는 락안하고 고정정으로 버퍼내용이 유지될때 최고성능을 발휘, D3DUSAGE_DYNAMIC 락 을 자주하여 버퍼 내용이 바뀌는 경우
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,				//D3DUSAGE_DYNAMIC 을 쓰려면 무조건 D3DPOOL_DEFAULT
		&m_pTerrainIB,
		0);
	//인덱스 버퍼에 값을 쓴다.

	m_pTerrainIB->Lock(0, 0, &p, 0);
	memcpy(p, pIndices, sizeof(TERRAINTRI)* m_nTotalTri);
	m_pTerrainIB->Unlock();

	//
	// 정점 선언부를 만든다.
	//

	//정점의 형태를 알려주는 배열
	D3DVERTEXELEMENT9 vertElement[7];			//배열을 정점정보 갯수 + 1

												//Position 
	vertElement[0].Stream = 0;							//Stream 은 0
	vertElement[0].Offset = 0;							//메모리의 시작 Byte 단위 0
	vertElement[0].Type = D3DDECLTYPE_FLOAT3;			//자료의 타입
	vertElement[0].Method = D3DDECLMETHOD_DEFAULT;		//일단 무조건  D3DDECLMETHOD_DEFAULT
	vertElement[0].Usage = D3DDECLUSAGE_POSITION;		//정보 타입
	vertElement[0].UsageIndex = 0;						//UsageIndex 일단 0


														//Normal
	vertElement[1].Stream = 0;					//Stream 은 0
	vertElement[1].Offset = 12;					//메모리의 시작 Byte 단위 0
	vertElement[1].Type = D3DDECLTYPE_FLOAT3;	//자료의 타입
	vertElement[1].Method = D3DDECLMETHOD_DEFAULT;		//일단 무조건  D3DDECLMETHOD_DEFAULT
	vertElement[1].Usage = D3DDECLUSAGE_NORMAL;		//정보 타입
	vertElement[1].UsageIndex = 0;						//UsageIndex 일단 0

														//BiNormal
	vertElement[2].Stream = 0;					//Stream 은 0
	vertElement[2].Offset = 24;					//메모리의 시작 Byte 단위 0
	vertElement[2].Type = D3DDECLTYPE_FLOAT3;	//자료의 타입
	vertElement[2].Method = D3DDECLMETHOD_DEFAULT;		//일단 무조건  D3DDECLMETHOD_DEFAULT
	vertElement[2].Usage = D3DDECLUSAGE_BINORMAL;		//정보 타입
	vertElement[2].UsageIndex = 0;						//UsageIndex 일단 0

														//Tangent
	vertElement[3].Stream = 0;					//Stream 은 0
	vertElement[3].Offset = 36;					//메모리의 시작 Byte 단위 0
	vertElement[3].Type = D3DDECLTYPE_FLOAT3;	//자료의 타입
	vertElement[3].Method = D3DDECLMETHOD_DEFAULT;		//일단 무조건  D3DDECLMETHOD_DEFAULT
	vertElement[3].Usage = D3DDECLUSAGE_TANGENT;		//정보 타입
	vertElement[3].UsageIndex = 0;						//UsageIndex 일단 0


														//BaseUV
	vertElement[4].Stream = 0;						//Stream 은 0
	vertElement[4].Offset = 48;						//메모리의 시작 Byte 단위 0
	vertElement[4].Type = D3DDECLTYPE_FLOAT2;		//자료의 타입
	vertElement[4].Method = D3DDECLMETHOD_DEFAULT;		//일단 무조건  D3DDECLMETHOD_DEFAULT
	vertElement[4].Usage = D3DDECLUSAGE_TEXCOORD;		//정보 타입
	vertElement[4].UsageIndex = 0;						//UsageIndex 일단 0


														//tileUV
	vertElement[5].Stream = 0;						//Stream 은 0
	vertElement[5].Offset = 56;						//메모리의 시작 Byte 단위 0
	vertElement[5].Type = D3DDECLTYPE_FLOAT2;		//자료의 타입
	vertElement[5].Method = D3DDECLMETHOD_DEFAULT;		//일단 무조건  D3DDECLMETHOD_DEFAULT
	vertElement[5].Usage = D3DDECLUSAGE_TEXCOORD;		//정보 타입
	vertElement[5].UsageIndex = 1;						//UsageIndex 두번째 UV 1 


														//더이상 없어
	D3DVERTEXELEMENT9 end = D3DDECL_END(); //{0xFF,0,D3DDECLTYPE_UNUSED,0,0,0}
	vertElement[6] = end;

	//
	// LPDIRECT3DVERTEXDECLARATION9 생성
	//
	Device->CreateVertexDeclaration(
		vertElement,			//앞에서 만든 D3DVERTEXELEMENT9 배열 포인터
		&m_pTerrainDecl			//얻어올 LPDIRECT3DVERTEXDECLARATION9 포인터
	);

	//만들어진 Terrain 삼각 인덱스 배열 물어 놓는다.
	this->m_pTerrainTri = pIndices;

	return S_OK;
}


//터레인 스무싱
void cTerrain::SmoothTerrain(int passed)
{
	//평균 높이값 정보에대한 배열
	float* smooth = new float[m_nTotalVerNum];

	for (int p = 0; p < passed; p++)
	{
		for (int z = 0; z < m_nVerNumZ; z++) {
			for (int x = 0; x < m_nVerNumX; x++) {

				int adjacentSections = 0;		//몇개의 정점과 평균값을 내니?
				float totalSections = 0.0f;		//주변의 정점 높이 총합은 얼마니?


												//왼쪽체크
				if ((x - 1) > 0) {
					totalSections += m_pTerrainVertices[(z * m_nVerNumX) + (x - 1)].pos.y;
					adjacentSections++;


					//왼쪽 상단
					if ((z - 1) > 0) {
						totalSections += m_pTerrainVertices[((z - 1) * m_nVerNumX) + (x - 1)].pos.y;
						adjacentSections++;
					}


					//왼쪽 하단
					if ((z + 1) < m_nVerNumZ) {
						totalSections += m_pTerrainVertices[((z + 1) * m_nVerNumX) + (x - 1)].pos.y;
						adjacentSections++;
					}

				}


				//오른쪽 체크
				if ((x + 1) < m_nVerNumX) {
					totalSections += m_pTerrainVertices[(z * m_nVerNumX) + (x + 1)].pos.y;
					adjacentSections++;

					//오른쪽 상단
					if ((z - 1) > 0) {
						totalSections += m_pTerrainVertices[((z - 1) * m_nVerNumX) + (x + 1)].pos.y;
						adjacentSections++;
					}

					//오른쪽 하단 
					if ((z + 1) < m_nVerNumZ) {
						totalSections += m_pTerrainVertices[((z + 1) * m_nVerNumX) + (x + 1)].pos.y;
						adjacentSections++;
					}

				}

				//상단
				if ((z - 1) > 0)
				{
					totalSections += m_pTerrainVertices[((z - 1) * m_nVerNumX) + x].pos.y;
					adjacentSections++;
				}

				//하단
				if ((z + 1) < m_nVerNumZ)
				{
					totalSections += m_pTerrainVertices[((z + 1) * m_nVerNumX) + x].pos.y;
					adjacentSections++;
				}


				//주변 평균
				float jubyunAvg = (totalSections / adjacentSections);

				float centerY = m_pTerrainVertices[(z * m_nVerNumX) + x].pos.y;

				smooth[(z * m_nVerNumX) + x] = (centerY + jubyunAvg) * 0.5f;


			}
		}

		//위에서 계산된 y 스무싱 적용
		for (int i = 0; i < m_nTotalVerNum; i++) {
			m_pTerrainVertices[i].pos.y = smooth[i];
		}

	}

	SAFE_DELETE_ARR(smooth);

}