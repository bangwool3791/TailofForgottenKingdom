#include "pch.h"
#include "ListUI.h"

ListUI::ListUI()
	: UI("ListUI")
	, m_iSelectedIdx(-1)
	, m_DBCInst(nullptr)
	, m_DBCFunc(nullptr)
{
	Close();
	SetSize(ImVec2(200.f, 500.f));
}

ListUI::~ListUI()
{

}

void ListUI::render_update()
{
	//ImVec2 vMax = ImGui::GetWindowContentRegionMax();
	ImVec2 vRegion = ImGui::GetContentRegionAvail();
	vRegion.x += 18.f;
	vRegion.y += 3.f;

	ImVec2 vCurPos = ImGui::GetWindowPos();
	vCurPos.x -= 0.f;
	vCurPos.y += 24.f;

	ImGui::SetNextWindowPos(vCurPos);

	if (ImGui::BeginListBox("##ListBox", vRegion))
	{		
		for (size_t i = 0; i < m_ItemList.size(); ++i)
		{
			bool Selectable = (m_iSelectedIdx == i);
			if (ImGui::Selectable(m_ItemList[i].c_str(), Selectable))
			{
				m_iSelectedIdx = (int)i;				
			}

			if (Selectable)
			{
				ImGui::SetItemDefaultFocus();
			}			

			// 해당 아이템이 더블클릭 되었다.
			if (ImGui::IsItemHovered() &&  ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (m_DBCInst && m_DBCFunc)
				{
					(m_DBCInst->*m_DBCFunc)((DWORD_PTR)m_ItemList[i].c_str());
				}
				Close();
			}
		}		

		ImGui::EndListBox();
	}	
}

void ListUI::Close()
{
	UI::Close();
	m_ItemList.clear();

	m_DBCInst = nullptr;
	m_DBCFunc = nullptr;

	ResetFocus();
}


void ListUI::SetItemList(const vector<string>& _inList)
{
	m_ItemList = _inList;
}


void ListUI::SetItemList(const vector<wstring>& _inList)
{
	for (size_t i = 0; i < _inList.size(); ++i)
	{
		m_ItemList.push_back(string(_inList[i].begin(), _inList[i].end()));
	}	
}