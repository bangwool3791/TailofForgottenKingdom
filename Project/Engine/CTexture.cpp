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

#include "CPathMgr.h"

void CTexture::SaveTexture(const wstring& path)
{
    ScratchImage image; 
    CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), image);
    SaveToDDSFile(*image.GetImages(), DDS_FLAGS_NONE, path.c_str());
   //HRESULT hr;
   // // Write out the render target as a PNG
  //  D3DX11SaveTextureToFileW(CONTEXT, m_Tex2D.Get(), D3DX11_IFF_PNG, L"D:\Dev\MyEngine\D3D\AssortRockDx112D-master\\SCREENSHOT.PNG");
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
#include "CImage.h"

void CTexture::SaveBmpFile(const wstring& _Path)
{
    ScratchImage image;
    CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), image);
    const Image* pImage = image.GetImages();

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
    //BYTE* buffer = new BYTE[w * h];

    //for (size_t i = 0; i < k; i += 4)
    //{
    //    // flip the order of every 3 bytes
    //    unsigned char tmp = pImage->pixels[i];
    //    pImage->pixels[i] = pImage->pixels[i + 2];
    //    pImage->pixels[i + 2] = tmp;
    //}

    //SaveBitmapToFile(pImage->pixels, w, h, 24, 0, _Path.c_str());


    return;
}


void CTexture::LoadBmpFile(const wstring& _Path)
{
    ScratchImage image;
    CONTEXT->CopyResource(m_TexCopy.Get(), m_Tex2D.Get());
    D3D11_MAPPED_SUBRESOURCE tSub;
    
    CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), image);
    Image* pImage = const_cast<Image*>(image.GetImages());

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
