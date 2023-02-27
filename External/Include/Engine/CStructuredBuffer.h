#pragma once
#include "CEntity.h"

class CStructuredBuffer :
	public CEntity
{
private:
	bool									m_bCPU = false;
	ComPtr<ID3D11Buffer>					m_SB;
	ComPtr<ID3D11ShaderResourceView>		m_SRV;
	ComPtr<ID3D11UnorderedAccessView>		m_UAV;

	ComPtr<ID3D11Buffer>                    m_WriteBuffer;
	ComPtr<ID3D11Buffer>                    m_ReadBuffer;

	D3D11_BUFFER_DESC						m_tDesc;
	SB_TYPE									m_eType;

	UINT									m_iElementSize;
	UINT									m_iElementCount;

	UINT									m_iRecentRegisterNum;
	UINT									m_iRecentRegisterNumRW;
public:
	int Create(UINT _iElementSize, UINT _iElementCount, SB_TYPE _eType, void* _pInitial, bool _CPUAccess = false);
	UINT GetElementsSize() { return m_iElementSize; }
	UINT GetElementsCount() { return m_iElementCount; }

	void SetData(void* _pSrc, UINT _iElementCount);
	void GetData(void* _pDst, UINT _iSizeByte = 0);
	void UpdateData(UINT _RegisterNum, UINT _PipelineStage);
	void UpdateData_CS(UINT _RegisterNum, bool _bShaderRes);

	void Clear();
public :
	CLONE(CStructuredBuffer);

public :
	CStructuredBuffer();
	CStructuredBuffer(const CStructuredBuffer& _origin);
	virtual ~CStructuredBuffer();
};