#include "pch.h"
#include "ProgressUI.h"

ProgressUI::ProgressUI()
	: UI("ProgressUI")
{
	Close();
}

ProgressUI::~ProgressUI()
{

}

void ProgressUI::render_update()
{
	ImGui::SetWindowSize(ImVec2(200.f, 50.f));

	if (m_animate)
	{
		m_progress += m_progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
		if (m_progress >= +1.1f) { m_progress = +1.1f; m_progress_dir *= -1.0f; }
		if (m_progress <= -0.1f) { m_progress = -0.1f; m_progress_dir *= -1.0f; }
	}

	// Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
	// or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.

	float progress_saturated = std::clamp(m_progress, 0.0f, 1.0f);
	char buf[32];
	sprintf_s(buf, sizeof(buf), "%d/%d", (int)(progress_saturated * 100.f), 100);
	ImGui::ProgressBar(m_progress, ImVec2(0.f, 0.f), buf);

	if (progress_saturated * 100.f == 100.f)
	{
		Close();
	}

}

void ProgressUI::Close()
{
	m_progress = 0;
	UI::Close();
	ResetFocus();
}


