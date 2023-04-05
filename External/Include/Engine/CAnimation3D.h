#pragma once

#include "pch.h"
#include "CEntity.h"
#include "CTexture.h"

class CAnimator3D;

class CAnimation3D :
	public CEntity
{
private:
	tAnim3DFrm m_tAnimationFrame;
public :
	void Set_StartFrame(double dStart) { m_tAnimationFrame.dStart = dStart; }
	void Set_EndFrame(double dEnd) { m_tAnimationFrame.dEnd = dEnd; }
	void Set_FrameCount(UINT iCount) { m_tAnimationFrame.iFrameCount = iCount; }
	void Set_Repeat(bool _bRepeat) { m_tAnimationFrame.bRepeat = _bRepeat; }

	double Get_StartFrame() { return m_tAnimationFrame.dStart; }
	double Get_EndFrame() { return m_tAnimationFrame.dEnd; }
	UINT Get_FrameCount() { return m_tAnimationFrame.iFrameCount; }
	bool Get_Repeat() { return m_tAnimationFrame.bRepeat; }

public:
	CAnimation3D();
	CAnimation3D(const CAnimation3D& rhs);
	~CAnimation3D();
};