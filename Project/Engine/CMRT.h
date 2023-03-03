#pragma once
#include "CEntity.h"

#include "Ptr.h"
#include "CTexture.h"

class CMRT
	: public CEntity
{
private:
	//∑£¥ı ≈∏∞Ÿ 8¿Â
	Ptr<CTexture>	m_arrTarget[8];
	Vec4			m_arrClearColor[8];

	//Depth Stencil Texture
	Ptr<CTexture>	m_DSTex;
	int				m_RTCount;

public :
	void Create(Ptr<CTexture>(&_arrTex)[8], Vec4(&arrClear)[8], Ptr<CTexture> _pDesTex);
	void Clear();
	void OMSet();
	void OMClear();

	CLONE_DUMMY(CMRT);

public :
	CMRT();
	~CMRT();
};