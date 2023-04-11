#pragma once

#include "CRes.h"
#include "CFBXLoader.h"
#include "CStructuredBuffer.h"

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
    //���� ���� �ʿ�
    void*                    m_pVtxSys;

    // �ϳ��� ���ؽ����ۿ� �������� �ε������۰� ����
    vector<tIndexInfo>		m_vecIdxInfo;

    ComPtr<ID3D11Buffer>     m_IB;
    D3D11_BUFFER_DESC        m_tIBDesc;
    UINT                     m_iIdxCount;

    //���� ���� ? 
    std::unique_ptr<Vtx[]>   m_vertices;

    //Ray
    Vec3                     m_vDir;
    Vec3                     m_vOrig;

    // Animation3D ����
    vector<tMTAnimClip>		m_vecAnimClip;
    vector<tMTBone>			m_vecBones;

    CStructuredBuffer*      m_pBoneFrameData;   // ��ü �� ������ ����(ũ��, �̵�, ȸ��) (������ ������ŭ)
    CStructuredBuffer*      m_pBoneOffset;	    // �� ���� offset ���(�� ���� ��ġ�� �ǵ����� ���) (1�� ¥��)
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
    void render_instancing(UINT _iSubset);
    void render_particle(UINT _iCount);
    void UpdateData(UINT _iSubset);
    void UpdateData_Inst(UINT _iSubset);

    void Read();
    void ReadByConsole();
    void Write();

    bool GetPosition(Ray _ray, Vec3& _vPos);
    Vec3 GetPosition(Ray _ray);
    Vtx* GetVertices(size_t& nVerts);
    void InitializeTerrainJps(vector<Vec3>& _vec);

    virtual void Save(const wstring& _strRelativePath);
    virtual int Load(const wstring& _strFilePath) override;
    CLONE_ASSERT(CMesh);

public:
    const vector<tMTBone>* GetBones() { return &m_vecBones; }
    UINT GetBoneCount() { return (UINT)m_vecBones.size(); }
    const vector<tMTAnimClip>* GetAnimClip() { return &m_vecAnimClip; }
    bool IsAnimMesh() { return !m_vecAnimClip.empty(); }

    CStructuredBuffer* GetBoneFrameDataBuffer() { return m_pBoneFrameData; } // ��ü �� ������ ����
    CStructuredBuffer* GetBoneOffsetBuffer() { return  m_pBoneOffset; }	   // �� ���� offset ���	    
public:
    Vtx* GetVtxSysMem() { return (Vtx*)m_pVtxSys; }
    UINT GetSubsetCount() { return (UINT)m_vecIdxInfo.size(); }

    static CMesh* CreateFromContainer(CFBXLoader& _loader);
    static CMesh* CreateFromContainer(CFBXLoader& _loader, UINT idx);
public:
    ComPtr<ID3D11Buffer> GetBuffer() { return m_VB; }
public:
    CMesh(bool _bEngineRes = false);
    virtual ~CMesh();
};