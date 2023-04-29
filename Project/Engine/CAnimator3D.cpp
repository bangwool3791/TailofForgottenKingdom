#include "pch.h"

#include "CAnimator3D.h"
#include "CTrailComponent.h"
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
	, m_bFinalMatUpdate{ false }
	, m_iFrameIdx{}
	, m_iNextFrameIdx{}
	, m_fRatio{ 0.f }
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	SetName(L"CAnimator3D");

	m_pBoneFinalMatBuffer = new CStructuredBuffer;
	m_pBoneSocketMat = new CStructuredBuffer;
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
	, m_tCurFrame(_origin.m_tCurFrame)
	, m_bPuase(_origin.m_bPuase)
	, m_fTimeScale(_origin.m_fTimeScale)
	, m_strCurKey(_origin.m_strCurKey)
	, m_mapAnimation(_origin.m_mapAnimation)
	, m_pMeshData(_origin.m_pMeshData)
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	SetName(L"CAnimator3D");

	m_pBoneFinalMatBuffer = new CStructuredBuffer;
	m_pBoneSocketMat = new CStructuredBuffer;
	SetBones(m_pVecBones);
	SetAnimClip(m_pVecClip);
}

CAnimator3D::~CAnimator3D()
{
	SAFE_DELETE(m_pBoneFinalMatBuffer);
	SAFE_DELETE(m_pBoneSocketMat);
}

void CAnimator3D::begin()
{
}

void CAnimator3D::finaltick()
{

	m_dCurTime = 0.f;

	if (!m_iEnd)
	{
		if (!m_bPuase)
			m_vecClipUpdateTime[m_iCurClip] += DT;
	}

	double dFrameIdx = 0.f;

	if (!m_bTrail && !m_bPuase)
	{
		m_dCurTime = m_pVecClip->at(m_iCurClip).dStartTime + m_vecClipUpdateTime[m_iCurClip];
		m_dCurTime *= m_fTimeScale;
		dFrameIdx = m_dCurTime * (double)m_iFrameCount + (double)m_tCurFrame.iStart;

		m_iFrameIdx = (int)(dFrameIdx);
	}

	//총 애니메이션 시간을 넘어가면 초기화

	if (m_tCurFrame.bRepeat)
	{
		if (m_iFrameIdx >= m_tCurFrame.iEnd)
		{
			m_vecClipUpdateTime[m_iCurClip] = 0.f;
		}
	}
	else
	{
		if (m_iFrameIdx >= m_tCurFrame.iEnd)
		{
			m_iEnd = true;
		}
	}

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
	if (!m_bPuase)
	{
		if (m_iFrameIdx >= m_tCurFrame.iEnd)
		{
			m_fRatio = 0.f;
		}
		else
		{
			m_fRatio = (float)(dFrameIdx - (double)m_iFrameIdx);
		}
	}

	// 컴퓨트 쉐이더 연산여부
	m_bFinalMatUpdate = false;
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip)
{
	m_pVecClip = _vecAnimClip;
	m_vecClipUpdateTime.resize(m_pVecClip->size());
	m_vecClipUpdateTime[0] = 0.f;
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
		const vector<tMTBone>* vecBones = pMesh->GetBones();
		pUpdateShader->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer());
		//뼈 Offset 행렬 Set
		pUpdateShader->SetOffsetMatBuffer(pMesh->GetBoneOffsetBuffer());
		//뼈 최종 행렬 Set
		pUpdateShader->SetOutputBuffer(m_pBoneFinalMatBuffer);
		pUpdateShader->SetBoneSocketBuffer(m_pBoneSocketMat);
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
	
		if (m_pBoneFinalMatBuffer->GetElementCount() != iBoneCount)
		{
			m_pBoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::UAV_INC, nullptr, false);
		}
		
		if (GetOwner()->IsAnimationMatrix())
		{
			Ptr<CMesh> pMesh = MeshRender()->GetMesh();
			check_mesh(pMesh);
			UINT iBoneCount = pMesh->GetBoneCount();
			vector<Matrix> vecBoneMat{};
			vector<Matrix> vecOffsetMat{};
			vecBoneMat.resize(iBoneCount);
			vecOffsetMat.resize(iBoneCount);

			m_pBoneSocketMat->GetData(vecBoneMat.data(), iBoneCount * sizeof(Matrix));
			pMesh->GetBoneOffsetBuffer()->GetData(vecOffsetMat.data(), iBoneCount * sizeof(Matrix));
			//vecBoneMat[0] = XMMatrixTranspose(vecBoneMat[0]);
			vecOffsetMat[0] = XMMatrixTranspose(vecOffsetMat[1]);

			Matrix InvOffMatrix = XMMatrixInverse(nullptr, vecOffsetMat[0]);
			//Vec3 vDir = XMVector3TransformNormal(Vec3{ 1.f, 1.f, 1.f }, vecBoneMat[0]);
			//vecOffsetMat[0].CreateRotationX(vDir.x);
			//vecOffsetMat[0].CreateRotationY(vDir.z);
			//vecOffsetMat[0].CreateRotation(vDir.y);
			//Vec3 vPos = vecBoneMat[0].Translation();
			//Vec3 vSacle = vecBoneMat[0].
			m_matAnimation = vecBoneMat[24];
		}
	}
	//61 레지스터에 최종행렬 데이터(구조버퍼) 바인딩		
	m_pBoneFinalMatBuffer->UpdateData(61, PIPELINE_STAGE::VS);
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
		pMtrl = MeshRender()->GetCurMaterial(i);
		if (nullptr == pMtrl)
			continue;

		//애니메이션 정보 false
		pMtrl->SetAnim3D(false); // Animation Mesh 알리기
		//뼈 개수 0
		pMtrl->SetBoneCount(0);
	}
}

void CAnimator3D::Init()
{
	m_iEnd = 0;
	m_bPuase = false;

	m_fTimeScale = 1.f;
	m_strCurKey.clear();
	m_mapAnimation.clear();
	std::unordered_map<wstring, tAnim3DFrm> map;
	m_mapAnimation.swap(map);
	memset(&m_tCurFrame, 0, sizeof(tAnim3DFrm));
	m_vecClipUpdateTime.clear();
	m_vecFinalBoneMat.clear();
}

/*
* 메시 뼈 카운트와, 뼈 구조화 버퍼 카운트가 불일치하면 갱신
*/
void CAnimator3D::SetMeshData(Ptr<CMeshData> _pMeshData)
{ 
	m_pMeshData = _pMeshData; 
}

void CAnimator3D::check_mesh(Ptr<CMesh> _pMesh)
{
	UINT iBoneCount = _pMesh->GetBoneCount();
	if (m_pBoneFinalMatBuffer->GetElementCount() != iBoneCount)
	{
		m_pBoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::UAV_INC, nullptr, true);
	}

	if (m_pBoneSocketMat->GetElementCount() != iBoneCount)
	{
		m_pBoneSocketMat->Create(sizeof(Matrix), iBoneCount, SB_TYPE::UAV_INC, nullptr, true);
	}
}

void CAnimator3D::SaveToFile(FILE* _pFile)
{
	COMPONENT_TYPE eType = GetType();

	fwrite(&eType, sizeof(COMPONENT_TYPE), 1, _pFile);

	SaveResourceRef(m_pMeshData, _pFile);
	
	fwrite(&m_fTimeScale, sizeof(float), 1, _pFile);

	SaveWStringToFile(m_strCurKey, _pFile);
	fwrite(&m_tCurFrame, sizeof(tAnim3DFrm), 1, _pFile);

	size_t iSize = m_mapAnimation.size();
	fwrite(&iSize, sizeof(size_t), 1, _pFile);

	for (auto iter{ m_mapAnimation.begin() }; iter != m_mapAnimation.end(); ++iter)
	{
		SaveWStringToFile(iter->first, _pFile);
		fwrite(&iter->second, sizeof(tAnim3DFrm), 1, _pFile);
	}
}

void CAnimator3D::LoadFromFile(FILE* _pFile)
{
	LoadResourceRef(m_pMeshData, _pFile);

	Ptr<CMesh> pMesh = m_pMeshData->GetMesh();
	
	MeshRender()->SetMesh(pMesh);
	
	const vector<Ptr<CMaterial>>& vec_Mtrls	= m_pMeshData->GetMaterials();

	for (UINT i = 0; i < vec_Mtrls.size(); ++i)
		MeshRender()->SetSharedMaterial(vec_Mtrls[i], i);

	SetBones(pMesh->GetBones());
	SetAnimClip(pMesh->GetAnimClip());

	fread(&m_fTimeScale, sizeof(float), 1, _pFile);

	LoadWStringFromFile(m_strCurKey, _pFile);
	fread(&m_tCurFrame, sizeof(tAnim3DFrm), 1, _pFile);

	size_t iSize = 0;
	fread(&iSize, sizeof(size_t), 1, _pFile);

	wstring strKey;
	tAnim3DFrm tFrame = {};
	for (size_t i = 0; i < iSize; ++i)
	{
		LoadWStringFromFile(strKey, _pFile);
		fread(&tFrame, sizeof(tAnim3DFrm), 1, _pFile);

		m_mapAnimation.insert(make_pair(strKey, tFrame));
	}
}

void CAnimator3D::SetCurFrame(const wstring& _Key, tAnim3DFrm _tData)
{
	auto iter = m_mapAnimation.find(_Key);

	if (iter != m_mapAnimation.end())
	{
		m_strCurKey = _Key;

		m_tCurFrame = iter->second = _tData;
		m_iEnd = 0;

		if (m_tCurFrame.iStart > m_iFrameIdx || m_tCurFrame.iEnd <= m_iFrameIdx)
			m_vecClipUpdateTime[m_iCurClip] = 0.f;

		m_iFrameIdx = m_tCurFrame.iStart;
	}
}

void CAnimator3D::SetCurFrameKey(const wstring& _Key)
{
	auto iter = m_mapAnimation.find(_Key);

	if (iter != m_mapAnimation.end())
	{
		m_strCurKey = _Key;
		m_tCurFrame = iter->second;

		if (m_tCurFrame.iStart > m_iFrameIdx || m_tCurFrame.iEnd <= m_iFrameIdx)
			m_vecClipUpdateTime[m_iCurClip] = 0.f;

		m_iEnd = 0;
	}
}

void CAnimator3D::Add_Frame(const wstring& _key, tAnim3DFrm _tData)
{
	wstring str = wstring(_key.begin(), _key.end());

	auto iter = m_mapAnimation.find(str);

	if (iter == m_mapAnimation.end())
		m_mapAnimation.emplace(make_vlaue(str.c_str(), { _tData.iStart, _tData.iEnd, _tData.iFrameCount, _tData.bRepeat }));
	else
		m_mapAnimation[str] = _tData;
}

const string& CAnimator3D::Delete(const wstring& _key)
{
	static string str;
	m_mapAnimation.erase(_key);

	auto iter = m_mapAnimation.begin();
	
	for(; iter != m_mapAnimation.end(); ++iter)
		str = string(iter->first.begin(), iter->first.end());
	
	return str;
}

void CAnimator3D::DeleteEnd()
{
	auto iter = m_mapAnimation.end();
	--iter;

	m_mapAnimation.erase(iter);
}

void CAnimator3D::SetTimeScale(float& _dScale)
{
	if (0 < _dScale)
	{
		m_fTimeScale = (double)_dScale;
	}
	else
	{
		_dScale = 1.f;
	}
}

void CAnimator3D::SetUseAnimationMatrix(bool _bUse)
{
	GetOwner()->SetAnimationMatrix(_bUse);
}

bool CAnimator3D::GetUseAnimationMatrix()
{
	return GetOwner()->IsAnimationMatrix();
}