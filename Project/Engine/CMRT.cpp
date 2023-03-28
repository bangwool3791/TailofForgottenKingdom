#include "pch.h"
#include "CMRT.h"

#include "CDevice.h"
#include "CTexture.h"

CMRT::CMRT()
	: m_arrTarget{}
	, m_RTCount{}
	, m_tViewPort{}
{

}

CMRT::~CMRT()
{

}

//Render Target, Depth Stencil Create
void CMRT::Create(Ptr<CTexture>(&_arrTex)[8], Vec4(&arrClear)[8], Ptr<CTexture> _pDesTex)
{
	for (int i = 0; i < 8; ++i)
	{
		if (nullptr == _arrTex[i].Get())
		{
			m_RTCount = i;
			break;
		}

		m_arrTarget[i] = _arrTex[i];
	}

	for (int i = 0; i < m_RTCount; ++i)
	{
		m_arrClearColor[i] = arrClear[i];
	}
	m_DSTex = _pDesTex;

	// ViewPort 설정	
	m_tViewPort.TopLeftX = 0;
	m_tViewPort.TopLeftY = 0;

	m_tViewPort.Width = m_arrTarget[0]->GetWidth();
	m_tViewPort.Height = m_arrTarget[0]->GetHeight();

	m_tViewPort.MinDepth = 0;
	m_tViewPort.MaxDepth = 1;
}

//Clear Render Target view, Depth stencil view
void CMRT::Clear()
{
	for (int i = 0; i < m_RTCount; ++i)
	{
		CONTEXT->ClearRenderTargetView(m_arrTarget[i]->GetRTV().Get(), m_arrClearColor[i]);
	}

	if (m_DSTex.Get())
	{
		CONTEXT->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH, 1.f, 0.f);
	}

}

//Output merge RenderTargetView, Depth Stencil View
void CMRT::OMSet()
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	for (int i = 0; i < m_RTCount; ++i)
	{
		arrRTV[i] = m_arrTarget[i]->GetRTV().Get();
	}

	if (nullptr != m_DSTex.Get())
		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, m_DSTex->GetDSV().Get());
	else
		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, nullptr);

	//카메라 여러대 일 때는 뷰포트 설청을 따로해야한다.
	// ViewPort 설정
	CONTEXT->RSSetViewports(1, &m_tViewPort);
}

void CMRT::OMClear()
{
	ID3D11RenderTargetView* arrRTV[8] = {};

	if (nullptr != m_DSTex.Get())
		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, m_DSTex->GetDSV().Get());
	else
		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, nullptr);
}
