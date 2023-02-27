#pragma once
#include "CShader.h"

class CComputeShader :
	public CShader
{
private:
	ComPtr<ID3DBlob>			m_CSBlob;
	ComPtr<ID3D11ComputeShader> m_CS;

protected:
	tMtrlConst					m_Param;

	UINT						m_iGroupX;
	UINT						m_iGroupY;
	UINT						m_iGroupZ;

	UINT						m_iGroupPerThreadX;
	UINT						m_iGroupPerThreadY;
	UINT						m_iGroupPerThreadZ;

public :
	void CreateComputeShader(const wstring& _strRelativePath, const string& _strFuncName);
	void Excute();

	virtual void UpdateData() = 0;
	virtual void Clear() = 0;

public :
	CComputeShader(UINT _iGroupPerX, UINT _iGroupPerY, UINT _iGroupPerZ);
	virtual ~CComputeShader();
};