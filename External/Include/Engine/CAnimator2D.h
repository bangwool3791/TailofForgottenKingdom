#pragma once
#include "CComponent.h"
#include "CAnimation2D.h"
#include "CTexture.h"

class CAnimator2D :
    public CComponent

{
private:
    std::map<wstring, CAnimation2D*>        m_mapAnim;
    CAnimation2D* m_pCurAnim;

    bool                                    m_bRepeat;
    wstring                                 m_strKey;
public:
    virtual void begin() override;
    virtual void finaltick() override;
public:
    const wstring& GetKey();
    const vector<wstring>& GetKeys();
    const vector<tAnim2DFrm>& GetFames() { return m_pCurAnim->GetFames(); }
    tAnim2DInfo GetAniInfo();
    const tAnim2DFrm& GetAniFrame();
    void ActiveRepeat() { m_bRepeat = true; }
    const bool GetRepeat() { return m_bRepeat; }
    bool IsEnd();
    void Add_Animation(CAnimation2D* _pAnimatnion);
    CAnimation2D* Add_Animation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS);
    CAnimation2D* Delete_Animation(const wstring& _strName);
    /*
    Vec2 vLeftTop;
    Vec2 vSlice;
    Vec2 vOffset;
    Vec2 vFullSize;
    float fDuration;
    */
    void SetLeftTopX(float _fx, int _index);
    void SetLeftTopY(float _fx, int _index);
    void SetLeftTop(Vec2 _vLeftTop, int _index);
    void SetSliceX(float _fx, int _index);
    void SetSliceY(float _fx, int _index);
    void SetSlice(Vec2 _vSlice, int _index);
    void SetOffsetX(float _fx, int _index);
    void SetOffsetY(float _fx, int _index);
    void SetOffset(Vec2 _vOffset, int _index);
    void SetFullSizeX(float _fx, int _index);
    void SetFullSizeY(float _fx, int _index);
    void SetFullSize(Vec2 _vFullSize, int _index);
    void SetDuration(float _fDuration, int _index);
    void SetState(ANIMATION_STATE _eState) { m_pCurAnim->SetState(_eState); }

    float GetLeftTopX(int _index) { return m_pCurAnim->GetLeftTopX(_index); }
    float GetLeftTopY(int _index) { return m_pCurAnim->GetLeftTopY(_index); }
    Vec2 GetLeftTop(int _index) { return m_pCurAnim->GetLeftTop(_index); }
    float GetSliceX(int _index) { return m_pCurAnim->GetSliceX(_index); }
    float GetSliceY(int _index) { return m_pCurAnim->GetSliceY(_index); }
    Vec2 GetSlice(int _index) { return m_pCurAnim->GetSlice(_index); }
    float GetOffsetX(int _index) { return m_pCurAnim->GetOffsetX(_index); }
    float GetOffsetY(int _index) { return m_pCurAnim->GetOffsetY(_index); }
    Vec2 GetOffset(int _index) { return m_pCurAnim->GetOffset(_index); }
    float GetFullSizeX(int _index) { return m_pCurAnim->GetFullSizeX(_index); }
    float GetFullSizeY(int _index) { return m_pCurAnim->GetFullSizeY(_index); }
    Vec2 GetFullSize(int _index) { return m_pCurAnim->GetFullSize(_index); }
    float GetDuration(int _index) { return m_pCurAnim->GetLeftTopX(_index); }
public:
    void CreateAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS);
    CAnimation2D* FindAnimation(const wstring& _strKey);
    CAnimation2D* EditAnimation(const wstring& _origin, const wstring& _dest);
    int  Add_Animation2D(Vec2 _vLeftTop, Vec2 _vSlice, float _fDuration, Vec2 _vFullSize);
    int  Delete_Animation2D();
    int  Delete_Animation2D(int _iIndex);
    void Play();
    void Play(const wstring& _strKey);
    void Play(const wstring& _strKey, bool _bRepeat);

    void Update();
    void UpdateData();
    void Clear();

    void SetTexture(Ptr<CTexture> _AtlasTex);
    Ptr<CTexture> GetTexture() { return m_pCurAnim->GetTexture(); }
public:
    virtual void SaveToFile(FILE* _File);
    virtual void LoadFromFile(FILE* _File);
    CLONE(CAnimator2D);

public:
    CAnimator2D();
    CAnimator2D(const CAnimator2D& _rhs);
    virtual ~CAnimator2D();
};

