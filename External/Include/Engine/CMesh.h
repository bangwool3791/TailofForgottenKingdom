#pragma once

#include "CRes.h"
#include "CFBXLoader.h"

struct tIndexInfo
{
    ComPtr<ID3D11Buffer>    pIB;
    D3D11_BUFFER_DESC       tIBDesc;
    UINT				    iIdxCount;
    void* pIdxSysMem;
};

class CMesh : 
    public CRes
{
private:

    ComPtr<ID3D11Buffer>     m_readVB;
    ComPtr<ID3D11Buffer>     m_VB;
    D3D11_BUFFER_DESC        m_tVBDesc;
    UINT                     m_iVtxCount;
    void*                    m_pVtxSys;

    // 하나의 버텍스버퍼에 여러개의 인덱스버퍼가 연결
    vector<tIndexInfo>		m_vecIdxInfo;

    ComPtr<ID3D11Buffer>     m_IB;
    D3D11_BUFFER_DESC        m_tIBDesc;
    UINT                     m_iIdxCount;

    std::unique_ptr<Vtx[]>   m_vertices;

    //Ray
    Vec3                     m_vDir;
    Vec3                     m_vOrig;
public :
    int Create(void* _pVtxSys, size_t _iVtxCount, void* _pIdxSys, size_t _iIdxCount);
    int Create(
        void* _pVtxSysmem,
        size_t _iVtxCount,
        void* _pIdxSysmem,
        size_t _iIdxCount,
        D3D11_USAGE _usage,
        UINT cpuflag);

    void UpdateVertex(Vtx* vtx, size_t size);
    void render(UINT _iSubset);
    void UpdateData(UINT _iSubset);
    void render_particle(UINT _iCount);
    void Read();
    void Write();
    bool GetPosition(Ray _ray, Vec3& _vPos);
    Vec3 GetPosition(Ray _ray);
    bool SetTextureID(Ray _ray, float _id);
    Vtx* GetVertices(size_t& nVerts);
    bool IntersectTriangle(const Vec3& orig, const Vec3& dir, Vec3& v0, Vec3& v1, Vec3& v2, FLOAT* t, FLOAT* u, FLOAT* v);
    void InitializeTerrainJps(vector<Vec3>& _vec);
    virtual void Save(const wstring& _strRelativePath);
    virtual int Load(const wstring& _strFilePath) override;
    CLONE_ASSERT(CMesh);
public:
    Vtx* GetVtxSysMem() { return (Vtx*)m_pVtxSys; }
    UINT GetSubsetCount() { return (UINT)m_vecIdxInfo.size(); }

    static CMesh* CreateFromContainer(CFBXLoader& _loader);
public:
    ComPtr<ID3D11Buffer> GetBuffer() { return m_VB; }
public:
    CMesh(bool _bEngineRes = false);
    virtual ~CMesh();
};