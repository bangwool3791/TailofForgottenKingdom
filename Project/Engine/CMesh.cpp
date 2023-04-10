#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"
#include "CFBXLoader.h"
#include <minmax.h>

CMesh::CMesh(bool _bEngineRes)
	: CRes(RES_TYPE::MESH, _bEngineRes)
	, m_tVBDesc{}
	, m_tIBDesc{}
    , m_iVtxCount(0)
    , m_pVtxSys(nullptr)
{
}


CMesh::~CMesh()
{
    SAFE_DELETE(m_pVtxSys);

    for (size_t i = 0; i < m_vecIdxInfo.size(); ++i)
    {
        SAFE_DELETE(m_vecIdxInfo[i].pIdxSysMem);
    }

    // Animation3D ����
    SAFE_DELETE(m_pBoneFrameData);
    SAFE_DELETE(m_pBoneOffset);
}

int CMesh::Create(void* _pVtxSys, size_t _iVtxCount, void* _pIdxSys, size_t _iIdxCount)
{
    m_iVtxCount = _iVtxCount;

    tIndexInfo IndexInfo = {};
    IndexInfo.iIdxCount = _iIdxCount;

    // ���� �����͸� ������ ���ؽ� ���۸� �����Ѵ�.	
    m_tVBDesc.ByteWidth = sizeof(Vtx) * _iVtxCount;

    // ���� ���۴� ó�� �������Ŀ� ���۸� �������� �ʴ´�.
    m_tVBDesc.CPUAccessFlags = 0;
    m_tVBDesc.Usage = D3D11_USAGE_DEFAULT;

    // ������ �����ϴ� ������ ���� ���� �˸�
    m_tVBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
    m_tVBDesc.MiscFlags = 0;
    m_tVBDesc.StructureByteStride = 0;

    // �ʱ� �����͸� �Ѱ��ֱ� ���� ���� ����ü
    D3D11_SUBRESOURCE_DATA tSubDesc = {};
    tSubDesc.pSysMem = _pVtxSys;

    if (FAILED(DEVICE->CreateBuffer(&m_tVBDesc, &tSubDesc, m_VB.GetAddressOf())))
    {
        return E_FAIL;
    }

    IndexInfo.tIBDesc.ByteWidth = sizeof(UINT) * _iIdxCount;

    // ���� ���� ���Ŀ���, ������ ������ ���� �� �� �ִ� �ɼ�
    IndexInfo.tIBDesc.CPUAccessFlags = 0;
    IndexInfo.tIBDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

    // ������ �����ϴ� ������ ���� ���� �˸�
    IndexInfo.tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
    IndexInfo.tIBDesc.MiscFlags = 0;
    IndexInfo.tIBDesc.StructureByteStride = 0;

    // �ʱ� �����͸� �Ѱ��ֱ� ���� ���� ����ü
    tSubDesc = {};
    tSubDesc.pSysMem = _pIdxSys;

    if (FAILED(DEVICE->CreateBuffer(&IndexInfo.tIBDesc, &tSubDesc, IndexInfo.pIB.GetAddressOf())))
    {
        return E_FAIL;
    }

    // �ý��� �޸� �Ҵ�
    m_pVtxSys = new Vtx[m_iVtxCount];
    memcpy(m_pVtxSys, _pVtxSys, sizeof(Vtx) * m_iVtxCount);

    IndexInfo.pIdxSysMem = new UINT[IndexInfo.iIdxCount];
    memcpy(IndexInfo.pIdxSysMem, _pIdxSys, sizeof(UINT) * IndexInfo.iIdxCount);

    m_vecIdxInfo.push_back(IndexInfo);

    Read();

    return S_OK;
}
#include "Waves.h"

int CMesh::Create(
    void* _pVtxSysmem, 
    size_t _iVtxCount,
    void* _pIdxSysmem, 
    size_t _iIdxCount, 
    D3D11_USAGE _usage,
    UINT cpuflag)
{
    m_tVBDesc.ByteWidth = UINT(sizeof(Vtx) * _iVtxCount);
    m_iVtxCount = (UINT)_iVtxCount;
    m_tVBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tVBDesc.Usage = _usage;
    m_tVBDesc.CPUAccessFlags = cpuflag;

    HRESULT hr = DEVICE->CreateBuffer(&m_tVBDesc, 0, m_VB.GetAddressOf());
    //m_VB->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CComputeShader::m_VB") - 1, "CComputeShader::m_VB");

    if (FAILED(hr))
    {
        return hr;
    }

    m_iIdxCount = (UINT)_iIdxCount;
    m_tIBDesc.ByteWidth = sizeof(UINT) * (UINT)_iIdxCount;
    m_tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
    m_tIBDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
    m_tIBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA tSubData = {};

    tSubData = {};
    //memset(&tSubData, 0, sizeof(D3D11_SUBRESOURCE_DATA));

    tSubData.pSysMem = _pIdxSysmem;

    hr = DEVICE->CreateBuffer(&m_tIBDesc, &tSubData, &m_IB);
    //m_IB->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CComputeShader::m_IB") - 1, "CComputeShader::m_IB");

    return hr;
}


void CMesh::UpdateData(UINT _iSubset)
{
    UINT iStride = sizeof(Vtx);
    UINT iOffset = 0;

    //���� ���� ����
    CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
    //iSubset ��ġ �ε��� ���� ����
    CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

CMesh* CMesh::CreateFromContainer(CFBXLoader& _loader)
{
    const tContainer* container = &_loader.GetContainer(0);

    UINT iVtxCount = (UINT)container->vecPos.size();

    D3D11_BUFFER_DESC tVtxDesc = {};

    tVtxDesc.ByteWidth = sizeof(Vtx) * iVtxCount;
    tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    tVtxDesc.Usage = D3D11_USAGE_DEFAULT;
    if (D3D11_USAGE_DYNAMIC == tVtxDesc.Usage)
        tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA tSub = {};
    tSub.pSysMem = malloc(tVtxDesc.ByteWidth);
    Vtx* pSys = (Vtx*)tSub.pSysMem;
    for (UINT i = 0; i < iVtxCount; ++i)
    {
        pSys[i].vPos = container->vecPos[i];
        pSys[i].vUV = container->vecUV[i];
        pSys[i].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
        pSys[i].vNormal = container->vecNormal[i];
        pSys[i].vTangent = container->vecTangent[i];
        pSys[i].vBinormal = container->vecBinormal[i];
        pSys[i].vWeights = container->vecWeights[i];
        pSys[i].vIndices = container->vecIndices[i];
    }

    ComPtr<ID3D11Buffer> pVB = NULL;
    if (FAILED(DEVICE->CreateBuffer(&tVtxDesc, &tSub, pVB.GetAddressOf())))
    {
        return NULL;
    }

    CMesh* pMesh = new CMesh;
    pMesh->m_VB = pVB;
    pMesh->m_tVBDesc = tVtxDesc;
    pMesh->m_pVtxSys = pSys;

    // �ε��� ����
    UINT iIdxBufferCount = (UINT)container->vecIdx.size();
    D3D11_BUFFER_DESC tIdxDesc = {};

    for (UINT i = 0; i < iIdxBufferCount; ++i)
    {
        tIdxDesc.ByteWidth = (UINT)container->vecIdx[i].size() * sizeof(UINT); // Index Format �� R32_UINT �̱� ����
        tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        tIdxDesc.Usage = D3D11_USAGE_DEFAULT;
        if (D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
            tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        void* pSysMem = malloc(tIdxDesc.ByteWidth);
        memcpy(pSysMem, container->vecIdx[i].data(), tIdxDesc.ByteWidth);
        tSub.pSysMem = pSysMem;

        ComPtr<ID3D11Buffer> pIB = nullptr;
        if (FAILED(DEVICE->CreateBuffer(&tIdxDesc, &tSub, pIB.GetAddressOf())))
        {
            return NULL;
        }

        tIndexInfo info = {};
        info.tIBDesc = tIdxDesc;
        info.iIdxCount = (UINT)container->vecIdx[i].size();
        info.pIdxSysMem = pSysMem;
        info.pIB = pIB;

        pMesh->m_vecIdxInfo.push_back(info);
    }

    pMesh->Read();

    // Animation3D
    if (!container->bAnimation)
        return pMesh;

    vector<tBone*>& vecBone = _loader.GetBones();
    UINT iFrameCount = 0;
    for (UINT i = 0; i < vecBone.size(); ++i)
    {
        tMTBone bone = {};
        bone.iDepth = vecBone[i]->iDepth;
        bone.iParentIndx = vecBone[i]->iParentIndx;
        bone.matBone = GetMatrixFromFbxMatrix(vecBone[i]->matBone);
        bone.matOffset = GetMatrixFromFbxMatrix(vecBone[i]->matOffset);
        bone.strBoneName = vecBone[i]->strBoneName;

        for (UINT j = 0; j < vecBone[i]->vecKeyFrame.size(); ++j)
        {
            tMTKeyFrame tKeyframe = {};
            tKeyframe.dTime = vecBone[i]->vecKeyFrame[j].dTime;
            tKeyframe.iFrame = j;
            tKeyframe.vTranslate.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[0];
            tKeyframe.vTranslate.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[1];
            tKeyframe.vTranslate.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[2];

            tKeyframe.vScale.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[0];
            tKeyframe.vScale.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[1];
            tKeyframe.vScale.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[2];

            tKeyframe.qRot.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[0];
            tKeyframe.qRot.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[1];
            tKeyframe.qRot.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[2];
            tKeyframe.qRot.w = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[3];

            bone.vecKeyFrame.push_back(tKeyframe);
        }

        iFrameCount = max(iFrameCount, (UINT)bone.vecKeyFrame.size());

        pMesh->m_vecBones.push_back(bone);
    }

    vector<tAnimClip*>& vecAnimClip = _loader.GetAnimClip();

    for (UINT i = 0; i < vecAnimClip.size(); ++i)
    {
        tMTAnimClip tClip = {};

        tClip.strAnimName = vecAnimClip[i]->strName;
        tClip.dStartTime = vecAnimClip[i]->tStartTime.GetSecondDouble();
        tClip.dEndTime = vecAnimClip[i]->tEndTime.GetSecondDouble();
        tClip.dTimeLength = tClip.dEndTime - tClip.dStartTime;

        tClip.iStartFrame = (int)vecAnimClip[i]->tStartTime.GetFrameCount(vecAnimClip[i]->eMode);
        tClip.iEndFrame = (int)vecAnimClip[i]->tEndTime.GetFrameCount(vecAnimClip[i]->eMode);
        tClip.iFrameLength = tClip.iEndFrame - tClip.iStartFrame;
        tClip.eMode = vecAnimClip[i]->eMode;

        pMesh->m_vecAnimClip.push_back(tClip);
    }

    // Animation �� �ִ� Mesh ��� structuredbuffer �����α�
    if (pMesh->IsAnimMesh())
    {
        // BoneOffet ���
        vector<Matrix> vecOffset;
        vector<tFrameTrans> vecFrameTrans;
        vecFrameTrans.resize((UINT)pMesh->m_vecBones.size() * iFrameCount);

        for (size_t i = 0; i < pMesh->m_vecBones.size(); ++i)
        {
            vecOffset.push_back(pMesh->m_vecBones[i].matOffset);

            for (size_t j = 0; j < pMesh->m_vecBones[i].vecKeyFrame.size(); ++j)
            {
                vecFrameTrans[(UINT)pMesh->m_vecBones.size() * j + i]
                    = tFrameTrans{ Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vTranslate, 0.f)
                    , Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vScale, 0.f)
                    , pMesh->m_vecBones[i].vecKeyFrame[j].qRot };
            }
        }

        pMesh->m_pBoneOffset = new CStructuredBuffer;
        pMesh->m_pBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), SB_TYPE::SRV_ONLY, vecOffset.data(), false);

        pMesh->m_pBoneFrameData = new CStructuredBuffer;
        pMesh->m_pBoneFrameData->Create(sizeof(tFrameTrans), (UINT)vecOffset.size() * iFrameCount
            , SB_TYPE::SRV_ONLY, vecFrameTrans.data(), false);
    }

    return pMesh;
}

CMesh* CMesh::CreateFromContainer(CFBXLoader& _loader, UINT idx)
{
    const tContainer* container = &_loader.GetContainer(idx);

    UINT iVtxCount = (UINT)container->vecPos.size();

    D3D11_BUFFER_DESC tVtxDesc = {};

    tVtxDesc.ByteWidth = sizeof(Vtx) * iVtxCount;
    tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    tVtxDesc.Usage = D3D11_USAGE_DEFAULT;
    if (D3D11_USAGE_DYNAMIC == tVtxDesc.Usage)
        tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA tSub = {};
    tSub.pSysMem = malloc(tVtxDesc.ByteWidth);
    Vtx* pSys = (Vtx*)tSub.pSysMem;
    for (UINT i = 0; i < iVtxCount; ++i)
    {
        pSys[i].vPos = container->vecPos[i];
        pSys[i].vUV = container->vecUV[i];
        pSys[i].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
        pSys[i].vNormal = container->vecNormal[i];
        pSys[i].vTangent = container->vecTangent[i];
        pSys[i].vBinormal = container->vecBinormal[i];
        pSys[i].vWeights = container->vecWeights[i];
        pSys[i].vIndices = container->vecIndices[i];
    }

    ComPtr<ID3D11Buffer> pVB = NULL;
    if (FAILED(DEVICE->CreateBuffer(&tVtxDesc, &tSub, pVB.GetAddressOf())))
    {
        return NULL;
    }

    CMesh* pMesh = new CMesh;
    pMesh->m_VB = pVB;
    pMesh->m_tVBDesc = tVtxDesc;
    pMesh->m_pVtxSys = pSys;

    // �ε��� ����
    UINT iIdxBufferCount = (UINT)container->vecIdx.size();
    D3D11_BUFFER_DESC tIdxDesc = {};

    for (UINT i = 0; i < iIdxBufferCount; ++i)
    {
        tIdxDesc.ByteWidth = (UINT)container->vecIdx[i].size() * sizeof(UINT); // Index Format �� R32_UINT �̱� ����
        tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        tIdxDesc.Usage = D3D11_USAGE_DEFAULT;
        if (D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
            tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        void* pSysMem = malloc(tIdxDesc.ByteWidth);
        memcpy(pSysMem, container->vecIdx[i].data(), tIdxDesc.ByteWidth);
        tSub.pSysMem = pSysMem;

        ComPtr<ID3D11Buffer> pIB = nullptr;
        if (FAILED(DEVICE->CreateBuffer(&tIdxDesc, &tSub, pIB.GetAddressOf())))
        {
            return NULL;
        }

        tIndexInfo info = {};
        info.tIBDesc = tIdxDesc;
        info.iIdxCount = (UINT)container->vecIdx[i].size();
        info.pIdxSysMem = pSysMem;
        info.pIB = pIB;

        pMesh->m_vecIdxInfo.push_back(info);
    }

    pMesh->Read();

    // Animation3D
    if (!container->bAnimation)
        return pMesh;

    vector<tBone*>& vecBone = _loader.GetBones();
    UINT iFrameCount = 0;
    for (UINT i = 0; i < vecBone.size(); ++i)
    {
        tMTBone bone = {};
        bone.iDepth = vecBone[i]->iDepth;
        bone.iParentIndx = vecBone[i]->iParentIndx;
        bone.matBone = GetMatrixFromFbxMatrix(vecBone[i]->matBone);
        bone.matOffset = GetMatrixFromFbxMatrix(vecBone[i]->matOffset);
        bone.strBoneName = vecBone[i]->strBoneName;

        for (UINT j = 0; j < vecBone[i]->vecKeyFrame.size(); ++j)
        {
            tMTKeyFrame tKeyframe = {};
            tKeyframe.dTime = vecBone[i]->vecKeyFrame[j].dTime;
            tKeyframe.iFrame = j;
            tKeyframe.vTranslate.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[0];
            tKeyframe.vTranslate.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[1];
            tKeyframe.vTranslate.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[2];

            tKeyframe.vScale.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[0];
            tKeyframe.vScale.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[1];
            tKeyframe.vScale.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[2];

            tKeyframe.qRot.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[0];
            tKeyframe.qRot.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[1];
            tKeyframe.qRot.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[2];
            tKeyframe.qRot.w = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[3];

            bone.vecKeyFrame.push_back(tKeyframe);
        }

        iFrameCount = max(iFrameCount, (UINT)bone.vecKeyFrame.size());

        pMesh->m_vecBones.push_back(bone);
    }

    vector<tAnimClip*>& vecAnimClip = _loader.GetAnimClip();

    for (UINT i = 0; i < vecAnimClip.size(); ++i)
    {
        tMTAnimClip tClip = {};

        tClip.strAnimName = vecAnimClip[i]->strName;
        tClip.dStartTime = vecAnimClip[i]->tStartTime.GetSecondDouble();
        tClip.dEndTime = vecAnimClip[i]->tEndTime.GetSecondDouble();
        tClip.dTimeLength = tClip.dEndTime - tClip.dStartTime;

        tClip.iStartFrame = (int)vecAnimClip[i]->tStartTime.GetFrameCount(vecAnimClip[i]->eMode);
        tClip.iEndFrame = (int)vecAnimClip[i]->tEndTime.GetFrameCount(vecAnimClip[i]->eMode);
        tClip.iFrameLength = tClip.iEndFrame - tClip.iStartFrame;
        tClip.eMode = vecAnimClip[i]->eMode;

        pMesh->m_vecAnimClip.push_back(tClip);
    }

    // Animation �� �ִ� Mesh ��� structuredbuffer �����α�
    if (pMesh->IsAnimMesh())
    {
        // BoneOffet ���
        vector<Matrix> vecOffset;
        vector<tFrameTrans> vecFrameTrans;
        vecFrameTrans.resize((UINT)pMesh->m_vecBones.size() * iFrameCount);

        for (size_t i = 0; i < pMesh->m_vecBones.size(); ++i)
        {
            vecOffset.push_back(pMesh->m_vecBones[i].matOffset);

            for (size_t j = 0; j < pMesh->m_vecBones[i].vecKeyFrame.size(); ++j)
            {
                vecFrameTrans[(UINT)pMesh->m_vecBones.size() * j + i]
                    = tFrameTrans{ Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vTranslate, 0.f)
                    , Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vScale, 0.f)
                    , pMesh->m_vecBones[i].vecKeyFrame[j].qRot };
            }
        }

        pMesh->m_pBoneOffset = new CStructuredBuffer;
        pMesh->m_pBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), SB_TYPE::SRV_ONLY, vecOffset.data(), false);

        pMesh->m_pBoneFrameData = new CStructuredBuffer;
        pMesh->m_pBoneFrameData->Create(sizeof(tFrameTrans), (UINT)vecOffset.size() * iFrameCount
            , SB_TYPE::SRV_ONLY, vecFrameTrans.data(), false);
    }

    return pMesh;
}
void CMesh::render(UINT _iSubset)
{
    //�ε��� ���� ������Ʈ
    UpdateData(_iSubset);
    //draw
    CONTEXT->DrawIndexed(m_vecIdxInfo[_iSubset].iIdxCount, 0, 0);
}

void CMesh::render_particle(UINT _iCount)
{
    UpdateData(0);
    CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].iIdxCount, _iCount, 0, 0, 0);
}

void CMesh::Read()
{
    if (nullptr != m_readVB)
    {
        m_readVB = nullptr;
    }

    D3D11_BUFFER_DESC VbufferDesc;

    VbufferDesc.Usage = D3D11_USAGE_STAGING;
    VbufferDesc.ByteWidth = m_tVBDesc.ByteWidth;
    VbufferDesc.BindFlags = 0;
    VbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    VbufferDesc.MiscFlags = 0;

    //Create the Vertex staging buffer.
    HRESULT hr = DEVICE->CreateBuffer(&VbufferDesc, NULL, &m_readVB);
    assert(hr == S_OK);
    //Copy veBuffer into staging buffer so  it can be mapped/read
    CONTEXT->CopyResource(m_readVB.Get(), m_VB.Get());

    size_t nVerts = m_tVBDesc.ByteWidth / sizeof(Vtx);
    m_vertices = std::make_unique<Vtx[]>(nVerts);

    //map vertex buffer on GPU
    D3D11_MAPPED_SUBRESOURCE msV = {};
    HRESULT hrm = CONTEXT->Map(m_readVB.Get(), NULL, D3D11_MAP_READ, NULL, &msV);
    assert(hrm == S_OK);

    //loop through buffer
    if (!nVerts)
        cout << "Failed computing nVerts" << endl;
    else
    {
        memcpy(m_vertices.get(), msV.pData, nVerts * sizeof(Vtx));
       //cout << m_VB.Get() << endl; //Current Buffer veing processed
        //for (size_t i = 0; i < nVerts; i++)
        //{
        //   cout << m_vertices[i].vPos.x << ", "
        //       << m_vertices[i].vPos.y << ", "
        //       << m_vertices[i].vPos.z << endl; //
        //} 
        //cout << "buffer end" << endl; //
        //cout << "" << endl; // 
    }

    CONTEXT->Unmap(m_readVB.Get(), 0);
}

void CMesh::ReadByConsole()
{
    if (nullptr != m_readVB)
    {
        m_readVB = nullptr;
    }

    D3D11_BUFFER_DESC VbufferDesc;

    VbufferDesc.Usage = D3D11_USAGE_STAGING;
    VbufferDesc.ByteWidth = m_tVBDesc.ByteWidth;
    VbufferDesc.BindFlags = 0;
    VbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    VbufferDesc.MiscFlags = 0;

    //Create the Vertex staging buffer.
    HRESULT hr = DEVICE->CreateBuffer(&VbufferDesc, NULL, &m_readVB);
    assert(hr == S_OK);
    //Copy veBuffer into staging buffer so  it can be mapped/read
    CONTEXT->CopyResource(m_readVB.Get(), m_VB.Get());

    size_t nVerts = m_tVBDesc.ByteWidth / sizeof(Vtx);
    m_vertices = std::make_unique<Vtx[]>(nVerts);

    //map vertex buffer on GPU
    D3D11_MAPPED_SUBRESOURCE msV = {};
    HRESULT hrm = CONTEXT->Map(m_readVB.Get(), NULL, D3D11_MAP_READ, NULL, &msV);
    assert(hrm == S_OK);

    //loop through buffer
    if (!nVerts)
        cout << "Failed computing nVerts" << endl;
    else
    {
        memcpy(m_vertices.get(), msV.pData, nVerts * sizeof(Vtx));
        cout << m_VB.Get() << endl; //Current Buffer veing processed
         for (size_t i = 0; i < nVerts; i++)
         {
            cout << m_vertices[i].vNormal.x << ", "
                << m_vertices[i].vNormal.y << ", "
                << m_vertices[i].vNormal.z << endl; //
         } 
         cout << "buffer end" << endl; //
         cout << "" << endl; // 
    }

    CONTEXT->Unmap(m_readVB.Get(), 0);
}

void CMesh::Write()
{

    ComPtr<ID3D11Buffer> WriteBuffer;
    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    // Not sure if the temp buffer needs to have D3D11_BIND_UNORDERED_ACCESS
    // like the Unity buffer does.
    // We're only writing into the temp buffer from a single thread.
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.ByteWidth = m_tVBDesc.ByteWidth;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    // Using MiscFlags and StructureByteStride from Unity's buffer doesn't seem to work
    desc.MiscFlags = 0; // unityDesc.MiscFlags;
    //desc.StructureByteStride = unityDesc.StructureByteStride;
    HRESULT hr = DEVICE->CreateBuffer(&desc, NULL, &WriteBuffer);

    D3D11_MAPPED_SUBRESOURCE mapped;

    size_t nVerts = m_tVBDesc.ByteWidth / sizeof(Vtx);

    hr = CONTEXT->Map(WriteBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
    memcpy(mapped.pData, m_vertices.get(), m_tVBDesc.ByteWidth);
    CONTEXT->Unmap(WriteBuffer.Get(), 0);

    CONTEXT->CopyResource(m_VB.Get(), WriteBuffer.Get());
}

void CMesh::UpdateVertex(Vtx* vtx, size_t size)
{
    for (int i = 0; i < size; ++i)
    {
        assert(&vtx[i]);
        m_vertices[i] = vtx[i];
    }
    Write();
}

bool CMesh::SetTextureID(Ray _ray, float _id)
{
	Read();

    size_t nVerts = m_tVBDesc.ByteWidth / sizeof(Vtx);

    for (UINT i = 0; i < nVerts; i+= 4)
    {
        float fDist;
        if (_ray.Intersects(m_vertices[i].vPos, m_vertices[i + 1].vPos, m_vertices[i + 2].vPos, fDist))
        {
            int a = 0;
            m_vertices[i].vColor.y = _id;
            m_vertices[i + 1].vColor.y = _id;
            m_vertices[i + 2].vColor.y = _id;
            m_vertices[i + 3].vColor.y = _id;
            Write();
            return true;
        }

        if (_ray.Intersects(m_vertices[i].vPos, m_vertices[i + 2].vPos, m_vertices[i + 3].vPos, fDist))
        {
            int a = 0;
            m_vertices[i].vColor.y = _id;
            m_vertices[i + 1].vColor.y = _id;
            m_vertices[i + 2].vColor.y = _id;
            m_vertices[i + 3].vColor.y = _id;
            Write();
            return true;
        }
    } 
	return false;
}

Vec3 CMesh::GetPosition(Ray _ray)
{
    size_t nVerts = m_tVBDesc.ByteWidth / sizeof(Vtx);

    for (UINT i = 0; i < nVerts; i += 4)
    {
        float fDist;
        if (_ray.Intersects(m_vertices[i].vPos, m_vertices[i + 1].vPos, m_vertices[i + 2].vPos, fDist))
        {
            Vec3 vTarget = _ray.direction * fDist;
            vTarget += _ray.position;
            return vTarget;
        }

        if (_ray.Intersects(m_vertices[i].vPos, m_vertices[i + 2].vPos, m_vertices[i + 3].vPos, fDist))
        {
            Vec3 vTarget = _ray.direction * fDist;
            vTarget += _ray.position;
            return vTarget;
        }
    }
    return Vec3(-1.f, -1.f, -1.f);
}

bool CMesh::GetPosition(Ray _ray, Vec3& _vPos)
{
    size_t nVerts = m_tVBDesc.ByteWidth / sizeof(Vtx);

    for (UINT i = 0; i < nVerts; i += 4)
    {
        float fDist;
        if (_ray.Intersects(m_vertices[i].vPos, m_vertices[i + 1].vPos, m_vertices[i + 2].vPos, fDist))
        {
            _vPos = _ray.direction * fDist;
            _vPos += _ray.position;
            return true;
        }

        if (_ray.Intersects(m_vertices[i].vPos, m_vertices[i + 2].vPos, m_vertices[i + 3].vPos, fDist))
        {
            _vPos = _ray.direction * fDist;
            _vPos += _ray.position;
            return true;
        }
    }
    return false;
}

void CMesh::InitializeTerrainJps(vector<Vec3>& _vec)
{
    Read();
    
    _vec.clear();

    size_t nVerts = m_tVBDesc.ByteWidth / sizeof(Vtx);

    _vec.resize(nVerts / 4);
    
    int index = 0;
    for (UINT i = 0; i < nVerts; i += 4)
    {
        _vec[index].x = (m_vertices[i].vPos.x + m_vertices[i + 2].vPos.x) * 0.5f;
        _vec[index].y = (m_vertices[i + 1].vPos.y + m_vertices[i + 3].vPos.y) * 0.5f;
        _vec[index].z = (m_vertices[i + 1].vPos.z + m_vertices[i + 3].vPos.z) * 0.5f;
        ++index;
    }
}

// ������ �浹 üũ
// vOrig�� ī�޶� ����, vDir�� Ray ����
// v0, v1, v2 �� �ﰢ���� ����
// t �� vOrig���� v0 ������ �Ÿ�
// u �� v0���� v1 �̰�, v �� v0���� v2 �̴�.
bool CMesh::IntersectTriangle(const Vec3& orig, const Vec3& dir, Vec3& v0, Vec3& v1, Vec3& v2, FLOAT* t, FLOAT* u, FLOAT* v)
{     // Find vectors for two edges sharing vert0     
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    // Begin calculating determinant - also used to calculate U parameter     
    Vec3 pvec;    
    pvec = dir.Cross(edge2);
    // If determinant is near zero, ray lies in plane of triangle     
    FLOAT det = edge1.Dot(pvec);
    Vec3 tvec;
    if( det > 0 )     
    {         tvec = orig - v0;     }     
    else     {         tvec = v0 - orig;         det = -det;     }    
    if( det < 0.0001f )         
        return FALSE;     
    // Calculate U parameter and test bounds     
    *u = tvec.Dot(pvec);     
    if( *u < 0.0f || *u > det )         
        return FALSE;    
    // Prepare to test V parameter    
    Vec3 qvec;     
    qvec = tvec.Cross(edge1);    
    // Calculate V parameter and test bounds    
    *v = dir.Dot(qvec);     
    if( *v < 0.0f || *u + *v > det )         
        return FALSE;     
    // Calculate t, scale parameters, ray intersects triangle     
    *t = edge2.Dot(qvec);     
    FLOAT fInvDet = 1.0f / det;     
    *t *= fInvDet;     
    *u *= fInvDet;     
    *v *= fInvDet;     
    return TRUE; 
}

void CMesh::Save(const wstring& _strRelativePath)
{
    // ����� ����
    SetRelativePath(_strRelativePath);

    // ���� ��� �����
    wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _strRelativePath;

    // ���� ������� ����
    FILE* pFile = nullptr;

    errno_t err = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
    assert(pFile);

    // Ű��, ��� ���	
    SaveWStringToFile(GetName(), pFile);
    SaveWStringToFile(GetKey(), pFile);
    SaveWStringToFile(GetRelativePath(), pFile);

    // ���� ������ ����				
    int iByteSize = m_tVBDesc.ByteWidth;
    fwrite(&iByteSize, sizeof(int), 1, pFile);
    fwrite(m_pVtxSys, iByteSize, 1, pFile);

    // �ε��� ����
    UINT iMtrlCount = (UINT)m_vecIdxInfo.size();
    fwrite(&iMtrlCount, sizeof(int), 1, pFile);

    UINT iIdxBuffSize = 0;
    /*
    * Save Index buffer as a Mtrl size 
    */
    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        fwrite(&m_vecIdxInfo[i], sizeof(tIndexInfo), 1, pFile);
        fwrite(m_vecIdxInfo[i].pIdxSysMem
            , m_vecIdxInfo[i].iIdxCount * sizeof(UINT)
            , 1, pFile);
    }

    // Animation3D ���� 
    UINT iCount = (UINT)m_vecAnimClip.size();
    fwrite(&iCount, sizeof(int), 1, pFile);
    for (UINT i = 0; i < iCount; ++i)
    {
        SaveWStringToFile(m_vecAnimClip[i].strAnimName, pFile);
        fwrite(&m_vecAnimClip[i].dStartTime, sizeof(double), 1, pFile);
        fwrite(&m_vecAnimClip[i].dEndTime, sizeof(double), 1, pFile);
        fwrite(&m_vecAnimClip[i].dTimeLength, sizeof(double), 1, pFile);
        fwrite(&m_vecAnimClip[i].eMode, sizeof(int), 1, pFile);
        fwrite(&m_vecAnimClip[i].fUpdateTime, sizeof(float), 1, pFile);
        fwrite(&m_vecAnimClip[i].iStartFrame, sizeof(int), 1, pFile);
        fwrite(&m_vecAnimClip[i].iEndFrame, sizeof(int), 1, pFile);
        fwrite(&m_vecAnimClip[i].iFrameLength, sizeof(int), 1, pFile);
    }

    iCount = (UINT)m_vecBones.size();
    fwrite(&iCount, sizeof(int), 1, pFile);

    for (UINT i = 0; i < iCount; ++i)
    {
        SaveWStringToFile(m_vecBones[i].strBoneName, pFile);
        fwrite(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
        fwrite(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
        fwrite(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
        fwrite(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

        int iFrameCount = (int)m_vecBones[i].vecKeyFrame.size();
        fwrite(&iFrameCount, sizeof(int), 1, pFile);

        for (int j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
        {
            fwrite(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
        }
    }

    fclose(pFile);
}

int CMesh::Load(const wstring& _strFilePath)
{
    // �б���� ���Ͽ���
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

    // Ű��, �����
    wstring strName, strKey, strRelativePath;
    LoadWStringFromFile(strName, pFile);
    LoadWStringFromFile(strKey, pFile);
    LoadWStringFromFile(strRelativePath, pFile);

    SetName(strName);
    SetKey(strKey);
    SetRelativePath(strRelativePath);

    // ����������
    UINT iByteSize = 0;
    fread(&iByteSize, sizeof(int), 1, pFile);

    m_pVtxSys = (Vtx*)malloc(iByteSize);
    fread(m_pVtxSys, 1, iByteSize, pFile);


    D3D11_BUFFER_DESC tDesc = {};
    tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    tDesc.ByteWidth = iByteSize;
    tDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA tSubData = {};
    tSubData.pSysMem = m_pVtxSys;

    if (FAILED(DEVICE->CreateBuffer(&tDesc, &tSubData, m_VB.GetAddressOf())))
    {
        assert(nullptr);
    }

    // �ε��� ����
    UINT iMtrlCount = 0;
    fread(&iMtrlCount, sizeof(int), 1, pFile);

    for (UINT i = 0; i < iMtrlCount; ++i)
    {
        tIndexInfo info = {};
        fread(&info, sizeof(tIndexInfo), 1, pFile);

        UINT iByteWidth = info.iIdxCount * sizeof(UINT);

        void* pSysMem = malloc(iByteWidth);
        info.pIdxSysMem = pSysMem;
        fread(info.pIdxSysMem, iByteWidth, 1, pFile);

        tSubData.pSysMem = info.pIdxSysMem;

        if (FAILED(DEVICE->CreateBuffer(&info.tIBDesc, &tSubData, info.pIB.GetAddressOf())))
        {
            assert(nullptr);
        }

        m_vecIdxInfo.push_back(info);
    }

    // Animation3D ���� �б�
    int iCount = 0;
    fread(&iCount, sizeof(int), 1, pFile);
    for (int i = 0; i < iCount; ++i)
    {
        tMTAnimClip tClip = {};

        LoadWStringFromFile(tClip.strAnimName, pFile);
        fread(&tClip.dStartTime, sizeof(double), 1, pFile);
        fread(&tClip.dEndTime, sizeof(double), 1, pFile);
        fread(&tClip.dTimeLength, sizeof(double), 1, pFile);
        fread(&tClip.eMode, sizeof(int), 1, pFile);
        fread(&tClip.fUpdateTime, sizeof(float), 1, pFile);
        fread(&tClip.iStartFrame, sizeof(int), 1, pFile);
        fread(&tClip.iEndFrame, sizeof(int), 1, pFile);
        fread(&tClip.iFrameLength, sizeof(int), 1, pFile);

        m_vecAnimClip.push_back(tClip);
    }

    iCount = 0;
    fread(&iCount, sizeof(int), 1, pFile);
    m_vecBones.resize(iCount);

    UINT _iFrameCount = 0;
    for (int i = 0; i < iCount; ++i)
    {
        LoadWStringFromFile(m_vecBones[i].strBoneName, pFile);
        fread(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
        fread(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
        fread(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
        fread(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

        UINT iFrameCount = 0;
        fread(&iFrameCount, sizeof(int), 1, pFile);
        m_vecBones[i].vecKeyFrame.resize(iFrameCount);
        _iFrameCount = max(_iFrameCount, iFrameCount);
        for (UINT j = 0; j < iFrameCount; ++j)
        {
            fread(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
        }
    }

    // Animation �� �ִ� Mesh ��� Bone StructuredBuffer �����
    if (m_vecAnimClip.size() > 0 && m_vecBones.size() > 0)
    {
        wstring strBone = GetName();

        // BoneOffet ���
        vector<Matrix> vecOffset;
        vector<tFrameTrans> vecFrameTrans;
        vecFrameTrans.resize((UINT)m_vecBones.size() * _iFrameCount);

        for (size_t i = 0; i < m_vecBones.size(); ++i)
        {
            vecOffset.push_back(m_vecBones[i].matOffset);

            for (size_t j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
            {
                vecFrameTrans[(UINT)m_vecBones.size() * j + i]
                    = tFrameTrans{ Vec4(m_vecBones[i].vecKeyFrame[j].vTranslate, 0.f)
                    , Vec4(m_vecBones[i].vecKeyFrame[j].vScale, 0.f)
                    , Vec4(m_vecBones[i].vecKeyFrame[j].qRot) };
            }
        }

        m_pBoneOffset = new CStructuredBuffer;
        m_pBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), SB_TYPE::SRV_ONLY, vecOffset.data(), false);

        m_pBoneFrameData = new CStructuredBuffer;
        m_pBoneFrameData->Create(sizeof(tFrameTrans), (UINT)vecOffset.size() * (UINT)_iFrameCount
            , SB_TYPE::SRV_ONLY, vecFrameTrans.data(), false);
    }



    fclose(pFile);

    return S_OK;
}


Vtx* CMesh::GetVertices(size_t& _nVerts)
{
    size_t nVerts = m_tVBDesc.ByteWidth / sizeof(Vtx);
    _nVerts = nVerts;
    return m_vertices.get();
}