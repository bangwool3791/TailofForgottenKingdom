#include "pch.h"
#include "CTimeMgr.h"

#include "CTrailComponent.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CStructuredBuffer.h"

CTrailComponent::CTrailComponent()
	:CComponent(COMPONENT_TYPE::TRAIL)
{
	SetName(L"CTrailComponent");
	m_pTrailTime = new CStructuredBuffer;
	m_pSwordTrailTime = new CStructuredBuffer;
	m_pSwordPos = new CStructuredBuffer;

	m_vecTrailTime.resize(10);
	m_vecSwordTrailTime.resize(10);
	m_vecSwordPos.resize(172);

	m_pTrailTime->Create(sizeof(float), m_vecTrailTime.size(), SB_TYPE::UAV_INC, m_vecTrailTime.data(), true);
	m_pSwordTrailTime->Create(sizeof(float), m_vecSwordTrailTime.size(), SB_TYPE::UAV_INC, m_vecSwordTrailTime.data(), true);
	m_pSwordPos->Create(sizeof(Vec3), m_vecSwordPos.size(), SB_TYPE::SRV_ONLY, m_vecSwordPos.data(), true);

	m_vTopOffset.z = 500.f;
}

CTrailComponent::CTrailComponent(const CTrailComponent& rhs)
	:CComponent(rhs)
	,m_vecTrailTime(rhs.m_vecTrailTime)
	,m_vecSwordTrailTime(rhs.m_vecSwordTrailTime)
	,m_vecSwordPos(rhs.m_vecSwordPos)
	, m_vTopOffset(rhs.m_vTopOffset)
	, m_iSwordTrailIdx(rhs.m_iSwordTrailIdx)
{
	m_pTrailTime = new CStructuredBuffer;
	m_pSwordTrailTime = new CStructuredBuffer;
	m_pSwordPos = new CStructuredBuffer;

	m_pTrailTime->Create(sizeof(float), m_vecTrailTime.size(), SB_TYPE::UAV_INC, m_vecTrailTime.data(), true);
	m_pSwordTrailTime->Create(sizeof(float), m_vecSwordTrailTime.size(), SB_TYPE::UAV_INC, m_vecSwordTrailTime.data(), true);
	m_pSwordPos->Create(sizeof(Vec3), m_vecSwordPos.size(), SB_TYPE::SRV_ONLY, m_vecSwordPos.data(), true);
}

CTrailComponent::~CTrailComponent()
{
	SAFE_DELETE(m_pTrailTime);
	SAFE_DELETE(m_pSwordTrailTime);
	SAFE_DELETE(m_pSwordPos);
}

void CTrailComponent::begin()
{

}

void CTrailComponent::tick()
{
}

void CTrailComponent::finaltick()
{
}

void CTrailComponent::UpdateData()
{

	m_pTrailTime->UpdateData(62, PIPELINE_STAGE::PS);

	m_pSwordTrailTime->UpdateData(63, PIPELINE_STAGE::PS);
	
	//GPU 전달 데이터 최적화 필요
	if (-1 != m_iSwordTrailIdx)
	{
		for (UINT i = 0; i < m_queSwordPos.size(); ++i)
		{
			m_vecSwordPos[i] = m_queSwordPos.front();
			Vec3 vPos = m_queSwordPos.front();
			m_queSwordPos.pop();
			m_queSwordPos.push(vPos);
		}

		float iSize = m_queSwordPos.size();
		MeshRender()->GetCurMaterial(0)->SetScalarParam(FLOAT_0, &iSize);

		m_pSwordPos->SetData(m_vecSwordPos.data(), m_vecSwordPos.size());
		m_pSwordPos->UpdateData(65, PIPELINE_STAGE::GS);
	}

	if (nullptr != m_pBoneSocketMat)
	{
		m_pBoneSocketMat->UpdateData(64, PIPELINE_STAGE::VS);
		if (m_vecBoneSocket.empty())
			m_vecBoneSocket.resize(m_pBoneSocketMat->GetElementCount());
		
		m_pBoneSocketMat->GetData(m_vecBoneSocket.data());

		//이전 프레임에서 읽어온 애니메이션 뼈 정보 데이터
		//Upper, Bottom 위치 회전 계산
		Vec3 vWorldPos{};

		Matrix matOffset{};
		Matrix matAnimation = m_vecBoneSocket[m_iBoneIdx];
		Matrix matWorld = Transform()->GetWorldMat();

		matOffset.m[3][0] = m_vTopOffset.x;
		matOffset.m[3][1] = m_vTopOffset.y;
		matOffset.m[3][2] = m_vTopOffset.z;

		vWorldPos.x = matWorld.m[3][0];
		vWorldPos.y = matWorld.m[3][1];
		vWorldPos.z = matWorld.m[3][2];

		matWorld.m[3][0] = 0;
		matWorld.m[3][1] = 0;
		matWorld.m[3][2] = 0;

		Matrix mat = matOffset * matAnimation * matWorld;
		XMVECTOR vScale;
		XMVECTOR vRot;
		XMVECTOR vTrans;

		XMMatrixDecompose(&vScale, &vRot, &vTrans, mat);
		m_vUpperSwordRot = vRot;
		m_vUpperSwordPos = vTrans;
		m_vUpperSwordPos += vWorldPos;

		matOffset.m[3][0] = m_vBottomOffset.x;
		matOffset.m[3][1] = m_vBottomOffset.y;
		matOffset.m[3][2] = m_vBottomOffset.z;

		mat = matOffset * matAnimation * matWorld;
		XMMatrixDecompose(&vScale, &vRot, &vTrans, mat);
		m_vBottomSwordRot = vRot;
		m_vBottomSwordPos = vTrans;
		m_vBottomSwordPos += vWorldPos;
	}
}

void CTrailComponent::Clear()
{
	m_pTrailTime->Clear();
	m_pSwordTrailTime->Clear();
	m_pSwordPos->Clear();

	if(nullptr != m_pBoneSocketMat)
		m_pBoneSocketMat->Clear();
}

void CTrailComponent::SetData(float fDelta)
{
	 m_vecTrailTime.push_back(fDelta); 

	 m_pTrailTime->Create(sizeof(float), m_vecTrailTime.size(), SB_TYPE::UAV_INC, m_vecTrailTime.data(), true);
}

void CTrailComponent::SetSwordTrailData(float fDelta)
{
	m_vecSwordTrailTime.push_back(fDelta);
	
	m_pSwordTrailTime->Create(sizeof(float), m_vecSwordTrailTime.size(), SB_TYPE::UAV_INC, m_vecSwordTrailTime.data(), true);
}

void CTrailComponent::SetBoneSocketBuffer(CStructuredBuffer *pBuffer)
{
	m_pBoneSocketMat = pBuffer;
	//m_pBoneSocketMat->GetData(m_vecBoneSocket.data());
	//m_pBoneSocketMat->Create(sizeof(float), m_vecBoneSocket.size(), SB_TYPE::UAV_INC, m_vecBoneSocket.data(), true);
}

void CTrailComponent::SetVectorTrailTime(const vector<float>& vec)
{
	m_vecTrailTime = vec;

	m_pTrailTime->Create(sizeof(float), m_vecTrailTime.size(), SB_TYPE::UAV_INC, m_vecTrailTime.data(), true);
}

void CTrailComponent::SetVectorSwordTrailTime(const vector<float>& vec)
{

}

void CTrailComponent::PushSwordPos(Vec3 vPos)
{
	if (m_iSwordTrailCount <= m_iSwordTrailIdx)
	{
		m_queSwordPos.pop();
		m_queSwordPos.push(vPos);
	}
	else
	{
		m_queSwordPos.push(vPos);
		++m_iSwordTrailIdx;
	}
	//cout << "큐 사이즈 " << m_queSwordPos.size() << endl;
	//m_pSwordPos->Create(sizeof(float) * 3, m_vecSwordPos.size(), SB_TYPE::UAV_INC, m_vecSwordPos.data(), true);
	//m_pSwordPos->SetData(m_vecSwordPos.data(), m_vecSwordPos.size());
}

void CTrailComponent::ClearSwordPos()
{
	m_iSwordTrailIdx = 0;

	m_bSwordTrail = false;

	if (m_queSwordPos.empty())
		return;

	std::queue<Vec3> garbage;
	m_queSwordPos.swap(garbage);
	m_vecSwordPos.clear();
	m_vecSwordPos.resize(172);
}

void CTrailComponent::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE type = __super::GetType();
	fwrite(&type, sizeof(UINT), 1, _File);

}

void CTrailComponent::LoadFromFile(FILE* _File)
{
}
