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


//MSAA(Multi Sampling Anti Aliasing)쓸꺼냐? 1, 0 이면 안쓴다는 옵션.
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
void CTexture::Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _Format, UINT _iBindFlag)
{
    m_Desc.BindFlags = _iBindFlag;
 
    m_Desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    m_Desc.CPUAccessFlags = 0;

    m_Desc.Format = _Format;
    m_Desc.Width = m_fWidth = _iWidth;
    m_Desc.Height = m_fHeight = _iHeight;
    m_Desc.ArraySize = 1;

    m_Desc.SampleDesc.Count = 1;
    m_Desc.SampleDesc.Quality = 0;

    m_Desc.MipLevels = 1;
    m_Desc.MiscFlags = 0;

    HRESULT hr = DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf());
    if (FAILED(hr))
    {
        int a = 0;
    }
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
            if (FAILED(hr))
            {
                int a = 0;
            }
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

void CTexture::CreateCubeTexture(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _Format, UINT _iBindFlag)
{
    //ID3D11Texture2D* cubeTexture = NULL;
    //ComPtr<ID3D11ShaderResourceView> shaderResourceView = NULL;

    ////Description of each face
    //D3D11_TEXTURE2D_DESC texDesc = {};

    //D3D11_TEXTURE2D_DESC texDesc1 = {};
    //texDesc1.Width = _iWidth;
    //texDesc1.Height = _iHeight;
    //texDesc1.MipLevels = 9;
    //texDesc1.Format = _Format;
    ////The Shader Resource view description
    //D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc = {};
    //wstring str[6] = { L"LeftTargetTex", L"RightTargetTex", L"DownTargetTex", L"UpTargetTex",L"BackTargetTex", L"FrontTargetTex" };
    //ComPtr<ID3D11Texture2D> tex[6]{};

    //for (size_t i = 0; i < 6; ++i)
    //{
    //    tex[i] = CResMgr::GetInst()->FindRes<CTexture>(str[i])->GetTex2D();
    //}

    //tex[0]->GetDesc(&texDesc1);

    //texDesc.Width = texDesc1.Width;
    //texDesc.Height = texDesc1.Height;
    //texDesc.MipLevels = texDesc1.MipLevels;
    //texDesc.ArraySize = 6;
    //texDesc.Format = texDesc1.Format;
    //texDesc.CPUAccessFlags = 0;
    //texDesc.SampleDesc.Count = 1;
    //texDesc.SampleDesc.Quality = 0;
    //texDesc.Usage = D3D11_USAGE_DEFAULT;
    //texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    //texDesc.CPUAccessFlags = 0;
    //texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    //SMViewDesc.Format = texDesc.Format;
    //SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    //SMViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
    //SMViewDesc.TextureCube.MostDetailedMip = 0;


    //DEVICE->CreateTexture2D(&texDesc, NULL, &cubeTexture);
    //for (int i = 0; i < 6; i++)
    //{

    //    for (UINT mipLevel = 0; mipLevel < texDesc.MipLevels; ++mipLevel)
    //    {
    //        D3D11_MAPPED_SUBRESOURCE mappedTex2D;
    //        memset(&mappedTex2D, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
    //        HRESULT hr = CONTEXT->Map(tex[i].Get(), mipLevel, D3D11_MAP_READ, 0, &mappedTex2D);
    //        assert(SUCCEEDED(hr));
    //        CONTEXT->UpdateSubresource(cubeTexture,
    //            D3D11CalcSubresource(mipLevel, i, texDesc.MipLevels),
    //            0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

    //        CONTEXT->Unmap(tex[i].Get(), mipLevel);
    //    }
    //}

    //DEVICE->CreateShaderResourceView(cubeTexture, &SMViewDesc, shaderResourceView.GetAddressOf());
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

void CTexture::Clear_CS(UINT _iRegisterNum, bool _bShaderRes)
{
    if (_bShaderRes)
    {
        ID3D11ShaderResourceView* pSRV = nullptr;
        CONTEXT->CSSetShaderResources(_iRegisterNum, 1, &pSRV);
    }
    else
    {
        ID3D11UnorderedAccessView* pUAV = nullptr;
        UINT i = -1;
        CONTEXT->CSSetUnorderedAccessViews(_iRegisterNum, 1, &pUAV, &i);
    }
}


void CTexture::SaveTexture()
{
   //HRESULT hr;
   // // Write out the render target as a PNG
    D3DX11SaveTextureToFileW(CONTEXT, m_Tex2D.Get(), D3DX11_IFF_PNG, L"D:\Dev\MyEngine\D3D\AssortRockDx112D-master\\SCREENSHOT.PNG");
   //
   // // Write out the render target as JPG
   // hr = SaveWICTextureToFile(context.Get(), backBufferTex.Get(), GUID_ContainerFormatJpeg, L"SCREENSHOT.JPG");
   //
   // // Write out the render target as BMP
   // hr = SaveWICTextureToFile(context.Get(), backBufferTex.Get(), GUID_ContainerFormatBmp, L"SCREENSHOT.BMP");
   //
   // // Write out the render target as BMP and explicitly use a 16-bit format
   // hr = SaveWICTextureToFile(context.Get(), backBufferTex.Get(), GUID_ContainerFormatBmp, L"SCREENSHOT.BMP", &GUID_WICPixelFormat16bppBGR565);
   //
   // // Write out the render target as a TIF
   // hr = SaveWICTextureToFile(context.Get(), backBufferTex.Get(), GUID_ContainerFormatTiff, L"SCREENSHOT.TIF");
   //
   // // Write out the render target as a TIF with explicit WIC codec properties
   // hr = SaveWICTextureToFile(context.Get(), backBufferTex.Get(), GUID_ContainerFormatTiff, L"SCREENSHOT.TIF", nullptr,
   //     [&](IPropertyBag2* props)
   //     {
   //         PROPBAG2 options[2] = { 0, 0 };
   //         options[0].pstrName = L"CompressionQuality";
   //         options[1].pstrName = L"TiffCompressionMethod";
   //
   //         VARIANT varValues[2];
   //         varValues[0].vt = VT_R4;
   //         varValues[0].fltVal = 0.75f;
   //
   //         varValues[1].vt = VT_UI1;
   //         varValues[1].bVal = WICTiffCompressionNone;
   //
   //         (void)props->Write(2, options, varValues);
   //     });
   //
   // // Write out the render target as a DDS
   // hr = SaveDDSTextureToFile(context.Get(), backBufferTex.Get(), L"SCREENSHOT.DDS");
}

HRESULT CTexture::CaptureTexture(_In_ ID3D11DeviceContext* pContext,
    _In_ ID3D11Resource* pSource,
    _Inout_ D3D11_TEXTURE2D_DESC& desc,
    _Inout_ ComPtr<ID3D11Texture2D>& pStaging)
{
    if (!pContext || !pSource)
        return E_INVALIDARG;

    D3D11_RESOURCE_DIMENSION resType = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    pSource->GetType(&resType);

    if (resType != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
        return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

    ComPtr<ID3D11Texture2D> pTexture;
    HRESULT hr = pSource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pTexture.GetAddressOf()));
    if (FAILED(hr))
        return hr;

    assert(pTexture);

    pTexture->GetDesc(&desc);

    ComPtr<ID3D11Device> d3dDevice;
    pContext->GetDevice(d3dDevice.GetAddressOf());

    if (desc.SampleDesc.Count > 1)
    {
        // MSAA content must be resolved before being copied to a staging texture
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;

        ComPtr<ID3D11Texture2D> pTemp;
        hr = d3dDevice->CreateTexture2D(&desc, 0, pTemp.GetAddressOf());
        if (FAILED(hr))
            return hr;

        assert(pTemp);

        DXGI_FORMAT fmt = EnsureNotTypeless(desc.Format);

        UINT support = 0;
        hr = d3dDevice->CheckFormatSupport(fmt, &support);
        if (FAILED(hr))
            return hr;

        if (!(support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE))
            return E_FAIL;

        for (UINT item = 0; item < desc.ArraySize; ++item)
        {
            for (UINT level = 0; level < desc.MipLevels; ++level)
            {
                UINT index = D3D11CalcSubresource(level, item, desc.MipLevels);
                pContext->ResolveSubresource(pTemp.Get(), index, pSource, index, fmt);
            }
        }

        desc.BindFlags = 0;
        desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.Usage = D3D11_USAGE_STAGING;

        hr = d3dDevice->CreateTexture2D(&desc, 0, pStaging.GetAddressOf());
        if (FAILED(hr))
            return hr;

        assert(pStaging);

        pContext->CopyResource(pStaging.Get(), pTemp.Get());
    }
    else if ((desc.Usage == D3D11_USAGE_STAGING) && (desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ))
    {
        // Handle case where the source is already a staging texture we can use directly
        pStaging = pTexture;
    }
    else
    {
        // Otherwise, create a staging texture from the non-MSAA source
        desc.BindFlags = 0;
        desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.Usage = D3D11_USAGE_STAGING;

        hr = d3dDevice->CreateTexture2D(&desc, 0, pStaging.GetAddressOf());
        if (FAILED(hr))
            return hr;

        assert(pStaging);

        pContext->CopyResource(pStaging.Get(), pSource);
    }
}

DXGI_FORMAT CTexture::EnsureNotTypeless(DXGI_FORMAT fmt)
{
        // Assumes UNORM or FLOAT; doesn't use UINT or SINT
    switch (fmt)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        return DXGI_FORMAT_R16G16B16A16_UNORM;
    case DXGI_FORMAT_R32G32_TYPELESS:
        return DXGI_FORMAT_R32G32_FLOAT;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        return DXGI_FORMAT_R10G10B10A2_UNORM;
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_R16G16_TYPELESS:
        return DXGI_FORMAT_R16G16_UNORM;
    case DXGI_FORMAT_R32_TYPELESS:
        return DXGI_FORMAT_R32_FLOAT;
    case DXGI_FORMAT_R8G8_TYPELESS:
        return DXGI_FORMAT_R8G8_UNORM;
    case DXGI_FORMAT_R16_TYPELESS:
        return DXGI_FORMAT_R16_UNORM;
    case DXGI_FORMAT_R8_TYPELESS:
        return DXGI_FORMAT_R8_UNORM;
    case DXGI_FORMAT_BC1_TYPELESS:
        return DXGI_FORMAT_BC1_UNORM;
    case DXGI_FORMAT_BC2_TYPELESS:
        return DXGI_FORMAT_BC2_UNORM;
    case DXGI_FORMAT_BC3_TYPELESS:
        return DXGI_FORMAT_BC3_UNORM;
    case DXGI_FORMAT_BC4_TYPELESS:
        return DXGI_FORMAT_BC4_UNORM;
    case DXGI_FORMAT_BC5_TYPELESS:
        return DXGI_FORMAT_BC5_UNORM;
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        return DXGI_FORMAT_B8G8R8X8_UNORM;
    case DXGI_FORMAT_BC7_TYPELESS:
        return DXGI_FORMAT_BC7_UNORM;
    default:
        return fmt;
    }
}