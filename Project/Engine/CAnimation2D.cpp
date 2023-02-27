#include "pch.h"
#include "CAnimation2D.h"

#include "CDevice.h"

#include "CTimeMgr.h"
#include "CConstBuffer.h"

CAnimation2D::CAnimation2D()
	: m_iCurIdx(-1)
	, m_pOwner(nullptr)
	, m_fAccTime(0.f)
	, m_fWidth{}
	, m_fHeight{}
	, m_eState{ ANIMATION_STATE::END }
{
	SetName(L"CAnimation2D");
}

CAnimation2D::CAnimation2D(const CAnimation2D& _rhs)
	:CEntity(_rhs)
	, m_iCurIdx(-1)
	, m_pOwner(nullptr)
	, m_fAccTime(_rhs.m_fAccTime)
	, m_fWidth{ _rhs.m_fWidth }
	, m_fHeight{ _rhs.m_fHeight }
	, m_eState{ _rhs.m_eState }
	, m_vecFrm{ _rhs.m_vecFrm }
{
	m_AtlasTex = _rhs.m_AtlasTex;
}

CAnimation2D::~CAnimation2D()
{
	m_AtlasTex = nullptr;
}

void CAnimation2D::finaltick()
{
	if (ANIMATION_STATE::PAUSE == m_eState ||
		ANIMATION_STATE::FINISH == m_eState)
		return;

	m_fAccTime += DT;

	if (m_vecFrm[m_iCurIdx].fDuration < m_fAccTime)
	{
		m_fAccTime = 0;
		++m_iCurIdx;

		if (m_iCurIdx >= m_vecFrm.size())
		{
			m_iCurIdx = (int)m_vecFrm.size() - 1;
			m_eState = ANIMATION_STATE::FINISH;
		}

	}
}

void CAnimation2D::Create(const wstring& _strKey, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS)
{
	SetName(_strKey);

	m_AtlasTex = _AtlasTex;

	m_fWidth = (float)_AtlasTex->GetWidth();
	m_fHeight = (float)_AtlasTex->GetHeight();

	for (UINT i{ 0 }; i < (UINT)_iMaxFrm; ++i)
	{
		tAnim2DFrm frm{};
		frm.vLeftTop = Vec2{ (_vLeftTop.x + _fStep * (float)i) / m_fWidth, _vLeftTop.y / m_fHeight };
		frm.vSlice = Vec2{ _vSlice.x / m_fWidth, _vSlice.y / m_fHeight };
		frm.fDuration = 1.f / _FPS;
		frm.vFullSize = Vec2{ 200.f / m_fWidth, 200.f / m_fHeight };
		m_vecFrm.push_back(frm);
	}
}

void CAnimation2D::Create(const wstring& _strKey, Ptr<CTexture> _AtlasTex, vector<tDxatlas> frames, float _FPS)
{
	SetName(_strKey);
	 
	m_AtlasTex = _AtlasTex;

	m_fWidth = (float)_AtlasTex->GetWidth();
	m_fHeight = (float)_AtlasTex->GetHeight();

	for (UINT i{ 0 }; i < (UINT)frames.size(); ++i)
	{
		tAnim2DFrm frm{};

		frm.vLeftTop = Vec2{ frames[i].x / m_fWidth, frames[i].y / m_fHeight };
		frm.vSlice = Vec2{ frames[i].w / m_fWidth, frames[i].h / m_fHeight };
		frm.fDuration = 1.f / _FPS;
		frm.vFullSize = Vec2{ frames[i].w / m_fWidth, frames[i].h / m_fHeight };
		m_vecFrm.push_back(frm);
	}
}

void CAnimation2D::Update()
{
	m_AtlasTex->UpdateData(59, PIPELINE_STAGE::PS);

	m_info.iAnim2DUse = 1;
	m_info.vLeftTop = m_vecFrm[m_iCurIdx].vLeftTop;
	m_info.vSlice = m_vecFrm[m_iCurIdx].vSlice;
	m_info.vOffset = m_vecFrm[m_iCurIdx].vOffset;
	m_info.vFullSize = m_vecFrm[m_iCurIdx].vFullSize;
}

void CAnimation2D::UpdateData()
{
	m_AtlasTex->UpdateData(59, PIPELINE_STAGE::PS);

	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::ANIMATION2D);

	m_info.iAnim2DUse = 1;
	m_info.vLeftTop = m_vecFrm[m_iCurIdx].vLeftTop;
	m_info.vSlice = m_vecFrm[m_iCurIdx].vSlice;
	m_info.vOffset = m_vecFrm[m_iCurIdx].vOffset;
	m_info.vFullSize = m_vecFrm[m_iCurIdx].vFullSize;

	pCB->SetData(&m_info);
	pCB->UpdateData(PIPELINE_STAGE::PS);
}

void CAnimation2D::Clear()
{
	CTexture::Clear(59);

	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::ANIMATION2D);

	memset(&m_info, 0, sizeof(tAnim2DInfo));

	pCB->SetData(&m_info);
	pCB->UpdateData(PIPELINE_STAGE::PS);
}

int  CAnimation2D::Add_Animation2D(Vec2 _vLeftTop, Vec2 _vSlice, float _fDuration, Vec2 _vFullSize)
{
	assert(m_AtlasTex.Get());

	float fWidth = (float)m_AtlasTex->GetWidth();
	float fHeight = (float)m_AtlasTex->GetHeight();

	tAnim2DFrm frm{};
	frm.vLeftTop = Vec2{ _vLeftTop.x / fWidth, _vLeftTop.y / fHeight };
	frm.vSlice = Vec2{ _vSlice.x / fWidth, _vSlice.y / fHeight };
	frm.fDuration = _fDuration;
	frm.vFullSize = Vec2{ _vFullSize.x / fWidth, _vFullSize.y / fHeight };
	m_vecFrm.push_back(frm);
	m_iCurIdx = (int)(m_vecFrm.size() - 1);
	return m_iCurIdx;
}

int  CAnimation2D::Delete_Animation2D()
{
	if (m_vecFrm.size() > 1)
	{
		m_vecFrm.pop_back();
		m_iCurIdx = m_vecFrm.size() - 1;
	}
	return m_iCurIdx;
}

int  CAnimation2D::Delete_Animation2D(int _iIndex)
{
	int iCur = 0;

	for (auto iter{ m_vecFrm.begin() }; iter != m_vecFrm.end(); ++iter)
	{
		if (iCur == _iIndex)
		{
			iter = m_vecFrm.erase(iter);
			m_iCurIdx = (int)(m_vecFrm.size() - 1);
			break;
		}
		++iCur;
	}
	return m_iCurIdx;
}

void CAnimation2D::SetLeftTopX(float _fx, int _index)
{
	m_vecFrm[_index].vLeftTop.x = _fx / m_fWidth;
}

void CAnimation2D::SetLeftTopY(float _fy, int _index)
{
	m_vecFrm[_index].vLeftTop.y = _fy / m_fWidth;
}

void CAnimation2D::SetLeftTop(Vec2 _vLeftTop, int _index)
{
	m_vecFrm[_index].vLeftTop = _vLeftTop / m_fWidth;
}

void CAnimation2D::SetSliceX(float _fx, int _index)
{
	m_vecFrm[_index].vSlice.x = _fx / m_fWidth;
}

void CAnimation2D::SetSliceY(float _fy, int _index)
{
	m_vecFrm[_index].vSlice.y = _fy / m_fWidth;
}

void CAnimation2D::SetSlice(Vec2 _vSlice, int _index)
{
	m_vecFrm[_index].vSlice = _vSlice / m_fWidth;
}

void CAnimation2D::SetOffsetX(float _fx, int _index)
{
	m_vecFrm[_index].vOffset.x = _fx / m_fWidth;
}

void CAnimation2D::SetOffsetY(float _fy, int _index)
{
	m_vecFrm[_index].vOffset.y = _fy / m_fWidth;
}

void CAnimation2D::SetOffset(Vec2 _vOffset, int _index)
{
	m_vecFrm[_index].vOffset = _vOffset / m_fWidth;
}

void CAnimation2D::SetFullSizeX(float _fx, int _index)
{
	m_vecFrm[_index].vFullSize.x = _fx / m_fWidth;
}

void CAnimation2D::SetFullSizeY(float _fy, int _index)
{
	m_vecFrm[_index].vFullSize.y = _fy / m_fWidth;
}

void CAnimation2D::SetFullSize(Vec2 _vFullSize, int _index)
{
	m_vecFrm[_index].vFullSize = _vFullSize / m_fWidth;
}

void CAnimation2D::SetDuration(float _fDuration, int _index)
{
	m_vecFrm[_index].fDuration = _fDuration;
}

float CAnimation2D::GetLeftTopX(int _index)
{
	return m_vecFrm[_index].vLeftTop.x * m_fWidth;
}
float CAnimation2D::GetLeftTopY(int _index)
{
	return m_vecFrm[_index].vLeftTop.y * m_fHeight;
}
Vec2 CAnimation2D::GetLeftTop(int _index)
{
	return m_vecFrm[_index].vLeftTop * Vec2(m_fWidth, m_fHeight);
}
float CAnimation2D::GetSliceX(int _index)
{
	return m_vecFrm[_index].vSlice.x * m_fWidth;
}
float CAnimation2D::GetSliceY(int _index)
{
	return m_vecFrm[_index].vSlice.y * m_fHeight;
}
Vec2 CAnimation2D::GetSlice(int _index)
{
	return m_vecFrm[_index].vSlice * Vec2(m_fWidth, m_fHeight);
}
float CAnimation2D::GetOffsetX(int _index)
{
	return m_vecFrm[_index].vOffset.x / m_fWidth;
}
float CAnimation2D::GetOffsetY(int _index)
{
	return m_vecFrm[_index].vOffset.y / m_fHeight;
}
Vec2 CAnimation2D::GetOffset(int _index)
{
	return m_vecFrm[_index].vOffset * Vec2(m_fWidth, m_fHeight);
}
float CAnimation2D::GetFullSizeX(int _index)
{
	return m_vecFrm[_index].vFullSize.x * m_fWidth;
}
float CAnimation2D::GetFullSizeY(int _index)
{
	return m_vecFrm[_index].vFullSize.y * m_fHeight;
}
Vec2 CAnimation2D::GetFullSize(int _index)
{
	return m_vecFrm[_index].vFullSize * Vec2(m_fWidth, m_fHeight);
}
float CAnimation2D::GetDuration(int _index)
{
	return m_vecFrm[_index].fDuration;
}

void CAnimation2D::SetTexture(Ptr<CTexture> _texture)
{
	m_AtlasTex = _texture;
	m_fWidth = (float)m_AtlasTex->GetWidth();
	m_fHeight = (float)m_AtlasTex->GetHeight();
}

void CAnimation2D::SaveToFile(FILE* _File)
{
	CEntity::SaveToFile(_File);

	// 프레임 개수, 데이터
	size_t iFrameCount = m_vecFrm.size();
	fwrite(&iFrameCount, sizeof(size_t), 1, _File);
	fwrite(m_vecFrm.data(), sizeof(tAnim2DFrm), iFrameCount, _File);

	fwrite(&m_fWidth, sizeof(float), 1, _File);
	fwrite(&m_fHeight, sizeof(float), 1, _File);
	// 참조 아틀라스 텍스쳐
	SaveResourceRef<CTexture>(m_AtlasTex, _File);
}

void CAnimation2D::LoadFromFile(FILE* _File)
{
	CEntity::LoadFromFile(_File);

	// 프레임 개수, 데이터
	size_t iFrameCount = 0;
	fread(&iFrameCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < iFrameCount; ++i)
	{
		tAnim2DFrm frm = {};
		fread(&frm, sizeof(tAnim2DFrm), 1, _File);
		m_vecFrm.push_back(frm);
	}

	fread(&m_fWidth, sizeof(float), 1, _File);
	fread(&m_fHeight, sizeof(float), 1, _File);

	// 참조 아틀라스 텍스쳐
	LoadResourceRef<CTexture>(m_AtlasTex, _File);
}

bool CAnimation2D::IsEnd()
{
	if (ANIMATION_STATE::FINISH == m_eState)
		return true;
	else
		return false;
}