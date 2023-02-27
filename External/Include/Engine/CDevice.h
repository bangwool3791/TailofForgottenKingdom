#pragma once

#include "Ptr.h"
#include "CTexture.h"

class CConstBuffer;

class CDevice
	:public CSingleton<CDevice>
{
private:
	CDevice();
	virtual ~CDevice();
	friend class CSingleton<CDevice>;
private:
	HWND							m_hWnd;

	ComPtr<ID3D11Device>			m_pDevice;				// GPU �޸� ����
	ComPtr<ID3D11DeviceContext>		m_pDeviceContext;		// GPU Rendering

	Ptr<CTexture>					m_pRenderTargetTex;		// Render Target
	Ptr<CTexture>					m_pDepthStencilTex;		// ���� ���� Ÿ��

	ComPtr<IDXGISwapChain>			m_pSwapChain;			// RenderTarget(FrontBuffer, BackBuffer) �� ���� �� ���� ��ü ����
	D3D11_VIEWPORT					m_tViewPort;			// ����۸� �����쿡 �׸� ����(��ġ, ũ��) ����

	Vec2							m_vRenderResolution;	// ������ ���� �ػ�

	ComPtr<ID3D11SamplerState>		m_arrSampler[(UINT)SAMPLER_TYPE::END];

	CConstBuffer*					m_arrCB[(UINT)CB_TYPE::END];
	ComPtr<ID3D11RasterizerState>   m_arrRS[(UINT)RS_TYPE::END];
	ComPtr<ID3D11BlendState>		m_arrBS[(UINT)RS_TYPE::END];
	ComPtr<ID3D11DepthStencilState> m_arrDS[(UINT)DS_TYPE::END];


public:
	int init(HWND _hWnd, Vec2 _vResolution);
	void TargetClear();
	void Present();
	Vec2 GetRenderResolution() { return m_vRenderResolution; }
private:
	int CreateSwapchain();
	int CreateTarget();
	int CreateConstBuffer();
	int CreateSampler();
	int CreateRasterizerState();
	int CreateBlendState();
	int CreateDepthStencilState();
public:
	ID3D11Device* GetDevice() { return m_pDevice.Get(); }
	ID3D11DeviceContext* GetContext() { return m_pDeviceContext.Get(); }
	CConstBuffer* GetConstBuffer(CB_TYPE _eType) { return m_arrCB[(UINT)_eType]; }
	ComPtr<ID3D11RasterizerState> GetRasterizerState(RS_TYPE _eType) { return m_arrRS[(UINT)_eType]; }
	ComPtr<ID3D11BlendState> GetBlendState(BS_TYPE _eType) { return m_arrBS[(UINT)_eType]; }
	ComPtr<ID3D11DepthStencilState> GetDepthStencilState(DS_TYPE _eType) { return m_arrDS[(UINT)_eType]; }
};

