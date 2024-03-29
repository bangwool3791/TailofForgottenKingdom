#pragma once
#include "CComponent.h"

class CStructuredBuffer;

class CTrailComponent :
	public CComponent
{
private:
	bool m_bSwordTrail;
	Vec3 m_vTopOffset;
	Vec3 m_vBottomOffset;

	Vec3 m_vUpperSwordPos;
	Vec3 m_vUpperSwordRot;
	Vec3 m_vBottomSwordPos;
	Vec3 m_vBottomSwordRot;

	std::vector<float> m_vecTrailTime;
	std::vector<float> m_vecSwordTrailTime;
	std::vector<Matrix> m_vecBoneSocket;

	std::queue<Vec3>    m_queSwordPos;
	std::vector<Vec3>	m_vecSwordPos;

	UINT				m_iBoneIdx = 0;
	const UINT			m_iSwordTrailCount = 172;
	int					m_iSwordTrailIdx = -1;
public:
	CStructuredBuffer* m_pTrailTime;
	CStructuredBuffer* m_pSwordTrailTime;
	CStructuredBuffer* m_pBoneSocketMat;
	CStructuredBuffer* m_pSwordPos;
public:
	virtual void begin();
	virtual void tick();
	virtual void finaltick();

public:
	virtual void SaveToFile(FILE* _File);
	virtual void LoadFromFile(FILE* _File);

	void SetVectorTrailTime(const vector<float>& vec);
	void SetVectorSwordTrailTime(const vector<float>& vec);

	void SetBoneSocketBuffer(CStructuredBuffer* pBuffer);
	void SetData(float fDelta);
	vector<float>& GetTrailTime() { return m_vecTrailTime; }

	void SetSwordTrailData(float fDelta);
	vector<float>& GetSwordTrailTime() { return m_vecSwordTrailTime; }

	const Matrix& GetBoneMat(UINT idx) { return m_vecBoneSocket[idx]; }
	void UpdateData();
	void Clear() ;

	UINT GetBoneIdx() { return m_iBoneIdx; }
	void SetBoneIdx(UINT idx) { m_iBoneIdx = idx; }

	void PushSwordPos(Vec3 vPos);
	
	void SetTopOffsetPos(Vec3 _vPos) { m_vTopOffset = _vPos; }
	Vec3 GetTopOffsetPos() { return m_vTopOffset; }

	void SetBottomOffsetPos(Vec3 _vPos) { m_vBottomOffset = _vPos; }
	Vec3 GetBottomOffsetPos() { return m_vBottomOffset; }

	void SetUpperSwordPos(Vec3 _vPos) { m_vUpperSwordPos = _vPos; }
	Vec3 GetUpperSwordPos() { return m_vUpperSwordPos; }
	void SetUpperSwordRot(Vec3 _vPos) { m_vUpperSwordRot = _vPos; }
	Vec3 GetUpperSwordRot() { return m_vUpperSwordRot; }

	void SetBottomSwordPos(Vec3 _vPos) { m_vBottomSwordPos = _vPos; }
	Vec3 GetBottomSwordPos() { return m_vBottomSwordPos; }
	void SetBottomSwordRot(Vec3 _vPos) { m_vBottomSwordRot = _vPos; }
	Vec3 GetBottomSwordRot() { return m_vBottomSwordRot; }

	void SetSwordTrail(bool _bTrail) { m_bSwordTrail = _bTrail; }
	void ClearSwordPos();
public:
	CLONE(CTrailComponent);
	CTrailComponent();
	CTrailComponent(const CTrailComponent& rhs);
	virtual ~CTrailComponent();
};