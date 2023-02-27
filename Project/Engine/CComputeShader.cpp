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
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	HRESULT hr = D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), "cs_5_0", 0, 0, m_CSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf());

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
	* 자식 Shader 클래스에서, 상수 버퍼(m_Param)를 업데이트한다. 
	*/
	UpdateData();

	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Param);
	//컴퓨트 셰이더에 상수 버퍼를 업데이트한다.
	pCB->UpdateData_CS();
	//컨텍스트에 컴퓨트 셰이더를 Set한다.
	//컨텍스트란 ? 
	/*
	* 디바이스 컨텍스트에는 디바이스가 사용되는 상황 또는 설정이 포함됩니다. 
	더 구체적으로, 디바이스 컨텍스트는 디바이스가 소유한 리소스를 사용하여 파이프라인 상태를 설정하고 렌더링 명령을 생성하는 데 사용됩니다. 
	*/
	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);
	//스레드 그룹에서 명령 목록을 실행합니다.
	CONTEXT->Dispatch(m_iGroupX, m_iGroupY, m_iGroupZ);

	Clear();
}
