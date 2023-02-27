#include "pch.h"
#include "ResUI.h"

ResUI::ResUI(const string& _UIName, RES_TYPE _eType)
	:UI(_UIName)
	,m_eResType{_eType}
{
}

ResUI::~ResUI()
{
}

void ResUI::render_update()
{
    ImGui::PushID(0);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));
    ImGui::Button(ToString(m_eResType));
    ImGui::PopStyleColor(3);
    ImGui::PopID();

    // 정보를 표현하려는 리소스가 해당 UI 와 일치하는 타입인지 확인한다.
    // 클릭 또는 드랍 리소스가 UI 객체 타입과 다르다면 assert
    assert(m_eResType == GetTarget()->GetResType());
        
}

