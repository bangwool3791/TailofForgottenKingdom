#pragma once
#include "CComponent.h"

#include "Ptr.h"
#include "CTexture.h"
#include "CMaterial.h"
#include "CMesh.h"

class CStructuredBuffer;

class CAnimation;

class CAnimator3D :
	public CComponent
{
private:
	bool	m_bEnd = false;
	wstring												m_strCurKey;
	std::unordered_map<wstring, CAnimation*>			m_mapAnimation;
	//뼈 데이터 vector 포인터
	const vector<tMTBone>*				m_pVecBones;
	//애니메이션 정보 vector 포인터
	const vector<tMTAnimClip>*			m_pVecClip;

	vector<float>						m_vecClipUpdateTime;
	//텍스쳐 전달 최종 행렬 정보
	vector<Matrix>						m_vecFinalBoneMat;
	int									m_iFrameCount; // 30
	double								m_dCurTime; // 현재 시간 누적 값
	int									m_iCurClip; // 클립 인덱스

	int									m_iFrameIdx; //클립의 현재 프레임
	int									m_iNextFrameIdx;//클립의 다음 프레임
	float								m_fRatio;//프레임 사이 비율

	CStructuredBuffer*					m_pBoneFinalMatBuffer;//특정 프레임의 최종 행렬
	bool								m_bFinalMatUpdate; //최종 행렬 연산 수행 여부

public:
	virtual void finaltick() override;
	void UpdateData();
public:
	void SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones = _vecBones; m_vecFinalBoneMat.resize(m_pVecBones->size()); }
	void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
	void SetClipTime(int _iCLipIdx, float _fTime) { m_vecClipUpdateTime[_iCLipIdx] = _fTime; }
	
	CStructuredBuffer* GetFinalBoneMat() { return m_pBoneFinalMatBuffer; }
	UINT GetBoneCount() {return (UINT)m_pVecBones->size();}
	void ClearData();

private:
	void check_mesh(Ptr<CMesh> _pMesh);

public :
	virtual void SaveToFile(FILE* _pFile) override;
	virtual void LoadFromFile(FILE* _pFile) override;
	CLONE(CAnimator3D);
public:
	CAnimator3D();
	CAnimator3D(const CAnimator3D& _origin);
	~CAnimator3D();
};