#include "pch.h"
#include "ShadowUI.h"


#include <Engine\CTransform.h>
#include <Engine\CGameObject.h>
#include <Script\CShadowScript.h>
#include <Engine\CMeshRender.h>
#include "Func.h"

ShadowUI::ShadowUI()
	: ComponentUI("Shadow", COMPONENT_TYPE::SCRIPT)
	, m_bIgnorScale(false)
{
}

ShadowUI::~ShadowUI()
{
}


void ShadowUI::update()
{
	if (nullptr != GetTarget())
	{
		m_vScale = GetTarget()->Transform()->GetRelativeScale();
		m_vRot = GetTarget()->Transform()->GetRelativeRotation();

		m_bIgnorScale = GetTarget()->Transform()->IsIgnoreParentScale();
	}

	ComponentUI::update();
}

void ShadowUI::render_update()
{
	ComponentUI::render_update();

	if (GetTarget())
	{
		m_vPos = GetTarget()->Transform()->GetRelativePos();
		m_vScale = GetTarget()->Transform()->GetRelativeScale();
		m_vRot = GetTarget()->Transform()->GetRelativeRotation();
		m_bIgnorScale = GetTarget()->Transform()->IsIgnoreParentScale();
		//상수 데이터 가져올 것
		//GetTarget()->MeshRender()->GetCurMaterial()->GetMaterial(
	}

	ImGui::Text("Position"); ImGui::SameLine(); ImGui::InputFloat3("##Position", m_vPos);
	ImGui::Text("Scale   "); ImGui::SameLine(); ImGui::InputFloat3("##Scale", m_vScale);

	m_vRot.ToDegree();
	ImGui::Text("Rotation"); ImGui::SameLine(); ImGui::InputFloat3("##Rotation", m_vRot);

	ImGui::Text("Ignore Parent Scale"); ImGui::SameLine(); ImGui::Checkbox("##IgnorParentScale", &m_bIgnorScale);

	static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;
	ImGui::Checkbox("With Alpha Preview", &alpha_preview);
	ImGui::Checkbox("With Half Alpha Preview", &alpha_half_preview);
	ImGui::Checkbox("With Drag and Drop", &drag_and_drop);
	ImGui::Checkbox("With Options Menu", &options_menu); ImGui::SameLine(); HelpMarker("Right-click on the individual color widget to show options.");
	ImGui::Checkbox("With HDR", &hdr); ImGui::SameLine(); HelpMarker("Currently all this does is to lift the 0..1 limits on dragging widgets.");
	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
	ImGui::Text("Color widget HSV with Alpha:");
	ImGui::ColorEdit4("MyColor##2", (float*)&color, ImGuiColorEditFlags_DisplayHSV | misc_flags);

	if (GetTarget())
	{
		GetTarget()->Transform()->SetRelativePos(m_vPos);
		GetTarget()->Transform()->SetRelativeScale(m_vScale);

		m_vRot.ToRadian();
		GetTarget()->Transform()->SetRelativeRotation(m_vRot);
		GetTarget()->Transform()->SetIgnoreParentScale(m_bIgnorScale);
	
		auto pShadow = GetTarget()->GetScript<CShadowScript>(L"ShadowScript");
		static Vec4 vColor;
		vColor = color;
		GetTarget()->MeshRender()->GetCurMaterial()->SetScalarParam(VEC4_0, &vColor);
	}
}