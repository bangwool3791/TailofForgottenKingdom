#include "pch.h"

#include "CAnimator3D.h"
#include "CTimeMgr.h"
#include "CMeshRender.h"
#include "CStructuredBuffer.h"
#include "CResMgr.h"

#include "CAnimation3DShader.h"

#include "CKeyMgr.h"

CAnimator3D::CAnimator3D()
	: m_pVecBones{}
	, m_pVecClip{}
	, m_iCurClip{}
	, m_dCurTime{}
	, m_iFrameCount(30)
	, m_pBoneFinalMatBuffer{}
	, m_bFinalMatUpdate{false}
	, m_iFrameIdx{}
	, m_iNextFrameIdx{}
	, m_fRatio{0.f}
	,CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::CAnimator3D(const CAnimator3D& _origin)
	: m_pVecBones(_origin.m_pVecBones)
	, m_pVecClip(_origin.m_pVecClip)
	, m_iCurClip(_origin.m_iCurClip)
	, m_dCurTime(_origin.m_dCurTime)
	, m_iFrameCount(_origin.m_iFrameCount)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_iFrameIdx(_origin.m_iFrameIdx)
	, m_iNextFrameIdx(_origin.m_iNextFrameIdx)
	, m_fRatio(_origin.m_fRatio)
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer;

	SetBones(m_pVecBones);
	SetAnimClip(m_pVecClip);
}

CAnimator3D::~CAnimator3D()
{
	SAFE_DELETE(m_pBoneFinalMatBuffer);
}

void CAnimator3D::finaltick()
{
	m_dCurTime = 0.f;

	m_vecClipUpdateTime[m_iCurClip] += DT;

	//총 애니메이션 시간을 넘어가면 초기화
	if (m_vecClipUpdateTime[m_iCurClip] >= m_pVecClip->at(m_iCurClip).dTimeLength)
	{
		m_vecClipUpdateTime[m_iCurClip] = 0.f;
	}

	m_dCurTime = m_pVecClip->at(m_iCurClip).dStartTime + m_vecClipUpdateTime[m_iCurClip];

	double dFrameIdx = m_dCurTime * (double)m_iFrameCount;

	m_iFrameIdx = (int)(dFrameIdx);

	//인덱스가 넘어가면 초기화
	if (m_iFrameIdx >= m_pVecClip->at(0).iFrameLength - 1)
	{
		m_iNextFrameIdx = m_iFrameIdx;
	}
	else
	{
		m_iNextFrameIdx = m_iFrameIdx + 1;
	}

	// 프레임간의 시간에 따른 비율을 구해준다.
	// double 형 frameidx - int형 framidx
	// 소수점만 남음
	m_fRatio = (float)(dFrameIdx - (double)m_iFrameIdx);

	// 컴퓨트 쉐이더 연산여부
	m_bFinalMatUpdate = false;
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip)
{
	m_pVecClip = _vecAnimClip;
	m_vecClipUpdateTime.resize(m_pVecClip->size());

	//시작 시간으로 초기화 필요 ?
	static float fTime = 0.f;
	fTime += 1.f;
	//업데이트 시간 초기화
	m_vecClipUpdateTime[0] = fTime;
}

void CAnimator3D::UpdateData()
{
	//뼈 행렬 업데이트 완료가 안됬으면
	if (!m_bFinalMatUpdate)
	{
		// Animation3D Update Compute Shader
		CAnimation3DShader* pUpdateShader = (CAnimation3DShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"Animation3DUpdateShader").Get();

		// Bone Data
		Ptr<CMesh> pMesh = MeshRender()->GetMesh();
		check_mesh(pMesh);

		//CS Set
		//뼈 프레임 Set
		pUpdateShader->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer());
		//뼈 Offset 행렬 Set
		pUpdateShader->SetOffsetMatBuffer(pMesh->GetBoneOffsetBuffer());
		//뼈 최종 행렬 Set
		pUpdateShader->SetOutputBuffer(m_pBoneFinalMatBuffer);
		//뼈 개수 Set
		UINT iBoneCount = (UINT)m_pVecBones->size();
		pUpdateShader->SetBoneCount(iBoneCount);
		//현재 프레임 인덱스 
		pUpdateShader->SetFrameIndex(m_iFrameIdx);
		//다음 프레임 인덱스
		pUpdateShader->SetNextFrameIdx(m_iNextFrameIdx);
		//다음 프레임 진행 비율 Set
		pUpdateShader->SetFrameRatio(m_fRatio);

		// 업데이트 쉐이더 실행
		pUpdateShader->Execute();

		m_bFinalMatUpdate = true;
	}

	// t30 레지스터에 최종행렬 데이터(구조버퍼) 바인딩		
	m_pBoneFinalMatBuffer->UpdateData(58, PIPELINE_STAGE::VS);
}

void CAnimator3D::ClearData()
{
	//최종 뼈 행렬 클리어
	m_pBoneFinalMatBuffer->Clear();
	//랜더
	UINT iMtrlCount = MeshRender()->GetMtrlCount();
	Ptr<CMaterial> pMtrl = nullptr;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		pMtrl = MeshRender()->GetSharedMaterial(i);
		if (nullptr == pMtrl)
			continue;

		//애니메이션 정보 false
		pMtrl->SetAnim3D(false); // Animation Mesh 알리기
		//뼈 개수 0
		pMtrl->SetBoneCount(0);
	}
}

/*
* 메시 뼈 카운트와, 뼈 구조화 버퍼 카운트가 불일치하면 갱신
*/
void CAnimator3D::check_mesh(Ptr<CMesh> _pMesh)
{
	UINT iBoneCount = _pMesh->GetBoneCount();
	if (m_pBoneFinalMatBuffer->GetElementCount() != iBoneCount)
	{
		m_pBoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::UAV_INC, nullptr, false);
	}
}

void CAnimator3D::SaveToFile(FILE* _pFile)
{
}

void CAnimator3D::LoadFromFile(FILE* _pFile)
{
}