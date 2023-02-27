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

public :
    uint8_t* GetPixel() { return m_pPixels; }
private:
    ScratchImage                        m_Image;    // System Mem 에 로딩된 이미지 픽셀데이터 관리
    ComPtr<ID3D11Texture2D>             m_Tex2D;    // GPU Mem 에 옮긴 데이터 관리하는 객체
    
    ComPtr<ID3D11RenderTargetView>      m_RTV;      // Tex2D 를 OM(OutputMerge) RenderTarget 으로 전달할 때 사용
    ComPtr<ID3D11DepthStencilView>      m_DSV;      // Tex2D 를 OM(OutputMerge) DepthStencil 로 전달할 때 사용
    ComPtr<ID3D11ShaderResourceView>    m_SRV;      // Tex2D 를 t 레지스터에 바인딩할 때 사용
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;      // Tex2D 를 컴퓨트 쉐이더 u 레지스터에 전달할 때 사용

    D3D11_TEXTURE2D_DESC                m_Desc;     // Tex2D Description 구조체
public:
    // PIPELINE_STAGE
    void UpdateData(UINT _iRegisterNum, UINT _iPipelineStage);
    void UpdateData_CS(UINT _iRegisterNum, bool _bShaderRes);

    UINT  GetWidth()     { return m_Desc.Width; }
    UINT  GetHeight()    { return m_Desc.Height; }

    ComPtr<ID3D11Texture2D>           GetTex2D() {return m_Tex2D;}
    ComPtr<ID3D11RenderTargetView>    GetRTV() { return  m_RTV; }
    ComPtr<ID3D11DepthStencilView>    GetDSV() { return  m_DSV; }
    ComPtr<ID3D11ShaderResourceView>  GetSRV() { return  m_SRV; }
    ComPtr<ID3D11UnorderedAccessView> GetUAV() { return  m_UAV; }

    static void Clear(UINT _iRegisterNum);
    static void Clear_CS(UINT _iRegisterNum);
private:
    // 파일로 부터 로딩
    virtual void Save(const wstring& _strRelativePath) {}
    virtual int Load(const wstring& _strFilePath);
public:
    // 생성
    void Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _Format, UINT _iBindFlag);
    void Create(ComPtr<ID3D11Texture2D> _Tex2D);

    CLONE_ASSERT(CTexture);
public:
    CTexture(bool _bEngineRes = false);
    virtual ~CTexture();
};

