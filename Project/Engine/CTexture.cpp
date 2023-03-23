#include "pch.h"
#include "CTexture.h"

#include "CDevice.h"

#include "CPathMgr.h"
#include "CImage.h"


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
    return Load(_strFilePath, 1);
}

int CTexture::Load(const wstring& _strFilePath, int _iMipLevel)
{
    wchar_t strBuff[50] = {};
    _wsplitpath_s(_strFilePath.c_str(), 0, 0, 0, 0, 0, 0, strBuff, 50);
    wstring strExt = strBuff;

    HRESULT hRet = S_OK;

    if (strExt == L".dds" || strExt == L".DDS")
    {
        // .dds .DDS
        hRet = LoadFromDDSFile(_strFilePath.c_str(), DDS_FLAGS_FORCE_RGB, nullptr, m_Image);
    }
    else if (strExt == L".tga" || strExt == L".TGA")
    {
        // .tga .TGA
        hRet = LoadFromTGAFile(_strFilePath.c_str(), nullptr, m_Image);
    }
    else
    {
        // .png .jpg .jpeg .bmp
        hRet = LoadFromWICFile(_strFilePath.c_str(), WIC_FLAGS_NONE, nullptr, m_Image);
    }

    if (FAILED(hRet))
    {
        wsprintf(strBuff, L"에러코드 : %d", hRet);
        MessageBox(nullptr, strBuff, L"텍스쳐 로딩 실패", MB_OK);
        return hRet;
    }

    // Texture2D 생성
    m_Desc.Format = m_Image.GetMetadata().format;

    if (m_Image.GetMetadata().IsCubemap())
    {
        m_Desc.MipLevels = 1;	// 0 ==> 가능한 모든 밉맵이 저장 될 수 있는 공간이 만들어짐
        m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        m_Desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    }
    else
    {
        m_Desc.MipLevels = _iMipLevel;// MAX_MIP;	// 0 ==> 가능한 모든 밉맵이 저장 될 수 있는 공간이 만들어짐	
        m_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        m_Desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    }

    m_Desc.ArraySize = m_Image.GetMetadata().arraySize;

    m_Desc.SampleDesc.Count = 1;
    m_Desc.SampleDesc.Quality = 0;

    m_Desc.Usage = D3D11_USAGE_DEFAULT;
    m_Desc.CPUAccessFlags = 0;

    m_Desc.Width = m_Image.GetMetadata().width;
    m_Desc.Height = m_Image.GetMetadata().height;

    HRESULT hr = DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf());

    // 원본데이터(밉맵 레벨 0) 를 각 칸에 옮긴다.	
    for (int i = 0; i < m_Desc.ArraySize; ++i)
    {
        // GPU 에 데이터 옮기기(밉맵 포함)
        UINT iSubresIdx = D3D11CalcSubresource(0, i, m_Desc.MipLevels);

        CONTEXT->UpdateSubresource(m_Tex2D.Get(), iSubresIdx, nullptr
            , m_Image.GetImage(0, i, 0)->pixels
            , m_Image.GetImage(0, i, 0)->rowPitch
            , m_Image.GetImage(0, i, 0)->slicePitch);
    }

    // SRV 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC viewdesc = {};
    viewdesc.Format = m_Desc.Format;

    if (m_Image.GetMetadata().IsCubemap())
    {
        viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    }
    else if (1 < m_Desc.ArraySize)
    {
        viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    }
    else
    {
        viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    }

    if (1 < m_Desc.ArraySize)
    {
        viewdesc.Texture2DArray.ArraySize = m_Desc.ArraySize;
        viewdesc.Texture2DArray.MipLevels = m_Desc.MipLevels;
        viewdesc.Texture2DArray.MostDetailedMip = 0;
    }
    else
    {
        viewdesc.Texture2DArray.ArraySize = 1;
        viewdesc.Texture2D.MipLevels = m_Desc.MipLevels;
        viewdesc.Texture2D.MostDetailedMip = 0;
    }


    DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &viewdesc, m_SRV.GetAddressOf());

    // 밉맵 생성
    if (false == m_Image.GetMetadata().IsCubemap())
    {
        CONTEXT->GenerateMips(m_SRV.Get());
    }

    m_Tex2D->GetDesc(&m_Desc);

    CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
}


int CTexture::LoadHeightMap(const wstring& _strFilePath)
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

    m_Desc.Format = m_Image.GetMetadata().format;
    m_Desc.MipLevels = 1;
    m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    m_Desc.ArraySize = m_Image.GetMetadata().arraySize;
    m_Desc.SampleDesc.Count = 1;
    m_Desc.SampleDesc.Quality = 0;
    m_Desc.Usage = D3D11_USAGE_DEFAULT;
    m_Desc.CPUAccessFlags = 0;
    m_Desc.Width = m_Image.GetMetadata().width;
    m_Desc.Height = m_Image.GetMetadata().height;
    hr = DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf());

    if (FAILED(hr))
        return hr;

    // 원본데이터(밉맵 레벨 0) 를 각 칸에 옮긴다.   
    for (int i = 0; i < m_Desc.ArraySize; ++i)
    {
        // GPU 에 데이터 옮기기(밉맵 포함)
        UINT iSubresIdx = D3D11CalcSubresource(0, i, m_Desc.MipLevels);

        CONTEXT->UpdateSubresource(m_Tex2D.Get(), iSubresIdx, nullptr
            , m_Image.GetImage(0, i, 0)->pixels
            , m_Image.GetImage(0, i, 0)->rowPitch
            , m_Image.GetImage(0, i, 0)->slicePitch);
    }

    if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};

        tSRVDesc.Format = m_Desc.Format;
        tSRVDesc.Texture2D.MipLevels = 1;
        tSRVDesc.Texture2D.MostDetailedMip = 0;
        tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
        hr = DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &tSRVDesc, m_SRV.GetAddressOf());
        m_SRV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CTexture::m_SRV") - 1, "CTexture::m_SRV");

        assert(!FAILED(hr));
    }

    if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};
        tUAVDesc.Format = m_Desc.Format;
        tUAVDesc.Texture2D.MipSlice = 0;
        tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
        hr = DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), &tUAVDesc, m_UAV.GetAddressOf());
        m_UAV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CTexture::m_UAV") - 1, "CTexture::m_UAV");
        assert(!FAILED(hr));
    }

    CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);

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

    assert(!FAILED(hr));
    
    //DEPTH Stencill View는 한가지의 View만 가질 수 있다.
    if (_iBindFlag & D3D11_BIND_DEPTH_STENCIL)
    {
        hr = DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
        m_DSV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CTexture::m_DSV") - 1, "CTexture::m_DSV");
    }
    else
    {
        if (_iBindFlag & D3D11_BIND_RENDER_TARGET)
        {
            hr = DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
            assert(!FAILED(hr));
            m_RTV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CTexture::m_RTV") - 1, "CTexture::m_RTV");
        }

        if (_iBindFlag & D3D11_BIND_SHADER_RESOURCE)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
            
            tSRVDesc.Format = m_Desc.Format;
            tSRVDesc.Texture2D.MipLevels = 1;
            tSRVDesc.Texture2D.MostDetailedMip = 0;
            tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
            hr = DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &tSRVDesc, m_SRV.GetAddressOf());
            m_SRV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CTexture::m_SRV") - 1, "CTexture::m_SRV");
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
            m_UAV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CTexture::m_UAV") - 1, "CTexture::m_UAV");
            assert(!FAILED(hr));
        }
    }

    CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
}


int CTexture::CreateArrayTexture(const vector<Ptr<CTexture>>& _vecTex, int _iMapLevel)
{
    m_Desc = _vecTex[0]->GetDesc();
    m_Desc.ArraySize = (UINT)_vecTex.size();
    m_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    m_Desc.MipLevels = _iMapLevel;

    HRESULT hr = DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf());
    if (FAILED(hr))
        return hr;

    // 원본데이터(밉맵 레벨 0) 를 각 칸에 옮긴다.	
    for (int i = 0; i < _vecTex.size(); ++i)
    {
        UINT iOffset = D3D11CalcSubresource(0, i, _iMapLevel);
        CONTEXT->UpdateSubresource(m_Tex2D.Get(), iOffset, nullptr, _vecTex[i]->GetSysMem()
            , _vecTex[i]->GetRowPitch(), _vecTex[i]->GetSlicePitch());
    }

    // Shader Resource View 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC viewdesc = {};
    viewdesc.Format = m_Desc.Format;
    viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    viewdesc.Texture2DArray.MipLevels = _iMapLevel;
    viewdesc.Texture2DArray.MostDetailedMip = 0;
    viewdesc.Texture2DArray.ArraySize = _vecTex.size();

    DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &viewdesc, m_SRV.GetAddressOf());

    // 밉맵 생성
    CONTEXT->GenerateMips(m_SRV.Get());
    m_Tex2D->GetDesc(&m_Desc);

    CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);

    return hr;
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

void CTexture::GenerateMip(UINT _iMipLevel)
{
    m_Tex2D = nullptr;
    m_SRV = nullptr;
    m_RTV = nullptr;
    m_DSV = nullptr;
    m_UAV = nullptr;

    // Texture2D 생성
    D3D11_TEXTURE2D_DESC tDesc = {};
    tDesc.Format = m_Image.GetMetadata().format;

    if (m_Image.GetMetadata().IsCubemap())
    {
        tDesc.MipLevels = 1;	// 0 ==> 가능한 모든 밉맵이 저장 될 수 있는 공간이 만들어짐
        tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    }
    else
    {
        tDesc.MipLevels = _iMipLevel;// MAX_MIP;	// 0 ==> 가능한 모든 밉맵이 저장 될 수 있는 공간이 만들어짐	
        tDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        tDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    }

    tDesc.ArraySize = m_Image.GetMetadata().arraySize;

    tDesc.SampleDesc.Count = 1;
    tDesc.SampleDesc.Quality = 0;

    tDesc.Usage = D3D11_USAGE_DEFAULT;
    tDesc.CPUAccessFlags = 0;

    tDesc.Width = m_Image.GetMetadata().width;
    tDesc.Height = m_Image.GetMetadata().height;

    HRESULT hr = DEVICE->CreateTexture2D(&tDesc, nullptr, m_Tex2D.GetAddressOf());

    // 원본데이터(밉맵 레벨 0) 를 각 칸에 옮긴다.	
    for (int i = 0; i < tDesc.ArraySize; ++i)
    {
        // GPU 에 데이터 옮기기(밉맵 포함)
        UINT iSubresIdx = D3D11CalcSubresource(0, i, tDesc.MipLevels);

        CONTEXT->UpdateSubresource(m_Tex2D.Get(), iSubresIdx, nullptr
            , m_Image.GetImage(0, i, 0)->pixels
            , m_Image.GetImage(0, i, 0)->rowPitch
            , m_Image.GetImage(0, i, 0)->slicePitch);
    }

    // SRV 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC viewdesc = {};
    viewdesc.Format = tDesc.Format;

    if (m_Image.GetMetadata().IsCubemap())
    {
        viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    }
    else if (1 < tDesc.ArraySize)
    {
        viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    }
    else
    {
        viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    }

    if (1 < tDesc.ArraySize)
    {
        viewdesc.Texture2DArray.ArraySize = tDesc.ArraySize;
        viewdesc.Texture2DArray.MipLevels = tDesc.MipLevels;
        viewdesc.Texture2DArray.MostDetailedMip = 0;
    }
    else
    {
        viewdesc.Texture2DArray.ArraySize = 1;
        viewdesc.Texture2D.MipLevels = tDesc.MipLevels;
        viewdesc.Texture2D.MostDetailedMip = 0;
    }

    DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &viewdesc, m_SRV.GetAddressOf());

    // 밉맵 생성
    if (false == m_Image.GetMetadata().IsCubemap())
    {
        CONTEXT->GenerateMips(m_SRV.Get());
    }

    m_Tex2D->GetDesc(&m_Desc);
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

void CTexture::SaveTexture(const wstring& path)
{
    CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
    SaveToDDSFile(*m_Image.GetImages(), DDS_FLAGS_NONE, path.c_str());
}

void CTexture::SaveBmpFile(const wstring& _Path)
{
    const Image* pImage = m_Image.GetImages();

    int s = pImage->rowPitch;
    int k = pImage->slicePitch;

    int w = pImage->width;
    int h = pImage->height;

    CImage tImage(w, h);

    int i = 0;
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            //tImage.SetColor(tColor((float)x / (float)w, 1.0f - ((float)x / (float)w), (float)y / (float)h), x, y);
            if (0 != (float)pImage->pixels[i + 2])
            {
                int a = 0;
           }
           tImage.SetColor(tColor{ (float)pImage->pixels[i + 2], (float)pImage->pixels[i + 1], (float)pImage->pixels[i] }, x, y);
           i += 3;
        }
    }
    tImage.Export(string(_Path.begin(), _Path.end()).c_str());

    return;
}

void CTexture::LoadBmpFile(const wstring& _Path)
{
    CONTEXT->CopyResource(m_TexCopy.Get(), m_Tex2D.Get());
    D3D11_MAPPED_SUBRESOURCE tSub;
    
    Image* pImage = const_cast<Image*>(m_Image.GetImages());

    int x, y;
    uint8_t r = 0, g, b;
    int w = pImage->width;
    int h = pImage->height;
    int k = pImage->slicePitch;
    unsigned long size = w * h;
   
    FILE* pFile = nullptr;

    BYTE* pbuffer = LoadBMP(&w, &h, &size, _Path.c_str());

    CONTEXT->Map(m_TexCopy.Get(), 0, D3D11_MAP_WRITE, 0, &tSub);

    tSub.pData = pbuffer;

    CONTEXT->Unmap(m_TexCopy.Get(), 0);

    return;
}

void CTexture::SaveBitmapToFile(BYTE* pBitmapBits,
    LONG lWidth,
    LONG lHeight,
    WORD wBitsPerPixel,
    const unsigned long& padding_size,
    LPCTSTR lpszFileName)
{
    // Some basic bitmap parameters  
    unsigned long headers_size = sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER);

    unsigned long pixel_data_size = lHeight * ((lWidth * (wBitsPerPixel / 8)) + padding_size);

    BITMAPINFOHEADER bmpInfoHeader = { 0 };

    // Set the size  
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);

    // Bit count  
    bmpInfoHeader.biBitCount = wBitsPerPixel;

    // Use all colors  
    bmpInfoHeader.biClrImportant = 0;

    // Use as many colors according to bits per pixel  
    bmpInfoHeader.biClrUsed = 0;

    // Store as un Compressed  
    bmpInfoHeader.biCompression = BI_RGB;

    // Set the height in pixels  
    bmpInfoHeader.biHeight = lHeight;

    // Width of the Image in pixels  
    bmpInfoHeader.biWidth = lWidth;

    // Default number of planes  
    bmpInfoHeader.biPlanes = 1;

    // Calculate the image size in bytes  
    bmpInfoHeader.biSizeImage = pixel_data_size;

    BITMAPFILEHEADER bfh = { 0 };

    // This value should be values of BM letters i.e 0x4D42  
    // 0x4D = M 0×42 = B storing in reverse order to match with endian  
    bfh.bfType = 0x4D42;
    //bfh.bfType = 'B'+('M' << 8); 

    // <<8 used to shift ‘M’ to end  */  

    // Offset to the RGBQUAD  
    bfh.bfOffBits = headers_size;

    // Total size of image including size of headers  
    bfh.bfSize = headers_size + pixel_data_size;

    // Create the file in disk to write  
    HANDLE hFile = CreateFile(lpszFileName,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    // Return if error opening file  
    if (!hFile) return;

    DWORD dwWritten = 0;

    // Write the File header  
    WriteFile(hFile,
        &bfh,
        sizeof(bfh),
        &dwWritten,
        NULL);

    // Write the bitmap info header  
    WriteFile(hFile,
        &bmpInfoHeader,
        sizeof(bmpInfoHeader),
        &dwWritten,
        NULL);

    // Write the RGB Data  
    WriteFile(hFile,
        pBitmapBits,
        bmpInfoHeader.biSizeImage,
        &dwWritten,
        NULL);

    // Close the file handle  
    CloseHandle(hFile);
}

BYTE* CTexture::LoadBMP(int* width, int* height, unsigned long* size, LPCTSTR bmpfile)
{
    BITMAPFILEHEADER bmpheader;
    BITMAPINFOHEADER bmpinfo;
    // value to be used in ReadFile funcs
    DWORD bytesread;
    // open file to read from
    HANDLE file = CreateFile(bmpfile,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);
    if (NULL == file)
        return NULL;

    if (ReadFile(file, &bmpheader, sizeof(BITMAPFILEHEADER), &bytesread, NULL) == false)
    {
        CloseHandle(file);
        return NULL;
    }

    // Read bitmap info
    if (ReadFile(file, &bmpinfo, sizeof(BITMAPINFOHEADER), &bytesread, NULL) == false)
    {
        CloseHandle(file);
        return NULL;
    }

    // check if file is actually a bmp
    if (bmpheader.bfType != 'MB')
    {
        CloseHandle(file);
        return NULL;
    }

    // get image measurements
    *width = bmpinfo.biWidth;
    *height = abs(bmpinfo.biHeight);

    // Check if bmp iuncompressed
    if (bmpinfo.biCompression != BI_RGB)
    {
        CloseHandle(file);
        return NULL;
    }

    // Check if we have 24 bit bmp
    if (bmpinfo.biBitCount != 24)
    {
        CloseHandle(file);
        return NULL;
    }

    // create buffer to hold the data
    *size = bmpheader.bfSize - bmpheader.bfOffBits;
    BYTE* Buffer = new BYTE[*size];
    // move file pointer to start of bitmap data
    SetFilePointer(file, bmpheader.bfOffBits, NULL, FILE_BEGIN);
    // read bmp data
    if (ReadFile(file, Buffer, *size, &bytesread, NULL) == false)
    {
        delete[] Buffer;
        CloseHandle(file);
        return NULL;
    }

    // everything successful here: close file and return buffer

    CloseHandle(file);

    return Buffer;
}
