#pragma once

#include "imgui.h"

#define HSV_SKY_GREY ImColor::HSV(0.f, 0.f, 0.5f)

static void Text(ImVec4 _vColor, Vec2 _vSize, string _str)
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, _vColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, _vColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, _vColor);
	ImGui::Button(_str.c_str(), ImVec2(_vSize.x, _vSize.y));
	ImGui::PopStyleColor(3);
	ImGui::PopID();
}

static float ConvertRadianToDegree(float _fRadian)
{
	return _fRadian / XM_PI * 180;
}

static float ConvertDegreeToRadian(float _fDegree)
{
	return _fDegree / 180 * XM_PI;
}
