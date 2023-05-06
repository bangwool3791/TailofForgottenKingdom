#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"
#include "CFBXLoader.h"
#include "CInstancingBuffer.h"
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

    // Animation3D 정보
    SAFE_DELETE(m_pBoneFrameData);
    SAFE_DELETE(m_pBoneOffset);
}

int CMesh::Create(void* _pVtxSys, size_t _iVtxCount, void* _pIdxSys, size_t _iIdxCount)
{
    m_iVtxCount = _iVtxCount;

    tIndexInfo IndexInfo = {};
    IndexInfo.iIdxCount = _iIdxCount;

    // 정점 데이터를 저장할 버텍스 버퍼를 생성한다.	
    m_tVBDesc.ByteWidth = sizeof(Vtx) * _iVtxCount;

    // 정점 버퍼는 처음 생성이후에 버퍼를 수정하지 않는다.
    m_tVBDesc.CPUAccessFlags = 0;
    m_tVBDesc.Usage = D3D11_USAGE_DEFAULT;

    // 정점을 저장하는 목적의 버퍼 임을 알림
    m_tVBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
    m_tVBDesc.MiscFlags = 0;
    m_tVBDesc.StructureByteStride = 0;

    // 초기 데이터를 넘겨주기 위한 정보 구조체
    D3D11_SUBRESOURCE_DATA tSubDesc = {};
    tSubDesc.pSysMem = _pVtxSys;

    if (FAILED(DEVICE->CreateBuffer(&m_tVBDesc, &tSubDesc, m_VB.GetAddressOf())))
    {
        return E_FAIL;
    }

    IndexInfo.tIBDesc.ByteWidth = sizeof(UINT) * _iIdxCount;

    // 버퍼 생성 이후에도, 버퍼의 내용을 수정 할 수 있는 옵션
    IndexInfo.tIBDesc.CPUAccessFlags = 0;
    IndexInfo.tIBDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

    // 정점을 저장하는 목적의 버퍼 임을 알림
    IndexInfo.tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
    IndexInfo.tIBDesc.MiscFlags = 0;
    IndexInfo.tIBDesc.StructureByteStride = 0;

    // 초기 데이터를 넘겨주기 위한 정보 구조체
    tSubDesc = {};
    tSubDesc.pSysMem = _pIdxSys;

    if (FAILED(DEVICE->CreateBuffer(&IndexInfo.tIBDesc, &tSubDesc, IndexInfo.pIB.GetAddressOf())))
    {
        return E_FAIL;
    }

    // 시스템 메모리 할당
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

    //정점 정보 전달
    CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
    //iSubset 위치 인덱스 정보 전달
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

    UINT iIdxBufferCount = (UINT)container->vecIdx.size();
    D3D11_BUFFER_DESC tIdxDesc = {};

    for (UINT i = 0; i < iIdxBufferCount; ++i)
    {
        tIdxDesc.ByteWidth = (UINT)container->vecIdx[i].size() * sizeof(UINT); // Index Format 이 R32_UINT 이기 때문
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

    // Animation 이 있는 Mesh 경우 structuredbuffer 만들어두기
    if (pMesh->IsAnimMesh())
    {
        // BoneOffet 행렬
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
        pMesh->m_pBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), SB_TYPE::SRV_ONLY, vecOffset.data(), true);

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

    // 인덱스 정보
    UINT iIdxBufferCount = (UINT)container->vecIdx.size();
    D3D11_BUFFER_DESC tIdxDesc = {};

    for (UINT i = 0; i < iIdxBufferCount; ++i)
    {
        tIdxDesc.ByteWidth = (UINT)container->vecIdx[i].size() * sizeof(UINT); // Index Format 이 R32_UINT 이기 때문
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

    // Animation 이 있는 Mesh 경우 structuredbuffer 만들어두기
    if (pMesh->IsAnimMesh())
    {
        // BoneOffet 행렬
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
        pMesh->m_pBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), SB_TYPE::SRV_ONLY, vecOffset.data(), true);

        pMesh->m_pBoneFrameData = new CStructuredBuffer;
        pMesh->m_pBoneFrameData->Create(sizeof(tFrameTrans), (UINT)vecOffset.size() * iFrameCount
            , SB_TYPE::SRV_ONLY, vecFrameTrans.data(), false);
    }

    return pMesh;
}

void CMesh::UpdateData_Inst(UINT _iSubset)
{
    if (_iSubset >= m_vecIdxInfo.size())
        assert(nullptr);

    //Instacing WVP, RowIdx 정보 2번 슬롯 전달
    ID3D11Buffer* arrBuffer[2] = { m_VB.Get(), CInstancingBuffer::GetInst()->GetBuffer().Get() };
    UINT		  iStride[2] = { sizeof(Vtx), sizeof(tInstancingData) };
    UINT		  iOffset[2] = { 0, 0 };

    CONTEXT->IASetVertexBuffers(0, 2, arrBuffer, iStride, iOffset);
    CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
}



void CMesh::render(UINT _iSubset)
{
    //인덱스 정보 업데이트
    UpdateData(_iSubset);
    //draw
    CONTEXT->DrawIndexed(m_vecIdxInfo[_iSubset].iIdxCount, 0, 0);
}

void CMesh::render_instancing(UINT _iSubset)
{
    UpdateData_Inst(_iSubset);

    CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[_iSubset].iIdxCount
        , CInstancingBuffer::GetInst()->GetInstanceCount(), 0, 0, 0);
}

void CMesh::render_particle(UINT _iCount)
{
    UpdateData_Inst(0);
   // CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].iIdxCount, sizeof(tInstancingData) * _iCount, 0, 0, 0);
    CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].iIdxCount
        , _iCount, 0, 0, 0);
}

void CMesh::Read()
{
    if (nullptr != m_readVB)
    {
        m_readVB = nullptr;
    }

    D3D11_BUFFER_DESC VbufferDesc{};

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

bool CMesh::IsNavValid(Vec3& _vPos)
{
    size_t nVerts = m_tVBDesc.ByteWidth / sizeof(Vtx);

    //A -> 0, B -> 1, C -> 2
    //AP X AB
    //BP X BC
    //CP X CA
    bool bValid = false;

    UINT i = 0;
    UINT cnt = 0;
    float fDistance{};
    float fMinHeight = 987654321.f;

    float fDistance1{};
    float fDistance2{};
    float fDistance3{};

    float W1{};
    float W2{};
    float W3{};

    float fMinDistance{ 987654321.f };

    //P-A = AP
    Vec3 vCenter{};
    Vec3 vResult[3]{};
    Vec3 vDebug[3]{};
    Vec3 vVtx[3]{};

    for (i = 0; i < nVerts; i += 3)
    {
        cnt = 0;
        vDebug[0] = m_vertices[i].vPos;
        vDebug[1] = m_vertices[i + 1].vPos;
        vDebug[2] = m_vertices[i + 2].vPos;

        Vec3 AP = m_vertices[i].vPos - _vPos;
        Vec3 AB = m_vertices[i].vPos - m_vertices[i + 1].vPos;

        Vec3 BP = m_vertices[i + 1].vPos - _vPos;
        Vec3 BC = m_vertices[i + 1].vPos - m_vertices[i + 2].vPos;

        Vec3 CP = m_vertices[i + 2].vPos - _vPos;
        Vec3 CA = m_vertices[i + 2].vPos - m_vertices[i].vPos;

        AB.Cross(AP, vResult[0]);
        BC.Cross(BP, vResult[1]);
        CA.Cross(CP, vResult[2]);
    
        for (UINT j = 0; j < 3; ++j)
        {
            if (0 > vResult[j].y)
                break;
            else if (0 < vResult[j].y)
                ++cnt;
        }

        fDistance1 = m_vertices[i].vPos.Distance(_vPos, m_vertices[i].vPos);
        fDistance2 = m_vertices[i + 1].vPos.Distance(_vPos, m_vertices[i + 1].vPos);
        fDistance3 = m_vertices[i + 2].vPos.Distance(_vPos, m_vertices[i + 2].vPos);

        W1 = 1.f / fDistance1;
        W2 = 1.f / fDistance2;
        W3 = 1.f / fDistance3;

        float fHeight = (m_vertices[i].vPos.y * W1 + m_vertices[i + 1].vPos.y * W2 + m_vertices[i + 2].vPos.y * W3) / (W1 + W2 + W3);

        if (3 == cnt && fabsf(_vPos.y - fHeight) < 10.f)
        {
            if (fHeight < fMinHeight)
            {
                fMinHeight = fHeight;
                _vPos.y = fHeight;
                return true;
            }
        }

    }

    return false;
}

bool CMesh::IsNavJumpValid(Vec3& _vPos)
{
    size_t nVerts = m_tVBDesc.ByteWidth / sizeof(Vtx);

    //A -> 0, B -> 1, C -> 2
    //AP X AB
    //BP X BC
    //CP X CA
    bool bValid = false;

    UINT i = 0;
    UINT cnt = 0;
    float fDistance{};

    float fDistance1{};
    float fDistance2{};
    float fDistance3{};

    float W1{};
    float W2{};
    float W3{};

    float fMinHeight = 987654321.f;
    float fMinDistance{ 987654321.f };

    //P-A = AP
    Vec3 vCenter{};
    Vec3 vResult[3]{};
    Vec3 vDebug[3]{};
    Vec3 vVtx[3]{};

    for (i = 0; i < nVerts; i += 3)
    {
        cnt = 0;

        Vec3 AP = m_vertices[i].vPos - _vPos;
        Vec3 AB = m_vertices[i].vPos - m_vertices[i + 1].vPos;

        Vec3 BP = m_vertices[i + 1].vPos - _vPos;
        Vec3 BC = m_vertices[i + 1].vPos - m_vertices[i + 2].vPos;

        Vec3 CP = m_vertices[i + 2].vPos - _vPos;
        Vec3 CA = m_vertices[i + 2].vPos - m_vertices[i].vPos;

        AB.Cross(AP, vResult[0]);
        BC.Cross(BP, vResult[1]);
        CA.Cross(CP, vResult[2]);

        fDistance1 = m_vertices[i].vPos.Distance(_vPos, m_vertices[i].vPos);
        fDistance2 = m_vertices[i + 1].vPos.Distance(_vPos, m_vertices[i + 1].vPos);
        fDistance3 = m_vertices[i + 2].vPos.Distance(_vPos, m_vertices[i + 2].vPos);

        W1 = 1.f / fDistance1;
        W2 = 1.f / fDistance2;
        W3 = 1.f / fDistance3;

        float fX = (m_vertices[i].vPos.x * W1 + m_vertices[i + 1].vPos.x * W2 + m_vertices[i + 2].vPos.x * W3) / (W1 + W2 + W3);
        float fHeight = (m_vertices[i].vPos.y * W1 + m_vertices[i + 1].vPos.y * W2 + m_vertices[i + 2].vPos.y * W3) / (W1 + W2 + W3);
        float fZ = (m_vertices[i].vPos.z * W1 + m_vertices[i + 1].vPos.z * W2 + m_vertices[i + 2].vPos.z * W3) / (W1 + W2 + W3);

        for (UINT j = 0; j < 3; ++j)
        {
            if (0 > vResult[j].y)
                break;
            else if (0 < vResult[j].y)
                ++cnt;
        }

        if (3 == cnt && _vPos.y <= fHeight)
        {
            if (fHeight < fMinHeight)
            {
                fMinHeight = fHeight;
                //_vPos.x = fX;
                _vPos.y = fHeight;
                //_vPos.z = fZ;
                bValid = true;
            }
        }
    }

    return bValid;
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

void CMesh::Save(const wstring& _strRelativePath)
{
    // 상대경로 복사
    SetRelativePath(_strRelativePath);

    // 파일 경로 만들기
    wstring strFilePath = CPathMgr::GetInst()->GetContentPath() + _strRelativePath;

    // 파일 쓰기모드로 열기
    FILE* pFile = nullptr;

    errno_t err = _wfopen_s(&pFile, strFilePath.c_str(), L"wb");
    assert(pFile);

    // 키값, 상대 경로	
    SaveWStringToFile(GetName(), pFile);
    SaveWStringToFile(GetKey(), pFile);
    SaveWStringToFile(GetRelativePath(), pFile);

    // 정점 데이터 저장				
    int iByteSize = m_tVBDesc.ByteWidth;
    fwrite(&iByteSize, sizeof(int), 1, pFile);
    fwrite(m_pVtxSys, iByteSize, 1, pFile);

    // 인덱스 정보
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

    // Animation3D 정보 
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
    // 읽기모드로 파일열기
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

    // 키값, 상대경로
    wstring strName, strKey, strRelativePath;
    LoadWStringFromFile(strName, pFile);
    LoadWStringFromFile(strKey, pFile);
    LoadWStringFromFile(strRelativePath, pFile);

    SetName(strName);
    SetKey(strKey);
    SetRelativePath(strRelativePath);

    // 정점데이터
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

    m_tVBDesc = tDesc;

    // 인덱스 정보
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

    // Animation3D 정보 읽기
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

    // Animation 이 있는 Mesh 경우 Bone StructuredBuffer 만들기
    if (m_vecAnimClip.size() > 0 && m_vecBones.size() > 0)
    {
        wstring strBone = GetName();

        // BoneOffet 행렬
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
        m_pBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), SB_TYPE::SRV_ONLY, vecOffset.data(), true);

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