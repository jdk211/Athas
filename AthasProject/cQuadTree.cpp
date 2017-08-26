#include "stdafx.h"
#include "cQuadTree.h"


cQuadTree::cQuadTree()
{
}


cQuadTree::~cQuadTree()
{
	SAFE_DELETE(this->m_pChilds[0]);
	SAFE_DELETE(this->m_pChilds[1]);
	SAFE_DELETE(this->m_pChilds[2]);
	SAFE_DELETE(this->m_pChilds[3]);
}


//���� Ʈ�� �ʱ�ȭ
HRESULT cQuadTree::Init(
	cTerrain::LPTERRAINVERTEX pVertices,	//Terrain ���� �迭
	DWORD nVerNumEdge						//QuadTree �� ���� �Ѹ��� ����
)
{
	//Terrain ���� ������ ������
	m_pTerrainVertices = pVertices;

	//�� �ڳ� �ε��� ����Ѵ�.
	m_Corners[CORNER_LT] = 0;
	m_Corners[CORNER_RT] = nVerNumEdge - 1;
	m_Corners[CORNER_LB] = (nVerNumEdge - 1) * nVerNumEdge;
	m_Corners[CORNER_RB] = nVerNumEdge * nVerNumEdge - 1;

	//�ڽ� Ʈ�� �����.
	CreateChildTree();

	return S_OK;

}

HRESULT cQuadTree::Init(LPPNTVERTEX pMapToolVertices, DWORD nVerNumEdge)
{
	//Terrain ���� ������ ������
	m_pMapToolinVertices = pMapToolVertices;

	//�� �ڳ� �ε��� ����Ѵ�.
	m_Corners[CORNER_LT] = 0;
	m_Corners[CORNER_RT] = nVerNumEdge - 1;
	m_Corners[CORNER_LB] = (nVerNumEdge - 1) * nVerNumEdge;
	m_Corners[CORNER_RB] = nVerNumEdge * nVerNumEdge - 1;

	//�ڽ� Ʈ�� �����.
	CreateMapToolChildTree();

	return S_OK;
}

//�ڽ� Ʈ�� �����.
void cQuadTree::CreateChildTree()
{
	//�߽���ġ
	//���ڳ��� ������ ���� ��
	m_CenterPos = (m_pTerrainVertices[m_Corners[0]].pos +
		m_pTerrainVertices[m_Corners[1]].pos +
		m_pTerrainVertices[m_Corners[2]].pos +
		m_pTerrainVertices[m_Corners[3]].pos) * 0.25f;


	//�ƹ��� �ϳ��� �Ÿ��� �������̴�.
	D3DXVECTOR3 dir = m_pTerrainVertices[m_Corners[0]].pos - m_CenterPos;
	float prevLengthSq = D3DXVec3LengthSq(&dir);
	m_Radius = sqrt(prevLengthSq);

	//�ڽ��� �ִ�....
	if ((m_Corners[CORNER_RT] - m_Corners[CORNER_LT]) > 1)
	{
		//���� �ε��� ���
		m_Center = (m_Corners[0] + m_Corners[1] + m_Corners[2] + m_Corners[3]) / 4;
		DWORD topCenter = (m_Corners[CORNER_RT] + m_Corners[CORNER_LT]) / 2;		//��� �߾�
		DWORD leftCenter = (m_Corners[CORNER_LT] + m_Corners[CORNER_LB]) / 2;		//�� �߾�
		DWORD rightCenter = (m_Corners[CORNER_RT] + m_Corners[CORNER_RB]) / 2;	//�� �߾�
		DWORD bottomCenter = (m_Corners[CORNER_RB] + m_Corners[CORNER_LB]) / 2;	//�� �߾�

		//�»�� �ڽ�
		m_pChilds[CORNER_LT] = new cQuadTree;
		m_pChilds[CORNER_LT]->m_Corners[CORNER_LT] = m_Corners[CORNER_LT];
		m_pChilds[CORNER_LT]->m_Corners[CORNER_RT] = topCenter;
		m_pChilds[CORNER_LT]->m_Corners[CORNER_LB] = leftCenter;
		m_pChilds[CORNER_LT]->m_Corners[CORNER_RB] = m_Center;
		m_pChilds[CORNER_LT]->m_pTerrainVertices = m_pTerrainVertices;
		m_pChilds[CORNER_LT]->CreateChildTree();

		//���� �ڽ�
		m_pChilds[CORNER_RT] = new cQuadTree;
		m_pChilds[CORNER_RT]->m_Corners[CORNER_LT] = topCenter;
		m_pChilds[CORNER_RT]->m_Corners[CORNER_RT] = m_Corners[CORNER_RT];
		m_pChilds[CORNER_RT]->m_Corners[CORNER_LB] = m_Center;
		m_pChilds[CORNER_RT]->m_Corners[CORNER_RB] = rightCenter;
		m_pChilds[CORNER_RT]->m_pTerrainVertices = m_pTerrainVertices;
		m_pChilds[CORNER_RT]->CreateChildTree();

		//���ϴ� �ڽ�
		m_pChilds[CORNER_LB] = new cQuadTree;
		m_pChilds[CORNER_LB]->m_Corners[CORNER_LT] = leftCenter;
		m_pChilds[CORNER_LB]->m_Corners[CORNER_RT] = m_Center;
		m_pChilds[CORNER_LB]->m_Corners[CORNER_LB] = m_Corners[CORNER_LB];
		m_pChilds[CORNER_LB]->m_Corners[CORNER_RB] = bottomCenter;
		m_pChilds[CORNER_LB]->m_pTerrainVertices = m_pTerrainVertices;
		m_pChilds[CORNER_LB]->CreateChildTree();

		//���ϴ� �ڽ�
		m_pChilds[CORNER_RB] = new cQuadTree;
		m_pChilds[CORNER_RB]->m_Corners[CORNER_LT] = m_Center;
		m_pChilds[CORNER_RB]->m_Corners[CORNER_RT] = rightCenter;
		m_pChilds[CORNER_RB]->m_Corners[CORNER_LB] = bottomCenter;
		m_pChilds[CORNER_RB]->m_Corners[CORNER_RB] = m_Corners[CORNER_RB];
		m_pChilds[CORNER_RB]->m_pTerrainVertices = m_pTerrainVertices;
		m_pChilds[CORNER_RB]->CreateChildTree();
	}

	//�ڽ��� ����.
	else
	{
		m_Center = -1;
		m_pChilds[0] = NULL;
		m_pChilds[1] = NULL;
		m_pChilds[2] = NULL;
		m_pChilds[3] = NULL;
	}

}

void cQuadTree::CreateMapToolChildTree()
{
	//�߽���ġ
	//���ڳ��� ������ ���� ��
	m_CenterPos = (m_pMapToolinVertices[m_Corners[0]].p +
		m_pMapToolinVertices[m_Corners[1]].p +
		m_pMapToolinVertices[m_Corners[2]].p +
		m_pMapToolinVertices[m_Corners[3]].p) * 0.25f;


	//�ƹ��� �ϳ��� �Ÿ��� �������̴�.
	D3DXVECTOR3 dir = m_pMapToolinVertices[m_Corners[0]].p - m_CenterPos;
	float prevLengthSq = D3DXVec3LengthSq(&dir);
	m_Radius = sqrt(prevLengthSq);

	//�ڽ��� �ִ�....
	if ((m_Corners[CORNER_RT] - m_Corners[CORNER_LT]) > 1)
	{
		//���� �ε��� ���
		m_Center = (m_Corners[0] + m_Corners[1] + m_Corners[2] + m_Corners[3]) / 4;
		DWORD topCenter = (m_Corners[CORNER_RT] + m_Corners[CORNER_LT]) / 2;		//��� �߾�
		DWORD leftCenter = (m_Corners[CORNER_LT] + m_Corners[CORNER_LB]) / 2;		//�� �߾�
		DWORD rightCenter = (m_Corners[CORNER_RT] + m_Corners[CORNER_RB]) / 2;	//�� �߾�
		DWORD bottomCenter = (m_Corners[CORNER_RB] + m_Corners[CORNER_LB]) / 2;	//�� �߾�

																				//�»�� �ڽ�
		m_pChilds[CORNER_LT] = new cQuadTree;
		m_pChilds[CORNER_LT]->m_Corners[CORNER_LT] = m_Corners[CORNER_LT];
		m_pChilds[CORNER_LT]->m_Corners[CORNER_RT] = topCenter;
		m_pChilds[CORNER_LT]->m_Corners[CORNER_LB] = leftCenter;
		m_pChilds[CORNER_LT]->m_Corners[CORNER_RB] = m_Center;
		m_pChilds[CORNER_LT]->m_pMapToolinVertices = m_pMapToolinVertices;
		m_pChilds[CORNER_LT]->CreateMapToolChildTree();

		//���� �ڽ�
		m_pChilds[CORNER_RT] = new cQuadTree;
		m_pChilds[CORNER_RT]->m_Corners[CORNER_LT] = topCenter;
		m_pChilds[CORNER_RT]->m_Corners[CORNER_RT] = m_Corners[CORNER_RT];
		m_pChilds[CORNER_RT]->m_Corners[CORNER_LB] = m_Center;
		m_pChilds[CORNER_RT]->m_Corners[CORNER_RB] = rightCenter;
		m_pChilds[CORNER_RT]->m_pMapToolinVertices = m_pMapToolinVertices;
		m_pChilds[CORNER_RT]->CreateMapToolChildTree();

		//���ϴ� �ڽ�
		m_pChilds[CORNER_LB] = new cQuadTree;
		m_pChilds[CORNER_LB]->m_Corners[CORNER_LT] = leftCenter;
		m_pChilds[CORNER_LB]->m_Corners[CORNER_RT] = m_Center;
		m_pChilds[CORNER_LB]->m_Corners[CORNER_LB] = m_Corners[CORNER_LB];
		m_pChilds[CORNER_LB]->m_Corners[CORNER_RB] = bottomCenter;
		m_pChilds[CORNER_LB]->m_pMapToolinVertices = m_pMapToolinVertices;
		m_pChilds[CORNER_LB]->CreateMapToolChildTree();

		//���ϴ� �ڽ�
		m_pChilds[CORNER_RB] = new cQuadTree;
		m_pChilds[CORNER_RB]->m_Corners[CORNER_LT] = m_Center;
		m_pChilds[CORNER_RB]->m_Corners[CORNER_RT] = rightCenter;
		m_pChilds[CORNER_RB]->m_Corners[CORNER_LB] = bottomCenter;
		m_pChilds[CORNER_RB]->m_Corners[CORNER_RB] = m_Corners[CORNER_RB];
		m_pChilds[CORNER_RB]->m_pMapToolinVertices = m_pMapToolinVertices;
		m_pChilds[CORNER_RB]->CreateMapToolChildTree();
	}

	//�ڽ��� ����.
	else
	{
		m_Center = -1;
		m_pChilds[0] = NULL;
		m_pChilds[1] = NULL;
		m_pChilds[2] = NULL;
		m_pChilds[3] = NULL;
	}
}


//���� �浹������ ��´�.
void cQuadTree::GetRayHits(
	vector<D3DXVECTOR3>* pHits, const LPRay pRay)
{

	//���� �ٿ��� �浹�ߴ�?
	checkNum++;
	if (PHYSICS_MGR->IsRayHitSphere(pRay, &m_CenterPos, this->m_Radius, 10000.0f, true, NULL, NULL))
	{
		//�ڽ��� �ִ�?
		if (this->m_Center != -1)
		{
			//�ڽ� ���
			for (int i = 0; i < 4; i++)
			{
				m_pChilds[i]->GetRayHits(pHits, pRay);
			}
		}

		//����������?
		else
		{
			//Tri 2�� ��´�.

			// lt-----rt
			//  |    /|
			//  |   / |
			//  |  /  |
			//  | /   |
			//  |/    |
			// lb-----rb


			//���� ������� Ray �� Hit ������ ����
			float dist = 0.0f;

			D3DXVECTOR3 lt = this->m_pTerrainVertices[m_Corners[CORNER_LT]].pos;
			D3DXVECTOR3 rt = this->m_pTerrainVertices[m_Corners[CORNER_RT]].pos;
			D3DXVECTOR3 lb = this->m_pTerrainVertices[m_Corners[CORNER_LB]].pos;
			D3DXVECTOR3 rb = this->m_pTerrainVertices[m_Corners[CORNER_RB]].pos;

			checkNum++;
			//�»�� ������ üũ
			if (D3DXIntersectTri(
				&lt, &rt, &lb,			//������ ���� 3 ��ġ ( �η�� ���� ���� )
				&pRay->origin,		//���� ��ġ
				&pRay->direction,		//���� ����
				NULL, NULL,
				&dist			//��Ʈ�� �Ǿ��ٸ� Origin ���� ���� hit ��ġ������ �Ÿ��� ���´�.
			))
			{
				//��Ʈ����
				D3DXVECTOR3 hitPos = pRay->origin + pRay->direction * dist;

				//Ǫ��
				pHits->push_back(hitPos);

				return;
			}

			checkNum++;
			//���ϴ� ������ üũ
			if (D3DXIntersectTri(
				&lb, &rt, &rb,			//������ ���� 3 ��ġ
				&pRay->origin,		//���� ��ġ
				&pRay->direction,		//���� ����
				NULL, NULL,
				&dist			//��Ʈ�� �Ǿ��ٸ� Origin ���� ���� hit ��ġ������ �Ÿ��� ���´�.
			))
			{
				//��Ʈ����
				D3DXVECTOR3 hitPos = pRay->origin + pRay->direction * dist;

				//Ǫ��
				pHits->push_back(hitPos);

				return;
			}

		}

	}
}

void cQuadTree::GetMapToolRayHits(vector<D3DXVECTOR3>* pHits, const LPRay pRay)
{
	//���� �ٿ��� �浹�ߴ�?
	checkNum++;

	//�ڽ��� �ִ�?
	if (this->m_Center != -1)
	{
		//�ڽ� ���
		for (int i = 0; i < 4; i++)
		{
			m_pChilds[i]->GetMapToolRayHits(pHits, pRay);
		}
	}

	//����������?
	else
	{
		//Tri 2�� ��´�.

		// lt-----rt
		//  |    /|
		//  |   / |
		//  |  /  |
		//  | /   |
		//  |/    |
		// lb-----rb

		//���� ������� Ray �� Hit ������ ����
		float dist = 0.0f;

		D3DXVECTOR3 lt = this->m_pMapToolinVertices[m_Corners[CORNER_LT]].p;
		D3DXVECTOR3 rt = this->m_pMapToolinVertices[m_Corners[CORNER_RT]].p;
		D3DXVECTOR3 lb = this->m_pMapToolinVertices[m_Corners[CORNER_LB]].p;
		D3DXVECTOR3 rb = this->m_pMapToolinVertices[m_Corners[CORNER_RB]].p;

		checkNum++;
		//�»�� ������ üũ
		if (D3DXIntersectTri(
			&lt, &rt, &lb,			//������ ���� 3 ��ġ ( �η�� ���� ���� )
			&pRay->origin,		//���� ��ġ
			&pRay->direction,		//���� ����
			NULL, NULL,
			&dist			//��Ʈ�� �Ǿ��ٸ� Origin ���� ���� hit ��ġ������ �Ÿ��� ���´�.
		))
		{
			//��Ʈ����
			D3DXVECTOR3 hitPos = pRay->origin + pRay->direction * dist;

			//Ǫ��
			pHits->push_back(hitPos);

			return;
		}

		checkNum++;
		//���ϴ� ������ üũ
		if (D3DXIntersectTri(
			&lb, &rt, &rb,			//������ ���� 3 ��ġ
			&pRay->origin,		//���� ��ġ
			&pRay->direction,		//���� ����
			NULL, NULL,
			&dist			//��Ʈ�� �Ǿ��ٸ� Origin ���� ���� hit ��ġ������ �Ÿ��� ���´�.
		))
		{
			//��Ʈ����
			D3DXVECTOR3 hitPos = pRay->origin + pRay->direction * dist;

			//Ǫ��
			pHits->push_back(hitPos);

			return;
		}
	}
}
