#include "pch.h"
#include "ComponentUI.h"


ComponentUI::ComponentUI(const string& _UIName, COMPONENT_TYPE _eType)
	: UI(_UIName)
	, m_TargetObj(nullptr)
	, m_eType(_eType)	
{
}

ComponentUI::ComponentUI(const string& _UIName, OBJECT_TYPE _eType)
    : UI(_UIName)
    , m_TargetObj(nullptr)
    , m_eObjType(_eType)
{
}

ComponentUI::~ComponentUI()
{
}

void ComponentUI::render_update()
{
    ImGui::PushID(0);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));

    if((int)m_eType != -1)
        ImGui::Button(ToString(m_eType));

    if ((int)m_eObjType != -1)
        ImGui::Button(ToString(m_eObjType));

    ImGui::PopStyleColor(3);
    ImGui::PopID();
}