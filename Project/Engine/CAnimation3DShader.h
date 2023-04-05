#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CAnimation3DShader :
	public CComputeShader
{
private:
	CStructuredBuffer* m_pFrameDataBuffer; // t16
	CStructuredBuffer* m_pOffsetMatBuffer; // t17

	CStructuredBuffer* m_pOutputBuffer;//u0

public :
	void SetBoneCount(int _iBoneCount) { m_Param.iArr[0] = _iBoneCount; }
	void SetFrameIndex(int _iFrameIdx) { m_Param.iArr[1] = _iFrameIdx; }
	void SetNextFrameIdx(int _iNextFrameIdx) { m_Param.iArr[2] = _iNextFrameIdx; }
	//g_float_0 : CurFrame - NextFrame ∫Ò¿≤
	void SetFrameRatio(float _fFrameRatio) { m_Param.fArr[0] = _fFrameRatio; }
	void SetFrameDataBuffer(CStructuredBuffer* _buffer) { m_pFrameDataBuffer = _buffer; }
	void SetOffsetMatBuffer(CStructuredBuffer* _buffer) { m_pOffsetMatBuffer = _buffer; }
	void SetOutputBuffer(CStructuredBuffer* _buffer) { m_pOutputBuffer = _buffer; }

public:
	virtual void UpdateData();
	virtual void Clear();

public:
	CAnimation3DShader();
	virtual ~CAnimation3DShader();
};