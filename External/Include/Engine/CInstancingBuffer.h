#pragma once

#include "CStructuredBuffer.h"
#include "CTexture.h"

class CTexture;
class CStructuredBuffer;
class CCopyBoneShader;


class CInstancingBuffer
	: public CSingleton<CInstancingBuffer>
{
private:
	ComPtr<ID3D11Buffer>		m_pInstancingBuffer;
	UINT						m_iMaxCount;
	vector<tInstancingData>		m_vecData;

	// anim3D 용도
	vector<CStructuredBuffer*>	m_vecBoneMat;
	//이 변수 사용 안함
	int							m_iAnimInstCount;
	CStructuredBuffer*			m_pBoneBuffer;

	Ptr<CCopyBoneShader>		m_pCopyShader;

public:
	void init();
	void Clear()
	{
		m_vecData.clear();
		m_vecBoneMat.clear();
		m_iAnimInstCount = 0;
	}
	void AddInstancingData(tInstancingData& _tData) { m_vecData.push_back(_tData); }

	UINT GetInstanceCount() { return (UINT)m_vecData.size(); }
	ComPtr<ID3D11Buffer> GetBuffer() { return m_pInstancingBuffer; }
	void SetData();

	void AddInstancingBoneMat(CStructuredBuffer* _pBuffer);
	int GetAnimInstancingCount() { return m_iAnimInstCount; };
	CStructuredBuffer* GetBoneBuffer() { return m_pBoneBuffer; }

private:
	void Resize(UINT _iCount);


public:
	CInstancingBuffer();
	~CInstancingBuffer();
};

