#include "stdafx.h"
#include "cRenderTexture.h"


cRenderTexture::cRenderTexture()
: pRenderTexture(NULL), pRenderSurface(NULL), useDepthAndStencil(FALSE)
{
}


cRenderTexture::~cRenderTexture()
{
}



void cRenderTexture::Init(int width, int height, D3DFORMAT format)
{

	this->width = width;
	this->height = height;


	//
	// RenderTarget �� �� Texture �����
	//
	Device->CreateTexture(
		width,					//Texture ���� �ػ� 
		height,					//Texture ���� �ػ�
		1,							//�Ӹ�ü�� ����
		D3DUSAGE_RENDERTARGET,		//RenderTarget �� Texture�̴�
		format,						//TEXTURE ���� RenderTexture ����D3DFMT_A8R8G8B8 ������
		D3DPOOL_DEFAULT,			//RenderTarget �� Texture �� Pool �� Default
		&pRenderTexture,			//������ Texture �޾ƿ� ������
		NULL						//�Ű澲������ NULL
		);

	//
	// Render �� Surface ( �̰� DepthBuffer �� StencilBuffer �� ����Ѵ� )
	//
	Device->CreateDepthStencilSurface(
		width,					//Texture ���� �ػ� 
		height,					//Texture ���� �ػ�
		D3DFMT_D24S8,			//Deapth �� 24 ��Ʈ Stencil �� 8 ��Ʈ	
		D3DMULTISAMPLE_NONE,		//��Ƽ ���ø� ��Ƽ�˸��ƽ��� �������� �ʴ´�, 
		0,							//��Ƽ ���ø� ����Ƽ�� 0
		TRUE,						//���� ��ü�� ���� �۹������� �������� �ʴ�? ( TRUE �� ���� ���۱�ü �ɶ� ������ ���� ���۳����� ������� �ʴ´� )
		&pRenderSurface,			//���� ������...
		NULL						//�Ű澲������ NULL
		);

}
void cRenderTexture::Release()
{
	SAFE_RELEASE(pRenderTexture);
	SAFE_RELEASE(pRenderSurface);
}

void cRenderTexture::BeginRender(
	DWORD stage,
	DWORD clearColor,
	DWORD clearFlag, 
	BOOL useDepthAndStencil )
{
	this->useDepthAndStencil = useDepthAndStencil;
	this->stageNum = stage;

	//
	// ȭ�鿡 ������ �Ǵ� ��� Texture�� �������Ѵ�.
	//

	//�� ����̽��� Target ������ǥ��� DepthStencil ������ ǥ�������� ���
	
	Device->GetRenderTarget(this->stageNum, &pDeviceTargetSurface);				//���� Device �� 0 �� ���� TargetSurface �� ���
	Device->GetDepthStencilSurface(&pDeviceDepthAndStencilSurface);	//���� Device �� ���ٽ� ���ۿ� �������� ǥ����


	//
	// TargetBuffer �� RenderTexture �� Surface �� ����
	//

	//RenderTexture �� Surface �� ��´�.
	LPDIRECT3DSURFACE9 texSurface = NULL;
	if (SUCCEEDED(this->pRenderTexture->GetSurfaceLevel(0, &texSurface)))
	{
		//���������� RenderTexture �� Surface �� ����ٸ�....

		//Texture ǥ���� Device �� Target ���۷� �����Ѵ�.
		Device->SetRenderTarget(this->stageNum, texSurface);

		//���õ� Surface ������ �ٷ� �����ִ� ���Ǹ� ������...
		SAFE_RELEASE(texSurface);
	}

	
	if (this->useDepthAndStencil)
	{
		//
		// Depth ���ۿ� Stencil ������ Surface �� pRenderSurface �� ����
		//
		Device->SetDepthStencilSurface(pRenderSurface);
	}


	//����̽� ���� Ŭ���� ( ��� ������ Setting �� Textuer �� Surface �� Ŭ���� �ȴ�  )
	Device->Clear(0, NULL,
		clearFlag,
		clearColor,
		1.0f,
		0);

}

void cRenderTexture::EndRender()
{

	//
	// ��ó�� ( ������� �������´� )
	//
	//Render Texture �� �׸� �۾��� �������� �ٽ� ���󺹱��ϴ� ����....
	Device->SetRenderTarget(this->stageNum, pDeviceTargetSurface);
	
	//���õ� Surface ������ �ٷ� �����ִ� ���Ǹ� ������...
	SAFE_RELEASE(pDeviceTargetSurface);

	if (this->useDepthAndStencil)
	{
		Device->SetDepthStencilSurface(pDeviceDepthAndStencilSurface);
		SAFE_RELEASE(pDeviceDepthAndStencilSurface);
	}
}