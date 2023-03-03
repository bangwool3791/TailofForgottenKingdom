#include "pch.h"
#include "CComputeShader.h"

#include "CDevice.h"
#include "CPathMgr.h"
#include "CConstBuffer.h"
#include "CTexture.h"

CComputeShader::CComputeShader(UINT _iGroupPerX, UINT _iGroupPerY, UINT _iGroupPerZ)
	:CShader(RES_TYPE::COMPUTE_SHADER)
	, m_iGroupX(0)
	, m_iGroupY(0)
	, m_iGroupZ(0)
	, m_iGroupPerThreadX(_iGroupPerX)
	, m_iGroupPerThreadY(_iGroupPerY)
	, m_iGroupPerThreadZ(_iGroupPerZ)
{

}

CComputeShader::~CComputeShader()
{

}

void CComputeShader::CreateComputeShader(const wstring& _strRelativePath, const string& _strFuncName)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), "cs_5_0", dwShaderFlags, 0, m_CSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

	if (FAILED(hr))
	{
		const char* pErr = (const char*)m_ErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Shader Compile Failed!!", MB_OK);
		assert(nullptr);
	}

	hr = DEVICE->CreateComputeShader(m_CSBlob->GetBufferPointer(), m_CSBlob->GetBufferSize()
		, nullptr, m_CS.GetAddressOf());
	//m_CS->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CComputeShader::m_CS") - 1, "CComputeShader::m_CS");
	assert(!FAILED(hr));
}

void CComputeShader::Excute()
{
	/*
	* �ڽ� Shader Ŭ��������, ��� ����(m_Param)�� ������Ʈ�Ѵ�. 
	*/
	UpdateData();

	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Param);
	//��ǻƮ ���̴��� ��� ���۸� ������Ʈ�Ѵ�.
	pCB->UpdateData_CS();
	//���ؽ�Ʈ�� ��ǻƮ ���̴��� Set�Ѵ�.
	//���ؽ�Ʈ�� ? 
	/*
	* ����̽� ���ؽ�Ʈ���� ����̽��� ���Ǵ� ��Ȳ �Ǵ� ������ ���Ե˴ϴ�. 
	�� ��ü������, ����̽� ���ؽ�Ʈ�� ����̽��� ������ ���ҽ��� ����Ͽ� ���������� ���¸� �����ϰ� ������ ����� �����ϴ� �� ���˴ϴ�. 
	*/
	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);
	//������ �׷쿡�� ��� ����� �����մϴ�.
	CONTEXT->Dispatch(m_iGroupX, m_iGroupY, m_iGroupZ);

	Clear();
}
