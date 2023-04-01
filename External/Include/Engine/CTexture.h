#pragma once
#include "CRes.h"

#include <DirectXTex/DirectXTex.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex_debug")
#else
#pragma comment(lib, "DirectXTex/DirectXTex")
#endif


class CTexture :
    public CRes
{
private:
    uint8_t*                            m_pPixels;
    long long                           m_iPixelSize;
    FLOAT                               m_fWidth;
    FLOAT                               m_fHeight;
public:
    uint8_t* GetPixel() { return m_pPixels; }
private:
    ScratchImage                        m_Image;    // System Mem �� �ε��� �̹��� �ȼ������� ����
    ComPtr<ID3D11Texture2D>             m_Tex2D;    // GPU Mem �� �ű� ������ �����ϴ� ��ü
    ComPtr<ID3D11Texture2D>             m_TexCopy;    // GPU Mem �� �ű� ������ �����ϴ� ��ü

    ComPtr<ID3D11RenderTargetView>      m_RTV;      // Tex2D �� OM(OutputMerge) RenderTarget ���� ������ �� ���
    ComPtr<ID3D11RenderTargetView>      m_arrRTV[6];
    ComPtr<ID3D11DepthStencilView>      m_DSV;      // Tex2D �� OM(OutputMerge) DepthStencil �� ������ �� ���
    ComPtr<ID3D11ShaderResourceView>    m_SRV;      // Tex2D �� t �������Ϳ� ���ε��� �� ���
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;      // Tex2D �� ��ǻƮ ���̴� u �������Ϳ� ������ �� ���

    D3D11_TEXTURE2D_DESC                m_Desc;     // Tex2D Description ����ü
public:
    // PIPELINE_STAGE
    UINT GetWidth() { return m_Desc.Width; }
    UINT GetHeight() { return m_Desc.Height; }
    UINT GetArraySize() { return m_Desc.ArraySize; }

    ComPtr<ID3D11Texture2D>           GetTex2D() { return m_Tex2D; }
    ComPtr<ID3D11RenderTargetView>    GetRTV() { return  m_RTV; }
    ComPtr<ID3D11DepthStencilView>    GetDSV() { return  m_DSV; }
    ComPtr<ID3D11ShaderResourceView>  GetSRV() { return  m_SRV; }
    ComPtr<ID3D11UnorderedAccessView> GetUAV() { return  m_UAV; }
    const D3D11_TEXTURE2D_DESC& GetDesc() { return m_Desc; }
    void* GetSysMem() { return m_Image.GetPixels(); }
    UINT  GetRowPitch() const { return (UINT)m_Image.GetImages()->rowPitch; }
    UINT  GetSlicePitch()const { return (UINT)m_Image.GetImages()->slicePitch; }

    void GenerateMip(UINT _iMipLevel);

    pair<UINT, UINT*> GetBuffer();

    void UpdateData(UINT _iRegisterNum, UINT _iPipelineStage);
    void UpdateData_CS(UINT _iRegisterNum, bool _bShaderRes);

    static void Clear(UINT _iRegisterNum);
    static void Clear_CS(UINT _iRegisterNum);
    static void Clear_CS(UINT _iRegisterNum, bool _bShaderRes);

    virtual int Load(const wstring& _strFilePath);
private:
    // ���Ϸ� ���� �ε�
    virtual void Save(const wstring& _strRelativePath) {}
    int Load(const wstring& _strFilePath, int _iMipLevel);
    virtual int LoadHeightMap(const wstring& _strFilePath);
    void SaveBitmapToFile(BYTE* pBitmapBits,
        LONG lWidth,
        LONG lHeight,
        WORD wBitsPerPixel,
        const unsigned long& padding_size,
        LPCTSTR lpszFileName);
    BYTE* LoadBMP(int* width, int* height, unsigned long* size, LPCTSTR bmpfile);

public:
    void SaveTexture(const wstring& _path);
    void SaveTextureArray(const wstring& path);
    void SaveBmpFile(const wstring& _strRelativePath);
    void LoadBmpFile(const wstring& _Path);
public:
    // ����
    void Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _Format, UINT _iBindFlag);
    void Create(ComPtr<ID3D11Texture2D> _Tex2D);
    void CreateCubeTexture(vector<Ptr<CTexture>> _arrTex);
    int CreateArrayTexture(const vector<Ptr<CTexture>>& _vecTex, int _iMapLevel);
    CLONE_ASSERT(CTexture);
public:
    CTexture(bool _bEngineRes = false);
    virtual ~CTexture();
};

