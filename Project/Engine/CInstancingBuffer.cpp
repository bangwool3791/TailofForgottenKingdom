#include "pch.h"
#include "CInstancingBuffer.h"

#include "CDevice.h"
#include "CStructuredBuffer.h"
#include "CCopyBoneShader.h"

#include "Ptr.h"
#include "CMaterial.h"
#include "CResMgr.h"

//인스턴싱으로 전달 될 구조화 버퍼 클래스
CInstancingBuffer::CInstancingBuffer()
	: m_iMaxCount(10)
	, m_iAnimInstCount(0)
	, m_pBoneBuffer(nullptr)
{
	m_pBoneBuffer = new CStructuredBuffer;
}

CInstancingBuffer::~CInstancingBuffer()
{
	SAFE_DELETE(m_pBoneBuffer);
}

void CInstancingBuffer::init()
{
	D3D11_BUFFER_DESC tDesc = {};

	//얘가 GPU로 바인딩 되는 원리 ?
	tDesc.ByteWidth = sizeof(tInstancingData) * m_iMaxCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, NULL, &m_pInstancingBuffer)))
		assert(NULL);

	m_pCopyShader = (CCopyBoneShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"CopyBoneShader").Get();
}

void CInstancingBuffer::SetData()
{
	if (m_vecData.size() > m_iMaxCount)
	{
		//최대 사이즈보다 크다면 구조화 버퍼 Resize
		Resize((UINT)m_vecData.size());
	}

	D3D11_MAPPED_SUBRESOURCE tMap = {};

	//쓰기로 접근
	/*
	row_major matrix matWorld : WORLD;
	row_major matrix matWV : WV;
	row_major matrix matWVP : WVP;
	*/
	CONTEXT->Map(m_pInstancingBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);
	memcpy(tMap.pData, &m_vecData[0], sizeof(tInstancingData) * m_vecData.size());
	//카메라 에서 받은 WVP, RowIdx 정보를 GPU로 전달한다.
	CONTEXT->Unmap(m_pInstancingBuffer.Get(), 0);

	// 본 행렬정보 메모리 복사
	if (m_vecBoneMat.empty())
		return;

	UINT iBufferSize = (UINT)m_vecBoneMat.size() * m_vecBoneMat[0]->GetBufferSize();
	if (m_pBoneBuffer->GetBufferSize() < iBufferSize)
	{
		m_pBoneBuffer->Create(m_vecBoneMat[0]->GetElementSize()
			, m_vecBoneMat[0]->GetElementCount() * (UINT)m_vecBoneMat.size(), SB_TYPE::UAV_INC, nullptr);
	}

	// 복사용 컴퓨트 쉐이더 실행
	UINT iBoneCount = m_vecBoneMat[0]->GetElementCount();
	m_pCopyShader->SetBoneCount(iBoneCount);

	for (UINT i = 0; i < (UINT)m_vecBoneMat.size(); ++i)
	{
		//구조화 버퍼의 가로 길이
		//뼈 행렬 1개당 Compute Shader 전달
		m_pCopyShader->SetRowIndex(i);
		m_pCopyShader->SetSourceBuffer(m_vecBoneMat[i]);
		m_pCopyShader->SetDestBuffer(m_pBoneBuffer);
		//결과 버퍼 전달
		m_pCopyShader->Execute();
	}

	// Bone 정보 전달 레지스터
	// Bone 정보는 구조화 버퍼 전달
	m_pBoneBuffer->UpdateData(61, PIPELINE_STAGE::VS);
}


void CInstancingBuffer::AddInstancingBoneMat(CStructuredBuffer* _pBuffer)
{
	++m_iAnimInstCount;
	m_vecBoneMat.push_back(_pBuffer);
}

void CInstancingBuffer::Resize(UINT _iCount)
{
	m_pInstancingBuffer = nullptr;

	m_iMaxCount = _iCount;

	D3D11_BUFFER_DESC tDesc = {};

	tDesc.ByteWidth = sizeof(tInstancingData) * m_iMaxCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, NULL, &m_pInstancingBuffer)))
		assert(NULL);
}

