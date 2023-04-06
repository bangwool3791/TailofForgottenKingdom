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

void CAnimator3D::begin() 
{
	/*
	* 	double dStart;
	double dEnd;
	UINT   iFrameCount;
	bool   bRepeat;
	*/
	m_mapAnimation.emplace(std::make_pair<wstring, tAnim3DFrm>(L"1", { 0, 10, 30, true }));
	m_mapAnimation.emplace(std::make_pair<wstring, tAnim3DFrm>(L"2", { 11, 20, 30, true }));

	m_tCurFrame = { 0, 5, 30, true };
}

void CAnimator3D::finaltick()
{
	if (KEY_PRESSED(KEY::NUM_1))
	{
		/*
		* 1440 ������ �� 0~200 ������ ���
		*/
		m_tCurFrame = { 0, 5, 30, true };
		m_vecClipUpdateTime[m_iCurClip] = 0.f;
		m_bEnd = false;
	}
	else if (KEY_PRESSED(KEY::NUM_2))
	{
		m_tCurFrame = { 6, 10, 30, false };
		m_vecClipUpdateTime[m_iCurClip] = 0.f;
		m_bEnd = false;
	}

	m_dCurTime = 0.f;

	if (!m_bEnd)
	{
		if(!m_bPuase)
			m_vecClipUpdateTime[m_iCurClip] += (double)(DT * m_fTimeScale);
	}

	double dFrameIdx = 0.f;

	if (!m_bPuase)
	{
		m_dCurTime = m_pVecClip->at(m_iCurClip).dStartTime + m_vecClipUpdateTime[m_iCurClip];

		dFrameIdx = m_dCurTime * (double)m_iFrameCount + (double)m_tCurFrame.iStart;

		m_iFrameIdx = (int)(dFrameIdx);
	}

	//�� �ִϸ��̼� �ð��� �Ѿ�� �ʱ�ȭ
	
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
			m_bEnd = true;
		}
	}

	//�ε����� �Ѿ�� �ʱ�ȭ
	if (m_iFrameIdx >= m_pVecClip->at(0).iFrameLength - 1)
	{
		m_iNextFrameIdx = m_iFrameIdx;
	}
	else
	{
		m_iNextFrameIdx = m_iFrameIdx + 1;
	}

	// �����Ӱ��� �ð��� ���� ������ �����ش�.
	// double �� frameidx - int�� framidx
	// �Ҽ����� ����
	if (!m_bPuase)
	{
		if (m_iFrameIdx >= m_tCurFrame.iEnd)
		{
			m_fRatio = 1.f;
		}
		else
		{
			m_fRatio = (float)(dFrameIdx - (double)m_iFrameIdx);
		}
	}

	// ��ǻƮ ���̴� ���꿩��
	m_bFinalMatUpdate = false;
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip)
{
	m_pVecClip = _vecAnimClip;
	m_vecClipUpdateTime.resize(m_pVecClip->size());

	//���� �ð����� �ʱ�ȭ �ʿ� ?
	static float fTime = 0.f;
	fTime += 1.f;
	//������Ʈ �ð� �ʱ�ȭ
	m_vecClipUpdateTime[0] = fTime;
}

void CAnimator3D::UpdateData()
{
	//�� ��� ������Ʈ �Ϸᰡ �ȉ�����
	if (!m_bFinalMatUpdate)
	{
		// Animation3D Update Compute Shader
		CAnimation3DShader* pUpdateShader = (CAnimation3DShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"Animation3DUpdateShader").Get();

		// Bone Data
		Ptr<CMesh> pMesh = MeshRender()->GetMesh();
		check_mesh(pMesh);

		//CS Set
		//�� ������ Set
		pUpdateShader->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer());
		//�� Offset ��� Set
		pUpdateShader->SetOffsetMatBuffer(pMesh->GetBoneOffsetBuffer());
		//�� ���� ��� Set
		pUpdateShader->SetOutputBuffer(m_pBoneFinalMatBuffer);
		//�� ���� Set
		UINT iBoneCount = (UINT)m_pVecBones->size();
		pUpdateShader->SetBoneCount(iBoneCount);
		//���� ������ �ε��� 
		pUpdateShader->SetFrameIndex(m_iFrameIdx);
		//���� ������ �ε���
		pUpdateShader->SetNextFrameIdx(m_iNextFrameIdx);
		//���� ������ ���� ���� Set
		pUpdateShader->SetFrameRatio(m_fRatio);

		// ������Ʈ ���̴� ����
		pUpdateShader->Execute();

		m_bFinalMatUpdate = true;
	}

	// t30 �������Ϳ� ������� ������(��������) ���ε�		
	m_pBoneFinalMatBuffer->UpdateData(58, PIPELINE_STAGE::VS);
}

void CAnimator3D::ClearData()
{
	//���� �� ��� Ŭ����
	m_pBoneFinalMatBuffer->Clear();
	//����
	UINT iMtrlCount = MeshRender()->GetMtrlCount();
	Ptr<CMaterial> pMtrl = nullptr;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		pMtrl = MeshRender()->GetSharedMaterial(i);
		if (nullptr == pMtrl)
			continue;

		//�ִϸ��̼� ���� false
		pMtrl->SetAnim3D(false); // Animation Mesh �˸���
		//�� ���� 0
		pMtrl->SetBoneCount(0);
	}
}

/*
* �޽� �� ī��Ʈ��, �� ����ȭ ���� ī��Ʈ�� ����ġ�ϸ� ����
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

void CAnimator3D::SetCurFrame(const wstring& _Key, tAnim3DFrm _tData)
{
	auto iter = m_mapAnimation.find(_Key);

	if (iter != m_mapAnimation.end())
	{
		m_tCurFrame = iter->second = _tData;
		m_bEnd = false;
	}
}

void CAnimator3D::SetCurFrameKey(const wstring& _Key)
{
	auto iter = m_mapAnimation.find(_Key);

	if (iter != m_mapAnimation.end())
	{
		m_tCurFrame = iter->second;
		m_bEnd = false;
	}
}

void CAnimator3D::Add_Frame(const wstring& _key, tAnim3DFrm _tData)
{
	wstring str = wstring(_key.begin(), _key.end());
	m_mapAnimation.emplace(make_vlaue(str.c_str(), { _tData.iStart, _tData.iEnd, _tData.iFrameCount, _tData.bRepeat }));
}

const string& CAnimator3D::Delete(const wstring& _key)
{
	static string str;
	m_mapAnimation.erase(_key);
	
	auto iter = m_mapAnimation.end();
	--iter;
	str = string(iter->first.begin(), iter->first.end());
	return str;
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
