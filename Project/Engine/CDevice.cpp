#include "pch.h"
#include "CDevice.h"

#include "CResMgr.h"
#include "CConstBuffer.h"


CDevice::CDevice()
	: m_hWnd(nullptr)
	, m_tViewPort{}
	, m_arrCB{}
	, m_arrRS{}
	, m_arrDS{}
	, m_arrBS{}
{
}

CDevice::~CDevice()
{
	for (UINT i = 0; i < (UINT)CB_TYPE::END; ++i)
	{
		if (nullptr != m_arrCB[i])
			delete m_arrCB[i];
	}
}

int CDevice::init(HWND _hWnd, Vec2 _vResolution)
{
	
	m_hWnd = _hWnd;
	m_vRenderResolution = _vResolution;
	g_global.vRenderResolution = m_vRenderResolution;

	UINT iDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#if defined(_DEBUG)
	// If the project is in a debug build, enable the debug layer.
	iDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// Define the ordering of feature levels that Direct3D attempts to create.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_1
	};

	ID3D11Device* pDevice = nullptr;
	HRESULT hr = D3D11CreateDevice(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE
		, nullptr

		, iDeviceFlag
		, featureLevels
		, ARRAYSIZE(featureLevels)
		, D3D11_SDK_VERSION
		, m_pDevice.GetAddressOf()
		, nullptr
		, m_pDeviceContext.GetAddressOf());

	UINT iQuality = 0;
	DEVICE->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &iQuality);


	// 스왚체인 만들기
	if (FAILED(CreateSwapchain()))
	{
		MessageBox(nullptr, L"스왚체인 생성 실패", L"Device 초기화 실패", MB_OK);
		return E_FAIL;
	}

	// RenderTarget, DepthStencilTarget 만들기
	if (FAILED(CreateTarget()))
	{
		MessageBox(nullptr, L"타겟 생성 실패", L"Device 초기화 실패", MB_OK);
		return E_FAIL;
	}


	// ViewPort
	//m_tViewPort.TopLeftX = 0;
	//m_tViewPort.TopLeftY = 0;
	//m_tViewPort.Width = _vResolution.x;
	//m_tViewPort.Height = _vResolution.y;
	//m_tViewPort.MinDepth = 0.f;
	//m_tViewPort.MaxDepth = 1.f;

	//m_pDeviceContext->RSSetViewports(1, &m_tViewPort);
	

	// 샘플러 생성 및 바인딩
	if (FAILED(CreateSampler()))
	{
		MessageBox(nullptr, L"샘플러 생성 실패", L"Device 초기화 실패", MB_OK);
		return E_FAIL;
	}


	// 상수버퍼 생성
	if (FAILED(CreateConstBuffer()))
	{
		MessageBox(nullptr, L"상수버퍼 생성 실패", L"Device 초기화 실패", MB_OK);
		return E_FAIL;
	}

	// 레스터라이저 스테이트 생성
	if (FAILED(CreateRasterizerState()))
	{
		MessageBox(nullptr, L"레스터라이저 스테이트 생성 실패", L"Device 초기화 실패", MB_OK);
		return E_FAIL;
	}

	// DepthStencil 스테이트 생성
	if (FAILED(CreateDepthStencilState()))
	{
		MessageBox(nullptr, L"뎊스 스텐실 스테이트 생성 실패", L"Device 초기화 실패", MB_OK);
		return E_FAIL;
	}


	// 블렌드 스테이트 생성
	if (FAILED(CreateBlendState()))
	{
		MessageBox(nullptr, L"블렌드 스테이트 생성 실패", L"Device 초기화 실패", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}


int CDevice::CreateSwapchain()
{
	DXGI_SWAP_CHAIN_DESC desc = {};

	desc.OutputWindow = m_hWnd;	// Front Buffer 를 출력시킬 윈도우 핸들
	desc.Windowed = true;		// 윈도우, 전체화면 모드
	desc.BufferCount = 1;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 이전 프레임 장면을 유지하지 않는다.

	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferDesc.Width = (UINT)m_vRenderResolution.x;
	desc.BufferDesc.Height = (UINT)m_vRenderResolution.y;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	ComPtr<IDXGIDevice> pDXGIDevice = nullptr;
	ComPtr<IDXGIAdapter> pAdapter = nullptr;
	ComPtr<IDXGIFactory> pFactory = nullptr;


	if (FAILED(m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf())))
	{
		return E_FAIL;
	}

	if (FAILED(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf())))
	{
		return E_FAIL;
	}

	if (FAILED(pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf())))
	{
		return E_FAIL;
	}

	if (FAILED(pFactory->CreateSwapChain(m_pDevice.Get(), &desc, m_pSwapChain.GetAddressOf())))
	{
		return E_FAIL;
	}
	//pDXGIDevice->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CDevice::pDXGIDevice") - 1, "CDevice::pDXGIDevice");
	//pAdapter->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CDevice::pAdapter") - 1, "CDevice::pAdapter");
	//pFactory->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CDevice::pFactory") - 1, "CDevice::pFactory");
	//m_pSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CDevice::m_pSwapChain") - 1, "CDevice::m_pSwapChain");
	return S_OK;
}

int CDevice::CreateTarget()
{
	HRESULT hr = S_OK;
	// RenderTargetTexture 를 스왚체인으로부터 참조하기
	ComPtr<ID3D11Texture2D> tex2D;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)tex2D.GetAddressOf());
	
	// RenderTargetTexture 등록
	m_pRenderTargetTex = CResMgr::GetInst()->CreateTexture(L"RenderTargetTex", tex2D);

	// DepthStencilTexture 생성
	m_pDepthStencilTex = CResMgr::GetInst()->CreateTexture(L"DepthStencilTex"
		, (UINT)m_vRenderResolution.x, (UINT)m_vRenderResolution.y
		, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);
	
	//tex2D->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CDevice::tex2D") - 1, "CDevice::tex2D");
	return hr;
}

int CDevice::CreateSampler()
{
	HRESULT hr = S_OK;

	// 샘플러 스테이트 생성
	D3D11_SAMPLER_DESC desc = {};

	desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	desc.Filter = D3D11_FILTER_ANISOTROPIC;

	hr = DEVICE->CreateSamplerState(&desc, m_arrSampler[(UINT)SAMPLER_TYPE::ANISOTROPIC].GetAddressOf());


	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	hr = DEVICE->CreateSamplerState(&desc, m_arrSampler[(UINT)SAMPLER_TYPE::POINT].GetAddressOf());

	desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.Filter = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
	hr = DEVICE->CreateSamplerState(&desc, m_arrSampler[(UINT)SAMPLER_TYPE::BLUR].GetAddressOf());

	//desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//desc.MipLODBias = 0.0f;
	//desc.MaxAnisotropy = 1;
	//desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//desc.BorderColor[0] = 0.f;
	//desc.BorderColor[1] = 0.f;
	//desc.BorderColor[2] = 0.f;
	//desc.BorderColor[3] = 0.f;
	//desc.MinLOD = 0;
	//desc.MaxLOD = D3D11_FLOAT32_MAX;
	//desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	//desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	//desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	// 샘플러 바인딩
	CONTEXT->VSSetSamplers((UINT)SAMPLER_TYPE::ANISOTROPIC, 1, m_arrSampler[(UINT)SAMPLER_TYPE::ANISOTROPIC].GetAddressOf());
	CONTEXT->HSSetSamplers((UINT)SAMPLER_TYPE::ANISOTROPIC, 1, m_arrSampler[(UINT)SAMPLER_TYPE::ANISOTROPIC].GetAddressOf());
	CONTEXT->DSSetSamplers((UINT)SAMPLER_TYPE::ANISOTROPIC, 1, m_arrSampler[(UINT)SAMPLER_TYPE::ANISOTROPIC].GetAddressOf());
	CONTEXT->GSSetSamplers((UINT)SAMPLER_TYPE::ANISOTROPIC, 1, m_arrSampler[(UINT)SAMPLER_TYPE::ANISOTROPIC].GetAddressOf());
	CONTEXT->PSSetSamplers((UINT)SAMPLER_TYPE::ANISOTROPIC, 1, m_arrSampler[(UINT)SAMPLER_TYPE::ANISOTROPIC].GetAddressOf());
	CONTEXT->CSSetSamplers((UINT)SAMPLER_TYPE::ANISOTROPIC, 1, m_arrSampler[(UINT)SAMPLER_TYPE::ANISOTROPIC].GetAddressOf());

	CONTEXT->VSSetSamplers((UINT)SAMPLER_TYPE::POINT, 1, m_arrSampler[(UINT)SAMPLER_TYPE::POINT].GetAddressOf());
	CONTEXT->HSSetSamplers((UINT)SAMPLER_TYPE::POINT, 1, m_arrSampler[(UINT)SAMPLER_TYPE::POINT].GetAddressOf());
	CONTEXT->DSSetSamplers((UINT)SAMPLER_TYPE::POINT, 1, m_arrSampler[(UINT)SAMPLER_TYPE::POINT].GetAddressOf());
	CONTEXT->GSSetSamplers((UINT)SAMPLER_TYPE::POINT, 1, m_arrSampler[(UINT)SAMPLER_TYPE::POINT].GetAddressOf());
	CONTEXT->PSSetSamplers((UINT)SAMPLER_TYPE::POINT, 1, m_arrSampler[(UINT)SAMPLER_TYPE::POINT].GetAddressOf());
	CONTEXT->CSSetSamplers((UINT)SAMPLER_TYPE::POINT, 1, m_arrSampler[(UINT)SAMPLER_TYPE::POINT].GetAddressOf());

	CONTEXT->VSSetSamplers((UINT)SAMPLER_TYPE::BLUR, 1, m_arrSampler[(UINT)SAMPLER_TYPE::BLUR].GetAddressOf());
	CONTEXT->HSSetSamplers((UINT)SAMPLER_TYPE::BLUR, 1, m_arrSampler[(UINT)SAMPLER_TYPE::BLUR].GetAddressOf());
	CONTEXT->DSSetSamplers((UINT)SAMPLER_TYPE::BLUR, 1, m_arrSampler[(UINT)SAMPLER_TYPE::BLUR].GetAddressOf());
	CONTEXT->GSSetSamplers((UINT)SAMPLER_TYPE::BLUR, 1, m_arrSampler[(UINT)SAMPLER_TYPE::BLUR].GetAddressOf());
	CONTEXT->PSSetSamplers((UINT)SAMPLER_TYPE::BLUR, 1, m_arrSampler[(UINT)SAMPLER_TYPE::BLUR].GetAddressOf());
	CONTEXT->CSSetSamplers((UINT)SAMPLER_TYPE::BLUR, 1, m_arrSampler[(UINT)SAMPLER_TYPE::BLUR].GetAddressOf());

	return hr;
}

int CDevice::CreateConstBuffer()
{
	m_arrCB[(UINT)CB_TYPE::TRANSFORM] = new CConstBuffer(CB_TYPE::TRANSFORM);
	m_arrCB[(UINT)CB_TYPE::TRANSFORM]->Create(sizeof(tTransform));

	m_arrCB[(UINT)CB_TYPE::MATERIAL] = new CConstBuffer(CB_TYPE::MATERIAL);
	m_arrCB[(UINT)CB_TYPE::MATERIAL]->Create(sizeof(tMtrlConst));

	m_arrCB[(UINT)CB_TYPE::GLOBAL] = new CConstBuffer(CB_TYPE::GLOBAL);
	m_arrCB[(UINT)CB_TYPE::GLOBAL]->Create(sizeof(tGlobalData));

	return S_OK;
}

int CDevice::CreateRasterizerState()
{
	HRESULT hr = S_OK;

	m_arrRS[(UINT)RS_TYPE::CULL_BACK] = nullptr;


	D3D11_RASTERIZER_DESC desc = {};

	// Cull Front Mode
	desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;

	hr = DEVICE->CreateRasterizerState(&desc, m_arrRS[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());


	// Cull None Mode
	desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;

	hr = DEVICE->CreateRasterizerState(&desc, m_arrRS[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());


	// WireFrame Mode
	desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;

	hr = DEVICE->CreateRasterizerState(&desc, m_arrRS[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());


	return hr;
}

int CDevice::CreateDepthStencilState()
{
	HRESULT hr = S_OK;

	// Less
	m_arrDS[(UINT)DS_TYPE::LESS] = nullptr;


	D3D11_DEPTH_STENCIL_DESC desc = {};

	// Less Equal
	desc.DepthEnable = true;
	desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.StencilEnable = false;

	DEVICE->CreateDepthStencilState(&desc, m_arrDS[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf());

	// Greater
	desc.DepthEnable = true;
	desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.StencilEnable = false;

	DEVICE->CreateDepthStencilState(&desc, m_arrDS[(UINT)DS_TYPE::GREATER].GetAddressOf());

	// NO_WRITE
	desc.DepthEnable = true;
	desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	desc.StencilEnable = false;

	DEVICE->CreateDepthStencilState(&desc, m_arrDS[(UINT)DS_TYPE::NO_WRITE].GetAddressOf());

	// NO_TEST_NO_WRITE
	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.StencilEnable = false;

	DEVICE->CreateDepthStencilState(&desc, m_arrDS[(UINT)DS_TYPE::NO_TEST_NO_WRITE].GetAddressOf());

	return hr;
}

int CDevice::CreateBlendState()
{
	HRESULT hr = S_OK;

	// Default 블렌딩
	m_arrBS[(UINT)BS_TYPE::DEFAULT] = nullptr;



	D3D11_BLEND_DESC desc = {};


	// Alpha Blend
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;


	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;




	hr = DEVICE->CreateBlendState(&desc, m_arrBS[(UINT)BS_TYPE::ALPHABLEND].GetAddressOf());


	// One One Blend
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DEVICE->CreateBlendState(&desc, m_arrBS[(UINT)BS_TYPE::ONE_ONE].GetAddressOf());


	return hr;
}

void CDevice::TargetClear()
{
	float arrColor[4] = { 0.2f, 0.2f, 0.2f, 1.f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetTex->GetRTV().Get(), arrColor);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}


void CDevice::Present()
{
	m_pSwapChain->Present(0, 0);
}