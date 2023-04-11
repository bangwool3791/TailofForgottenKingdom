#include "pch.h"
#include "CGraphicsShader.h"

#include "CPathMgr.h"
#include "CDevice.h"
#include "CResMgr.h"


CGraphicsShader::CGraphicsShader()
	: CShader(RES_TYPE::GRAPHICS_SHADER)
	, m_eTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_eRSType(RS_TYPE::CULL_BACK)
	, m_eDomain(SHADER_DOMAIN::NONE)
{
}

CGraphicsShader::~CGraphicsShader()
{
}

void CGraphicsShader::CreateVertexShader(const wstring& _strRelativePath, const string& _strFuncName)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), "vs_5_0", dwShaderFlags, 0, m_VSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		const char* pErr = (const char*)m_ErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Shader Compile Failed!!", MB_OK);
	}

	hr = DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize(), nullptr, m_VS.GetAddressOf());


	const vector<D3D11_INPUT_ELEMENT_DESC>& vecLayout = CResMgr::GetInst()->GetInputLayoutInfo();
	//Instacing 포함 InputLayout 작성
	hr = DEVICE->CreateInputLayout(vecLayout.data(), (UINT)vecLayout.size()
		, m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize()
		, m_Layout.GetAddressOf());

	assert(!FAILED(hr));


	// Vertex Inst Shader 컴파일 하기
	hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, (_strFuncName + "_Inst").c_str(), "vs_5_0", 0, 0, m_VSInstBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		// 컴파일 된 코드로 Vertex Instancing Shader 객체 만들기
		DEVICE->CreateVertexShader(m_VSInstBlob->GetBufferPointer(), m_VSInstBlob->GetBufferSize(), nullptr, m_VSInst.GetAddressOf());


		hr = DEVICE->CreateInputLayout(vecLayout.data(), (UINT)vecLayout.size()
			, m_VSInstBlob->GetBufferPointer(), m_VSInstBlob->GetBufferSize()
			, m_LayoutInst.GetAddressOf());
		assert(!FAILED(hr));
	}
}

void CGraphicsShader::CreateHullShader(const wstring& _strRelativePath, const string& _strFuncName)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), "hs_5_0", 0, 0, m_HSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		const char* pErr = (const char*)m_ErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Shader Compile Failed!!", MB_OK);
		assert(nullptr);
	}

	hr = DEVICE->CreateHullShader(m_HSBlob->GetBufferPointer(), m_HSBlob->GetBufferSize(), nullptr, m_HS.GetAddressOf());

	assert(!FAILED(hr));
}

void CGraphicsShader::CreateDomainShader(const wstring& _strRelativePath, const string& _strFuncName)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), "ds_5_0", 0, 0, m_DSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		const char* pErr = (const char*)m_ErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Shader Compile Failed!!", MB_OK);
		assert(nullptr);
	}

	hr = DEVICE->CreateDomainShader(m_DSBlob->GetBufferPointer(), m_DSBlob->GetBufferSize(), nullptr, m_DS.GetAddressOf());

	assert(!FAILED(hr));
}

void CGraphicsShader::CreateGeometryShader(const wstring& _strRelativePath, const string& _strFuncName)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), "gs_5_0", dwShaderFlags, 0, m_GSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		const char* pErr = (const char*)m_ErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Shader Compile Failed!!", MB_OK);
		assert(nullptr);
	}

	hr = DEVICE->CreateGeometryShader(m_GSBlob->GetBufferPointer(), m_GSBlob->GetBufferSize(), nullptr, m_GS.GetAddressOf());

	assert(!FAILED(hr));
}

void CGraphicsShader::CreatePixelShader(const wstring& _strRelativePath, const string& _strFuncName)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), "ps_5_0", dwShaderFlags, 0, m_PSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		const char* pErr = (const char*)m_ErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Shader Compile Failed!!", MB_OK);
		assert(nullptr);
	}


	hr = DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer(), m_PSBlob->GetBufferSize(), nullptr, m_PS.GetAddressOf());

	assert(!FAILED(hr));
}

void CGraphicsShader::UpdateData()
{
	CONTEXT->VSSetShader(m_VS.Get(), 0, 0);
	CONTEXT->HSSetShader(m_HS.Get(), 0, 0);
	CONTEXT->DSSetShader(m_DS.Get(), 0, 0);
	CONTEXT->GSSetShader(m_GS.Get(), 0, 0);
	CONTEXT->PSSetShader(m_PS.Get(), 0, 0);

	// 레스터라이즈 스테이트 설정
	ComPtr<ID3D11RasterizerState> RSState = CDevice::GetInst()->GetRasterizerState(m_eRSType);
	CONTEXT->RSSetState(RSState.Get());

	// 뎊스 스텐실 스테이트 설정
	ComPtr<ID3D11DepthStencilState> DSState = CDevice::GetInst()->GetDepthStencilState(m_eDSType);
	CONTEXT->OMSetDepthStencilState(DSState.Get(), 0);

	// 블렌드 스테이트 설정
	ComPtr<ID3D11BlendState> BSState = CDevice::GetInst()->GetBlendState(m_eBSType);
	CONTEXT->OMSetBlendState(BSState.Get(), nullptr, 0xffffffff);
	//Forward Shader
	CONTEXT->IASetInputLayout(m_Layout.Get());
	CONTEXT->IASetPrimitiveTopology(m_eTopology);
}

void CGraphicsShader::UpdateData_Inst()
{
	CONTEXT->VSSetShader(m_VSInst.Get(), 0, 0);
	CONTEXT->HSSetShader(m_HS.Get(), 0, 0);
	CONTEXT->DSSetShader(m_DS.Get(), 0, 0);
	CONTEXT->GSSetShader(m_GS.Get(), 0, 0);
	CONTEXT->PSSetShader(m_PS.Get(), 0, 0);

	// 레스터라이즈 스테이트 설정
	ComPtr<ID3D11RasterizerState> RSState = CDevice::GetInst()->GetRasterizerState(m_eRSType);
	CONTEXT->RSSetState(RSState.Get());

	// 뎊스 스텐실 스테이트 설정
	ComPtr<ID3D11DepthStencilState> DSState = CDevice::GetInst()->GetDepthStencilState(m_eDSType);
	CONTEXT->OMSetDepthStencilState(DSState.Get(), 0);

	// 블렌드 스테이트 설정
	ComPtr<ID3D11BlendState> BSState = CDevice::GetInst()->GetBlendState(m_eBSType);
	CONTEXT->OMSetBlendState(BSState.Get(), nullptr, 0xffffff);

	//레이 아웃만 다르게 GPU로 던져준다
	CONTEXT->IASetInputLayout(m_LayoutInst.Get());
	CONTEXT->IASetPrimitiveTopology(m_eTopology);
}

