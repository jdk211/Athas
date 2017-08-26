#pragma once

#include "cSingletonBase.h"
class cGizmoManager : public cSingletonBase<cGizmoManager>
{
private:
	//정점 구조체
	typedef struct tagGIZMOVERTEX{
		D3DXVECTOR3 position;
		DWORD color;
		enum{ FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE };
	}GIZMOVERTEX, *LPGIZMOVERTEX;

	//그리드 셋팅 정보
	int gridCellNum;				//그리드 셸의 수
	float gridCellSize;				//그리드 셸의 사이즈
	int gridLineNum;				//그리드 라인 총갯수



	LPGIZMOVERTEX pGridVertices;		//그리드의 정점 배열
	GIZMOVERTEX	lineVertices[2];		//라인그리기 전용 배열


private:
	cGizmoManager();
	friend cGizmoManager* cSingletonBase<cGizmoManager>::GetInstance(void);

public:
	~cGizmoManager();


	//매니져 초기화 
	HRESULT Init(void);

	//매니져 해재
	void Release(void);

	//그리드 초기화
	void InitGrid(int gridCellNum, float gridCellSize);

	//그리드 랜더 함수
	void RenderGrid();

	//라인 랜더
	void RenderLine(const D3DXVECTOR3* pStart, const D3DXVECTOR3* pEnd, DWORD color);
	void DrawWireSphere(D3DXVECTOR3* worldCenter, float radius, DWORD color = 0xffffffff);


	void DrawWireObbBox(D3DXVECTOR3* min, D3DXVECTOR3* max, D3DXMATRIXA16* mat, DWORD color = 0xffffffff);
};

#define GIZMO_MGR cGizmoManager::GetInstance()