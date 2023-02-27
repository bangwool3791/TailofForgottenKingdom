#pragma once

#include "imgui.h"

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

static float ConvertRadianToDegree(float _fRadian)
{
	return _fRadian / XM_PI * 180;
}

static float ConvertDegreeToRadian(float _fDegree)
{
	return _fDegree / 180 * XM_PI;
}
