#pragma once
#include "CComponent.h"

#include "Ptr.h"
#include "CTexture.h"
#include "CMaterial.h"
#include "CMesh.h"

#define make_vlaue std::make_pair<wstring, tAnim3DFrm>
class CStructuredBuffer;

class CAnimation;

class CAnimator3D :
	public CComponent
{
private:
	bool											m_bTrail = false;
	bool											m_bPuase = false;
	UINT											m_iEnd = 0;
	float											m_fTimeScale = 1.f;
	Ptr<CMeshData>									m_pMeshData;
	Matrix											m_matAnimation;
	//UI
	wstring											m_strCurKey;
	std::unordered_map<wstring, tAnim3DFrm>			m_mapAnimation;
	tAnim3DFrm										m_tCurFrame;
	//�� ������ vector ������
	const vector<tMTBone>* m_pVecBones;
	//�ִϸ��̼� ���� vector ������
	const vector<tMTAnimClip>* m_pVecClip;

	//�ʱ�ȭ �ʿ�
	vector<float>						m_vecClipUpdateTime;
	//�ؽ��� ���� ���� ��� ����
	//�ʱ�ȭ �ʿ�
	vector<Matrix>						m_vecFinalBoneMat;
	//UI
	int									m_iFrameCount; // 30
	int									m_iCurClip; // Ŭ�� �ε���
	int									m_iFrameIdx; //Ŭ���� ���� ������
	int									m_iNextFrameIdx;//Ŭ���� ���� ������
	double								m_dCurTime; // ���� �ð� ���� ��

	double								m_dInterval;
	float								m_fRatio;//������ ���� ����

	CStructuredBuffer* m_pBoneFinalMatBuffer;//Ư�� �������� ���� ���
	CStructuredBuffer* m_pBoneSocketMat;
	bool								m_bFinalMatUpdate; //���� ��� ���� ���� ����

public:
	virtual void begin() override;
	virtual void finaltick() override;
	void UpdateData();
public:
	void SetTrail(bool _bTrail) { m_bTrail = _bTrail; }
	void SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones = _vecBones; m_vecFinalBoneMat.resize(m_pVecBones->size()); }
	void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
	void SetClipTime(int _iCLipIdx, float _fTime) { m_vecClipUpdateTime[_iCLipIdx] = _fTime; }
	void SetCurFrame(const wstring& _Key, tAnim3DFrm _tData);
	void SetCurFrameKey(const wstring& _Key);
	void SetCurFrameIdx(int _iFrame) { m_iFrameIdx = _iFrame; }
	void SetCurEndIdx(UINT idx) { m_tCurFrame.iEnd = idx; }
	void SetMeshData(Ptr<CMeshData> _pMeshData);
	void SetPuase(bool _bPause) { m_bPuase = _bPause; }
	void SetTimeScale(float& _dScale);

	CStructuredBuffer* GetFinalBoneMat() { return m_pBoneFinalMatBuffer; }
	CStructuredBuffer* GetBoneSocketMat() { return m_pBoneSocketMat; }
	UINT GetBoneCount() { return (UINT)m_pVecBones->size(); }
	UINT GetEndFrame() { return (UINT)m_pVecClip->at(0).iEndFrame; }
	int GetCurFrameIdx() { return m_iFrameIdx; }
	const Matrix& GetMatrix() { return m_matAnimation; }
	const wstring& GetCurFrameKey() { return m_strCurKey; }
	tAnim3DFrm GetCurFrame() { return m_tCurFrame; }
	Ptr<CMeshData> GetMeshData() { return m_pMeshData; }

	double GetInterval()
	{
		m_dInterval = (1 / (double)m_iFrameCount) * (1 / (double)m_fTimeScale);
		return m_dInterval;
	}
	void Init();
	void ClearData();
	UINT IsEnd() { return 0 < m_iEnd; }

	void Add_Frame(const wstring& _key, tAnim3DFrm _tData);
	const std::unordered_map<wstring, tAnim3DFrm>& GetFrames() { return m_mapAnimation; }

	const string& Delete(const wstring& _key);
	void DeleteEnd();
	void SetUseAnimationMatrix(bool _bUse);
	bool GetUseAnimationMatrix();
private:
	void check_mesh(Ptr<CMesh> _pMesh);

public:
	virtual void SaveToFile(FILE* _pFile) override;
	virtual void LoadFromFile(FILE* _pFile) override;
	CLONE(CAnimator3D);
public:
	CAnimator3D();
	CAnimator3D(const CAnimator3D& _origin);
	~CAnimator3D();
};