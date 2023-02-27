#include "pch.h"
#include "CTexture.h"

#include "CDevice.h"


CTexture::CTexture(bool _bEngineRes)
    : CRes(RES_TYPE::TEXTURE, _bEngineRes)
    , m_Desc{}
{
}


CTexture::~CTexture()
{
}

int CTexture::Load(const wstring& _strFilePath)
{
    HRESULT hr = E_FAIL;

    wchar_t szExt[50] = {};
    _wsplitpath_s(_strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);

    wstring strExt = szExt;
    if (strExt == L".dds" || strExt == L".DDS")
    {
        hr = LoadFromDDSFile(_strFilePath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, m_Image);
    }

    else if (strExt == L".tga" || strExt == L".TGA")
    {
        hr = LoadFromTGAFile(_strFilePath.c_str(), nullptr, m_Image);
    }

    else // WIC (png, jpg, jpeg, bmp )
    {
        hr = LoadFromWICFile(_strFilePath.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, nullptr, m_Image);
        m_pPixels = m_Image.GetPixels();
        m_iPixelSize = m_Image.GetPixelsSize();
    }

    if (FAILED(hr))
        return hr;


    // sysmem -> GPU
    hr = CreateShaderResourceView(DEVICE
        , m_Image.GetImages()
        , m_Image.GetImageCount()
        , m_Image.GetMetadata()
        , m_SRV.GetAddressOf());

    m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());

    m_Tex2D.Get()->GetDesc(&m_Desc);
    
    return hr;
}

void CTexture::Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _Format, UINT _iBindFlag)
{
    m_Desc.BindFlags = _iBindFlag;
    
    m_Desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    m_Desc.CPUAccessFlags = 0;

    m_Desc.Format = _Format;
    m_Desc.Width = _iWidth;
    m_Desc.Height = _iHeight;
    m_Desc.ArraySize = 1;

    m_Desc.SampleDesc.Count = 1;////MSAA(Multi Sampling Anti Aliasing)쓸꺼냐? 1, 0 이면 안쓴다는 옵션.
    m_Desc.SampleDesc.Quality = 0;
    //이미지중에 하나를 추출하는것을 SampleDesc 라고한다.
    /*
        픽셀이 적으면 계단현상이 나오는데 이것을 Aliasing이라고한다.
        그래서 그 계단현상을 없애려는 것이 Anti Aliasing이라고 한다.
        밑에 AA는 Anti Aliasing이다.
        SSAA//Super Sampling Anti Aliasing
        - 4배씩 늘리고 보정을 한 후 다시 줄인다. 그래서 선명하게 만든다.
        단점 : 비용이 너무 쎄다. 그래서 잘 안쓴다.

        MSAA//Multi Sampling Anti Aliasing (쓰레드에서 많이 쓰는 기법이다)
        - Anti Aliasing이 일어나는건 결국 테두리이다. 그래서 테두리를 잡고 보정을한다.
        단점 : 이것도 비용이 쎄다. 사용자가 이 기능을 킬 수 있게 옵션을 넣어 줄 수 있다.
    */
    m_Desc.MipLevels = 1;
    m_Desc.MiscFlags = 0;
    HRESULT hr = DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf());
    assert(!FAILED(hr));
    
    //DEPTH Stencill View는 한가지의 View만 가질 수 있다.
    if (_iBindFlag & D3D11_BIND_DEPTH_STENCIL)
    {
        hr = DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
      //  m_DSV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CTexture::m_DSV") - 1, "CTexture::m_DSV");
    }
    else
    {
        if (_iBindFlag & D3D11_BIND_RENDER_TARGET)
        {
            hr = DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
            assert(!FAILED(hr));
         //   m_RTV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CTexture::m_RTV") - 1, "CTexture::m_RTV");
        }

        if (_iBindFlag & D3D11_BIND_SHADER_RESOURCE)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
            tSRVDesc.Format = m_Desc.Format;
            tSRVDesc.Texture2D.MipLevels = 1;
            tSRVDesc.Texture2D.MostDetailedMip = 0;
            tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
            hr = DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &tSRVDesc, m_SRV.GetAddressOf());
         //   m_SRV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CTexture::m_SRV") - 1, "CTexture::m_SRV");
            assert(!FAILED(hr));
        }

        if (_iBindFlag & D3D11_BIND_UNORDERED_ACCESS)
        {
            D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};
            tUAVDesc.Format = m_Desc.Format;
            tUAVDesc.Texture2D.MipSlice = 0;
            tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
            hr = DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), &tUAVDesc, m_UAV.GetAddressOf());
        //    m_UAV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CTexture::m_UAV") - 1, "CTexture::m_UAV");
            assert(!FAILED(hr));
        }
    }
}

void CTexture::Create(ComPtr<ID3D11Texture2D> _Tex2D)
{
    m_Tex2D = _Tex2D;
   // m_Tex2D->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CTexture::m_Tex2D") - 1, "CTexture::m_Tex2D");
    m_Tex2D->GetDesc(&m_Desc);

    HRESULT hr = S_OK;

    if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
    {
        hr = DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
    }
    else
    {
        if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
        {
            hr = DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
            assert(!FAILED(hr));
        }

        if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
            tSRVDesc.Format = m_Desc.Format;
            tSRVDesc.Texture2D.MipLevels = 1;
            tSRVDesc.Texture2D.MostDetailedMip = 0;
            tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
            hr = DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf());
            assert(!FAILED(hr));
        }

        if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
        {
            D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};
            tUAVDesc.Format = m_Desc.Format;
            tUAVDesc.Texture2D.MipSlice = 0;
            tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
            hr = DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), &tUAVDesc, m_UAV.GetAddressOf());
            assert(!FAILED(hr));
        }
    }
}

void CTexture::UpdateData(UINT _iRegisterNum, UINT _iPipelineStage)
{
    if ((UINT)PIPELINE_STAGE::VS & _iPipelineStage)
        CONTEXT->VSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

    if ((UINT)PIPELINE_STAGE::HS & _iPipelineStage)
        CONTEXT->HSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

    if ((UINT)PIPELINE_STAGE::DS & _iPipelineStage)
        CONTEXT->DSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

    if ((UINT)PIPELINE_STAGE::GS & _iPipelineStage)
        CONTEXT->GSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

    if ((UINT)PIPELINE_STAGE::PS & _iPipelineStage)
        CONTEXT->PSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
}

void CTexture::UpdateData_CS(UINT _iRegisterNum, bool _bShaderRes)
{
    if (_bShaderRes)
    {
        CONTEXT->CSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
    }
    else
    {
        UINT i = -1;
        CONTEXT->CSSetUnorderedAccessViews(_iRegisterNum, 1, m_UAV.GetAddressOf(), &i);
    }
}

void CTexture::Clear(UINT _iRegisterNum)
{
    ID3D11ShaderResourceView* pSRV = nullptr;

    CONTEXT->VSSetShaderResources(_iRegisterNum, 1, &pSRV);
    CONTEXT->HSSetShaderResources(_iRegisterNum, 1, &pSRV);
    CONTEXT->DSSetShaderResources(_iRegisterNum, 1, &pSRV);
    CONTEXT->GSSetShaderResources(_iRegisterNum, 1, &pSRV);
    CONTEXT->PSSetShaderResources(_iRegisterNum, 1, &pSRV);
}

void CTexture::Clear_CS(UINT _iRegisterNum)
{
    ID3D11UnorderedAccessView* pUAV = nullptr;
    UINT i = -1;
    CONTEXT->CSSetUnorderedAccessViews(_iRegisterNum, 1, &pUAV, &i);
}
