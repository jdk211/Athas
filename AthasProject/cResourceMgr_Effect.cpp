#include "stdafx.h"
#include "cResourceMgr_Effect.h"


cResourceMgr_Effect::cResourceMgr_Effect()
{
}


cResourceMgr_Effect::~cResourceMgr_Effect()
{
}


//�ε� ��ĸ� ������ 
LPD3DXEFFECT cResourceMgr_Effect::LoadResource(std::string filePath, void* pParam)
{
	LPD3DXEFFECT pEffect;

	//���� ������ �޾ƿ� ���� ����
	LPD3DXBUFFER pError = NULL;

	//���̴��� �ε��� ���ÿ� ������ �Ѵ�.
	D3DXCreateEffectFromFile(
		Device,				//����̽��� �ѱ��.
		filePath.c_str(),	//�ε��� ���̴� �ڵ� ��� �̸�
		NULL,				//��ũ�� ���� �Ⱦ� NULL
		NULL,				//���Լ��� �Ⱦ� NULL, 
		0,					//�÷��� 0
		NULL,				//�ε� �޸� Ǯ �׳�  NULL
		&pEffect,			//�ε��� Effect �� ����� LPD3DXEFFECT ����������
		&pError				//Error ���� ( ���� LPD3DXBUFFER �� �ּҰ� ���ԵǸ� �����ϰ����� ������ ���� ���� �����޽����� ���� ����� �����ϵǸ� NULL �� �ȴ� )
		);

	//���̴� �ڵ� ������ �ε��� ������ �ִ�...
	if (pError != NULL && pEffect == NULL )
	{
		//pError ���ۿ� �ִ� ������ ���ڿ��� ����غ���...
		LOG_MGR->AddLog((char*)pError->GetBufferPointer());

		//���� ���۴� ��������.
		SAFE_RELEASE(pError);

		return NULL;
	}


	return pEffect;
}

//���� ��ĸ� ������
void cResourceMgr_Effect::ReleaseResource(LPD3DXEFFECT data)
{
	SAFE_RELEASE(data);
}