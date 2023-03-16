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
 
    m_Desc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;
    m_Desc.CPUAccessFlags = 0;

    m_Desc.Format = _Format;
    m_Desc.Width = m_fWidth = _iWidth;
    m_Desc.Height = m_fHeight = _iHeight;
    m_Desc.ArraySize = 1;

    m_Desc.SampleDesc.Count = 1;////MSAA(Multi Sampling Anti Aliasing)������? 1, 0 �̸� �Ⱦ��ٴ� �ɼ�.
    m_Desc.SampleDesc.Quality = 0;
    //�̹����߿� �ϳ��� �����ϴ°��� SampleDesc ����Ѵ�.
    /*
        �ȼ��� ������ ��������� �����µ� �̰��� Aliasing�̶���Ѵ�.
        �׷��� �� ��������� ���ַ��� ���� Anti Aliasing�̶�� �Ѵ�.
        �ؿ� AA�� Anti Aliasing�̴�.
        SSAA//Super Sampling Anti Aliasing
        - 4�辿 �ø��� ������ �� �� �ٽ� ���δ�. �׷��� �����ϰ� �����.
        ���� : ����� �ʹ� ���. �׷��� �� �Ⱦ���.

        MSAA//Multi Sampling Anti Aliasing (�����忡�� ���� ���� ����̴�)
        - Anti Aliasing�� �Ͼ�°� �ᱹ �׵θ��̴�. �׷��� �׵θ��� ��� �������Ѵ�.
        ���� : �̰͵� ����� ���. ����ڰ� �� ����� ų �� �ְ� �ɼ��� �־� �� �� �ִ�.
    */
    m_Desc.MipLevels = 1;
    m_Desc.MiscFlags = 0;
    HRESULT hr = DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf());
    assert(!FAILED(hr));
    
    //DEPTH Stencill View�� �Ѱ����� View�� ���� �� �ִ�.
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

void CTexture::CreateCubeTexture(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _Format, UINT _iBindFlag)
{
    ID3D11Texture2D* cubeTexture = NULL;
    ComPtr<ID3D11ShaderResourceView> shaderResourceView = NULL;

    //Description of each face
    D3D11_TEXTURE2D_DESC texDesc = {};

    D3D11_TEXTURE2D_DESC texDesc1 = {};
    texDesc1.Width = _iWidth;
    texDesc1.Height = _iHeight;
    texDesc1.MipLevels = 9;
    texDesc1.Format = _Format;
    //The Shader Resource view description
    D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc = {};
    wstring str[6] = { L"LeftTargetTex", L"RightTargetTex", L"DownTargetTex", L"UpTargetTex",L"BackTargetTex", L"FrontTargetTex" };
    ComPtr<ID3D11Texture2D> tex[6]{};

    for (size_t i = 0; i < 6; ++i)
    {
        tex[i] = CResMgr::GetInst()->FindRes<CTexture>(str[i])->GetTex2D();
    }

    tex[0]->GetDesc(&texDesc1);

    texDesc.Width = texDesc1.Width;
    texDesc.Height = texDesc1.Height;
    texDesc.MipLevels = texDesc1.MipLevels;
    texDesc.ArraySize = 6;
    texDesc.Format = texDesc1.Format;
    texDesc.CPUAccessFlags = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    SMViewDesc.Format = texDesc.Format;
    SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    SMViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
    SMViewDesc.TextureCube.MostDetailedMip = 0;


    DEVICE->CreateTexture2D(&texDesc, NULL, &cubeTexture);
    for (int i = 0; i < 6; i++)
    {

        for (UINT mipLevel = 0; mipLevel < texDesc.MipLevels; ++mipLevel)
        {
            D3D11_MAPPED_SUBRESOURCE mappedTex2D;
            memset(&mappedTex2D, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
            HRESULT hr = CONTEXT->Map(tex[i].Get(), mipLevel, D3D11_MAP_READ, 0, &mappedTex2D);
            assert(SUCCEEDED(hr));
            CONTEXT->UpdateSubresource(cubeTexture,
                D3D11CalcSubresource(mipLevel, i, texDesc.MipLevels),
                0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

            CONTEXT->Unmap(tex[i].Get(), mipLevel);
        }
    }

    DEVICE->CreateShaderResourceView(cubeTexture, &SMViewDesc, shaderResourceView.GetAddressOf());
}

void CTexture::Init_EnvViewPort()
{
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
