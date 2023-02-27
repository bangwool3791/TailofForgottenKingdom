#include "pch.h"
#include "UI.h"

UI::UI(const string& _strName)
	: m_strName(_strName)
	, m_ParentUI(nullptr)
	, m_bOpen(true)
	, m_bModal(false)
	, m_bSeperator{true}
{
}

UI::~UI()
{
	Safe_Del_Vec(m_vecChildUI);
}

void UI::ResetFocus()
{
	ImGui::SetWindowFocus(nullptr);
}

void UI::update()
{
	for (int i = 0; i < m_vecChildUI.size(); ++i)
	{
		m_vecChildUI[i]->update();
	}
}

void UI::render()
{
	if (m_bOpen)
	{
		// 부모 UI
		if (nullptr == m_ParentUI)
		{
			if (m_bModal)
			{
				// 메인 윈도우 중앙에서 켜지게 함
				ImVec2 vCenter = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(vCenter, ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(m_vSize, ImGuiCond_::ImGuiCond_Appearing);

				// Modal Popup UI 띄우기
				ImGui::OpenPopup(m_strName.c_str());
				if (ImGui::BeginPopupModal(m_strName.c_str(), &m_bOpen))
				{
					render_update();

					for (int i = 0; i < m_vecChildUI.size(); ++i)
					{
						m_vecChildUI[i]->render();
						//if (m_vecChildUI[i]->m_bSeperator && i != (m_vecChildUI.size() - 1))
						//	ImGui::Separator();
					}

					ImGui::EndPopup();
				}				
			}

			else
			{
				ImGui::Begin(m_strName.c_str(), &m_bOpen);

				render_update();

				for (int i = 0; i < m_vecChildUI.size(); ++i)
				{
					m_vecChildUI[i]->render();

					if (m_vecChildUI[i]->m_bOpen == true && m_vecChildUI[i]->m_bSeperator && (i != m_vecChildUI.size() - 1))
						ImGui::Separator();
				}

				ImGui::End();
			}			
		}

		// 자식 UI 라면
		else
		{
			ImGui::BeginChild(m_strName.c_str(), m_vSize);

			render_update();

			for (int i = 0; i < m_vecChildUI.size(); ++i)
			{
				m_vecChildUI[i]->render();
			}

			ImGui::EndChild();
		}	

		if (false == m_bOpen)
		{
			Close();
		}
	}
}

void UI::AddChild(UI* _ChildUI)
{
	_ChildUI->m_ParentUI = this;
	m_vecChildUI.push_back(_ChildUI);
}