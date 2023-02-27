#pragma once
#include "CShader.h"

class CGraphicsShader
	:public CShader
{
private:
    ComPtr<ID3DBlob>                m_VSBlob;
    ComPtr<ID3DBlob>                m_HSBlob;
    ComPtr<ID3DBlob>                m_DSBlob;
    ComPtr<ID3DBlob>                m_GSBlob;
    ComPtr<ID3DBlob>                m_PSBlob;

    ComPtr<ID3D11VertexShader>      m_VS;
    ComPtr<ID3D11HullShader>        m_HS;
    ComPtr<ID3D11DomainShader>      m_DS;
    ComPtr<ID3D11GeometryShader>    m_GS;
    ComPtr<ID3D11PixelShader>       m_PS;

    ComPtr<ID3D11InputLayout>       m_Layout;

    D3D10_PRIMITIVE_TOPOLOGY        m_eTopology;

    RS_TYPE                         m_eRSType;
    BS_TYPE                         m_eBSType;
    DS_TYPE                         m_eDSType;
    SHADER_DOMAIN                   m_eDomain;

public :
    void CreateVertexShader(const wstring& _strRelativePath, const string& _strFuncName);

    //void CreateHullShader(const wstring& _strRelativePath, const string& _strFuncName);
    //void CreateDomainShader(const wstring& _strRelativePath, const string& _strFuncName);
    void CreateGeometryShader(const wstring& _strRelativePath, const string& _strFuncName);

    void CreatePixelShader(const wstring& _strRelativePath, const string& _strFuncName);

    void UpdateData();
    
public :
    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY _eTopology) { m_eTopology = _eTopology; }
    void SetRSType(RS_TYPE _eType) { m_eRSType = _eType; }
    void SetBSType(BS_TYPE _eType) { m_eBSType = _eType; }
    void SetDSType(DS_TYPE _eType) { m_eDSType = _eType; }

    void SetDomain(SHADER_DOMAIN _eDomain) { m_eDomain = _eDomain; }
    SHADER_DOMAIN GetDomain()              { return m_eDomain; }
public :
    CGraphicsShader();
    virtual ~CGraphicsShader();
};