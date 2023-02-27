#include "pch.h"
#include "CAnimator2DUI.h"

#include <Engine/CGameObject.h>
#include <Engine\CAnimator2D.h>

CAnimator2DUI::CAnimator2DUI()
	: ComponentUI("Animator2D", COMPONENT_TYPE::ANIMATOR2D)
{
}

CAnimator2DUI::~CAnimator2DUI()
{
}


void CAnimator2DUI::update()
{
	if (nullptr != GetTarget() && nullptr != GetTarget()->Animator2D())
	{
		m_strTexName = WStringToString(GetTarget()->Animator2D()->GetTexture()->GetName());
		m_CurKey = WStringToString(GetTarget()->Animator2D()->GetKey());
		vector<wstring> vec = GetTarget()->Animator2D()->GetKeys();
		m_vecKey.clear();
		m_vecKey.resize(vec.size());
		for (size_t i{}; i < vec.size(); ++i)
		{
			m_vecKey[i] = WStringToString(vec[i]);

			if (m_CurKey == m_vecKey[i])
				m_iCurIndex = (int)i;
		}
	}

	ComponentUI::update();
}

void CAnimator2DUI::render_update()
{
	ComponentUI::render_update();

	if (nullptr != GetTarget() && nullptr != GetTarget()->Animator2D())
	{
		ImGui::Text("Texture Name"); ImGui::SameLine(); ImGui::InputText("##Texture Name", (char*)m_strTexName.data(), m_strTexName.length(), ImGuiInputTextFlags_ReadOnly);
		ImGui::Text("Curent Key"); ImGui::SameLine(); ImGui::InputText("##Current Key", (char*)m_CurKey.data(), m_CurKey.length(), ImGuiInputTextFlags_ReadOnly);

		const char* combo_preview_value = m_vecKey[m_iCurIndex].c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
		if (ImGui::BeginCombo("Key", combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
		{
			for (int n = 0; n < m_vecKey.size(); n++)
			{
				const bool is_selected = (m_iCurIndex == n);
				if (ImGui::Selectable(m_vecKey[n].c_str(), is_selected))
				{
					m_iCurIndex = n;
					m_CurKey = m_vecKey[n];
					GetTarget()->Animator2D()->Play(StringToWString(m_vecKey[m_iCurIndex]));
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
}