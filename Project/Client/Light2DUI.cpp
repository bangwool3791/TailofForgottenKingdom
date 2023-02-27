#include "pch.h"
#include "Func.h"
#include "Light2DUI.h"
#include <Engine\CGameObject.h>
#include <Engine\CLight2D.h>

Light2DUI::Light2DUI()
	: ComponentUI("Light2D", OBJECT_TYPE::LIGHT2D)
	, m_vColor{}
	, m_pDirectionalLight{}
{

}


Light2DUI::~Light2DUI()
{

}


void Light2DUI::update()
{
	ComponentUI::update();
}

void Light2DUI::render_update()
{
	ComponentUI::render_update();

	/*if (m_pDirectionalLight)
		m_vColor = m_pDirectionalLight->Light2D()->GetLightColor();

	static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
	static ImVec4 color2 = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;
	ImGui::Checkbox("With Alpha Preview", &alpha_preview);
	ImGui::Checkbox("With Half Alpha Preview", &alpha_half_preview);
	ImGui::Checkbox("With Drag and Drop", &drag_and_drop);
	ImGui::Checkbox("With Options Menu", &options_menu); ImGui::SameLine(); 
	ImGui::Checkbox("With HDR", &hdr); ImGui::SameLine(); 
	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
	ImGui::Text("Color widget HSV with Alpha:");
	ImGui::ColorEdit3("MyColor##2", (float*)&color, ImGuiColorEditFlags_DisplayHSV | misc_flags);
	m_vColor.x = color.x;
	m_vColor.y = color.y;
	m_vColor.z = color.z;

	if (m_pDirectionalLight)
		m_pDirectionalLight->Light2D()->SetLightColor(m_vColor);

	if (m_pSpotLight)
	{
		m_fSpotLightRadius = m_pSpotLight->Light2D()->GetRadius();
		m_fSpotLightAngle = m_pSpotLight->Light2D()->GetAngle();
		m_fSpotLightAngle = ConvertRadianToDegree(m_fSpotLightAngle);
		m_vSpotLightColor = m_pSpotLight->Light2D()->GetLightColor();
		color2.x = m_vSpotLightColor.x;
		color2.y = m_vSpotLightColor.y;
		color2.z = m_vSpotLightColor.z;
	}

	ImGui::Text("SpotLightRadius"); ImGui::SameLine(); ImGui::DragFloat("##SpotLightRadius", &m_fSpotLightRadius, 1);
	ImGui::Text("SpotLightAngle"); ImGui::SameLine();  ImGui::DragFloat("##SpotLightAngle", &m_fSpotLightAngle, 1);
	ImGui::Text("Color widget HSV with Alpha:");
	ImGui::ColorEdit3("MyColor##3", (float*)&color2, ImGuiColorEditFlags_DisplayHSV | misc_flags);

	if (m_pSpotLight)
	{
		m_vSpotLightColor.x = color2.x;
		m_vSpotLightColor.y = color2.y;
		m_vSpotLightColor.z = color2.z;
		m_pSpotLight->Light2D()->SetRadius(m_fSpotLightRadius);
		m_fSpotLightAngle = ConvertDegreeToRadian(m_fSpotLightAngle);
		m_pSpotLight->Light2D()->SetAngle(m_fSpotLightAngle);
		m_pSpotLight->Light2D()->SetLightColor(m_vSpotLightColor);
	}*/
}