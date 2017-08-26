#pragma once

#include "cSingletonBase.h"
class cGizmoManager : public cSingletonBase<cGizmoManager>
{
private:
	//���� ����ü
	typedef struct tagGIZMOVERTEX{
		D3DXVECTOR3 position;
		DWORD color;
		enum{ FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE };
	}GIZMOVERTEX, *LPGIZMOVERTEX;

	//�׸��� ���� ����
	int gridCellNum;				//�׸��� ���� ��
	float gridCellSize;				//�׸��� ���� ������
	int gridLineNum;				//�׸��� ���� �Ѱ���



	LPGIZMOVERTEX pGridVertices;		//�׸����� ���� �迭
	GIZMOVERTEX	lineVertices[2];		//���α׸��� ���� �迭


private:
	cGizmoManager();
	friend cGizmoManager* cSingletonBase<cGizmoManager>::GetInstance(void);

public:
	~cGizmoManager();


	//�Ŵ��� �ʱ�ȭ 
	HRESULT Init(void);

	//�Ŵ��� ����
	void Release(void);

	//�׸��� �ʱ�ȭ
	void InitGrid(int gridCellNum, float gridCellSize);

	//�׸��� ���� �Լ�
	void RenderGrid();

	//���� ����
	void RenderLine(const D3DXVECTOR3* pStart, const D3DXVECTOR3* pEnd, DWORD color);
	void DrawWireSphere(D3DXVECTOR3* worldCenter, float radius, DWORD color = 0xffffffff);


	void DrawWireObbBox(D3DXVECTOR3* min, D3DXVECTOR3* max, D3DXMATRIXA16* mat, DWORD color = 0xffffffff);
};

#define GIZMO_MGR cGizmoManager::GetInstance()