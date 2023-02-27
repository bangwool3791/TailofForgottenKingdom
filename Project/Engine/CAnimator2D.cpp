#include "pch.h"

#include "CTexture.h"
#include "CDevice.h"
#include "CConstBuffer.h"
#include "CAnimator2D.h"
#include "CAnimation2D.h"

CAnimator2D::CAnimator2D()
    :CComponent(COMPONENT_TYPE::ANIMATOR2D)
    , m_bRepeat(true)
    , m_mapAnim{}
    , m_strKey{}
{
    SetName(L"CAnimator2D");
}

CAnimator2D::CAnimator2D(const CAnimator2D& _rhs)
    :CComponent(COMPONENT_TYPE::ANIMATOR2D)
    , m_bRepeat(_rhs.m_bRepeat)
    , m_mapAnim{}
    , m_strKey{ _rhs.m_strKey }
{
    SetName(L"CAnimator2D");

    CAnimation2D* pAnimation2D{};
    auto iter = _rhs.m_mapAnim.begin();
    for (; iter != _rhs.m_mapAnim.end(); ++iter)
    {
        pAnimation2D = nullptr;
        m_mapAnim.insert(make_pair(iter->first, pAnimation2D = iter->second->Clone()));
        pAnimation2D->SetOwner(this);
    }
    assert(m_mapAnim.size());
    --iter;
    Play(iter->first, true);
}

CAnimator2D::~CAnimator2D()
{
    Safe_Del_Map(m_mapAnim);
}

void CAnimator2D::CreateAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS)
{
    assert(nullptr != _AtlasTex);

    CAnimation2D* pAnimation2D;

    pAnimation2D = FindAnimation(_strKey);

    assert(nullptr == pAnimation2D);

    pAnimation2D = new CAnimation2D;
    pAnimation2D->Create(_strKey, _AtlasTex, _vLeftTop, _vSlice, _fStep, _iMaxFrm, _FPS);
    pAnimation2D->SetName(_strKey);
    pAnimation2D->m_pOwner = this;
    m_mapAnim[_strKey] = pAnimation2D;
}


bool SortAtlas(const tDxatlas& lhs, const tDxatlas& rhs)
{
    if (lhs.iFrame < rhs.iFrame)
        return true;
    else
        return false;
}

void CAnimator2D::begin()
{
}

void CAnimator2D::finaltick()
{
    if (!IsValid(m_pCurAnim))
        return;

    if (ANIMATION_STATE::FINISH == m_pCurAnim->m_eState && m_bRepeat)
    {
        m_pCurAnim->Reset();
    }

    m_pCurAnim->finaltick();
}


CAnimation2D* CAnimator2D::FindAnimation(const wstring& _strKey)
{
    CAnimation2D* iter = m_mapAnim[_strKey];

    if (iter == nullptr)
        return nullptr;

    return iter;
}

void CAnimator2D::Play()
{
    auto iter = m_mapAnim.begin();

    m_strKey = iter->first;
    CAnimation2D* pAnimation = iter->second;

    if (nullptr == pAnimation)
    {
        assert(nullptr != pAnimation);
    }

    if (IsValid(pAnimation))
    {
        m_pCurAnim = pAnimation;
    }
    m_bRepeat = true;
    pAnimation->Reset();
}

void CAnimator2D::Play(const wstring& _strKey)
{
    m_strKey = _strKey;
    CAnimation2D* pAnimation = FindAnimation(_strKey);

    if (nullptr == pAnimation)
    {
        assert(nullptr != pAnimation);
    }

    if (IsValid(pAnimation))
    {
        m_pCurAnim = pAnimation;
    }
    pAnimation->Reset();
}

void CAnimator2D::Play(const wstring& _strKey, bool _bRepeat)
{
    m_strKey = _strKey;
    CAnimation2D* pAnimation = FindAnimation(_strKey);

    assert(nullptr != pAnimation);

    if (IsValid(pAnimation))
    {
        m_pCurAnim = pAnimation;
    }
    m_bRepeat = _bRepeat;
    pAnimation->Reset();
}

void CAnimator2D::Update()
{
    if (!IsValid(m_pCurAnim))
        return;

    if (m_bActive)
        m_pCurAnim->Update();
}

void CAnimator2D::UpdateData()
{
    if (!IsValid(m_pCurAnim))
        return;

    if (m_bActive)
        m_pCurAnim->UpdateData();
}

void CAnimator2D::Clear()
{
    if (!IsValid(m_pCurAnim))
        return;

    m_pCurAnim->Clear();
}


int  CAnimator2D::Add_Animation2D(Vec2 _vLeftTop, Vec2 _vSlice, float _fDuration, Vec2 _vFullSize)
{
    return m_pCurAnim->Add_Animation2D(_vLeftTop, _vSlice, _fDuration, _vFullSize);
}

int CAnimator2D::Delete_Animation2D()
{
    return m_pCurAnim->Delete_Animation2D();
}

int CAnimator2D::Delete_Animation2D(int _iIndex)
{
    return m_pCurAnim->Delete_Animation2D(_iIndex);
}

CAnimation2D* CAnimator2D::EditAnimation(const wstring& _origin, const wstring& _dest)
{
    auto iter{ m_mapAnim.begin() };
    CAnimation2D* pAnimation2D = iter->second;

    for (; iter != m_mapAnim.end(); ++iter)
    {
        if (!lstrcmp(iter->first.c_str(), _origin.c_str()))
        {
            m_mapAnim.erase(iter);
            break;
        }
    }
    pAnimation2D->SetName(_dest);
    m_mapAnim.insert(make_pair(_dest, pAnimation2D));
    return pAnimation2D;
}

void CAnimator2D::SaveToFile(FILE* _File)
{
    COMPONENT_TYPE type = GetType();
    fwrite(&type, sizeof(UINT), 1, _File);
    // Animation ÀúÀå
    size_t iAnimCount = m_mapAnim.size();
    fwrite(&iAnimCount, sizeof(size_t), 1, _File);

    for (const auto& pair : m_mapAnim)
    {
        pair.second->SaveToFile(_File);
    }

}

void CAnimator2D::LoadFromFile(FILE* _File)
{
    // Animation
    size_t iAnimCount = 0;
    fread(&iAnimCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < iAnimCount; ++i)
    {
        CAnimation2D* pAnim = new CAnimation2D;
        pAnim->LoadFromFile(_File);

        pAnim->m_pOwner = this;
        m_mapAnim.insert(make_pair(pAnim->GetName(), pAnim));
    }
}

void CAnimator2D::SetTexture(Ptr<CTexture> _AtlasTex)
{

    for (auto iter{ m_mapAnim.begin() }; iter != m_mapAnim.end(); ++iter)
    {
        iter->second->SetTexture(_AtlasTex);
    }
}

tAnim2DInfo CAnimator2D::GetAniInfo()
{
    if (m_pCurAnim)
        return m_pCurAnim->GetAniInfo();
    else
    {
        return tAnim2DInfo();
    }
}

const tAnim2DFrm& CAnimator2D::GetAniFrame()
{
    return m_pCurAnim->GetAniFrame();
}

void CAnimator2D::SetLeftTopX(float _fValue, int _index)
{
    m_pCurAnim->SetLeftTopX(_fValue, _index);
}

void CAnimator2D::SetLeftTopY(float _fValue, int _index)
{
    m_pCurAnim->SetLeftTopY(_fValue, _index);
}

void CAnimator2D::SetLeftTop(Vec2 _vLeftTop, int _index)
{
    m_pCurAnim->SetLeftTop(_vLeftTop, _index);
}

void CAnimator2D::SetSliceX(float _fValue, int _index)
{
    m_pCurAnim->SetSliceX(_fValue, _index);
}

void CAnimator2D::SetSliceY(float _fValue, int _index)
{
    m_pCurAnim->SetSliceY(_fValue, _index);
}

void CAnimator2D::SetSlice(Vec2 _vSlice, int _index)
{
    m_pCurAnim->SetSlice(_vSlice, _index);
}

void CAnimator2D::SetOffsetX(float _fValue, int _index)
{
    m_pCurAnim->SetOffsetX(_fValue, _index);
}

void CAnimator2D::SetOffsetY(float _fValue, int _index)
{
    m_pCurAnim->SetOffsetY(_fValue, _index);
}

void CAnimator2D::SetOffset(Vec2 _vOffset, int _index)
{
    m_pCurAnim->SetOffset(_vOffset, _index);
}

void CAnimator2D::SetFullSizeX(float _fValue, int _index)
{
    m_pCurAnim->SetFullSizeX(_fValue, _index);
}

void CAnimator2D::SetFullSizeY(float _fValue, int _index)
{
    m_pCurAnim->SetFullSizeY(_fValue, _index);
}

void CAnimator2D::SetFullSize(Vec2 _vFullSize, int _index)
{
    m_pCurAnim->SetFullSize(_vFullSize, _index);
}

void CAnimator2D::SetDuration(float _fDuration, int _index)
{
    m_pCurAnim->SetDuration(_fDuration, _index);
}

const wstring& CAnimator2D::GetKey()
{
    return m_pCurAnim->GetName();
}

const vector<wstring>& CAnimator2D::GetKeys()
{
    static vector<wstring> vec{};
    vec.clear();
    for (auto iter{ m_mapAnim.begin() }; iter != m_mapAnim.end(); ++iter)
    {
        vec.push_back(iter->first);
    }

    return vec;
}

void CAnimator2D::Add_Animation(CAnimation2D* _pAnimatnion)
{
    map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.find(_pAnimatnion->GetName());

    if (iter != m_mapAnim.end())
        return;

    CAnimation2D* pAnimation2D = _pAnimatnion->Clone();
    pAnimation2D->SetOwner(this);
    m_mapAnim.insert(make_pair(_pAnimatnion->GetName(), pAnimation2D));
}

CAnimation2D* CAnimator2D::Add_Animation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS)
{
    map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.find(_strKey);

    if (iter != m_mapAnim.end())
        return nullptr;

    CAnimation2D* pAnimation2D = new CAnimation2D;
    pAnimation2D->SetName(_strKey);
    pAnimation2D->Create(_strKey, _AtlasTex, _vLeftTop, _vSlice, _fStep, _iMaxFrm, _FPS);
    m_mapAnim.insert(make_pair(_strKey, pAnimation2D));
    return pAnimation2D;
}

CAnimation2D* CAnimator2D::Delete_Animation(const wstring& _strName)
{
    auto iter = m_mapAnim.find(_strName);

    if (iter == m_mapAnim.end())
        return nullptr;

    delete iter->second;

    iter = m_mapAnim.erase(iter);

    if (iter == m_mapAnim.end() && m_mapAnim.size() > 0)
    {
        --iter;
        return iter->second;
    }
    else if(m_mapAnim.size() > 0)
    {
        return iter->second;
    }
    else
        return nullptr;
}

bool CAnimator2D::IsEnd()
{
    if (!IsValid(m_pCurAnim))
        return false;

    return m_pCurAnim->IsEnd();  
}
