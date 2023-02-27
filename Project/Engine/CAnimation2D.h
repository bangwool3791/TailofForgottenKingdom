#pragma once

#include "CEntity.h"
#include "CTexture.h"

class CAnimator2D;

enum class ANIMATION_STATE
{
	PLAY,
	PAUSE,
	FINISH,
	END,
};
class CAnimation2D :
	public CEntity
{
private:
	vector<tAnim2DFrm>	m_vecFrm;
	int					m_iCurIdx;

	CAnimator2D* m_pOwner;
	Ptr<CTexture>		m_AtlasTex;

	float				m_fAccTime;
	ANIMATION_STATE		m_eState;

	float				m_fWidth;
	float				m_fHeight;
	tAnim2DInfo			m_info;
public:
	void finaltick();
public:
	vector<tAnim2DFrm>& GetFamesRef() { return m_vecFrm; }
	const vector<tAnim2DFrm>& GetFames() { return m_vecFrm; }
	const tAnim2DInfo& GetAniInfo() { return m_info; }
	const tAnim2DFrm& GetAniFrame() { return m_vecFrm[m_iCurIdx]; }
	Ptr<CTexture> GetTexture() { return m_AtlasTex; }
	void SetTexture(Ptr<CTexture> _texture);
	void SetState(ANIMATION_STATE _eState) { m_eState = _eState; }
	bool IsEnd();
	//Set Get
public:

	void SetLeftTopX(float _fx, int _index);
	void SetLeftTopY(float _fy, int _index);
	void SetLeftTop(Vec2 _vLeftTop, int _index);
	void SetSliceX(float, int _index);
	void SetSliceY(float _fy, int _index);
	void SetSlice(Vec2 _vSlice, int _index);
	void SetOffsetX(float, int _index);
	void SetOffsetY(float _fy, int _index);
	void SetOffset(Vec2 _vOffset, int _index);
	void SetFullSizeX(float _vFullSize, int _index);
	void SetFullSizeY(float _vFullSize, int _index);
	void SetFullSize(Vec2 _vFullSize, int _index);
	void SetDuration(float _fDuration, int _index);

	float GetLeftTopX(int _index);
	float GetLeftTopY(int _index);
	Vec2 GetLeftTop(int _index);
	float GetSliceX(int _index);
	float GetSliceY(int _index);
	Vec2 GetSlice(int _index);
	float GetOffsetX(int _index);
	float GetOffsetY(int _index);
	Vec2 GetOffset(int _index);
	float GetFullSizeX(int _index);
	float GetFullSizeY(int _index);
	Vec2 GetFullSize(int _index);
	float GetDuration(int _index);
public:
	void Create(const wstring& _strKey, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS);
	void Create(const wstring& _strKey, Ptr<CTexture> _AtlasTex, vector<tDxatlas> frames, float _FPS);
	int  Add_Animation2D(Vec2 _vLeftTop, Vec2 _vSlice, float _fDuration, Vec2 _vFullSize);
	int  Delete_Animation2D();
	int  Delete_Animation2D(int _iIndex);
	void Reset()
	{
		m_fAccTime = 0.f;
		m_iCurIdx = 0;
		m_eState = ANIMATION_STATE::PLAY;
	}

	void Update();
	void UpdateData();
	void Clear();
	void SetOwner(CAnimator2D* _pOwner) {
		m_pOwner = _pOwner;
	}

	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;
	CLONE(CAnimation2D);

public:
	CAnimation2D();
	CAnimation2D(const CAnimation2D& _rhs);
	virtual ~CAnimation2D();

	friend class CAnimator2D;
};