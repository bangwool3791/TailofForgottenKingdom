#include "pch.h"
#include "ParticleSystemUI.h"

#include <Engine/CParticleSystem.h>

#include "CImGuiMgr.h"
#include "ListUI.h"

ParticleSystemUI::ParticleSystemUI()
	: ComponentUI("ParticleSystem", COMPONENT_TYPE::PARTICLESYSTEM)
	, m_pParticleSystem{ nullptr }
	, m_iMaxCount{ 5 }
	, m_bPosInherit{ false }
	, m_iAliveCount{ 1 }
	, m_fArrMinMaxTime{ {0.f}, {5.f} }
	, m_fArrMinMaxSpeed{ {10.f}, {100.f} }
	, m_fRange{ 10.f }
	, m_fTerm{ 3.f }
	, m_bShaderUseDirection{ false }
{
	SetSize(Vec2{ 0, 330 });
}

ParticleSystemUI::~ParticleSystemUI() {}

void ParticleSystemUI::SetData()
{
	//m_pParticleSystem = GetTargetObject()->ParticleSystem();

	//m_iMaxCount = m_pParticleSystem->GetMaxParticleCount();
	//m_bPosInherit = m_pParticleSystem->PosInherit();
	//m_iAliveCount = m_pParticleSystem->GetAliveCount();

	//const Vec2& vLifeTime = m_pParticleSystem->GetMinMaxLifeTime();
	//m_fArrMinMaxTime[0] = vLifeTime.x;
	//m_fArrMinMaxTime[1] = vLifeTime.y;

	//const Vec2& vMinMaxSpeed = m_pParticleSystem->GetMinMaxSpeed();
	//m_fArrMinMaxSpeed[0] = vMinMaxSpeed.x;
	//m_fArrMinMaxSpeed[1] = vMinMaxSpeed.y;

	//const std::pair<Vec4, Vec4>& pairColor = m_pParticleSystem->GetStartEndColor();
	//m_vArrStartEndColor[0] = pairColor.first;
	//m_vArrStartEndColor[1] = pairColor.second;

	//const std::pair<Vec3, Vec3>& pairScale = m_pParticleSystem->GetStartEndScale();
	//m_vArrStartEndScale[0] = pairScale.first;
	//m_vArrStartEndScale[1] = pairScale.second;

	//const Vec2& vDirection = m_pParticleSystem->GetDirection();
	//m_iArrDirection[0] = static_cast<int>(vDirection.x);
	//m_iArrDirection[1] = static_cast<int>(vDirection.y);


	//m_fRange = m_pParticleSystem->GetRange();
	//m_fTerm = m_pParticleSystem->GetTerm();
}

void ParticleSystemUI::render_update()
{
	//ComponentUI::render_update();
	//if (ComponentUI::IsFold()) { SetSize(Vec2(0.f, 25.f)); return; }
	//else { SetSize(Vec2(0.f, 330.f)); }

	//SetData();

	//ImGui::PushItemWidth(200); // Float3 위젯 간격 설정

	//ImGui::Text("Shader");
	//ImGui::SameLine(100);
	//if (nullptr == m_pParticleSystem->GetComputeShader())
	//{
	//	ImGui::Text("NONE");
	//}
	//else
	//{
	//	ImGui::Text(ToString(m_pParticleSystem->GetComputeShader()->GetKey()).c_str());
	//}
	//ImGui::SameLine();

	//if (ImGui::Button("##PARTICLE_SHADER", Vec2(15, 15)))
	//{
	//	// ListUI 활성화한다.
	//	const auto& mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::COMPUTE_SHADER);
	//	ListUI* pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
	//	pListUI->Clear();
	//	pListUI->SetTitle("COMPUTESHADER_LIST");

	//	for (const auto& pairData : mapRes)
	//	{
	//		pListUI->AddList(ToString(pairData.first));
	//	}

	//	pListUI->Activate();
	//	pListUI->SetDBCEvent(this, (DBCLKED)&ParticleSystemUI::ShaderSelect);
	//}


	//ImGui::Text("Pos Inherit");
	//ImGui::SameLine(100);
	//ImGui::Checkbox("##POS_INHERIT", &m_bPosInherit);
	//m_pParticleSystem->SetPosInherit(m_bPosInherit);

	//ImGui::Text("Max Count");
	//ImGui::SameLine(100);
	//ImGui::InputInt("##MAX_COUNT", &m_iMaxCount);
	//m_pParticleSystem->SetMaxParticleCount(m_iMaxCount);

	//ImGui::Text("Alive Count");
	//ImGui::SameLine(100);
	//ImGui::InputInt("##ALIVE_COUNT", &m_iAliveCount);
	//m_pParticleSystem->SetAliveCount(m_iAliveCount);

	//ImGui::Text("MIN/MAX Time");
	//ImGui::SameLine(100);
	//ImGui::InputFloat2("##MIN_MAX_TIME", m_fArrMinMaxTime);
	//m_pParticleSystem->SetMinMaxLifeTime(m_fArrMinMaxTime[0], m_fArrMinMaxTime[1]);

	//ImGui::Text("MIN/MAX Speed");
	//ImGui::SameLine(100);
	//ImGui::InputFloat2("##MIN_MAX_SPEED", m_fArrMinMaxSpeed);
	//m_pParticleSystem->SetMinMaxSpeed(m_fArrMinMaxSpeed[0], m_fArrMinMaxSpeed[1]);

	//ImGui::Text("Start Color");
	//ImGui::SameLine(100);
	//ImGui::ColorEdit4("##START_COLOR", m_vArrStartEndColor[0]);
	//ImGui::Text("End Color");
	//ImGui::SameLine(100);
	//ImGui::ColorEdit4("##END_COLOR", m_vArrStartEndColor[1]);
	//m_pParticleSystem->SetStartEndColor(m_vArrStartEndColor[0], m_vArrStartEndColor[1]);

	//ImGui::Text("Start Scale");
	//ImGui::SameLine(100);
	//ImGui::InputFloat3("##START_SCALE", m_vArrStartEndScale[0]);
	//ImGui::Text("End Scale");
	//ImGui::SameLine(100);
	//ImGui::InputFloat3("##END_SCALE", m_vArrStartEndScale[1]);
	//m_pParticleSystem->SetStartEndScale(m_vArrStartEndScale[0], m_vArrStartEndScale[1]);

	//ImGui::Text("Creation Range");
	//ImGui::SameLine(100);
	//ImGui::InputFloat("##CREATION_RANGE", &m_fRange);
	//m_pParticleSystem->SetRange(m_fRange);

	//ImGui::Text("Creation Term");
	//ImGui::SameLine(100);
	//ImGui::InputFloat("##CREATION_TERM", &m_fTerm);
	//m_pParticleSystem->SetTerm(m_fTerm);

	//ImGui::Text("DIRECTION");
	//ImGui::SameLine((100));
	//ImGui::Checkbox("##SHADER_USE_DIRECTION", &m_bShaderUseDirection);
	//if (m_bShaderUseDirection)
	//{
	//	static std::map<std::string, Vec2> s_mapDirection = {
	//		{"FROM LEFT", Vec2{1.f, 0.f}},
	//		{"FROM RIGHT", Vec2{-1.f, 0.f}},
	//		{"FROM TOP", Vec2{0.f, 1.f}},
	//		{"FROM BOTTOM", Vec2{0.f, -1.f}}
	//	};

	//	static const char* s_arrDirectionName[] = {
	//		"FROM LEFT",
	//		"FROM RIGHT",
	//		"FROM TOP",
	//		"FROM BOTTOM"
	//	};

	//	static Vec2 s_arrDirection[] = {
	//		{1.f, 0.f},
	//		{-1.f, 0.f},
	//		{0.f, 1.f},
	//		{0.f, -1.f}
	//	};

	//	size_t index{};
	//	for (size_t i = 0; i < std::size(s_arrDirection); ++i)
	//	{
	//		if (s_arrDirection[i].x == m_iArrDirection[0]
	//			&& s_arrDirection[i].y == m_iArrDirection[1])
	//		{
	//			index = i;
	//		}
	//	}
	//	ImGui::SameLine();
	//	if (ImGui::BeginCombo("##PARTICLE_DIRECTION_COMBO", s_arrDirectionName[index]))
	//	{
	//		for (size_t i = 0; i < std::size(s_arrDirectionName); ++i)
	//		{
	//			if (ImGui::Selectable(s_arrDirectionName[i]))
	//			{
	//				m_pParticleSystem->SetDirection(s_mapDirection[s_arrDirectionName[i]]);
	//			}
	//		}
	//		ImGui::EndCombo();
	//	}
	//}
	//ImGui::Text("PARTICLE IMAGE");
	//ImGui::SameLine();
	//Ptr<CTexture> pTexture = m_pParticleSystem->GetMaterial(0)->GetTexParam(TEX_PARAM::TEX_0);
	//if (nullptr != pTexture)
	//{
	//	ImGui::Text(ToString(pTexture->GetKey()).c_str());
	//}
	//else
	//	ImGui::Text("NONE");
	//ImGui::SameLine();
	//if (ImGui::Button("##PARTICLE_TEXTURE_BUTTON", Vec2(15, 15)))
	//{
	//	// ListUI 활성화한다.
	//	const auto& mapRes = CResMgr::GetInst()->GetResList(RES_TYPE::TEXTURE);
	//	ListUI* pListUI = static_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("##ListUI"));
	//	pListUI->Clear();
	//	pListUI->SetTitle("TEXTURE_LIST");

	//	for (const auto& pairData : mapRes)
	//	{
	//		pListUI->AddList(ToString(pairData.first));
	//	}

	//	pListUI->Activate();
	//	pListUI->SetDBCEvent(this, (DBCLKED)&ParticleSystemUI::TextureSelect);
	//}
}

void ParticleSystemUI::TextureSelect(void* _pTextureName)
{
	//const std::wstring  key = ToWString(static_cast<char*>(_pTextureName));
	//const Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(key);

	//// 변경점이 있을 때만 세팅
	//if (m_pParticleSystem->GetMaterial(0)->GetTexParam(TEX_PARAM::TEX_0) != pTexture)
	//{
	//	m_pParticleSystem->GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_0, pTexture);
	//}
}

void ParticleSystemUI::ShaderSelect(void* _pShaderName)
{
	//const std::wstring               key = ToWString(static_cast<char*>(_pShaderName));
	//const Ptr<CParticleUpdateShader> pComputeShader = static_cast<CParticleUpdateShader*>(CResMgr::GetInst()->FindRes<
	//	CComputeShader>(key).Get());

	//// 변경점이 있을 때만 세팅
	//if (m_pParticleSystem->GetComputeShader() != pComputeShader)
	//{
	//	m_pParticleSystem->SetComputeShader(pComputeShader);
	//}
}