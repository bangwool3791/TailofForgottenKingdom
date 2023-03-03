#include "pch.h"
#include "CSLight.h"

#include "CDevice.h"
#include "CStructuredBuffer.h"

CSLight::CSLight()
	: CComputeShader(32, 32, 1)
	, m_LightShareBuffer(nullptr)
{
	m_LightShareBuffer = new CStructuredBuffer;
	m_LightShareBuffer->Create(sizeof(tLightShare), 1, SB_TYPE::UAV_INC, nullptr, true);
	m_Param.iArr[0] = 1600 * 900;
}

CSLight::~CSLight()
{
	if (nullptr != m_LightShareBuffer)
		delete m_LightShareBuffer;
}


void CSLight::UpdateData()
{
	assert(m_LightShareBuffer);


	tLightShare shader = {};
	m_LightShareBuffer->SetData(&shader, 1);

	m_LightShareBuffer->UpdateData_CS(0, false);
	m_pTargetTex->UpdateData_CS(1, true);

	//Vec4 vData = {};
	//m_LightShareBuffer->GetData(&vData);

	//100 / 128
	//1024 -> 2번
	// 그룹 수 계산
	m_iGroupX = m_pTargetTex->GetWidth() / m_iGroupPerThreadX + 1;
	m_iGroupY = m_pTargetTex->GetHeight() / m_iGroupPerThreadY + 1;
	m_iGroupZ = 1;
}

void CSLight::Clear()
{
	m_pTargetTex->Clear_CS(1, true);
	m_LightShareBuffer->Clear();

	tLightShare shader = {};
	m_LightShareBuffer->GetData(&shader, sizeof(tLightShare));

	static Ptr<CMaterial> pMergeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeMtrl");

	if (shader.R != 0 || shader.G != 0 || shader.B != 0)
	{
		UINT32 r = shader.R;
		UINT32 g = shader.G;
		UINT32 b = shader.B;
		UINT32 a = shader.A;

		Vec4 v = Vec4(r / 100.f, g / 100.f, b / 100.f, a / 100.f);
		v /= (1600.f * 900.f);
		v.w = 1.f;
		m_fLumAverage = (v.x + v.y + v.z) / 3;
	}

	pMergeMtrl->SetScalarParam(FLOAT_0, &m_fLumAverage);
}
