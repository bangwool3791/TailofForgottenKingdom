#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CCopyBoneShader :
	public CComputeShader
{
private:
	CStructuredBuffer* m_pSrcBuffer;
	CStructuredBuffer* m_pDestBuffer;

public:
	// g_int_0 : BonCount
	void SetBoneCount(UINT _iBoneCount) { m_Param.iArr[0] = _iBoneCount; }

	// g_int_1 : RowIndex	
	void SetRowIndex(UINT _iRowIdx) { m_Param.iArr[1] = _iRowIdx; }

	void SetSourceBuffer(CStructuredBuffer* _pBuffer) { m_pSrcBuffer = _pBuffer; }
	void SetDestBuffer(CStructuredBuffer* _pBuffer) { m_pDestBuffer = _pBuffer; }

public:
	virtual void UpdateData();
	virtual void Clear();

public:
	CCopyBoneShader();
	virtual ~CCopyBoneShader();
};

